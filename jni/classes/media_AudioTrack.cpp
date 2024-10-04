#include <vector>
#include <SDL2/SDL.h>

#include "platform.h"
#include "jni.h"
#include "jni_internals.h"
#include "media_AudioTrack.h"

static int GetSDLFormat(int audioFormat)
{
    switch (audioFormat) {
    case ENCODING_PCM_16BIT: return AUDIO_S16;
    case ENCODING_PCM_8BIT: return AUDIO_U8;
    case ENCODING_PCM_FLOAT: return AUDIO_F32SYS;
    default: return AUDIO_U8;
    }
}

static int GetSDLFormatBytes(int audioFormat)
{
    switch (audioFormat) {
    case ENCODING_PCM_16BIT: return 2;
    case ENCODING_PCM_8BIT: return 1;
    case ENCODING_PCM_FLOAT: return 4;
    default: return 1;
    }
}

AudioTrack::AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat, int bufferSizeInBytes, int mode)
{    
    SDL_zero(desired);
    desired.freq = sampleRateInHz;
    desired.format = GetSDLFormat(audioFormat);
    desired.channels = (channelConfig == 4) ? 1 : 2;
    desired.samples = bufferSizeInBytes / GetSDLFormatBytes(audioFormat);
    desired.callback = NULL;
    
    needed_bytes = bufferSizeInBytes;
    deviceId = SDL_OpenAudioDevice(NULL, 0, &desired, &obtained, 0);
    SDL_PauseAudioDevice(deviceId, 0);
    this->mode = mode;
}

static void AudioClass_ctor1(JNIEnv *env, jobject obj, jclass clazz, int streamType, int sampleRateInHz, int channelConfig, int audioFormat, int bufferSizeInBytes, int mode)
{
    AudioTrack *track = new (obj) AudioTrack(streamType, sampleRateInHz, channelConfig, audioFormat, bufferSizeInBytes, mode);
}

int AudioTrack::getMinBufferSize(JNIEnv *env, jclass clazz, int sampleRateInHz, int channelConfig, int audioFormat)
{
    return 2048 * GetSDLFormatBytes(audioFormat);
}

void AudioTrack::play(JNIEnv *env, jobject obj, jclass clazz)
{
    AudioTrack *track = (AudioTrack*)obj;
    SDL_PauseAudioDevice(track->deviceId, 0);
}

void AudioTrack::stop(JNIEnv *env, jobject obj, jclass clazz)
{
    AudioTrack *track = (AudioTrack*)obj;
    SDL_PauseAudioDevice(track->deviceId, 1);
}

void AudioTrack::release(JNIEnv *env, jobject obj, jclass clazz)
{
    AudioTrack *track = (AudioTrack*)obj;
    SDL_ClearQueuedAudio(track->deviceId);
}

int AudioTrack::write(JNIEnv *env, jobject obj, jclass clazz, jbyteArray audioData, int offsetInBytes, int sizeInBytes)
{
    return AudioTrack::write(env, obj, clazz, audioData, offsetInBytes, sizeInBytes, WRITE_BLOCKING);
}


int AudioTrack::write(JNIEnv *env, jobject obj, jclass clazz, jbyteArray audioData, int offsetInBytes, int sizeInBytes, int writeMode)
{
    Class *clz = (Class*)clazz;
    AudioTrack *track = (AudioTrack*)obj;
    ArrayObject *data = (ArrayObject*)audioData;
    uintptr_t where = (uintptr_t)data->elements + offsetInBytes;

    int ret = SDL_QueueAudio(track->deviceId, (void*)where, sizeInBytes);
    if (writeMode == WRITE_BLOCKING)
        while (SDL_GetQueuedAudioSize(track->deviceId) >= track->needed_bytes * 2);

    if (ret == 0)
        return sizeInBytes;
    else
        return 0;
}

const FieldId mediaAudioTrackClassFields[] = {
    {NULL},
};

const ManagedMethod mediaAudioTrackClassMethods[] = {
    REGISTER_INIT_METHOD(AudioTrack, AudioClass_ctor1, "(IIIIII)V"),
    REGISTER_STATIC_METHOD(AudioTrack, getMinBufferSize, "(III)I"),
    REGISTER_NONVIRTUAL(AudioTrack, play   , "()V"),
    REGISTER_NONVIRTUAL(AudioTrack, stop   , "()V"),
    REGISTER_NONVIRTUAL(AudioTrack, release, "()V"),
    REGISTER_NONVIRTUAL(AudioTrack, write, "([BII)I", int, jbyteArray, int, int),
    REGISTER_NONVIRTUAL(AudioTrack, write, "([BIII)I", int, jbyteArray, int, int, int),
    NULL
};

Class AudioTrack::clazz = {
    .classpath = "android/media/AudioTrack",
    .classname = "AudioClass",
    .managed_methods = mediaAudioTrackClassMethods,
    .fields = mediaAudioTrackClassFields,
    .instance_size = sizeof(AudioTrack)
};

static const int registered = ClassRegistry::register_class(AudioTrack::clazz);
