// Adapted from SDL_kitchensink example_complex.c
#ifdef VIDEO_SUPPORT

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"
#include "video.h"
#include "loader/so_util.h"
#include "gmloader/libyoyo.h"
#include <SDL2/SDL.h>
#include <kitchensink/internal/kitdecoder.h>
#include <kitchensink/internal/kitlibstate.h>
#include <kitchensink/internal/utils/kithelpers.h>
#include <kitchensink/internal/video/kitvideo.h>
#include <kitchensink/kitchensink.h>
#include <kitchensink/kiterror.h>
#include <string.h>

#define AUDIOBUFFER_SIZE (1024 * 64)

static SDL_Window* sdl_win = NULL;
static SDL_AudioDeviceID video_audiodev = NULL;
static Kit_Source* video_src = NULL;
static Kit_Player* video_player = NULL;
static char video_audiobuf[AUDIOBUFFER_SIZE];

static bool video_loaded = false;
static int video_width = 0;
static int video_height = 0;
static double video_volume = 1.0;
static double video_status = -2;
static double video_playback_status = 0;
static void* video_buffer = NULL;
static int video_buffer_size = 0;
static double video_format = 0;
static double video_loop_enabled = false;
static const char* save_dir=NULL;

enum DecoderIndex {
    KIT_VIDEO_DEC = 0,
    KIT_AUDIO_DEC,
    KIT_SUBTITLE_DEC,
    KIT_DEC_COUNT
};

int video_init(SDL_Window* sdl_window, const char* save_path)
{
    warning("Initializing Video Playback...\n");
    int err = Kit_Init(0);
    if (err != 0) {
        fatal_error("Unable to initialize Kitchensink: %s", Kit_GetError());
        return 1;
    }
    // Set to 0 to allow ffmpeg decide thread count.
    Kit_SetHint(KIT_HINT_THREAD_COUNT, 0);
    Kit_SetHint(KIT_HINT_VIDEO_BUFFER_FRAMES, 5);
    Kit_SetHint(KIT_HINT_AUDIO_BUFFER_FRAMES, 8192);

    save_dir=save_path;
    sdl_win=sdl_window;

    return 0;
}

void video_open_internal(const char* path)
{
    warning("Loading video file %s\n", path);
    if (video_loaded)
        video_close_internal();


    // Join save_dir and path with a "/" , but only if save_dir isn't empty
    size_t save_dir_len = strlen(save_dir);
    size_t path_len = strlen(path);
    size_t total_len = save_dir_len + (save_dir_len > 0 ? 1 : 0) + path_len + 1;
    char* full_path = (char*)malloc(total_len);
    if (save_dir_len > 0) {
        sprintf(full_path, "%s/%s", save_dir, path);
    } else {
        sprintf(full_path, "%s", path);
    }

    // Load video file
    video_src = Kit_CreateSourceFromUrl(full_path);
    if (video_src == NULL) {
        fatal_error("Unable to load video file '%s': %s\n", path, Kit_GetError());
        video_status = -1;
        send_async_social("video_end"); //Just pretend we have ended the video, avoids stalling the game
        return;
    }

    // Select streams
    int video_vindex = Kit_GetBestSourceStream(video_src, KIT_STREAMTYPE_VIDEO);
    int video_aindex = Kit_GetBestSourceStream(video_src, KIT_STREAMTYPE_AUDIO);
    if (video_vindex < 0)
        warning("Video file %s does not contain a video stream.\n", path);

    // Create player with window dimensions, later update it to video dimensions
    int win_w, win_h;
    SDL_GetWindowSize(sdl_win, &win_w, &win_h);
    video_player = Kit_CreatePlayer(
        video_src,
        video_vindex,
        video_aindex,
        -1,
        win_w, win_h);

    Kit_PlayerInfo pinfo;
    Kit_GetPlayerInfo(video_player, &pinfo);
    if (Kit_GetPlayerAudioStream(video_player) >= 0) {
        warning(" * Audio: %s (%s), threads=%d, %dHz, %dch, %db, %s\n",
            pinfo.audio.codec.name,
            pinfo.audio.codec.description,
            pinfo.video.codec.threads,
            pinfo.audio.output.samplerate,
            pinfo.audio.output.channels,
            pinfo.audio.output.bytes,
            pinfo.audio.output.is_signed ? "signed" : "unsigned");
    }

    if (Kit_GetPlayerVideoStream(video_player) >= 0) {
        warning(" * Video: %s (%s), threads=%d, %dx%d\n",
            pinfo.video.codec.name,
            pinfo.video.codec.description,
            pinfo.video.codec.threads,
            pinfo.video.output.width,
            pinfo.video.output.height);

        video_width = pinfo.video.output.width;
        video_height = pinfo.video.output.height;
        Kit_SetPlayerScreenSize(video_player, video_width, video_height);

        // Android Game Maker does not support YUV Video, so we need to force the output to RGBA, making ffmpeg convert it on the fly
        Kit_Decoder* video_decoder = (Kit_Decoder*)video_player->decoders[KIT_VIDEO_DEC];
        video_decoder->output.format = SDL_PIXELFORMAT_RGBA32;
        video_format = 0;
    }

    // Set up frame buffer
    video_buffer_size = pinfo.video.output.width * pinfo.video.output.height * 4;
    video_buffer = malloc(video_buffer_size);

    // Init audio
    SDL_AudioSpec wanted_spec, audio_spec;
    SDL_memset(&wanted_spec, 0, sizeof(wanted_spec));
    wanted_spec.freq = pinfo.audio.output.samplerate;
    wanted_spec.format = pinfo.audio.output.format;
    wanted_spec.channels = pinfo.audio.output.channels;
    wanted_spec.size = 32768;
    video_audiodev = SDL_OpenAudioDevice(NULL, 0, &wanted_spec, &audio_spec, 0);
    SDL_PauseAudioDevice(video_audiodev, 0);

    // Start player, set start variables
    Kit_PlayerPlay(video_player);
    video_loaded = true;
    video_status = 0;
    video_playback_status = 2;
    video_loop_enabled = false;
    video_volume = 1;
    send_async_social("video_start");
}

void video_close_internal()
{
    if (!video_loaded)
        return;

    Kit_ClosePlayer(video_player);
    Kit_CloseSource(video_src);
    SDL_CloseAudioDevice(video_audiodev);
    free(video_buffer);
    video_status = -2;
    video_playback_status = 0;
}

void video_pause_internal()
{
    if (!video_loaded)
        return;

    video_playback_status = 3;
    SDL_PauseAudioDevice(video_audiodev, 1);
    Kit_PlayerPause(video_player);
}

void video_resume_internal()
{
    if (!video_loaded)
        return;

    video_playback_status = 2;
    SDL_PauseAudioDevice(video_audiodev, 0);
    Kit_PlayerPlay(video_player);
}

void video_enable_loop_internal(double loop)
{
    if (!video_loaded)
        return;

    video_loop_enabled = loop >= 0.5;
}

void video_seek_to_internal(double time)
{
    if (!video_loaded)
        return;

    Kit_PlayerSeek(video_player, time / 1000);
}

void video_set_volume_internal(double volume)
{
    video_volume = volume;
}

double video_get_volume_internal()
{
    return video_volume;
}

double video_status_internal()
{
    return video_status;
}

double video_get_status_internal()
{
    return video_playback_status;
}

double video_get_width_internal()
{
    return video_width;
}

double video_get_height_internal()
{
    return video_height;
}

double video_get_duration_internal()
{
    if (!video_loaded)
        return 0.0;

    return Kit_GetPlayerDuration(video_player) * 1000; // Needs to be in milliseconds for GM
}

double video_get_position_internal()
{
    if (!video_loaded)
        return 0.0;

    return Kit_GetPlayerPosition(video_player) * 1000; // Needs to be in milliseconds for GM
}

double video_get_format_internal()
{
    if (!video_loaded)
        return 0.0;
    return video_format;
}

double video_is_looping_internal()
{
    if (!video_loaded)
        return 0.0;
    return (video_loop_enabled >= 0.5) ? 1.0 : 0.0;
}

bool video_draw_internal(void* buffer)
{
    memcpy(buffer, video_buffer, video_buffer_size);
    return true;
}

void video_process()
{
    if (!video_loaded)
        return;

    if (video_playback_status==0)
        return;

    // If player has stopped, either loop or send end event
    if (Kit_GetPlayerState(video_player) == KIT_STOPPED) {
        video_seek_to_internal(0);
        if (video_loop_enabled) {
            video_resume_internal();
        } else {
            send_async_social("video_end");
            video_playback_status=0;
        }

    } else {
        video_decode();

        int queued = SDL_GetQueuedAudioSize(video_audiodev);
        if (queued < AUDIOBUFFER_SIZE) {
            int ret;
            while ((ret = Kit_GetPlayerAudioData(video_player, (unsigned char*)video_audiobuf, AUDIOBUFFER_SIZE)) > 0) {
                SDL_QueueAudio(video_audiodev, video_audiobuf, ret);
            }
        }
    }
}

typedef struct Kit_VideoDecoder {
    struct SwsContext* sws;
    AVFrame* scratch_frame;
} Kit_VideoDecoder;

typedef struct Kit_VideoPacket {
    double pts;
    AVFrame* frame;
} Kit_VideoPacket;

// This is basically duplicated from kitvideo.c Kit_GetVideoDecoderData, but we need to data to be raw, not in an SDL_Texture
void video_decode()
{
    Kit_Decoder* dec = (Kit_Decoder*)video_player->decoders[KIT_VIDEO_DEC];
    if (dec == NULL) {
        return;
    }

    Kit_VideoPacket* packet = NULL;
    double sync_ts = 0;
    unsigned int limit_rounds = 0;

    // First, peek the next packet. Make sure we have something to read.
    packet = (Kit_VideoPacket*)Kit_PeekDecoderOutput(dec);
    if (packet == NULL) {
        return;
    }

    // If packet should not yet be played, stop here and wait.
    // If packet should have already been played, skip it and try to find a better packet.
    // For video, we *try* to return a frame, even if we are out of sync. It is better than
    // not showing anything.
    sync_ts = _GetSystemTime() - dec->clock_sync;
    if (packet->pts > sync_ts + 0.01) {
        return;
    }

    limit_rounds = Kit_GetDecoderOutputLength(dec);
    while (packet != NULL && packet->pts < sync_ts - 0.01 && --limit_rounds) {
        Kit_AdvanceDecoderOutput(dec);
        av_freep(&packet->frame->data[0]);
        av_frame_free(&packet->frame);
        free(packet);
        packet = (Kit_VideoPacket*)Kit_PeekDecoderOutput(dec);
    }
    if (packet == NULL) {
        return;
    }

    // Update output texture with current video data.
    // Note that frame size may change on the fly. Take that into account.
    video_width = packet->frame->width;
    video_height = packet->frame->height;
    switch (dec->output.format) {
    case SDL_PIXELFORMAT_YV12:
    case SDL_PIXELFORMAT_IYUV:
        // Normally YUV output should never happen since we're setting the output format to RGBA in video_open
        memcpy(video_buffer, packet->frame->data[0], packet->frame->linesize[0] * video_height);
        memcpy(video_buffer + packet->frame->linesize[0] * video_height, packet->frame->data[1], packet->frame->linesize[1] * video_height);
        memcpy(video_buffer + packet->frame->linesize[0] * video_height + packet->frame->linesize[1] * video_height, packet->frame->data[2], packet->frame->linesize[2] * video_height);
        break;
    default:
        memcpy(video_buffer, packet->frame->data[0], video_buffer_size);
        break;
    }

    // Advance buffer, and free the decoded frame.
    Kit_AdvanceDecoderOutput(dec);
    dec->clock_pos = packet->pts;
    dec->aspect_ratio = packet->frame->sample_aspect_ratio;
    av_freep(&packet->frame->data[0]);
    av_frame_free(&packet->frame);
    free(packet);
    return;
}

#ifdef __cplusplus
}
#endif
#endif // VIDEO_SUPPORT