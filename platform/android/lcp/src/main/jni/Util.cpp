//
// Created by clebeaupin on 15/03/16.
//

#include "Util.h"

static JavaVM *jVm = nullptr;

void lcp::initJvm(JNIEnv *env) {
    env->GetJavaVM(&jVm);
}

JNIEnv * lcp::getJNIEnv() {
    // Thread safe function to retrieve JNIEnv
    JNIEnv *env;
    int envStat = jVm->GetEnv((void **) &env, JNI_VERSION_1_6);

    if (envStat == JNI_EDETACHED) {
        jVm->AttachCurrentThread(&env, nullptr);
    } else if (envStat == JNI_OK) {
        //
    } else if (envStat == JNI_EVERSION) {
        // Version not supported
    }

    return env;
}