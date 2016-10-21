package org.readium.sdk.lcp;

import android.content.Context;

/**
 * Created by clebeaupin on 15/02/16.
 */
public class Lcp {
    private static StorageProvider storageProvider;

    //#if ENABLE_NET_PROVIDER
//    private static NetProvider netProvider;

    public static void initialize(StorageProvider storageProvider
                                  //#if ENABLE_NET_PROVIDER
//            , NetProvider netProvider
    ) {
        Lcp.storageProvider = storageProvider;

        //#if ENABLE_NET_PROVIDER
//        Lcp.netProvider = netProvider;
    }

    public static StorageProvider getStorageProvider() {
        return Lcp.storageProvider;
    }

    //#if ENABLE_NET_PROVIDER
//    public static NetProvider getNetProvider() {
//        return Lcp.netProvider;
//    }
}
