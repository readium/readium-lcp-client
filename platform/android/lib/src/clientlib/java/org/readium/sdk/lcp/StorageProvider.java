// Licensed to the Readium Foundation under one or more contributor license agreements.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation and/or
//    other materials provided with the distribution.
// 3. Neither the name of the organization nor the names of its contributors may be
//    used to endorse or promote products derived from this software without specific
//    prior written permission
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


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
