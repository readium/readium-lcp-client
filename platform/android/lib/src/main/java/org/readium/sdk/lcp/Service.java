package org.readium.sdk.lcp;

import java.io.IOException;
import java.io.InputStream;

public class Service {
    /**
     * Native Container Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long nativePtr;

    private Service(long nativePtr) {
        this.nativePtr = nativePtr;
    }

    public License openLicense(String licenseContent) {
        return this.nativeOpenLicense(this.nativePtr, licenseContent);
    }

    public void injectLicense(String epubPath, String licenseContent) {
        this.nativeInjectLicense(this.nativePtr, epubPath, licenseContent);
    }

    public void injectLicense(String epubPath, License license) {
        this.nativeInjectLicense(this.nativePtr, epubPath, license.getOriginalContent());
    }


    /**
     * Returns the native Container pointer.
     * DO NOT USE FROM JAVA SIDE UNLESS TO PASS TO NATIVE CODE!
     * @return Native Container Pointer
     */
    private long getNativePtr() {
        return this.nativePtr;
    }

    private native License nativeOpenLicense(long nativePtr, String licenseContent);

    private native void nativeInjectLicense(long nativePtr, String epubPath, String licenseContent);
//    private native void nativeInjectLicense(long nativePtr, String epubPath, License license);

}
