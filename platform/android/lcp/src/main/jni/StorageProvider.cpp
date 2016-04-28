#include "StorageProvider.h"
#include "Util.h"

namespace lcp {
    StorageProvider::StorageProvider(jobject jStorageProvider) {
        JNIEnv * env = getJNIEnv();
        this->jStorageProvider = env->NewGlobalRef(jStorageProvider);
        jclass jStorageProviderClass = env->GetObjectClass(this->jStorageProvider);
        this->jGetValueMethodId = env->GetMethodID(jStorageProviderClass, "getValue", "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;");
        this->jSetValueMethodId = env->GetMethodID(jStorageProviderClass, "setValue", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
        this->jGetKeysMethodId = env->GetMethodID(jStorageProviderClass, "getKeys", "(Ljava/lang/String;)[Ljava/lang/String;");
    }

    StorageProvider::~StorageProvider(){
        JNIEnv * env = getJNIEnv();
        env->DeleteGlobalRef(this->jStorageProvider);
    }

    std::string StorageProvider::GetValue(const std::string &vaultId, const std::string &key) {
        JNIEnv * env = getJNIEnv();
        jstring jVaultId = env->NewStringUTF(vaultId.c_str());
        jstring jKey = env->NewStringUTF(key.c_str());
        jstring value = (jstring) env->CallObjectMethod(this->jStorageProvider, this->jGetValueMethodId, jVaultId, jKey);
        const char *cValue = env->GetStringUTFChars(value, 0);
        return std::string(cValue);
    }

    void StorageProvider::SetValue(const std::string &vaultId, const std::string &key,
                                   const std::string &value) {
        JNIEnv * env = getJNIEnv();
        jstring jVaultId = env->NewStringUTF(vaultId.c_str());
        jstring jKey = env->NewStringUTF(key.c_str());
        jstring jValue = env->NewStringUTF(value.c_str());
        env->CallVoidMethod(this->jStorageProvider, this->jSetValueMethodId, jVaultId, jKey, jValue);
    }

    KvStringsIterator * StorageProvider::EnumerateVault(const std::string &vaultId) {
        JNIEnv * env = getJNIEnv();
        jstring jVaultId = env->NewStringUTF(vaultId.c_str());
        jobjectArray jKeys = (jobjectArray) env->CallObjectMethod(this->jStorageProvider, this->jGetKeysMethodId, jVaultId);
        jsize count = env->GetArrayLength(jKeys);
        std::vector<std::string> keys;
        std::vector<std::string> values;

        for (int i=0; i<count; i++) {
            jstring jKey = (jstring) env->GetObjectArrayElement(jKeys, i);
            std::string key(env->GetStringUTFChars(jKey, 0));
            keys.push_back(key);
            values.push_back(this->GetValue(vaultId, key));
        }

        return new KeyChainIterator(keys, values);
    }
}