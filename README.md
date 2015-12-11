# Mantano LCP Client library

Created by Artem Brazhnikov, Mickaël Menu.  
Copyright © 2015 Mantano. All rights reserved.  
Any commercial use is strictly prohibited.


## Overview

Cross-platform (iOS, Android and Windows) C++ decryption library for LCP protected documents.

 * Open and read metadata from a License Document.
 * Securely save User Keys on the device.
 * Handle License rights consumption (eg. Copy *n* characters).
 * Download a publication from a standalone LCPL file.
 * Implementation of a Content Filter compatible with Readium SDK to decrypt resources. With support for random access in media streams.
 
 
The library is a standalone solution and can be used without the Readium SDK. An optional Content Filter is available to add Readium compatiblity.


It is composed of several sub-projects:

* **LcpCryptoLib**, which is the standalone LCP C++ decryption library
* **LcpContentFilter**, an implementation of Readium SDK's Content Filter for LCP protected EPUB.
* **Apple bridge**, an Objective-C bridge for the public API of LcpCryptoLib, to use on iOS and OS X.


## Getting Started

IDE projects are available for major supported platforms under `platform/`.
Once built, you can link the library to your projects. Public headers to use in your application are put in `include/`.


### Building for iOS

The Xcode project for Apple platforms is located in `platform/apple/LCP Client.xcodeproj`. An iOS target is part of this project.

The target will build the library, the Objective-C bridge and Readium's Content Filter. You have to specify a location for the Readium SDK in `LCP Client.xcconfig` > `READIUM_PATH`.


## Library API

This section summarizes the concept and main classes of the library. Please refer to the public headers in `include/` for the documentation of specific API.

### LCP Service

`C++: lcp::ILcpService, Obj-C: LCPService`

The LCP Service is the main gateway to the public API of the library. You must create an instance and initialize it with a LCP Root Certificate. Multiple instances with different configurations can live side-by-side.

The service is used to open a License from its JSON, acquire a new publication, decrypt content, add user keys and handle License rights.


#### Providers

The LCP Service expect a number of Provider interface implementations at the initialization. These interfaces are here to make profit of specific platforms tools. For example, on the iOS target, the implementation of `IStorageProvider` will use the iOS KeyChain to securely store rights consumption and User Keys. Also, the implementation of `INetProvider` will use `NSURLSession` to avoid the dependency to cURL.

The library offers a C++ implementation of all providers, except for the IStorageProvider. If you are not on iOS, you must implement it and make sure that data is secured.

* `IFileSystemProvider` handles IO operations on the file system.
* `INetProvider` handles network requests (downloads).
* `IStorageProvider` handles the *secure* persistence of library's data.
* `ICryptoProvider` handles the decryption of data.


### License

`C++: lcp::ILicense, Obj-C: LCPLicense`

The License object represents a single License Document (LCPL file). It is passed around to almost every other API.

All the metadata contained in the License Document will be available through the License object, thanks to accessors.

#### Encryption

A License instance can be in two states: *encrypted* or *decrypted*. If its User Key was already stored in the device, the LCP Service will decrypt the License automatically on opening. Otherwise, the License will be in the state *encrypted*. Some APIs (eg. for decrypting content) need a *decrypted* License to operate.

#### Shared Instances

The owner of the License instance is the LCP Service that created it. And the same License object will be returned for subsequent calls using the same canonical JSON.

You can take profit of that to have different components of your application using the same License object. For example, the Readium Content Filter will open a License using the shared LCP Service, but will not try to decrypt it using the User Key (which it doesn't know). It is the host app that will promt the user for his Passphrase, if needed, and decrypt the shared License instance.
 

 

## Attributions

Thanks to the following pieces of technology that made this library possible:

* [Crypto++](http://www.cryptopp.com/) - Boost
software license 1.0
* [RapidJSON](https://github.com/miloyip/rapidjson) - MIT license
* [ZipLib](https://bitbucket.org/wbenny/ziplib) - zlib license
* [UTF8-CPP](https://github.com/nemtrif/utfcpp) - MIT license
* time64 by Michael G Schwern - MIT license
* [cURL](http://curl.haxx.se/) - MIT/X derivate license
* [UICKeyChainStore](https://github.com/kishikawakatsumi/UICKeyChainStore) - MIT license
* [Google Test](http://code.google.com/p/googletest/) - BSD 3-clause license
