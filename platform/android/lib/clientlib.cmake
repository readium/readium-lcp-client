# Path definitions
set(CLIENTLIB_DIR ${ROOT_DIR}/src/lcp-client-lib)
set(THIRDPARTY_DIR ${ROOT_DIR}/src/third-parties)

# CryptoPP
file(GLOB_RECURSE CRYPTOPP_SOURCES ${THIRDPARTY_DIR}/cryptopp/*.cpp)
list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX "TestScripts/.*\.cpp")
list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX "bench.*\.cpp")
list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX ".*test\.cpp")
list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX "validat.*\.cpp")
list(FILTER CRYPTOPP_SOURCES EXCLUDE REGEX "adhoc\.cpp")

# ZipLib
file(GLOB ZIPLIB_SOURCES
    ${THIRDPARTY_DIR}/ziplib/Source/ZipLib/extlibs/zlib/*\.c
    ${THIRDPARTY_DIR}/ziplib/Source/ZipLib/extlibs/lzma/unix/*\.c
    ${THIRDPARTY_DIR}/ziplib/Source/ZipLib/extlibs/bzip2/*\.c
    ${THIRDPARTY_DIR}/ziplib/Source/ZipLib/detail/*\.cpp
    ${THIRDPARTY_DIR}/ziplib/Source/ZipLib//*\.cpp
    )

# Time64
file(GLOB_RECURSE TIME64_SOURCES ${THIRDPARTY_DIR}/time64/*.c)


# Client lib
file(GLOB CLIENTLIB_SOURCES ${CLIENTLIB_DIR}/*.cpp)

set(CLIENTLIB_ALL_SOURCES
    ${CRYPTOPP_SOURCES}
    ${ZIPLIB_SOURCES}
    ${TIME64_SOURCES}
    ${CLIENTLIB_SOURCES}
)
