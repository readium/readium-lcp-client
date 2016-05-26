// Copyright (c) 2016 Readium
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