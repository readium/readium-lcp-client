package org.readium.sdk.lcp;

import android.content.SharedPreferences;

/**
 * Created by clebeaupin on 29/01/16.
 */
public class StorageProvider {
    private String vaultId;
    private SharedPreferences storage;

    public StorageProvider(String vaultId, SharedPreferences storage) {
        this.storage = storage;
        this.vaultId = vaultId;
    }

    public String getValue(String key) {
        //s = getSharedPreferences(PREFS_NAME, 0);
        return "tti";
    }

    public void setValue(String key,String value) {

    }

    public static String getToto() {
        return "test Toto";
    }

    public static String getRealValue(String key) {
        return "coucou";
    }
}
