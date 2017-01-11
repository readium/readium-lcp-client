package org.readium.sdk.lcp;

import com.koushikdutta.async.future.FutureCallback;
import com.koushikdutta.ion.ProgressCallback;

import java.io.File;
import java.util.concurrent.CancellationException;
import java.util.concurrent.TimeoutException;

/**
 * Created by clebeaupin on 15/03/16.
 */
public class NetProviderCallback
//#if !DISABLE_NET_PROVIDER
        implements ProgressCallback, FutureCallback<File>
{
    /**
     * Native Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long nativePtr;
    private final long requestPtr;

    // Set to true if the download has started
    private boolean hasStarted = false;
    private boolean hasEnded = false;

    NetProviderCallback(long nativePtr, long requestPtr) {
        this.nativePtr = nativePtr;
        this.requestPtr = requestPtr;
    }

    private void checkStart() {
        if (!this.hasStarted) {
            this.nativeOnRequestStarted(this.nativePtr, this.requestPtr);
            this.hasStarted = true;
        }
    }

    @Override
    public void onCompleted(Exception e, File result) {
        this.checkStart();
        if (hasEnded) {
            return;
        }
        hasEnded = true;
        if (e instanceof CancellationException) {
            // Request has been canceled
            this.nativeOnRequestCanceled(this.nativePtr, this.requestPtr);
        } else if (e instanceof TimeoutException) {
            // Request timeout
            this.nativeOnRequestCanceled(this.nativePtr, this.requestPtr);
        } else if (e == null && result != null) {
            this.nativeOnRequestEnded(this.nativePtr, this.requestPtr);
        } else {
            // Other errors
            this.nativeOnRequestCanceled(this.nativePtr, this.requestPtr);
        }
    }

    @Override
    public void onProgress(long downloaded, long total) {
        this.checkStart();

        // total is -1 when HTTP content-length header is not set.
        if (total < downloaded) {
            total = downloaded*2;
        }
        float val = (downloaded/(float)total);
        this.nativeOnRequestProgressed(this.nativePtr, this.requestPtr, val);
//
//        if (total == downloaded) {
//            hasEnded = true;
//            this.nativeOnRequestEnded(this.nativePtr, this.requestPtr);
//        }
    }

    private native void nativeOnRequestStarted(long nativePtr, long requestPtr);
    private native void nativeOnRequestProgressed(long nativePtr, long requestPtr,
                                                  float progress);
    private native void nativeOnRequestEnded(long nativePtr, long requestPtr);
    private native void nativeOnRequestCanceled(long nativePtr, long requestPtr);
}
