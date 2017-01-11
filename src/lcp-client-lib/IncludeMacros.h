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

#ifndef IncludeMacros_h
#define IncludeMacros_h

// Used to avoid getting warnings from third-party sources


// Crypto++
#define CRYPTOPP_INCLUDE_START \
    _Pragma("clang diagnostic push"); \
    _Pragma("clang diagnostic ignored \"-Wunused\""); \
    _Pragma("clang diagnostic ignored \"-Wunknown-pragmas\""); \
    _Pragma("clang diagnostic ignored \"-Wshorten-64-to-32\""); \
    _Pragma("clang diagnostic ignored \"-Wconditional-uninitialized\""); \
    _Pragma("clang diagnostic ignored \"-Wdelete-non-virtual-dtor\""); \
    _Pragma("clang diagnostic ignored \"-Wmissing-braces\""); \
    _Pragma("clang diagnostic ignored \"-Wreorder\""); \
    _Pragma("clang diagnostic ignored \"-Wdeprecated-register\""); \
    _Pragma("clang diagnostic ignored \"-Wtautological-constant-out-of-range-compare\""); \
    _Pragma("clang diagnostic ignored \"-Wshift-op-parentheses\""); \
    _Pragma("clang diagnostic ignored \"-Wswitch\""); \
    _Pragma("clang diagnostic ignored \"-Wmultiple-move-vbase\""); \
    _Pragma("clang diagnostic ignored \"-Wtautological-compare\"");
#define CRYPTOPP_INCLUDE_END \
    _Pragma("clang diagnostic pop");


// ZipLib
#define ZIPLIB_INCLUDE_START \
    _Pragma("clang diagnostic push"); \
    _Pragma("clang diagnostic ignored \"-Wtautological-undefined-compare\"");
#define ZIPLIB_INCLUDE_END \
    _Pragma("clang diagnostic pop");


// Readium
#define READIUM_INCLUDE_START \
    _Pragma("clang diagnostic push"); \
    _Pragma("clang diagnostic ignored \"-Winconsistent-missing-override\""); \
    _Pragma("clang diagnostic ignored \"-Wunused-function\"");
#define READIUM_INCLUDE_END \
    _Pragma("clang diagnostic pop");

#endif /* IncludeMacros_h */

