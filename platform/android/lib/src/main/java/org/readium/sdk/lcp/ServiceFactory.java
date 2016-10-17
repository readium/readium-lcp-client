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
                                NetProvider netProvider,
                                CredentialHandler credentialHandler, StatusDocumentHandler statusDocumentHandler) {
        certContent = certContent.replaceAll("-*BEGIN CERTIFICATE-*", "");
        certContent = certContent.replaceAll("-*END CERTIFICATE-*", "");
        certContent = certContent.replaceAll("[\r\n]*", "");
        return ServiceFactory.nativeBuild(certContent, storageProvider, netProvider, credentialHandler, statusDocumentHandler);
    }

    private native static Service nativeBuild(
            String certContent, StorageProvider storageProvider, NetProvider netProvider,
            CredentialHandler credentialHandler, StatusDocumentHandler statusDocumentHandler);
}
