package org.readium.sdk.lcp;


import java.util.Timer;
import java.util.TimerTask;

//#if !DISABLE_LSD
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

    public static abstract class Listener implements IListener {
        final private StatusDocumentProcessing m_StatusDocumentProcessing;
        public Listener(StatusDocumentProcessing sdp) {
            m_StatusDocumentProcessing = sdp;
        }
        public void onStatusDocumentProcessingComplete() {

            // TODO: comment this! Just to simulate lengthy operation (testing cancellable dialog)
            Timer timer = new Timer();
            timer.schedule(new TimerTask() {
                @Override
                public void run() {
                    Listener.this.onStatusDocumentProcessingComplete_(m_StatusDocumentProcessing);
                }
            }, 1000);
        }
        public abstract void onStatusDocumentProcessingComplete_(StatusDocumentProcessing sdp);
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
