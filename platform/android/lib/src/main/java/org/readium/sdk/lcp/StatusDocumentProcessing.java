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


import android.annotation.TargetApi;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.net.Uri;
import android.os.Build;
import android.util.Log;

import com.koushikdutta.async.future.Future;
import com.koushikdutta.async.future.FutureCallback;
import com.koushikdutta.ion.Ion;
import com.koushikdutta.ion.Response;

import org.apache.commons.io.IOUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.io.StringWriter;
import java.net.URLEncoder;
import java.util.Locale;

public class StatusDocumentProcessing {

    @TargetApi(Build.VERSION_CODES.N)
    public Locale getCurrentLocale() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            return m_context.getResources().getConfiguration().getLocales().get(0);
        } else {
            //noinspection deprecation
            return m_context.getResources().getConfiguration().locale;
        }
    }

    protected abstract class DoneCallback {
        public abstract void Done(boolean done);
    }

    public interface IDeviceIDManager {
        String getDeviceNAME();

        String getDeviceID();

        String checkDeviceID(String key);

        void recordDeviceID(String key);
    }

    private Context m_context;
    private Service mLcpService = null;
    private String mBookPath = null;
    private License mLicense = null;
    private IDeviceIDManager m_deviceIDManager = null;

    public StatusDocumentProcessing(Context context, Service service, String path, License license, IDeviceIDManager deviceIDManager) {
        m_context = context;
        mLcpService = service;
        mBookPath = path;
        mLicense = license;
        m_deviceIDManager = deviceIDManager;
    }

    public interface IListener {
        void onStatusDocumentProcessingComplete();
    }

    public static abstract class Listener implements IListener {
        final private StatusDocumentProcessing m_StatusDocumentProcessing;

        public Listener(StatusDocumentProcessing sdp) {
            m_StatusDocumentProcessing = sdp;
        }

        public void onStatusDocumentProcessingComplete() {
            this.onStatusDocumentProcessingComplete_(m_StatusDocumentProcessing);
        }

        public abstract void onStatusDocumentProcessingComplete_(StatusDocumentProcessing sdp);
    }

    private boolean m_wasCancelled = false;

    public boolean wasCancelled() {
        return m_wasCancelled;
    }

    public void cancel() {
        if (m_wasCancelled) {
            return;
        }
        m_wasCancelled = true;

        // forces re-check of LSD
        // (user cancellation does not mean byassing the initial LSD check,
        // unlike say: network failure)
        mLicense.setStatusDocumentProcessingFlag(false);

        if (m_statusDocumentProcessingListener != null) {
            m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
        }
    }

    private IListener m_statusDocumentProcessingListener = null;

    public void start(IListener statusDocumentProcessingListener) {

        m_statusDocumentProcessingListener = statusDocumentProcessingListener;

        final String url = mLicense.getLink_Status();

// An alternative method to configure Ion requests:
//
//        final AsyncHttpRequestFactory current = Ion.getDefault(context).configure().getAsyncHttpRequestFactory();
//        Ion.getDefault(context).configure().setAsyncHttpRequestFactory(new AsyncHttpRequestFactory() {
//            @Override
//            public AsyncHttpRequest createAsyncHttpRequest(Uri uri, String method, Headers headers) {
//                AsyncHttpRequest ret = current.createAsyncHttpRequest(uri, method, headers);
//                ret.setTimeout(1000);
//                return ret;
//            }
//        });

        Locale currentLocale = getCurrentLocale();
        String langCode = currentLocale.toString().replace('_', '-');
        langCode = langCode + ",en-US;q=0.7,en;q=0.5";

        Future<Response<InputStream>> request = Ion.with(m_context)
                .load("GET", url)
                .setLogging("Readium Ion", Log.VERBOSE)

                //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                .setTimeout(6000)

                // TODO: comment this in production! (this is only for testing a local HTTP server)
                //.setHeader("X-Add-Delay", "2s")

                // LCP / LSD server with message localization
                .setHeader("Accept-Language", langCode)

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

                            if (!m_wasCancelled) {
                                m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
                            }
                            return;
                        }

                        try {

// alternative methods for checking response payload
//                                    int length = 0;
//                                    try {
//                                        String strLength = response.getHeaders().getHeaders().get("Content-Length");
//                                        length = Integer.parseInt(strLength);
//                                    } catch(Exception exc){
//                                        // ignore
//                                    }
//                                    if (length == 0) {
//                                        length = inputStream.available();
//                                        length = 20*1024; // TODO: 20kB max okay?
//                                    }
//
//                                    byte[] buffer = new byte[length];
//                                    int bytesRead = inputStream.read(buffer, 0, length);
//                                    String json = new String(buffer, 0, bytesRead, "UTF-8");
//
//                                    InputStreamReader inputStreamReader = new InputStreamReader(inputStream, "UTF-8");
//                                    BufferedReader bufferedReader = new BufferedReader(inputStreamReader);
//                                    StringBuilder stringBuilder = new StringBuilder();
//
//                                    String line = null;
//                                    while ((line = bufferedReader.readLine()) != null)
//                                    {
//                                        stringBuilder.append(line + "\n");
//                                    }
//
//                                    String json = stringBuilder.toString();


                            StringWriter writer = new StringWriter();
                            IOUtils.copy(inputStream, writer, "UTF-8");
                            String json = writer.toString().trim();

                            boolean okay = parseStatusDocumentJson(json);

                            if (okay) {
                                processStatusDocument();
                            } else {
                                if (!m_wasCancelled) {
                                    m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
                                }
                            }

                        } catch (Exception ex) {
                            ex.printStackTrace();

                            if (!m_wasCancelled) {
                                m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
                            }
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
    }


    final protected class StatusDocumentLink {
        public final String m_rel;
        public final String m_href;
        public final String m_type;
        public final String m_templated;
        public final String m_title;
        public final String m_profile;

        public StatusDocumentLink(String rel, String href, String type, String templated, String title, String profile) {
            m_rel = rel == null ? "" : rel;
            m_href = href == null ? "" : href;
            m_type = type == null ? "" : type;
            m_templated = templated == null ? "" : templated;
            m_title = title == null ? "" : title;
            m_profile = profile == null ? "" : profile;
        }
    }

    private String m_statusDocument_ID = "";
    private String m_statusDocument_STATUS = ""; // ready, active, revoked, returned, cancelled, expired
    private String m_statusDocument_MESSAGE = ""; // localized as per HTTP Accept-Language
    private String m_statusDocument_UPDATED_LICENSE = ""; //ISO 8601 time and date
    private String m_statusDocument_UPDATED_STATUS = ""; //ISO 8601 time and date
    private StatusDocumentLink m_statusDocument_LINK_LICENSE = null; // HTTP GET URL, no-template
    private StatusDocumentLink m_statusDocument_LINK_REGISTER = null;
    private StatusDocumentLink m_statusDocument_LINK_RETURN = null;
    private StatusDocumentLink m_statusDocument_LINK_RENEW = null;
    private String m_statusDocument_POTENTIAL_RIGHTS_END = ""; // ISO 8601 time and date

    private boolean parseStatusDocumentJson(String json) {
        try {
            JSONObject jsonObject_ROOT = new JSONObject(json);
            m_statusDocument_ID = jsonObject_ROOT.getString("id");
            m_statusDocument_STATUS = jsonObject_ROOT.getString("status");
            m_statusDocument_MESSAGE = jsonObject_ROOT.getString("message");

            JSONObject jsonObject_UPDATED = jsonObject_ROOT.getJSONObject("updated");
            m_statusDocument_UPDATED_LICENSE = jsonObject_UPDATED.getString("license");
            m_statusDocument_UPDATED_STATUS = jsonObject_UPDATED.getString("status");

            JSONArray jsonArray_LINKS = jsonObject_ROOT.getJSONArray("links");
            if (jsonArray_LINKS != null) {
                for (int i = 0; i < jsonArray_LINKS.length(); i++) {
                    JSONObject jsonObject_LINK = jsonArray_LINKS.getJSONObject(i);

                    String rel = jsonObject_LINK.getString("rel");
                    String href = jsonObject_LINK.getString("href");
                    String type = jsonObject_LINK.optString("type");
                    String templated = jsonObject_LINK.optString("templated");
                    String title = jsonObject_LINK.optString("title");
                    String profile = jsonObject_LINK.optString("profile");

                    StatusDocumentLink link = new StatusDocumentLink(rel, href, type, templated, title, profile);
                    if (rel.equals("license")) {
                        m_statusDocument_LINK_LICENSE = link;
                    } else if (rel.equals("register")) {
                        m_statusDocument_LINK_REGISTER = link;
                    } else if (rel.equals("return")) {
                        m_statusDocument_LINK_RETURN = link;
                    } else if (rel.equals("renew")) {
                        m_statusDocument_LINK_RENEW = link;
                    } else {
                        boolean breakpoint = true;
                    }
                }
            }

            JSONObject jsonObject_POTENTIAL_RIGHTS = jsonObject_ROOT.optJSONObject("potential_rights");
            if (jsonObject_POTENTIAL_RIGHTS != null) {
                m_statusDocument_POTENTIAL_RIGHTS_END = jsonObject_POTENTIAL_RIGHTS.getString("end");
            }

            JSONArray jsonArray_EVENTS = jsonObject_ROOT.optJSONArray("events");
            if (jsonArray_EVENTS != null) {
                for (int i = 0; i < jsonArray_EVENTS.length(); i++) {
                    JSONObject jsonObject_EVENT = jsonArray_EVENTS.getJSONObject(i);

                    String type = jsonObject_EVENT.getString("type");
                    String name = jsonObject_EVENT.getString("name");
                    String timestamp = jsonObject_EVENT.getString("timestamp"); // ISO 8601 time and date
                    String id = jsonObject_EVENT.getString("id");
                }
            }

            return true;

        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

    private void processStatusDocument() {
        boolean licenseNeedsUpdating = mLicense.isOlderThan(m_statusDocument_UPDATED_LICENSE);
        if (licenseNeedsUpdating) {
            fetchAndInjectUpdatedLicense(new DoneCallback() {
                @Override
                public void Done(boolean done) {

                    if (!m_wasCancelled) {
                        m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
                    }
                }
            });
            return;
        }

        if (m_statusDocument_STATUS.equals("revoked")
                || m_statusDocument_STATUS.equals("returned")
                || m_statusDocument_STATUS.equals("cancelled")
                || m_statusDocument_STATUS.equals("expired")
                ) {
            // Actually, this should never occur, because the LCP license should not even pass validation due to passed end date / expired timestamp

            if (!m_wasCancelled) {
                m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
            }
            return;
        }

        checkLink_REGISTER(new DoneCallback() {
            @Override
            public void Done(boolean done) {

                if (!m_wasCancelled) {
                    m_statusDocumentProcessingListener.onStatusDocumentProcessingComplete();
                }
            }
        });
    }

    private void checkLink_REGISTER(final DoneCallback doneCallback_checkLink_REGISTER) {

        //checked in registerDevice(), and allows processing of renew and return interactions
        //if (m_statusDocument_LINK_REGISTER == null) {
        //    doneCallback_checkLink_REGISTER.Done(false);
        //    return;
        //}

        registerDevice(new DoneCallback() {
            @Override
            public void Done(final boolean done_registerDevice) {
                checkLink_RENEW(new DoneCallback() {
                    @Override
                    public void Done(final boolean done_checkLink_RENEW) {
                        if (done_checkLink_RENEW) {
                            doneCallback_checkLink_REGISTER.Done(done_registerDevice);
                            return;
                        }
                        checkLink_RETURN(new DoneCallback() {
                            @Override
                            public void Done(final boolean done_checkLink_RETURN) {
                                doneCallback_checkLink_REGISTER.Done(done_registerDevice);
                            }
                        });
                    }
                });
            }
        });
    }

    private void registerDevice(final DoneCallback doneCallback_registerDevice) {

        String deviceID = m_deviceIDManager.getDeviceID();
        String deviceNAME = m_deviceIDManager.getDeviceNAME();

        boolean doRegister = false;
        if (m_statusDocument_LINK_REGISTER == null) {
            doRegister = false;
        } else if (m_statusDocument_STATUS.equals("ready")) {
            doRegister = true;
        } else if (m_statusDocument_STATUS.equals("active")) {

            String deviceIDForStatusDoc = m_deviceIDManager.checkDeviceID(m_statusDocument_ID);

            if (deviceIDForStatusDoc == null) {
                doRegister = true;
            } else if (!deviceIDForStatusDoc.equals(deviceID)) { // this should really never happen ... but let's ensure anyway.
                doRegister = true;
            }
        }

        if (!doRegister) {
            doneCallback_registerDevice.Done(false);
            return;
        }


        String url = m_statusDocument_LINK_REGISTER.m_href;
        if (m_statusDocument_LINK_REGISTER.m_templated.equals("true")) {

// URLEncoder.encode() doesn't generate %20 for space character (instead: '+')
// So we use android.net.Uri's appendQueryParameter() instead (see below)
//        try {
//            deviceID = URLEncoder.encode(deviceID, "UTF-8");
//            deviceNAME = URLEncoder.encode(deviceNAME, "UTF-8");
//        } catch (Exception ex) {
//            // noop
//        }
//        url = url.replace("{?id,name}", "?id=" + deviceID + "&name=" + deviceNAME);

            url = url.replace("{?id,name}", ""); // TODO: smarter regexp?
            url = Uri.parse(url).buildUpon()
                    .appendQueryParameter("id", deviceID)
                    .appendQueryParameter("name", deviceNAME)
                    .build().toString();
        }

        Locale currentLocale = getCurrentLocale();
        String langCode = currentLocale.toString().replace('_', '-');
        langCode = langCode + ",en-US;q=0.7,en;q=0.5";

        Future<Response<InputStream>> request = Ion.with(m_context)
                .load("POST", url)
                .setLogging("Readium Ion", Log.VERBOSE)

                //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                .setTimeout(6000)

                // TODO: comment this in production! (this is only for testing a local HTTP server)
                //.setHeader("X-Add-Delay", "2s")

                // LCP / LSD server with message localization
                .setHeader("Accept-Language", langCode)

// QUERY params (templated URI)
//                        .setBodyParameter("id", dID)
//                        .setBodyParameter("name", dNAME)

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

                            doneCallback_registerDevice.Done(false);
                            return;
                        }

                        try {

                            StringWriter writer = new StringWriter();
                            IOUtils.copy(inputStream, writer, "UTF-8");
                            String json = writer.toString().trim();

                            boolean okay = parseStatusDocumentJson(json);

                            if (okay && m_statusDocument_STATUS.equals("active")) {
                                m_deviceIDManager.recordDeviceID(m_statusDocument_ID);
                            }

                            doneCallback_registerDevice.Done(true);

                        } catch (Exception ex) {
                            ex.printStackTrace();
                            doneCallback_registerDevice.Done(false);
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
    }

    private void fetchAndInjectUpdatedLicense(final DoneCallback doneCallback_fetchAndInjectUpdatedLicense) {

        if (m_statusDocument_LINK_LICENSE == null) {
            doneCallback_fetchAndInjectUpdatedLicense.Done(false);
            return;
        }

        final String url = m_statusDocument_LINK_LICENSE.m_href;

        Locale currentLocale = getCurrentLocale();
        String langCode = currentLocale.toString().replace('_', '-');
        langCode = langCode + ",en-US;q=0.7,en;q=0.5";

        Future<Response<InputStream>> request = Ion.with(m_context)
                .load("GET", url)
                .setLogging("Readium Ion", Log.VERBOSE)

                //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                .setTimeout(6000)

                // TODO: comment this in production! (this is only for testing a local HTTP server)
                //.setHeader("X-Add-Delay", "2s")

                // LCP / LSD server with message localization
                .setHeader("Accept-Language", langCode)

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

                            doneCallback_fetchAndInjectUpdatedLicense.Done(false);
                            return;
                        }

                        try {
                            StringWriter writer = new StringWriter();
                            IOUtils.copy(inputStream, writer, "UTF-8");
                            String json = writer.toString().trim();

                            // new LCP license
                            mLcpService.injectLicense(mBookPath, json);

                            // forces re-check of LSD, now with updated LCP timestamp
                            mLicense.setStatusDocumentProcessingFlag(false);

                            doneCallback_fetchAndInjectUpdatedLicense.Done(true);

                        } catch (Exception ex) {
                            ex.printStackTrace();
                            doneCallback_fetchAndInjectUpdatedLicense.Done(false);
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
    }

    // Note that the user interface / user experience implemented for return/renew operations
    // is only for testing / demonstration purposes. In the real world,
    // ebook lending use-cases scenarios would probably not be handled
    // directly from the reading system app, but via an intermediary process.

    private void checkLink_RENEW(final DoneCallback doneCallback_checkLink_RENEW) {

        if (!m_statusDocument_STATUS.equals("active")) {
            doneCallback_checkLink_RENEW.Done(false);
            return;
        }

        if (m_statusDocument_LINK_RENEW == null) {
            doneCallback_checkLink_RENEW.Done(false);
            return;
        }

        showStatusDocumentDialog_RETURN_RENEW("renew", new DoneCallback() {
            @Override
            public void Done(boolean done) {
                if (!done) {
                    doneCallback_checkLink_RENEW.Done(false);
                    return;
                }

                String url = m_statusDocument_LINK_RENEW.m_href;
                if (m_statusDocument_LINK_RENEW.m_templated.equals("true")) {

                    String deviceID = m_deviceIDManager.getDeviceID();
                    String deviceNAME = m_deviceIDManager.getDeviceNAME();

// URLEncoder.encode() doesn't generate %20 for space character (instead: '+')
// So we use android.net.Uri's appendQueryParameter() instead (see below)
//        try {
//            deviceID = URLEncoder.encode(deviceID, "UTF-8");
//            deviceNAME = URLEncoder.encode(deviceNAME, "UTF-8");
//        } catch (Exception ex) {
//            // noop
//        }
//        url = url.replace("{?end,id,name}", "?id=" + deviceID + "&name=" + deviceNAME);

                    url = url.replace("{?end,id,name}", ""); // TODO: smarter regexp?
                    url = Uri.parse(url).buildUpon()
                            .appendQueryParameter("id", deviceID)
                            .appendQueryParameter("name", deviceNAME)
                            .build().toString();
                }

                Locale currentLocale = getCurrentLocale();
                String langCode = currentLocale.toString().replace('_', '-');
                langCode = langCode + ",en-US;q=0.7,en;q=0.5";

                Future<Response<InputStream>> request = Ion.with(m_context)
                        .load("PUT", url)
                        .setLogging("Readium Ion", Log.VERBOSE)

                        //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                        .setTimeout(6000)

                        // TODO: comment this in production! (this is only for testing a local HTTP server)
                        //.setHeader("X-Add-Delay", "2s")

                        // LCP / LSD server with message localization
                        .setHeader("Accept-Language", langCode)

// QUERY params (templated URI)
//                        .setBodyParameter("id", deviceID)
//                        .setBodyParameter("name", deviceNAME)
//.setBodyParameter("end", "") //ISO 8601 date-time

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

                                    doneCallback_checkLink_RENEW.Done(false);
                                    return;
                                }

                                try {
                                    // forces re-check of LSD, now with updated LCP timestamp
                                    mLicense.setStatusDocumentProcessingFlag(false);

                                    doneCallback_checkLink_RENEW.Done(true);

                                } catch (Exception ex) {
                                    ex.printStackTrace();
                                    doneCallback_checkLink_RENEW.Done(false);
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
            }
        });
    }

    private void checkLink_RETURN(final DoneCallback doneCallback_checkLink_RETURN) {

        if (!m_statusDocument_STATUS.equals("active")) {
            doneCallback_checkLink_RETURN.Done(false);
            return;
        }

        if (m_statusDocument_LINK_RETURN == null) {
            doneCallback_checkLink_RETURN.Done(false);
            return;
        }

        showStatusDocumentDialog_RETURN_RENEW("return", new DoneCallback() {
            @Override
            public void Done(boolean done) {
                if (!done) {
                    doneCallback_checkLink_RETURN.Done(false);
                    return;
                }

                String url = m_statusDocument_LINK_RETURN.m_href;
                if (m_statusDocument_LINK_RETURN.m_templated.equals("true")) {

                    String deviceID = m_deviceIDManager.getDeviceID();
                    String deviceNAME = m_deviceIDManager.getDeviceNAME();

// URLEncoder.encode() doesn't generate %20 for space character (instead: '+')
// So we use android.net.Uri's appendQueryParameter() instead (see below)
//        try {
//            deviceID = URLEncoder.encode(deviceID, "UTF-8");
//            deviceNAME = URLEncoder.encode(deviceNAME, "UTF-8");
//        } catch (Exception ex) {
//            // noop
//        }
//        url = url.replace("{?id,name}", "?id=" + deviceID + "&name=" + deviceNAME);

                    url = url.replace("{?id,name}", ""); // TODO: smarter regexp?
                    url = Uri.parse(url).buildUpon()
                            .appendQueryParameter("id", deviceID)
                            .appendQueryParameter("name", deviceNAME)
                            .build().toString();
                }

                Locale currentLocale = getCurrentLocale();
                String langCode = currentLocale.toString().replace('_', '-');
                langCode = langCode + ",en-US;q=0.7,en;q=0.5";

                Future<Response<InputStream>> request = Ion.with(m_context)
                        .load("PUT", url)
                        .setLogging("Readium Ion", Log.VERBOSE)

                        //.setTimeout(AsyncHttpRequest.DEFAULT_TIMEOUT) //30000
                        .setTimeout(6000)

                        // TODO: comment this in production! (this is only for testing a local HTTP server)
                        //.setHeader("X-Add-Delay", "2s")

                        // LCP / LSD server with message localization
                        .setHeader("Accept-Language", langCode)

// QUERY params (templated URI)
//                        .setBodyParameter("id", deviceID)
//                        .setBodyParameter("name", deviceNAME)

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

                                    doneCallback_checkLink_RETURN.Done(false);
                                    return;
                                }

                                try {
                                    // forces re-check of LSD, now with updated LCP timestamp
                                    mLicense.setStatusDocumentProcessingFlag(false);

                                    doneCallback_checkLink_RETURN.Done(true);

                                } catch (Exception ex) {
                                    ex.printStackTrace();
                                    doneCallback_checkLink_RETURN.Done(false);
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
            }
        });
    }

    public AlertDialog showStatusDocumentDialog_RETURN_RENEW(String msgType, final DoneCallback doneCallback_showStatusDocumentDialog_RETURN_RENEW) {

        AlertDialog.Builder alertBuilder = new AlertDialog.Builder(m_context);

        alertBuilder.setTitle("LCP EPUB => LSD [" + msgType + "]?");
        alertBuilder.setMessage("License Status Document [" + msgType + "] LCP EPUB?");

        alertBuilder.setOnCancelListener(
                new DialogInterface.OnCancelListener() {
                    @Override
                    public void onCancel(DialogInterface dialog) {
                        doneCallback_showStatusDocumentDialog_RETURN_RENEW.Done(false);
                    }
                }
        );

        alertBuilder.setOnDismissListener(
                new DialogInterface.OnDismissListener() {
                    @Override
                    public void onDismiss(DialogInterface dialog) {
                    }
                }
        );

        alertBuilder.setPositiveButton("Yes",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.dismiss();

                        doneCallback_showStatusDocumentDialog_RETURN_RENEW.Done(true);
                    }
                }
        );
        alertBuilder.setNegativeButton("No",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int which) {
                        dialog.cancel();
                    }
                }
        );

        alertBuilder.setCancelable(true);
        AlertDialog alert = alertBuilder.create();
        alert.setCanceledOnTouchOutside(true);

        alert.show(); //async!

        return alert;
    }
}
