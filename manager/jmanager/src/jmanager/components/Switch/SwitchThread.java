/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components.Switch;

import java.util.logging.Level;
import java.util.logging.Logger;
import jmanager.components.SwitchViewer;

/**
 *
 * @author caguero
 */
public class SwitchThread extends Thread{

	SwitchViewer switchViewer = null;
	boolean stop = false;

	public SwitchThread(SwitchViewer mypp) {
		switchViewer = mypp;
		stop = false;
	}

	@Override
	public void run() {
		stop = false;
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            Logger.getLogger(SwitchThread.class.getName()).log(Level.SEVERE, null, ex);
        }

		while (!stop) {
			switchViewer.refreshSwitchViewer();
			try {
				Thread.sleep(500); // 2 Hz.
			} catch (InterruptedException ex) {
				Logger.getLogger(SwitchThread.class.getName()).log(Level.SEVERE, null, ex);
			}
		}
	}

	public void stopThread() {
		stop = true;
	}
}
