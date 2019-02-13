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

import android.app.Activity;
import android.content.Context;
import android.net.Uri;
import android.os.AsyncTask;
import android.util.Log;

import com.koushikdutta.async.future.Future;
import com.koushikdutta.async.future.FutureCallback;
import com.koushikdutta.async.http.AsyncHttpRequest;
import com.koushikdutta.async.http.Headers;
import com.koushikdutta.ion.Ion;
import com.koushikdutta.ion.Response;
import com.koushikdutta.ion.loader.AsyncHttpRequestFactory;

import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.net.URI;
import java.util.HashMap;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;

public class NetProvider {
//#if !DISABLE_NET_PROVIDER

    private Context context;
    private Activity activity;
    private Map<Long, Future<Response<InputStream>>> requests;

    /**
     * Cancel request asynchronously
     */
    private class CancelRequest extends AsyncTask<Long, Void, Void> {
        @Override
        protected Void doInBackground(Long... requestPtrs) {
            for (Long requestPtr: requestPtrs) {
                Future<Response<InputStream>> request = NetProvider.this.requests.get(requestPtr);
                request.cancel();
            }

            return null;
        }
    }

    public NetProvider(Context context, Activity activity) {
        this.context = context;
        this.activity = activity;

        this.requests = new HashMap<>();
    }

    public void download(final String url, String dstPath, final long requestPtr, long callbackPtr) {

        final NetProviderCallback callback = new NetProviderCallback(callbackPtr, requestPtr);

        if (dstPath == null || dstPath.isEmpty()) {
            File outputDir = this.context.getCacheDir();
            File outputFile = null;
            try {
                outputFile = File.createTempFile("readium_TEMP_download", ".tmp", outputDir);
                dstPath = outputFile.getAbsolutePath();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        final String destPath = dstPath;

//        Timer timer = new Timer();
//        timer.schedule(new TimerTask() {
//            @Override
//            public void run() {
//
//            }
//        }, 500);

//        runOnUiThread(new Runnable() {
//            @Override
//            public void run() {
//
//        new AsyncTask<Void, Void, Void>() {
//            @Override
//            protected Void doInBackground(Void... params) {

//        final AsyncHttpRequestFactory current = Ion.getDefault(context).configure().getAsyncHttpRequestFactory();
//        Ion.getDefault(context).configure().setAsyncHttpRequestFactory(new AsyncHttpRequestFactory() {
//            @Override
//            public AsyncHttpRequest createAsyncHttpRequest(Uri uri, String method, Headers headers) {
//                AsyncHttpRequest ret = current.createAsyncHttpRequest(uri, method, headers);
//                ret.setTimeout(1000);
//                return ret;
//            }
//        });

//        this.activity.runOnUiThread(new Runnable() {
//            @Override
//            public void run() {

                Future<Response<InputStream>> request = Ion.with(NetProvider.this.context)
                        .load("GET", url)
                        .setLogging("Readium Ion", Log.VERBOSE)

                        //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                        .setTimeout(6000)

                        // TODO: comment this in production! (this is only for testing a local HTTP server)
                        //.setHeader("X-Add-Delay", "2s")

                        .progress(callback) // not UI thread
                        //.progressHandler(callback) // UI thread

                        .asInputStream()
                        .withResponse()

                        // UI thread
                        .setCallback(new FutureCallback<Response<InputStream>>() {
                            @Override
                            public void onCompleted(Exception e, Response<InputStream> response) {

                                InputStream inputStream = response != null ? response.getResult() : null;
                                int httpResponseCode = response != null ? response.getHeaders().code() : 0;
                                if (e != null || inputStream == null
                                        || httpResponseCode < 200 || httpResponseCode >= 300) {

                                    callback.onCompleted(e, null);
                                    return;
                                }

                                File destFile = new File(destPath);
                                try {
                                    FileOutputStream outputStream = new FileOutputStream(destFile);
                                    //inputStream.transferTo(outputStream);

                                    byte[] buf = new byte[4096];
                                    int n;
                                    int total = 0;
                                    while ((n = inputStream.read(buf)) > 0) {
                                        total += n;
                                        outputStream.write(buf, 0, n);
                                    }
                                    outputStream.close();
//                                    inputStream.close();
                                    callback.onCompleted(null, destFile);

                                } catch (Exception ex) {
                                    ex.printStackTrace();
                                    callback.onCompleted(ex, null);
                                } finally {
                                    try {
                                        inputStream.close();
                                    } catch (IOException ex) {
                                        ex.printStackTrace();
                                        // ignore
                                    }
                                }
                            }
                        });

                NetProvider.this.requests.put(requestPtr, request);

//// ASYNC TASK
//                return null;

//            }
//        });
    }

    public void cancel(long requestPtr) {
        // Cancel ion request
        if (!this.requests.containsKey(requestPtr)) {
            return;
        }

        new CancelRequest().execute(requestPtr);
    }
}
