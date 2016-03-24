#ifndef LCP_ANDROID_UTIL_H
#define LCP_ANDROID_UTIL_H
#include <jni.h>

namespace lcp {
    void initJvm(JNIEnv * env);
    JNIEnv * getJNIEnv();
}
#endif //LCP_ANDROID_UTIL_H
