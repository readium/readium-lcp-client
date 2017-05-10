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

public class License {
    /**
     * Native Container Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long servicePtr;
    private final long nativePtr;

    private License(long nativePtr, long servicePtr) {
        this.nativePtr = nativePtr;
        this.servicePtr = servicePtr;
    }

    public String getOriginalContent() {
        return this.nativeGetOriginalContent(this.nativePtr);
    }

    public String getPassphraseHint() {
        return this.nativeGetPassphraseHint(this.nativePtr);
    }

    public void decrypt(String passphrase) {
        this.nativeDecrypt(this.nativePtr, this.servicePtr, passphrase);
    }

    public boolean isOlderThan(String timestamp) {
        return this.nativeIsOlderThan(this.nativePtr, this.servicePtr, timestamp);
    }

    public boolean isDecrypted() {
        return this.nativeIsDecrypted(this.nativePtr);
    }

    public String getLink_Publication() {
        return this.nativeGetLinkPublication(this.nativePtr);
    }
    public String getLink_Status() {
        return this.nativeGetLinkStatus(this.nativePtr);
    }

    public void setStatusDocumentProcessingFlag(boolean flag)
    {
        this.nativeSetStatusDocumentProcessingFlag(this.nativePtr, flag);
    }

//#if ENABLE_NET_PROVIDER_ACQUISITION
//    public Acquisition createAcquisition(String dstPath) {
//        return this.nativeCreateAcquisition(this.nativePtr, this.servicePtr, dstPath);
//    }


    /**
     * Returns the native Container pointer.
     * DO NOT USE FROM JAVA SIDE UNLESS TO PASS TO NATIVE CODE!
     * @return Native Container Pointer
     */
    private long getNativePtr() {
        return this.nativePtr;
    }

    private native boolean nativeIsDecrypted(long nativePtr);

    private native String nativeGetOriginalContent(long nativePtr);

    private native String nativeGetPassphraseHint(long nativePtr);

    private native String nativeGetLinkPublication(long nativePtr);
    private native String nativeGetLinkStatus(long nativePtr);

    private native void nativeDecrypt(long nativePtr, long servicePtr, String passphrase);

    private native boolean nativeIsOlderThan(long nativePtr, long servicePtr, String timestamp);

    private native void nativeSetStatusDocumentProcessingFlag(long nativePtr, boolean flag);

//#if ENABLE_NET_PROVIDER_ACQUISITION
    //private native Acquisition nativeCreateAcquisition(long nativePtr, long servicePtr, String dstPath);

}
