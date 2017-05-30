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
            this.nativeOnRequestEnded(this.nativePtr, this.requestPtr, result.getAbsolutePath());
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
    private native void nativeOnRequestEnded(long nativePtr, long requestPtr, String path);
    private native void nativeOnRequestCanceled(long nativePtr, long requestPtr);
}
