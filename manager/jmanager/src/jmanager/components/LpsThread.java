/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author carlos
 */
public class LpsThread extends Thread{

	LpsViewer lps = null;
	boolean stop = false;

	LpsThread(LpsViewer mypp) {
		lps = mypp;
		stop = false;
	}

	@Override
	public void run() {
		stop = false;
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            Logger.getLogger(LpsThread.class.getName()).log(Level.SEVERE, null, ex);
        }

		while (!stop) {
			lps.refreshLps();
			try {
				Thread.sleep(500); // 2 Hz.
			} catch (InterruptedException ex) {
				Logger.getLogger(LpsThread.class.getName()).log(Level.SEVERE, null, ex);
			}
		}
	}

	public void stopThread() {
		stop = true;
	}
}
