package org.readium.sdk.lcp;

public class Service {
    /**
     * Native Container Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long nativePtr;

    private Service(long nativePtr) {
        this.nativePtr = nativePtr;
    }

    public License openLicense(String licenseJson) {
        return this.nativeOpenLicense(this.nativePtr, licenseJson);
    }

    /**
     * Returns the native Container pointer.
     * DO NOT USE FROM JAVA SIDE UNLESS TO PASS TO NATIVE CODE!
     * @return Native Container Pointer
     */
    private long getNativePtr() {
        return this.nativePtr;
    }


    private native License nativeOpenLicense(long nativePtr, String licenseJson);
}
