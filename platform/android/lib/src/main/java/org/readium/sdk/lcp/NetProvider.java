package org.readium.sdk.lcp;

import android.content.Context;

import com.koushikdutta.ion.Ion;

import java.io.File;

public class NetProvider {
    private Context context;



    public NetProvider(Context context) {
        this.context = context;
    }

    public void download(String url, String dstPath, long requestPtr, long callbackPtr) {
        // Build ion request
        NetProviderCallback callback = new NetProviderCallback(callbackPtr, requestPtr);
        Ion.with(this.context)
                .load(url)
                .progress(callback)
                .write(new File(dstPath))
                .setCallback(callback);
    }
}
