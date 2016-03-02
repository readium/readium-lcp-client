#include "StorageProvider.h"

namespace lcp {
    std::string StorageProvider::GetValue(const std::string &vaultId, const std::string &key) {
        return "";
    }

    void StorageProvider::SetValue(const std::string &vaultId, const std::string &key,
                              const std::string &value) {
        std::string result = "ti";
    }

    KvStringsIterator * StorageProvider::EnumerateVault(const std::string &vaultId) {
        return new KeyChainIterator();
    }
}

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_StorageProvider_getValue(JNIEnv* env, jobject obj, jstring key) {
    /*jstring result = env->NewStringUTF("Hello from JNI !");
    return result;*/
    //jclass cls = env->GetObjectClass(obj);
    jclass cls = env->FindClass("org/readium/sdk/lcp/StorageProvider");
    jmethodID methodId = env->GetStaticMethodID(cls, "getToto", "()Ljava/lang/String;");
    jobject stringObj = env->CallStaticObjectMethod(cls, methodId);
    //jmethodID methodId = env->GetStaticMethodID(cls, "getRealValue", "(Ljava/lang/String;)Ljava/lang/String;");
    const char *c = env->GetStringUTFChars((jstring) stringObj, 0);
    return env->NewStringUTF(c);


    /*lcp::StorageProvider storageProvider;
    return env->NewStringUTF(storageProvider.GetValue(
            lcp::UserKeysVaultId, env->GetStringUTFChars(key, 0)).c_str());*/
}