package org.readium.sdk.lcp;

public class StatusDocumentProcessing {
    /**
     * Native Container Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long nativePtr;

    public interface Listener {
        void onStatusDocumentProcessingComplete();
    }

    private StatusDocumentProcessing(long nativePtr) {
        this.nativePtr = nativePtr;
    }

    public void cancel() {
        this.nativeCancel(this.nativePtr);
    }

    public void start(Listener listener) {
        this.nativeStart(listener, this.nativePtr);
    }

    private native void nativeStart(Listener listener, long nativePtr);
    private native void nativeCancel(long nativePtr);

}
