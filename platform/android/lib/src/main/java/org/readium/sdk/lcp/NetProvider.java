package org.readium.sdk.lcp;

import android.content.Context;
import android.os.AsyncTask;

import com.koushikdutta.async.future.Future;
import com.koushikdutta.ion.Ion;

import java.io.File;
import java.io.IOException;
import java.util.HashMap;
import java.util.Map;

public class NetProvider {
    private Context context;
    private Map<Long, Future<File>> requests;

    /**
     * Cancel request asynchronously
     */
    private class CancelRequest extends AsyncTask<Long, Void, Void> {
        @Override
        protected Void doInBackground(Long... requestPtrs) {
            for (Long requestPtr: requestPtrs) {
                Future<File> request = NetProvider.this.requests.get(requestPtr);
                request.cancel();
            }

            return null;
        }
    }

    public NetProvider(Context context) {
        this.context = context;
        this.requests = new HashMap<>();
    }

    public void download(String url, String dstPath, long requestPtr, long callbackPtr) {
        // Build ion request
        NetProviderCallback callback = new NetProviderCallback(callbackPtr, requestPtr);

        if (dstPath == null || dstPath.isEmpty()) {
            File outputDir = this.context.getCacheDir();
            File outputFile = null;
            try {
                outputFile = File.createTempFile("readium_lcp_download", ".tmp", outputDir);
                dstPath = outputFile.getAbsolutePath();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        Future<File> request = Ion.with(this.context)
                .load(url)
                .progress(callback) // not UI thread
                //.progressHandler(callback) // UI thread
                //.setTimeout(1000)
                .write(new File(dstPath))
                .setCallback(callback);
        this.requests.put(requestPtr, request);
    }

    public void cancel(long requestPtr) {
        // Cancel ion request
        if (!this.requests.containsKey(requestPtr)) {
            return;
        }

        new CancelRequest().execute(requestPtr);
    }
}
