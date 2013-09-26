package jmanager.components;

import java.util.logging.Level;
import java.util.logging.Logger;

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author paco
 */
public class VideoThread extends Thread {

    PerceptionPanel pp = null;
    DetectCorners dc = null;
    boolean stop = false;

    VideoThread(PerceptionPanel mypp) {
        pp = mypp;
        stop = false;
    }

    VideoThread(DetectCorners mydc) {
        dc = mydc;
        stop = false;
    }

    @Override
    public void run() {
        stop = false;

        while (!stop) {           
            if (pp != null)
                //pp.displayAllImages();
            if (dc != null)
            {
                dc.displayAllImages();
                try {
                    Thread.sleep(3000);
                } catch (InterruptedException ex) {
                    Logger.getLogger(VideoThread.class.getName()).log(Level.SEVERE, null, ex);
                 }
            }

        }
    }

    public void stopThread() {
        stop = true;
    }
}


