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


package org.readium.sdk.lcp;

/**
 * Created by clebeaupin on 05/02/16.
 */
public class ServiceFactory {
    /*
	 * Static Native library loader
	 */
    static {
        // Load the lcp native lib
        System.loadLibrary("epub3");
        System.loadLibrary("lcp");
    }

    /**
     * Private constructor to prevent creating objects
     * from this class.
     */
    private ServiceFactory() {}



    public static Service build(String certContent, StorageProvider storageProvider,

//#if !DISABLE_NET_PROVIDER
                                NetProvider netProvider,

                                CredentialHandler credentialHandler,
                                StatusDocumentHandler statusDocumentHandler
    ) {
        certContent = certContent.replaceAll("-*BEGIN CERTIFICATE-*", "");
        certContent = certContent.replaceAll("-*END CERTIFICATE-*", "");
        certContent = certContent.replaceAll("[\r\n]*", "");
        return ServiceFactory.nativeBuild(certContent, storageProvider,

//#if !DISABLE_NET_PROVIDER
                netProvider,

                credentialHandler,
                statusDocumentHandler
        );
    }

    private native static Service nativeBuild(
            String certContent, StorageProvider storageProvider,

//#if !DISABLE_NET_PROVIDER
            NetProvider netProvider,

            CredentialHandler credentialHandler,
            StatusDocumentHandler statusDocumentHandler
    );
}
