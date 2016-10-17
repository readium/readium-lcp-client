package org.readium.sdk.lcp;

import android.content.Context;
import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;

import com.koushikdutta.async.future.Future;
import com.koushikdutta.async.future.FutureCallback;
import com.koushikdutta.async.http.AsyncHttpRequest;
import com.koushikdutta.async.http.Headers;
import com.koushikdutta.ion.Ion;
import com.koushikdutta.ion.loader.AsyncHttpRequestFactory;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URI;
import java.util.HashMap;
import java.util.Map;

public class NetProvider {
    private Context context;
    private Map<Long, Future<InputStream>> requests;

    /**
     * Cancel request asynchronously
     */
    private class CancelRequest extends AsyncTask<Long, Void, Void> {
        @Override
        protected Void doInBackground(Long... requestPtrs) {
            for (Long requestPtr: requestPtrs) {
                Future<InputStream> request = NetProvider.this.requests.get(requestPtr);
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

        final NetProviderCallback callback = new NetProviderCallback(callbackPtr, requestPtr);
        final String destPath = dstPath;

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
//        final AsyncHttpRequestFactory current = Ion.getDefault(context).configure().getAsyncHttpRequestFactory();
//        Ion.getDefault(context).configure().setAsyncHttpRequestFactory(new AsyncHttpRequestFactory() {
//            @Override
//            public AsyncHttpRequest createAsyncHttpRequest(Uri uri, String method, Headers headers) {
//                AsyncHttpRequest ret = current.createAsyncHttpRequest(uri, method, headers);
//                ret.setTimeout(1000);
//                return ret;
//            }
//        });


        Future<InputStream> request = Ion.with(this.context)
                .load(url)
                .setLogging("Ion", Log.VERBOSE)
                .progress(callback) // not UI thread
                //.progressHandler(callback) // UI thread
                //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                .setTimeout(6000)
                //.setHeader(name, value)
                .asInputStream().setCallback(new FutureCallback<InputStream>() {
                    @Override
                    public void onCompleted(Exception e, InputStream inputStream) {
                        if (e != null) {
                            callback.onCompleted(e, null);
                            return;
                        }
                        if (inputStream == null) {
                            callback.onCompleted(null, null);
                            return;
                        }
                        File destFile = new File(destPath);
                        try {
                            FileOutputStream outputStream = new FileOutputStream(destFile);
                            //inputStream.transferTo(outputStream);

                            byte[] buf = new byte[4096];
                            int n;
                            int total = 0;
                            while((n = inputStream.read(buf))>0){
                                total += n;
                                outputStream.write(buf, 0, n);
                            }
                            outputStream.close();
                            inputStream.close();
                            callback.onCompleted(null, destFile);
                        } catch (Exception ex) {
                            ex.printStackTrace();
                            callback.onCompleted(ex, null);
                        }
                    }
                })
//                .write(new File(dstPath))
//                .setCallback(callback)
                ;

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
