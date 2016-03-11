package org.readium.sdk.lcp;

import android.content.SharedPreferences;
import 	android.content.Context;

import java.util.Set;

/**
 * Store keys in shared preferences
 */
public class StorageProvider {
    private static final String PREFS_NAME_PREFIX = "org.readium.sdk.lcp.";
    private Context context;

    public StorageProvider(Context context) {
        this.context = context;
    }

    private SharedPreferences getVault(String vaultId) {
        return this.context.getSharedPreferences(PREFS_NAME_PREFIX + vaultId, Context.MODE_PRIVATE);
    }

    public String getValue(String vaultId, String key) {
        SharedPreferences prefs = this.getVault(vaultId);
        return prefs.getString(key, "");
    }

    public void setValue(String vaultId, String key, String value) {
        SharedPreferences prefs = this.getVault(vaultId);
        prefs.edit().putString(key, value).apply();
    }

    /**
     * Return all keys of vault
     * @param vaultId
     * @return
     */
    public String[] getKeys(String vaultId) {
        SharedPreferences prefs = this.getVault(vaultId);
        Set<String> keySet = prefs.getAll().keySet();
        return keySet.toArray(new String[keySet.size()]);
    }
}
