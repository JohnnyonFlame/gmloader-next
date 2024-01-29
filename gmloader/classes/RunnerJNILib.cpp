#include <stdio.h>
#include <vector>

#include "so_util.h"
#include "jni.h"
#include "jni_internals.h"
#include "RunnerJNILib.h"
#include "libyoyo.h"

#define MANGLED_CLASSPATH "Java_com_yoyogames_runner_RunnerJNILib_"
#define CLASS RunnerJNILibClass

/*
    Native Function Pointers
*/

decltype(RunnerJNILibClass::Startup)                  RunnerJNILibClass::Startup = NULL;
decltype(RunnerJNILibClass::CreateVersionDSMap)       RunnerJNILibClass::CreateVersionDSMap = NULL;
decltype(RunnerJNILibClass::Process)                  RunnerJNILibClass::Process = NULL;
decltype(RunnerJNILibClass::TouchEvent)               RunnerJNILibClass::TouchEvent = NULL;
decltype(RunnerJNILibClass::RenderSplash)             RunnerJNILibClass::RenderSplash = NULL;
decltype(RunnerJNILibClass::Resume)                   RunnerJNILibClass::Resume = NULL;
decltype(RunnerJNILibClass::Pause)                    RunnerJNILibClass::Pause = NULL;
decltype(RunnerJNILibClass::KeyEvent)                 RunnerJNILibClass::KeyEvent = NULL;
decltype(RunnerJNILibClass::SetKeyValue)              RunnerJNILibClass::SetKeyValue = NULL;
decltype(RunnerJNILibClass::GetAppID)                 RunnerJNILibClass::GetAppID = NULL;
decltype(RunnerJNILibClass::GetSaveFileName)          RunnerJNILibClass::GetSaveFileName = NULL;
decltype(RunnerJNILibClass::ExpandCompressedFile)     RunnerJNILibClass::ExpandCompressedFile = NULL;
decltype(RunnerJNILibClass::iCadeEventDispatch)       RunnerJNILibClass::iCadeEventDispatch = NULL;
decltype(RunnerJNILibClass::registerGamepadConnected) RunnerJNILibClass::registerGamepadConnected = NULL;
decltype(RunnerJNILibClass::initGLFuncs)              RunnerJNILibClass::initGLFuncs = NULL;
decltype(RunnerJNILibClass::canFlip)                  RunnerJNILibClass::canFlip = NULL;
decltype(RunnerJNILibClass::GCMPushResult)            RunnerJNILibClass::GCMPushResult = NULL;

#define NATIVE_METHOD(so, sym) {&CLASS::clazz, MANGLED_CLASSPATH #sym, so, (void**)&CLASS::sym}
const NativeMethod RunnerJNILibClassNativeMethods[] = {
    NATIVE_METHOD("libyoyo.so", Startup),
    NATIVE_METHOD("libyoyo.so", CreateVersionDSMap),
    NATIVE_METHOD("libyoyo.so", Process),
    NATIVE_METHOD("libyoyo.so", TouchEvent),
    NATIVE_METHOD("libyoyo.so", RenderSplash),
    NATIVE_METHOD("libyoyo.so", Resume),
    NATIVE_METHOD("libyoyo.so", Pause),
    NATIVE_METHOD("libyoyo.so", KeyEvent),
    NATIVE_METHOD("libyoyo.so", SetKeyValue),
    NATIVE_METHOD("libyoyo.so", GetAppID),
    NATIVE_METHOD("libyoyo.so", GetSaveFileName),
    NATIVE_METHOD("libyoyo.so", ExpandCompressedFile),
    NATIVE_METHOD("libyoyo.so", iCadeEventDispatch),
    NATIVE_METHOD("libyoyo.so", registerGamepadConnected),
    NATIVE_METHOD("libyoyo.so", initGLFuncs),
    NATIVE_METHOD("libyoyo.so", canFlip),
    NATIVE_METHOD("libyoyo.so", GCMPushResult),
    {NULL}
};

/*
    Static Class Members
*/

double RunnerJNILibClass::mGameSpeedControl = 60.0;

const FieldId RunnerJNILibClassFields[] = {
    REGISTER_STATIC_FIELD(RunnerJNILibClass, mGameSpeedControl),
    {NULL},
};

/*
    Managed Class Methods
*/

static jfloatArray GamepadAxesValues(void *ins, jint deviceIndex, jstring test)
{
    return 0;
}

extern int RunnerJNILib_MoveTaskToBackCalled;
static void MoveTaskToBack()
{
    RunnerJNILib_MoveTaskToBackCalled = 1;
    warning("MoveTaskToBack called.\n");
}

static int RunnerJNILib_OsGetInfo()
{
    JNIEnv *env = JNIEXT_GetCurrentEnv();
    String osinfo_arr[] = {
        /* "RELEASE", */       String("v1.0"),
        /* "MODEL", */         String("Homebrew"),
        /* "DEVICE", */        String("Homebrew"),
        /* "MANUFACTURER", */  String("JohnnyonFlame"),
        /* "CPU_ABI", */       String("armeabi-v7a"),
        /* "CPU_ABI2", */      String("arm64-v8a"),
        /* "BOOTLOADER", */    String("U-Boot"),
        /* "BOARD", */         String("You tell me"),
        /* "VERSION", */       String("v1.0"),
        /* "REGION", */        String("Global"),
        /* "VERSION_NAME", */  String("v1.0"),
    };

    int osinfo = RunnerJNILibClass::CreateVersionDSMap(env, NULL, 0x13,
        &osinfo_arr[0], &osinfo_arr[1], &osinfo_arr[2], &osinfo_arr[3], &osinfo_arr[4], &osinfo_arr[5],
        &osinfo_arr[6], &osinfo_arr[7], &osinfo_arr[8], &osinfo_arr[9], &osinfo_arr[10], (jboolean)1);
    warning(" -- Retuning OsInfo %d. --\n", osinfo);
    return osinfo;
}

const ManagedMethod RunnerJNILibClassManagedMethods[] = {
    ManagedMethod::Register<GamepadAxesValues>(RunnerJNILibClass::clazz, "GamepadAxesValues", "(I)[F"),
    ManagedMethod::Register<MoveTaskToBack>(RunnerJNILibClass::clazz, "MoveTaskToBack", "()V"),
    NULL
};

Class RunnerJNILibClass::clazz = {
    .classpath = "com/yoyogames/runner/RunnerJNILib",
    .classname = "RunnerJNILib",
    .managed_methods = RunnerJNILibClassManagedMethods,
    .native_methods = RunnerJNILibClassNativeMethods,
    .fields = RunnerJNILibClassFields,
    .instance_size = sizeof(String)
};

static const int registered = ClassRegistry::register_class(RunnerJNILibClass::clazz);