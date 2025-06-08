#include <SDL2/SDL.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

int video_init(SDL_Window* sdl_window, const char* save_path);
void video_open_internal(const char* path);
void video_close_internal();
bool video_draw_internal(void* buffer);
void video_pause_internal();
void video_resume_internal();
void video_enable_loop_internal(double loop);
void video_seek_to_internal(double time);
void video_set_volume_internal(double volume);
double video_get_volume_internal();
double video_is_looping_internal();
double video_status_internal(); // 0 if file is open, -1 if error during loading, -2 if file closed
double video_get_status_internal(); // 0 if video is stopped, 1 if video is "preparing", 2 if video is playing, 3 if video is paused
double video_get_format_internal(); // 0 for RGBA, 1 for YUV
double video_get_width_internal();
double video_get_height_internal();
double video_get_duration_internal();
double video_get_position_internal();

void video_decode();
void video_process();

#ifdef __cplusplus
}
#endif