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

#import "LCPLicense.h"
#import "ICrypto.h"
#import "ILicense.h"
#import "ILinks.h"
#import "IRights.h"
#import "IUser.h"

@interface LCPLicense ()
@property (nonatomic) lcp::ILicense *nativeLicense;

@end


@implementation LCPLicense

- (instancetype)init
{
    return [self initWithLicense:nullptr];
}

- (instancetype)initWithLicense:(lcp::ILicense *)nativeLicense
{
    self = [super init];
    if (self) {
        _nativeLicense = nativeLicense;
    }
    
    return self;
}

- (NSString *)identifier
{
    return [NSString stringWithUTF8String:_nativeLicense->Id().c_str()];
}

- (NSString *)linkPublication
{
    std::string pubStr("publication");
    if (!_nativeLicense->Links()->Has(pubStr)) {
        return @"";
    }
    
    lcp::Link link;
    _nativeLicense->Links()->GetLink(pubStr, link);
    
    return [NSString stringWithUTF8String:link.href.c_str()];
}

- (BOOL)isDecrypted
{
    return _nativeLicense->Decrypted();
}

- (NSString *)username
{
    if (_nativeLicense->User() != NULL) { // _nativeLicense->User()->Name() can be std::string.empty(), but not nil
        return [NSString stringWithUTF8String:_nativeLicense->User()->Name().c_str()];
    }
    return @""; // no nil return to be consistent with the above potential missing Name() JSON value (empty std::string)
}

- (NSString *)userHint
{
    return [NSString stringWithUTF8String:_nativeLicense->Crypto()->UserKeyHint().c_str()];
}

- (NSString *)originalJSON
{
    return [NSString stringWithUTF8String:_nativeLicense->OriginalContent().c_str()];
}

- (NSString *)canonicalJSON
{
    return [NSString stringWithUTF8String:_nativeLicense->CanonicalContent().c_str()];
}

- (NSDate *)rightsStart {
    std::string start;
    // _nativeLicense->Rights()->HasRightValue(lcp::StartRight)
    if (_nativeLicense->Rights() != NULL && _nativeLicense->Rights()->GetRightValue(lcp::StartRight, start)) {
        NSString *startDateString = [NSString stringWithUTF8String:start.c_str()];

        NSISO8601DateFormatter *dateFormatter = [[NSISO8601DateFormatter alloc] init];
        NSDate *startDate = [dateFormatter dateFromString:startDateString]; // may be nil
        return startDate;
    }
    return nil;

//    NSData *data = [[self canonicalJSON] dataUsingEncoding:NSUTF8StringEncoding];
//
//    NSError *error;
//    NSDictionary *jsonDictionary = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
//    if (!jsonDictionary) {
//        NSLog(@"Failed to parse license JSON: %@", error);
//        return nil;
//    }
//
//    NSDictionary *rights = [jsonDictionary objectForKey:@"rights"];
//    if (rights == nil) { //  no need to check for (id)[NSNull null] here (because source data is LCP license JSON)
//        return nil;
//    }
//    NSString *startDateString = [rights objectForKey:@"start"];
//    if (startDateString == nil || [startDateString length] == 0) { // early exit
//        return nil;
//    }
}

- (NSDate *)rightsEnd {
    std::string end;
    // _nativeLicense->Rights()->HasRightValue(lcp::StartRight)
    if (_nativeLicense->Rights() != NULL && _nativeLicense->Rights()->GetRightValue(lcp::EndRight, end)) {
        NSString *endDateString = [NSString stringWithUTF8String:end.c_str()];

        NSISO8601DateFormatter *dateFormatter = [[NSISO8601DateFormatter alloc] init];
        NSDate *endDate = [dateFormatter dateFromString:endDateString]; // may be nil
        return endDate;
    }
    return nil;

//    NSData *data = [[self canonicalJSON] dataUsingEncoding:NSUTF8StringEncoding];
//
//    NSError *error;
//    NSDictionary *jsonDictionary = [NSJSONSerialization JSONObjectWithData:data options:0 error:&error];
//    if (!jsonDictionary) {
//        NSLog(@"Failed to parse license JSON: %@", error);
//        return nil;
//    }
//
//    NSDictionary *rights = [jsonDictionary objectForKey:@"rights"];
//    if (rights == nil) { //  no need to check for (id)[NSNull null] here (because source data is LCP license JSON)
//        return nil;
//    }
//    NSString *endDateString = [rights objectForKey:@"end"];
//    if (endDateString == nil || [endDateString length] == 0) { // early exit
//        return nil;
//    }
}

@end
