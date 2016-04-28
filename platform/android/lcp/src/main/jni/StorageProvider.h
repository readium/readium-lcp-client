//
// Created by clebeaupin on 29/01/16.
//

#ifndef LCP_ANDROID_STORAGE_PROVIDER_H
#define LCP_ANDROID_STORAGE_PROVIDER_H

#include <public/IStorageProvider.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

namespace lcp {
    class KeyChainIterator  : public KvStringsIterator
    {
    public:
        KeyChainIterator(std::vector<std::string> keys, std::vector<std::string> values)
        {
            this->index = 0;
            this->keys = keys;
            this->values = values;
        }

        void First()
        {
            this->index = 0;
        }

        void Next()
        {
            this->index++;
        }

        bool IsDone() const
        {
            return this->index >= this->values.size();
        }

        const std::string & Current() const
        {
            if (IsDone()) {
                throw std::out_of_range("Iterator is out of range");
            }

            return this->values.at(index);
        }

        std::string CurrentKey() const
        {
            if (IsDone()) {
                throw std::out_of_range("Iterator is out of range");
            }

            return this->keys.at(index);
        }

    protected:
        std::vector<std::string> values;
        std::vector<std::string> keys;
        uint index;
    };

    class StorageProvider : public IStorageProvider {
    private:
        jobject jStorageProvider;
        jmethodID jGetValueMethodId;
        jmethodID jSetValueMethodId;
        jmethodID jGetKeysMethodId;
    public:
        StorageProvider(jobject jStorageProvider);
        ~StorageProvider();
        std::string GetValue(const std::string &vaultId, const std::string &key);


        void SetValue(const std::string &vaultId, const std::string &key,
                      const std::string &value);

        KvStringsIterator *EnumerateVault(const std::string &vaultId);
    };
}

JNIEXPORT jstring JNICALL Java_org_readium_sdk_lcp_StorageProvider_getValue(JNIEnv *env,
                                                                            jobject obj,
                                                                            jstring key);


#ifdef __cplusplus
}
#endif

#endif //LCP_ANDROID_STORAGE_PROVIDER_H
