package org.readium.sdk.lcp;

public class StatusDocumentProcessing {
    /**
     * Native Container Pointer.
     * DO NOT USE FROM JAVA SIDE!
     */
    private final long nativePtr;

    public interface IListener {
        void onStatusDocumentProcessingComplete();
        //void onStatusDocumentProcessingComplete_(StatusDocumentProcessing sdp);
    }

    private StatusDocumentProcessing(long nativePtr) {
        this.nativePtr = nativePtr;
    }

    private boolean m_wasCancelled = false;
    public boolean wasCancelled() {
        return m_wasCancelled;
    }

    public void cancel() {
        m_wasCancelled = true;
        this.nativeCancel(this.nativePtr);
    }

    public void start(IListener listener) {
        this.nativeStart(listener, this.nativePtr);
    }

    private native void nativeStart(IListener listener, long nativePtr);
    private native void nativeCancel(long nativePtr);

}
