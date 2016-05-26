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
THIRD_PARTY_PATH := $(LOCAL_PATH)/../../../src/third-parties

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
