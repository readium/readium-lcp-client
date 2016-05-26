// Copyright (c) 2016 Mantano
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



#include <gtest/gtest.h>
#include "public/lcp.h"
#include "TestInfo.h"
#include "AesCbcSymmetricAlgorithm.h"
#include "DecryptionContextImpl.h"

namespace lcptest
{
    class AesCbcRangedDecryptionTest : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            m_key.assign(TestContentKey, TestContentKey + sizeof(TestContentKey) / sizeof(TestContentKey[0]));

            m_file.reset(
                m_fsProvider.GetFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\OPS\\chapter_001.xhtml",
                    lcp::IFileSystemProvider::ReadOnly)
                );

            m_aesCbc.reset(new lcp::AesCbcSymmetricAlgorithm(m_key));
        }
        void TearDown()
        {
            m_aesCbc.reset();
            m_file.reset();
        }

    protected:
        lcp::KeyType m_key;
        std::unique_ptr<lcp::AesCbcSymmetricAlgorithm> m_aesCbc;
        lcp::DefaultFileSystemProvider m_fsProvider;
        std::unique_ptr<lcp::IFile> m_file;
        DecryptionContextImpl m_context;
    };
   

    TEST_F(AesCbcRangedDecryptionTest, DecryptFirst2Bytes)
    {
        m_context.SetDecryptionRange(0, 2);
        std::string decrypted(2, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("<?", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptFirst15Bytes)
    {
        m_context.SetDecryptionRange(0, 15);
        std::string decrypted(15, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("<?xml version=\"", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptFirs16Bytes)
    {
        m_context.SetDecryptionRange(0, 16);
        std::string decrypted(16, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("<?xml version=\"1", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptFirst32Bytes)
    {
        m_context.SetDecryptionRange(0, 32);
        std::string decrypted(32, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("<?xml version=\"1.0\" encoding=\"UT", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptFirst133Bytes)
    {
        m_context.SetDecryptionRange(0, 133);
        std::string decrypted(133, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<html xmlns=\"http://www.w3.org/1999/xhtml\""
            " xmlns:epub=\"http://www.idpf.org/2007/ops\">\n<head>\n<", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptFirstBlock10BytesNotMultipleOfBlock)
    {
        m_context.SetDecryptionRange(5, 10);
        std::string decrypted(10, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ(" version=\"", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptSecondBlock15BytesNotMultipleOfBlock)
    {
        m_context.SetDecryptionRange(17, 15);
        std::string decrypted(15, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("0\" encoding=\"UT", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptThirdBlock17Bytes)
    {
        m_context.SetDecryptionRange(32, 17);
        std::string decrypted(17, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("F-8\"?>\n<html xmln", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptThirdBlock18BytesNotMultipleOfBlock)
    {
        m_context.SetDecryptionRange(34, 18);
        std::string decrypted(18, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("8\"?>\n<html xmlns=\"", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, Decrypt2BytesInTheMiddle)
    {
        m_context.SetDecryptionRange(128, 2);
        std::string decrypted(2, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("ad", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, Decrypt16BytesInTheMiddle)
    {
        m_context.SetDecryptionRange(128, 16);
        std::string decrypted(16, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("ad>\n<title>\nMoby", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, Decrypt32BytesInTheMiddle)
    {
        m_context.SetDecryptionRange(128, 32);
        std::string decrypted(32, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("ad>\n<title>\nMoby-Dick</title><li", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, Decrypt45BytesInTheMiddle)
    {
        m_context.SetDecryptionRange(128, 45);
        std::string decrypted(45, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("ad>\n<title>\nMoby-Dick</title><link rel=\"style", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, Decrypt10BytesInTheMiddleNotMultipleOfBlock)
    {
        m_context.SetDecryptionRange(123, 10);
        std::string decrypted(10, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ(">\n<head>\n<", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, Decrypt50BytesInTheMiddleNotMultipleOfBlock)
    {
        m_context.SetDecryptionRange(123, 50);
        std::string decrypted(50, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ(">\n<head>\n<title>\nMoby-Dick</title><link rel=\"style", decrypted.c_str());
    }
    
    TEST_F(AesCbcRangedDecryptionTest, DecryptLast2Bytes)
    {
        size_t realDataSize = m_aesCbc->PlainTextSize(m_file.get());
        m_context.SetDecryptionRange(realDataSize - 2, 2);
        std::string decrypted(2, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ(">\n", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptLast5BytesAllFromLastBlock)
    {
        size_t realDataSize = m_aesCbc->PlainTextSize(m_file.get());
        m_context.SetDecryptionRange(realDataSize - 5, 5);
        std::string decrypted(5, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("tml>\n", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptLast16Bytes)
    {
        size_t realDataSize = m_aesCbc->PlainTextSize(m_file.get());
        m_context.SetDecryptionRange(realDataSize - 16, 16);
        std::string decrypted(16, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("></body></html>\n", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptLast32Bytes)
    {
        size_t realDataSize = m_aesCbc->PlainTextSize(m_file.get());
        m_context.SetDecryptionRange(realDataSize - 32, 32);
        std::string decrypted(32, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("n></p>\n</section></body></html>\n", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptLast111Bytes)
    {
        size_t realDataSize = m_aesCbc->PlainTextSize(m_file.get());
        m_context.SetDecryptionRange(realDataSize - 111, 111);
        std::string decrypted(111, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );

        ASSERT_STREQ("d most of them all, one grand hooded phantom, like a snow"
            " hill in the air.</span></p>\n</section></body></html>\n", decrypted.c_str());
    }

    TEST_F(AesCbcRangedDecryptionTest, DecryptAllBytesCompareWithOneShotDecryption)
    {
        size_t realDataSize = m_aesCbc->PlainTextSize(m_file.get());
        m_context.SetDecryptionRange(0, realDataSize);
        std::string decrypted(realDataSize, 0);

        m_aesCbc->Decrypt(
            &m_context,
            m_file.get(),
            reinterpret_cast<unsigned char *>(&decrypted.front()),
            decrypted.size()
            );
        
        std::vector<unsigned char> encryptedBuffer(static_cast<size_t>(m_file->Size()));
        m_file->SetReadPosition(0);
        m_file->Read(&encryptedBuffer.at(0), encryptedBuffer.size());
        std::string decryptedBuffer(encryptedBuffer.size(), 0);

        size_t outSize = m_aesCbc->Decrypt(
            &encryptedBuffer.at(0),
            encryptedBuffer.size(),
            reinterpret_cast<unsigned char *>(&decryptedBuffer.at(0)),
            decryptedBuffer.size()
            );
        decryptedBuffer.resize(outSize);

        ASSERT_STREQ(decryptedBuffer.c_str(), decrypted.c_str());
    }

    TEST(AesCbcOneShotDecryptionTest, XhtmlFileDecrypt)
    {
        lcp::KeyType key(TestContentKey, TestContentKey + sizeof(TestContentKey) / sizeof(TestContentKey[0]));
        lcp::AesCbcSymmetricAlgorithm aesCbc(key);

        std::fstream encryptedFile("..\\..\\..\\test\\lcp-client-lib\\data\\moby-dick-20120118.epub\\OPS\\chapter_001.xhtml", std::ios::in | std::ios::binary);
        std::string encryptedFileStr(
            (std::istreambuf_iterator<char>(encryptedFile)),
            std::istreambuf_iterator<char>()
            );

        std::vector<unsigned char> decrypted(encryptedFileStr.size());
        size_t outSize = aesCbc.Decrypt(
            reinterpret_cast<const unsigned char *>(encryptedFileStr.data()),
            encryptedFileStr.size(),
            decrypted.data(),
            decrypted.size()
            );

        decrypted.resize(outSize);
        std::string decryptedStr(decrypted.begin(), decrypted.end());

        std::fstream decryptedFile("..\\..\\..\\test\\lcp-client-lib\\data\\chapter_001_decrypted.xhtml", std::ios::in | std::ios::binary);
        std::string decryptedFileStr(
            (std::istreambuf_iterator<char>(decryptedFile)),
            std::istreambuf_iterator<char>()
            );

        ASSERT_STREQ(decryptedFileStr.c_str(), decryptedStr.c_str());

        decryptedFile.close();
        encryptedFile.close();
    }

    TEST(AesCbcBase64DecryptionTest, UserKeyCheckDecrypt)
    {
        lcp::KeyType key(TestUserKey, TestUserKey + sizeof(TestUserKey) / sizeof(TestUserKey[0]));
        lcp::AesCbcSymmetricAlgorithm aesCbc(key);

        std::string id = aesCbc.Decrypt("XW3MtvGZrWe74uhsrPRVki0eSDTTtM2x+g6YIA8BSObfWEeaLRRkoxGOgRuEcsnT2Fphhs5rB2xW8NI+mnHlIg==");

        ASSERT_STREQ("df09ac25-a386-4c5c-b167-33ce4c36ca65", id.c_str());
    }
}