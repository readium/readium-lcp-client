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

#import <Foundation/Foundation.h>

#ifdef __cplusplus
namespace lcp {
    class ILicense;
}
#endif

@interface LCPLicense : NSObject

@property (readonly, nonatomic) NSString *identifier;
@property (readonly, nonatomic) NSString *linkPublication;
@property (readonly, nonatomic) BOOL isDecrypted;

@property (readonly, nonatomic) NSString *username;
@property (readonly, nonatomic) NSString *userHint;

- (NSString *)originalJSON;
- (NSString *)canonicalJSON;

- (NSDate *)rightsStart;
- (NSDate *)rightsEnd;

#ifdef __cplusplus
@property (readonly, nonatomic) lcp::ILicense *nativeLicense;
- (instancetype)initWithLicense:(lcp::ILicense *)nativeLicense NS_DESIGNATED_INITIALIZER;
#endif

@end
