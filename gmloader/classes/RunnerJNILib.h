#pragma once
#include "jni.h"
#include "jni_internals.h"

class RunnerJNILib : public Object {
public:
    static Class clazz;
    Class *_getClass() { return &clazz; }

    static void        (*Startup)(JNIEnv *env, jclass clz, String *_apkPath, String *_saveFilesDir, String *_packageName, int _sleepMargin, char useAssetManager);
    static int         (*CreateVersionDSMap)(JNIEnv *env, jclass clz, int sdkint, String *RELEASE, String *MODEL, String *DEVICE, String *MANUFACTURER, String *CPU_ABI, String *CPU_ABI2, String *BOOTLOADER, String *BOARD, String *version, String *region, String *versionName, jboolean physicalKeyboardAvailable);
    static int         (*Process)(JNIEnv *env, jclass clz, int _width, int _height, float _accelX, float _accelY, float _accelZ, int _keypadStatus, int _orientation, float _refreshrate);
    static void        (*TouchEvent)(JNIEnv *env, jclass clz, int _type, int _index, float _x, float _y);
    static void        (*RenderSplash)(JNIEnv *env, jclass clz, String *_apkPath, String *_splashName, int  _screenWidth, int _screenHeight, int _texWidth, int _texHeight,int _pngWidth, int _pngHeight);
    static void        (*Resume)(JNIEnv *env, jclass clz, int _param );
    static void        (*Pause)(JNIEnv *env, jclass clz, int _param );
    static void        (*KeyEvent)(JNIEnv *env, jclass clz, int _type, int _keycode, int _keychar, int _eventSource);
    static void        (*SetKeyValue)(JNIEnv *env, jclass clz, int _type, int _val, String *_valjstring);
    static String *    (*GetAppID)(JNIEnv *env, jclass clz, int _param );
    static String *    (*GetSaveFileName)(JNIEnv *env, jclass clz, String *_fileName );
    static jobjectArray(*ExpandCompressedFile)(JNIEnv *env, jclass clz, String *_destLocalPath, String *_compressedFileName );
    static void        (*iCadeEventDispatch)(JNIEnv *env, jclass clz, int _button, char _down);
    static void        (*registerGamepadConnected)(JNIEnv *env, jclass clz, int _deviceIndex, int _buttonCount, int _axisCount);
    static int         (*initGLFuncs)(JNIEnv *env, jclass clz, int _usingGL2);
    static char        (*canFlip)(JNIEnv *env, jclass clz);
    static void        (*GCMPushResult)(JNIEnv *env, jclass clz, String *_dataString, int _type, char _bSuccess);
    /* ... */

    static jfloatArray GamepadAxesValues(JNIEnv *env, jclass clz, void *ins, jint deviceIndex, jstring test);
    static void MoveTaskToBack(JNIEnv *env, jclass clz);
    static int OsGetInfo(JNIEnv *env, jclass clz);

    static double mGameSpeedControl;
};
