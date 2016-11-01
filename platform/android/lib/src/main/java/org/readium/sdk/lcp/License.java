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

    public void decrypt(String passphrase) {
        this.nativeDecrypt(this.nativePtr, this.servicePtr, passphrase);
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


//#if ENABLE_NET_PROVIDER
//    public Acquisition createAcquisition(String dstPath) {
//        return this.nativeCreateAcquisition(this.nativePtr, this.servicePtr, dstPath);
//    }

    ////#if !DISABLE_LSD
    public StatusDocumentProcessing createStatusDocumentProcessing(String dstPath) {
        return this.nativeCreateStatusDocumentProcessing(this.nativePtr, this.servicePtr, dstPath);
    }




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

    private native String nativeGetLinkPublication(long nativePtr);
    private native String nativeGetLinkStatus(long nativePtr);

    private native void nativeDecrypt(long nativePtr, long servicePtr, String passphrase);

    //#if ENABLE_NET_PROVIDER
    //private native Acquisition nativeCreateAcquisition(long nativePtr, long servicePtr, String dstPath);

    //#if !DISABLE_LSD
    private native StatusDocumentProcessing nativeCreateStatusDocumentProcessing(long nativePtr, long servicePtr, String dstPath);
}
