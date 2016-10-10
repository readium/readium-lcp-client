package org.readium.sdk.lcp;

import com.koushikdutta.async.future.FutureCallback;
import com.koushikdutta.ion.ProgressCallback;

import java.io.File;
import java.util.concurrent.CancellationException;

/**
 * Created by clebeaupin on 15/03/16.
 */
public class NetProviderCallback  implements ProgressCallback, FutureCallback<File> {
    /**
     * Native Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long nativePtr;
    private final long requestPtr;

    // Set to true if the download has started
    private boolean hasStarted = false;

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
        if (e instanceof CancellationException) {
            // Request has been canceled
            this.nativeOnRequestCanceled(this.nativePtr, this.requestPtr);
        } else {
            this.nativeOnRequestEnded(this.nativePtr, this.requestPtr);
        }
    }

    @Override
    public void onProgress(long downloaded, long total) {
        this.checkStart();

        // total is -1, bug with LCP server? (content-disposition attachment / HTTP header)
        if (total <= downloaded) {
            total = downloaded*2;
        }
        float val = (downloaded/(float)total);
        this.nativeOnRequestProgressed(this.nativePtr, this.requestPtr, val);
    }

    private native void nativeOnRequestStarted(long nativePtr, long requestPtr);
    private native void nativeOnRequestProgressed(long nativePtr, long requestPtr,
                                                  float progress);
    private native void nativeOnRequestEnded(long nativePtr, long requestPtr);
    private native void nativeOnRequestCanceled(long nativePtr, long requestPtr);
}
