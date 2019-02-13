// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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
