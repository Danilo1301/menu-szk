
#include "mod/iaml.h"

#include "../config.h"

#include <filesystem>
#include <fstream>
#include <filesystem>
#include <jni.h>
#include <string>

inline bool g_deviceChanged = false;

inline std::string GetDeviceId()
{
    JNIEnv* env = aml->GetJNIEnvironment();
    jobject context = aml->GetAppContextObject();

    if (!env || !context) return {};

    jclass secureClass = env->FindClass("android/provider/Settings$Secure");
    if (!secureClass) return {};

    jmethodID getString =
        env->GetStaticMethodID(secureClass, "getString", "(Landroid/content/ContentResolver;Ljava/lang/String;)Ljava/lang/String;");

    if (!getString)
    {
        env->DeleteLocalRef(secureClass);
        return {};
    }

    jclass contextClass = env->GetObjectClass(context);
    jmethodID getContentResolver = env->GetMethodID(contextClass, "getContentResolver", "()Landroid/content/ContentResolver;");

    if (!getContentResolver)
    {
        env->DeleteLocalRef(contextClass);
        env->DeleteLocalRef(secureClass);
        return {};
    }

    jobject resolver = env->CallObjectMethod(context, getContentResolver);
    jstring key = env->NewStringUTF("android_id");

    jstring value = static_cast<jstring>(env->CallStaticObjectMethod(secureClass, getString, resolver, key));

    std::string deviceId;

    if (value)
    {
        const char* chars = env->GetStringUTFChars(value, nullptr);

        if (chars)
        {
            deviceId = chars;
            env->ReleaseStringUTFChars(value, chars);
        }

        env->DeleteLocalRef(value);
    }

    env->DeleteLocalRef(key);

    if (resolver) env->DeleteLocalRef(resolver);

    env->DeleteLocalRef(contextClass);
    env->DeleteLocalRef(secureClass);

    return deviceId;
}

inline void CheckChangedDevice()
{
    const std::string cacheFolder = GetMenuFolder() + "/cache";
    const std::string deviceFile = cacheFolder + "/device";

    std::filesystem::create_directories(cacheFolder);

    const std::string currentDevice = GetDeviceId();

    if (currentDevice.empty()) return;

    std::ifstream in(deviceFile);

    if (!in.good())
    {
        in.close();

        std::ofstream out(deviceFile);
        out << currentDevice;
        return;
    }

    std::string savedDevice;
    std::getline(in, savedDevice);

    if (savedDevice != currentDevice) g_deviceChanged = true;
}