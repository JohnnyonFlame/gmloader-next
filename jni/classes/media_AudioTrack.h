#pragma once
#include <SDL2/SDL.h>
#include "jni.h"
#include "jni_internals.h"

#define ENCODING_PCM_16BIT  2
#define ENCODING_PCM_8BIT   3
#define ENCODING_PCM_FLOAT  4
#define WRITE_BLOCKING      0
#define WRITE_NON_BLOCKING  1
#define MODE_STATIC 1
#define MODE_STREAM 0

class AudioTrack : public Object {
public:
    static Class clazz;
    Class *_getClass() { return &clazz; }
    AudioTrack(int streamType, int sampleRateInHz, int channelConfig, int audioFormat, int bufferSizeInBytes, int mode);
    static int getMinBufferSize(JNIEnv *env, jclass clazz, int sampleRateInHz, int channelConfig, int audioFormat);
    static void play(JNIEnv *env, jobject obj, jclass clazz);
    static void stop(JNIEnv *env, jobject obj, jclass clazz);
    static void release(JNIEnv *env, jobject obj, jclass clazz);
    static int write(JNIEnv *env, jobject obj, jclass clazz, jbyteArray audioData, int offsetInBytes, int sizeInBytes);
    static int write(JNIEnv *env, jobject obj, jclass clazz, jbyteArray audioData, int offsetInBytes, int sizeInBytes, int writeMode);

    SDL_AudioSpec desired, obtained;
    SDL_AudioDeviceID deviceId;
    int mode;
};