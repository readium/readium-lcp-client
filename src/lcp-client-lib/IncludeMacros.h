//
//  Created by Mickaël Menu on 18/12/15.
//  Copyright (c) 2014 Readium Foundation and/or its licensees. All rights reserved.
//
//

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

