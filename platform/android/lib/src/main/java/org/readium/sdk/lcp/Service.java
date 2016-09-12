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

    // public License openLicense(InputStream licenseInputStream) {
    //     // Read the license input stream
    //     String licenseContent = "";

    //     try {
    //         byte[] data = new byte[licenseInputStream.available()];
    //         licenseInputStream.read(data);
    //         licenseInputStream.close();
    //         licenseContent = new String(data, "UTF-8");
    //     } catch (IOException e) {
    //         // Unable to open license
    //         return null;
    //     }

    //     return this.openLicense(licenseContent);
    // }

    // public License openLicense(String licenseContent) {
    //     return this.nativeOpenLicense(this.nativePtr, licenseContent);
    // }

    /**
     * Returns the native Container pointer.
     * DO NOT USE FROM JAVA SIDE UNLESS TO PASS TO NATIVE CODE!
     * @return Native Container Pointer
     */
    private long getNativePtr() {
        return this.nativePtr;
    }

    //private native License nativeOpenLicense(long nativePtr, String licenseContent);
}
