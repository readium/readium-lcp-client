#include "StorageProvider.h"

namespace lcp {
    StorageProvider::StorageProvider(JNIEnv* env, jobject jStorageProvider) {
        this->jEnv = env;
        this->jStorageProvider = env->NewGlobalRef(jStorageProvider);
        jclass jStorageProviderClass = env->GetObjectClass(this->jStorageProvider);
        this->jGetValueMethodId = env->GetMethodID(jStorageProviderClass, "getValue", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        this->jSetValueMethodId = env->GetMethodID(jStorageProviderClass, "setValue", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        this->jGetKeysMethodId = env->GetMethodID(jStorageProviderClass, "getKeys", "(Ljava/lang/String;)[Ljava/lang/String;");
    }

    StorageProvider::~StorageProvider(){
        this->jEnv->DeleteGlobalRef(this->jStorageProvider);
    }

    std::string StorageProvider::GetValue(const std::string &vaultId, const std::string &key) {
        jstring jVaultId = this->jEnv->NewStringUTF(vaultId.c_str());
        jstring jKey = this->jEnv->NewStringUTF(key.c_str());
        jstring value = (jstring) this->jEnv->CallObjectMethod(this->jStorageProvider, this->jGetValueMethodId, jVaultId, jKey);
        const char *cValue = this->jEnv->GetStringUTFChars(value, 0);
        return std::string(cValue);
    }

    void StorageProvider::SetValue(const std::string &vaultId, const std::string &key,
                                   const std::string &value) {
        jstring jVaultId = this->jEnv->NewStringUTF(vaultId.c_str());
        jstring jKey = this->jEnv->NewStringUTF(key.c_str());
        jstring jValue = this->jEnv->NewStringUTF(value.c_str());
        this->jEnv->CallVoidMethod(this->jStorageProvider, this->jSetValueMethodId, jVaultId, jKey, jValue);
    }

    KvStringsIterator * StorageProvider::EnumerateVault(const std::string &vaultId) {
        jstring jVaultId = this->jEnv->NewStringUTF(vaultId.c_str());
        jobjectArray jKeys = (jobjectArray) this->jEnv->CallObjectMethod(this->jStorageProvider, this->jGetKeysMethodId, jVaultId);
        jsize count = this->jEnv->GetArrayLength(jKeys);
        std::vector<std::string> keys;
        std::vector<std::string> values;

        for (int i=0; i<count; i++) {
            jstring jKey = (jstring) this->jEnv->GetObjectArrayElement(jKeys, i);
            std::string key(this->jEnv->GetStringUTFChars(jKey, 0));
            keys.push_back(key);
            values.push_back(this->GetValue(vaultId, key));
        }

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