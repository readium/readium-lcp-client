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
THIRD_PARTY := $(LOCAL_PATH)/../../../src/third-parties

###########################################################
# cryptopp
include $(CLEAR_VARS)
LOCAL_MODULE := cryptopp

LOCAL_CPPFLAGS := -std=gnu++11 -fpermissive
LOCAL_CXXFLAGS := -std=gnu++11 -fpermissive
LOCAL_CPP_FEATURES += exceptions rtti

LOCAL_C_INCLUDES := $(THIRD_PARTY)/cryptopp
LOCAL_SRC_FILES := $(wildcard $(THIRD_PARTY)/cryptopp/*.cpp)

include $(BUILD_STATIC_LIBRARY)

###########################################################
# ZipLib
include $(CLEAR_VARS)
LOCAL_MODULE := ziplib

LOCAL_CFLAGS := -std=gnu11
LOCAL_CPPFLAGS := -std=gnu++11
LOCAL_CXXFLAGS := -std=gnu++11
LOCAL_CPP_FEATURES += exceptions rtti

LOCAL_C_INCLUDES := $(THIRD_PARTY)/Source/ZipLib
LOCAL_SRC_FILES := \
    $(wildcard $(THIRD_PARTY)/ziplib/Source/ZipLib/extlibs/zlib/*.c) \
    $(wildcard $(THIRD_PARTY)/ziplib/Source/ZipLib/extlibs/lzma/unix/*.c) \
    $(wildcard $(THIRD_PARTY)/ziplib/Source/ZipLib/extlibs/bzip2/*.c) \
    $(wildcard $(THIRD_PARTY)/ziplib/Source/ZipLib/detail/*.cpp) \
    $(wildcard $(THIRD_PARTY)/ziplib/Source/ZipLib/*.cpp)

include $(BUILD_STATIC_LIBRARY)