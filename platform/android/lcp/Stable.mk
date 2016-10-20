# Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
#
#  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
#  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#  Licensed under Gnu Affero General Public License Version 3 (provided, notwithstanding this
#  notice, Readium Foundation reserves the right to license this material under a different
#  separate license, and if you have done so, the terms of that separate license control and
#  the following references to GPL do not apply).
#
#  This program is free software: you can redistribute it and/or modify it under the terms
#  of the GNU Affero General Public License as published by the Free Software Foundation,
#  either version 3 of the License, or (at your option) any later version. You should have
#  received a copy of the GNU Affero General Public License along with this program.  If not,
#  see <http://www.gnu.org/licenses/>.

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

LOCAL_CPPFLAGS := -std=gnu++11 -DDISABLE_LSD_
LOCAL_CXXFLAGS := -std=gnu++11 -fpermissive -DFEATURES_READIUM -DDISABLE_LSD_
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
