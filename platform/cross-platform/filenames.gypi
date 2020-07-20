{
  'variables': {
    'root_dir': '../../src',
    'third_party_dir': '<(root_dir)/third-parties',
    'lcp_content_filter_dir': '<(root_dir)/lcp-content-filter',
    'lcp_client_lib_dir': '<(root_dir)/lcp-client-lib',
    'cryptopp_dir': '<(third_party_dir)/cryptopp',
    'zip_lib_dir': '<(third_party_dir)/ziplib/Source/ZipLib',
    'zlib_dir': '<(zip_lib_dir)/extlibs/zlib',
    'bzip2_dir': '<(zip_lib_dir)/extlibs/bzip2',
    'time64_dir': '<(third_party_dir)/time64',
    'zlib_sources': [
      '<(zlib_dir)/compress.c',
      '<(zlib_dir)/zutil.c',
      '<(zlib_dir)/trees.c',
      '<(zlib_dir)/inftrees.c',
      '<(zlib_dir)/inffast.c',
      '<(zlib_dir)/crc32.c',
      '<(zlib_dir)/inflate.c',
      '<(zlib_dir)/uncompr.c',
      '<(zlib_dir)/adler32.c',
      '<(zlib_dir)/infback.c',
      '<(zlib_dir)/deflate.c'
    ],
    'bzip2_sources': [
      '<(bzip2_dir)/blocksort.c',
      '<(bzip2_dir)/bzerror.c',
      '<(bzip2_dir)/bzlib.c',
      '<(bzip2_dir)/compress.c',
      '<(bzip2_dir)/crctable.c',
      '<(bzip2_dir)/decompress.c',
      '<(bzip2_dir)/huffman.c',
      '<(bzip2_dir)/randtable.c',
    ],
    'zip_lib_sources': [
      '<(zip_lib_dir)/detail/EndOfCentralDirectoryBlock.cpp',
      '<(zip_lib_dir)/detail/ZipCentralDirectoryFileHeader.cpp',
      '<(zip_lib_dir)/detail/ZipGenericExtraField.cpp',
      '<(zip_lib_dir)/detail/ZipLocalFileHeader.cpp',
      '<(zip_lib_dir)/ZipArchive.cpp',
      '<(zip_lib_dir)/ZipArchiveEntry.cpp',
      '<(zip_lib_dir)/ZipFile.cpp'
    ],
    'time64_sources': [
      '<(time64_dir)/time64.c'
    ],
    'cryptopp_sources': [
      '<(cryptopp_dir)/3way.cpp',
      '<(cryptopp_dir)/adhoc.cpp',
      '<(cryptopp_dir)/adler32.cpp',
      '<(cryptopp_dir)/algebra.cpp',
      '<(cryptopp_dir)/algparam.cpp',
      '<(cryptopp_dir)/arc4.cpp',
      '<(cryptopp_dir)/asn.cpp',
      '<(cryptopp_dir)/authenc.cpp',
      '<(cryptopp_dir)/base32.cpp',
      '<(cryptopp_dir)/base64.cpp',
      '<(cryptopp_dir)/basecode.cpp',
      '<(cryptopp_dir)/bench2.cpp',
      '<(cryptopp_dir)/bench.cpp',
      '<(cryptopp_dir)/bfinit.cpp',
      '<(cryptopp_dir)/blowfish.cpp',
      '<(cryptopp_dir)/blumshub.cpp',
      '<(cryptopp_dir)/camellia.cpp',
      '<(cryptopp_dir)/cast.cpp',
      '<(cryptopp_dir)/casts.cpp',
      '<(cryptopp_dir)/cbcmac.cpp',
      '<(cryptopp_dir)/ccm.cpp',
      '<(cryptopp_dir)/channels.cpp',
      '<(cryptopp_dir)/cmac.cpp',
      '<(cryptopp_dir)/cpu.cpp',
      '<(cryptopp_dir)/crc.cpp',
      '<(cryptopp_dir)/cryptlib_bds.cpp',
      '<(cryptopp_dir)/cryptlib.cpp',
      '<(cryptopp_dir)/datatest.cpp',
      '<(cryptopp_dir)/default.cpp',
      '<(cryptopp_dir)/des.cpp',
      '<(cryptopp_dir)/dessp.cpp',
      '<(cryptopp_dir)/dh2.cpp',
      '<(cryptopp_dir)/dh.cpp',
      '<(cryptopp_dir)/dll.cpp',
      '<(cryptopp_dir)/dlltest.cpp',
      '<(cryptopp_dir)/dsa.cpp',
      '<(cryptopp_dir)/eax.cpp',
      '<(cryptopp_dir)/ec2n.cpp',
      '<(cryptopp_dir)/eccrypto.cpp',
      '<(cryptopp_dir)/ecp.cpp',
      '<(cryptopp_dir)/elgamal.cpp',
      '<(cryptopp_dir)/emsa2.cpp',
      '<(cryptopp_dir)/eprecomp.cpp',
      '<(cryptopp_dir)/esign.cpp',
      '<(cryptopp_dir)/files.cpp',
      '<(cryptopp_dir)/filters.cpp',
      '<(cryptopp_dir)/fips140.cpp',
      '<(cryptopp_dir)/fipsalgt.cpp',
      '<(cryptopp_dir)/fipstest.cpp',
      '<(cryptopp_dir)/gcm.cpp',
      '<(cryptopp_dir)/gf2_32.cpp',
      '<(cryptopp_dir)/gf256.cpp',
      '<(cryptopp_dir)/gf2n.cpp',
      '<(cryptopp_dir)/gfpcrypt.cpp',
      '<(cryptopp_dir)/gost.cpp',
      '<(cryptopp_dir)/gzip.cpp',
      '<(cryptopp_dir)/hex.cpp',
      '<(cryptopp_dir)/hmac.cpp',
      '<(cryptopp_dir)/hrtimer.cpp',
      '<(cryptopp_dir)/ida.cpp',
      '<(cryptopp_dir)/idea.cpp',
      '<(cryptopp_dir)/integer.cpp',
      '<(cryptopp_dir)/iterhash.cpp',
      '<(cryptopp_dir)/luc.cpp',
      '<(cryptopp_dir)/mars.cpp',
      '<(cryptopp_dir)/marss.cpp',
      '<(cryptopp_dir)/md2.cpp',
      '<(cryptopp_dir)/md4.cpp',
      '<(cryptopp_dir)/md5.cpp',
      '<(cryptopp_dir)/misc.cpp',
      '<(cryptopp_dir)/modes.cpp',
      '<(cryptopp_dir)/mqueue.cpp',
      '<(cryptopp_dir)/mqv.cpp',
      '<(cryptopp_dir)/nbtheory.cpp',
      '<(cryptopp_dir)/network.cpp',
      '<(cryptopp_dir)/oaep.cpp',
      '<(cryptopp_dir)/osrng.cpp',
      '<(cryptopp_dir)/panama.cpp',
      '<(cryptopp_dir)/pch.cpp',
      '<(cryptopp_dir)/pkcspad.cpp',
      '<(cryptopp_dir)/polynomi.cpp',
      '<(cryptopp_dir)/pssr.cpp',
      '<(cryptopp_dir)/pubkey.cpp',
      '<(cryptopp_dir)/queue.cpp',
      '<(cryptopp_dir)/rabin.cpp',
      '<(cryptopp_dir)/randpool.cpp',
      '<(cryptopp_dir)/rc2.cpp',
      '<(cryptopp_dir)/rc5.cpp',
      '<(cryptopp_dir)/rc6.cpp',
      '<(cryptopp_dir)/rdtables.cpp',
      '<(cryptopp_dir)/regtest.cpp',
      '<(cryptopp_dir)/rijndael.cpp',
      '<(cryptopp_dir)/ripemd.cpp',
      '<(cryptopp_dir)/rng.cpp',
      '<(cryptopp_dir)/rsa.cpp',
      '<(cryptopp_dir)/rw.cpp',
      '<(cryptopp_dir)/safer.cpp',
      '<(cryptopp_dir)/salsa.cpp',
      '<(cryptopp_dir)/seal.cpp',
      '<(cryptopp_dir)/seed.cpp',
      '<(cryptopp_dir)/serpent.cpp',
      '<(cryptopp_dir)/sha3.cpp',
      '<(cryptopp_dir)/shacal2.cpp',
      '<(cryptopp_dir)/sha.cpp',
      '<(cryptopp_dir)/sharkbox.cpp',
      '<(cryptopp_dir)/shark.cpp',
      '<(cryptopp_dir)/simple.cpp',
      '<(cryptopp_dir)/skipjack.cpp',
      '<(cryptopp_dir)/socketft.cpp',
      '<(cryptopp_dir)/sosemanuk.cpp',
      '<(cryptopp_dir)/square.cpp',
      '<(cryptopp_dir)/squaretb.cpp',
      '<(cryptopp_dir)/strciphr.cpp',
      '<(cryptopp_dir)/tea.cpp',
      '<(cryptopp_dir)/test.cpp',
      '<(cryptopp_dir)/tftables.cpp',
      '<(cryptopp_dir)/tiger.cpp',
      '<(cryptopp_dir)/tigertab.cpp',
      '<(cryptopp_dir)/trdlocal.cpp',
      '<(cryptopp_dir)/ttmac.cpp',
      '<(cryptopp_dir)/twofish.cpp',
      '<(cryptopp_dir)/validat1.cpp',
      '<(cryptopp_dir)/validat2.cpp',
      '<(cryptopp_dir)/validat3.cpp',
      '<(cryptopp_dir)/vmac.cpp',
      '<(cryptopp_dir)/wait.cpp',
      '<(cryptopp_dir)/wake.cpp',
      '<(cryptopp_dir)/whrlpool.cpp',
      '<(cryptopp_dir)/winpipes.cpp',
      '<(cryptopp_dir)/xtr.cpp',
      '<(cryptopp_dir)/xtrcrypt.cpp',
      '<(cryptopp_dir)/zdeflate.cpp',
      '<(cryptopp_dir)/zinflate.cpp',
      '<(cryptopp_dir)/zlib.cpp'
    ],
    'lcp_client_lib_sources': [
      '<(lcp_client_lib_dir)/Acquisition.cpp',
      '<(lcp_client_lib_dir)/AesCbcSymmetricAlgorithm.cpp',
      '<(lcp_client_lib_dir)/AesGcmSymmetricAlgorithm.cpp',
      '<(lcp_client_lib_dir)/AlgorithmNames.cpp',
      '<(lcp_client_lib_dir)/Certificate.cpp',
      '<(lcp_client_lib_dir)/CertificateExtension.cpp',
      '<(lcp_client_lib_dir)/CertificateRevocationList.cpp',
      '<(lcp_client_lib_dir)/CrlDistributionPoints.cpp',
      '<(lcp_client_lib_dir)/CrlUpdater.cpp',
      '<(lcp_client_lib_dir)/CryptoLcpNode.cpp',
      '<(lcp_client_lib_dir)/CryptoppCryptoProvider.cpp',
      '<(lcp_client_lib_dir)/CryptoppUtils.cpp',
      '<(lcp_client_lib_dir)/DateTime.cpp',
      '<(lcp_client_lib_dir)/EcdsaSha256SignatureAlgorithm.cpp',
      '<(lcp_client_lib_dir)/EncryptionProfileNames.cpp',
      '<(lcp_client_lib_dir)/EncryptionProfilesManager.cpp',
      '<(lcp_client_lib_dir)/JsonCanonicalizer.cpp',
      '<(lcp_client_lib_dir)/JsonValueReader.cpp',
      '<(lcp_client_lib_dir)/Lcp1dot0EncryptionProfile.cpp',
      '<(lcp_client_lib_dir)/LcpService.cpp',
      '<(lcp_client_lib_dir)/LcpServiceCreator.cpp',
      '<(lcp_client_lib_dir)/LcpUtils.cpp',
      '<(lcp_client_lib_dir)/LinksLcpNode.cpp',
      '<(lcp_client_lib_dir)/RightsLcpNode.cpp',
      '<(lcp_client_lib_dir)/RightsService.cpp',
      '<(lcp_client_lib_dir)/RootLcpNode.cpp',
      '<(lcp_client_lib_dir)/RsaSha256SignatureAlgorithm.cpp',
      '<(lcp_client_lib_dir)/Sha256HashAlgorithm.cpp',
      '<(lcp_client_lib_dir)/SymmetricAlgorithmEncryptedStream.cpp',
      '<(lcp_client_lib_dir)/ThreadTimer.cpp',
      '<(lcp_client_lib_dir)/UserLcpNode.cpp'
    ],
    'lcp_content_filter_sources': [
      '<(lcp_content_filter_dir)/LcpContentFilter.cpp',
      '<(lcp_content_filter_dir)/LcpContentModule.cpp'
    ]
  }
}
