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

    public void decrypt(String passphrase) {
        this.nativeDecrypt(this.nativePtr, this.servicePtr, passphrase);
    }

    public boolean isDecrypted() {
        return this.nativeIsDecrypted(this.nativePtr);
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
    private native void nativeDecrypt(long nativePtr, long servicePtr, String passphrase);
}
