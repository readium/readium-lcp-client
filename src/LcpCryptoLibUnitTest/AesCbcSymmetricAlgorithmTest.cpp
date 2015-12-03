#include <gtest/gtest.h>
#include "Public/lcp.h"
#include "AesCbcSymmetricAlgorithm.h"
#include "DecryptionContextImpl.h"

namespace lcptest
{
    const unsigned char contentKey[] =
    { 0xe5, 0x42, 0xdb, 0x50, 0xa5, 0xa8, 0x89, 0x68, 0x4f, 0xa8, 0x14, 0x01, 0xc0, 0x42, 0x80, 0x3a,
      0xd7, 0x4a, 0xf6, 0xfe, 0x3f, 0x27, 0xfb, 0x11, 0x1b, 0x30, 0x0a, 0x44, 0x34, 0xf8, 0xb2, 0x5d };

    class AesCbcRangedDecryptionTest : public ::testing::Test
    {
    protected:
        void SetUp()
        {
            m_key.assign(contentKey, contentKey + sizeof(contentKey) / sizeof(contentKey[0]));

            m_file.reset(
                m_fsProvider.GetFile("..\\..\\..\\src\\testing-data\\moby-dick-20120118.epub\\OPS\\chapter_001.xhtml",
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
}