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
public class Go2gposObsThread extends Thread {

   Go2gposObs pp = null;
    boolean stop = false;

    Go2gposObsThread(Go2gposObs mypp) {
        pp = mypp;
        stop = false;
    }

    @Override
    public void run() {
        stop = false;

        while (!stop) {
            try {
                Thread.sleep(50);
            } catch (InterruptedException ex) {
                Logger.getLogger(Go2gposObsThread.class.getName()).log(Level.SEVERE, null, ex);
            }
            pp.getRemoteData();
        }
    }

    public void stopThread() {
        stop = true;
    }
}


