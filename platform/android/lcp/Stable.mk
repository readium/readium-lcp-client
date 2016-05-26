#  Copyright (c) 2016 Readium
# 
#  Redistribution and use in source and binary forms, with or without modification,
#  are permitted provided that the following conditions are met:
# 
#  1. Redistributions of source code must retain the above copyright notice, this
#     list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation and/or
#     other materials provided with the distribution.
#  3. Neither the name of the organization nor the names of its contributors may be
#     used to endorse or promote products derived from this software without specific
#     prior written permission
# 
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
#  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
#  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
#  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
#  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
#  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
#  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
#  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
#  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

LOCAL_PATH := $(call my-dir)
SRC_PATH := $(LOCAL_PATH)/../../../src
LCP_CLIENT_LIB_PATH := $(SRC_PATH)/lcp-client-lib
LCP_CONTENT_FILTER_PATH := $(SRC_PATH)/lcp-content-filter
THIRD_PARTY_PATH := $(SRC_PATH)/third-parties

###########################################################
# cryptopp
include $(CLEAR_VARS)
LOCAL_MODULE := cryptopp

LOCAL_CPPFLAGS := -std=gnu++11 -fpermissive
LOCAL_CXXFLAGS := -std=gnu++11 -fpermissive
LOCAL_CPP_FEATURES += exceptions rtti

LOCAL_C_INCLUDES := $(THIRD_PARTY_PATH)/cryptopp
LOCAL_SRC_FILES := $(wildcard $(THIRD_PARTY_PATH)/cryptopp/*.cpp)

include $(BUILD_STATIC_LIBRARY)

###########################################################
# ZipLib
include $(CLEAR_VARS)
LOCAL_MODULE := ziplib

LOCAL_CFLAGS := -std=gnu11
LOCAL_CPPFLAGS := -std=gnu++11
LOCAL_CXXFLAGS := -std=gnu++11
LOCAL_CPP_FEATURES += exceptions rtti

LOCAL_C_INCLUDES := $(THIRD_PARTY_PATH)/Source/ZipLib
LOCAL_SRC_FILES := \
    $(wildcard $(THIRD_PARTY_PATH)/ziplib/Source/ZipLib/extlibs/zlib/*.c) \
    $(wildcard $(THIRD_PARTY_PATH)/ziplib/Source/ZipLib/extlibs/lzma/unix/*.c) \
    $(wildcard $(THIRD_PARTY_PATH)/ziplib/Source/ZipLib/extlibs/bzip2/*.c) \
    $(wildcard $(THIRD_PARTY_PATH)/ziplib/Source/ZipLib/detail/*.cpp) \
    $(wildcard $(THIRD_PARTY_PATH)/ziplib/Source/ZipLib/*.cpp)

include $(BUILD_STATIC_LIBRARY)

###########################################################
# Epub3
include $(CLEAR_VARS)
LOCAL_MODULE := epub3

LOCAL_SRC_FILES := $(EPUB3_PATH)/libs/$(TARGET_ARCH_ABI)/libepub3.so

include $(PREBUILT_SHARED_LIBRARY)

###########################################################
# Lcp
include $(CLEAR_VARS)
LOCAL_MODULE := lcp

LOCAL_CPPFLAGS := -std=gnu++11
LOCAL_CXXFLAGS := -std=gnu++11 -fpermissive -DFEATURES_READIUM
LOCAL_CPP_FEATURES += exceptions rtti
LOCAL_STATIC_LIBRARIES := cryptopp ziplib
LOCAL_SHARED_LIBRARIES := epub3
LOCAL_LDLIBS := -lz -landroid -llog

LOCAL_C_INCLUDES += \
        $(EPUB3_PATH)/include \
        $(LCP_CLIENT_LIB_PATH) \
        $(LCP_CLIENT_LIB_PATH)/public \
        $(LCP_CONTENT_FILTER_PATH) \
        $(LCP_CONTENT_FILTER_PATH)/public \
        $(THIRD_PARTY_PATH)

LOCAL_SRC_FILES := \
    $(wildcard $(THIRD_PARTY_PATH)/time64/*.c) \
    $(wildcard $(LCP_CLIENT_LIB_PATH)/*.cpp) \
    $(wildcard $(LCP_CONTENT_FILTER_PATH)/*.cpp) \
    $(wildcard $(LOCAL_PATH)/src/main/jni/*.cpp)

include $(BUILD_SHARED_LIBRARY)
