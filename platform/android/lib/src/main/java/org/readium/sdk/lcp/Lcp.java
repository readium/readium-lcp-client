package org.readium.sdk.lcp;

import android.content.Context;

/**
 * Created by clebeaupin on 15/02/16.
 */
public class Lcp {
    private static StorageProvider storageProvider;
    private static NetProvider netProvider;

    public static void initialize(StorageProvider storageProvider, NetProvider netProvider) {
        Lcp.storageProvider = storageProvider;
        Lcp.netProvider = netProvider;
    }

    public static StorageProvider getStorageProvider() {
        return Lcp.storageProvider;
    }

    public static NetProvider getNetProvider() {
        return Lcp.netProvider;
    }
}
