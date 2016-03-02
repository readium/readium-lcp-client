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

    public static native Service build(String certPath);
}
