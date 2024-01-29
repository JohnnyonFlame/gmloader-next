#pragma once
#include "jni.h"
#include "jni_internals.h"

class RunnerJNILibClass {
public:
    static Class clazz;
    static void        (*Startup)(JNIEnv *env, jobject obj, String *_apkPath, String *_saveFilesDir, String *_packageName, int _sleepMargin, int useAssetManager);
    static int         (*CreateVersionDSMap)(JNIEnv *env, jobject obj, int sdkint, String *RELEASE, String *MODEL, String *DEVICE, String *MANUFACTURER, String *CPU_ABI, String *CPU_ABI2, String *BOOTLOADER, String *BOARD, String *version, String *region, String *versionName, jboolean physicalKeyboardAvailable);
    static int         (*Process)(JNIEnv *env, jobject obj, int _width, int _height, float _accelX, float _accelY, float _accelZ, int _keypadStatus, int _orientation, float _refreshrate);
    static void        (*TouchEvent)(JNIEnv *env, jobject obj, int _type, int _index, float _x, float _y);
    static void        (*RenderSplash)(JNIEnv *env, jobject obj, String *_apkPath, String *_splashName, int  _screenWidth, int _screenHeight, int _texWidth, int _texHeight,int _pngWidth, int _pngHeight);
    static void        (*Resume)(JNIEnv *env, jobject obj, int _param );
    static void        (*Pause)(JNIEnv *env, jobject obj, int _param );
    static void        (*KeyEvent)(JNIEnv *env, jobject obj, int _type, int _keycode, int _keychar, int _eventSource);
    static void        (*SetKeyValue)(JNIEnv *env, jobject obj, int _type, int _val, String *_valjstring);
    static String *    (*GetAppID)(JNIEnv *env, jobject obj, int _param );
    static String *    (*GetSaveFileName)(JNIEnv *env, jobject obj, String *_fileName );
    static jobjectArray(*ExpandCompressedFile)(JNIEnv *env, jobject obj, String *_destLocalPath, String *_compressedFileName );
    static void        (*iCadeEventDispatch)(JNIEnv *env, jobject obj, int _button, char _down);
    static void        (*registerGamepadConnected)(JNIEnv *env, jobject obj, int _deviceIndex, int _buttonCount, int _axisCount);
    static int         (*initGLFuncs)(JNIEnv *env, jobject obj, int _usingGL2);
    static char        (*canFlip)(JNIEnv *env, jobject obj);
    static void        (*GCMPushResult)(JNIEnv *env, jobject obj, String *_dataString, int _type, char _bSuccess);
    /* ... */

    static double mGameSpeedControl;
};
