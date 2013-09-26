/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.field;

import jmanager.components.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author carlos
 */
public class GLFieldThread extends Thread{

	GLFieldPanel glp = null;
	boolean stop = false;

	GLFieldThread(GLFieldPanel mypp) {
		glp = mypp;
		stop = false;
	}

	@Override
	public void run() {
		stop = false;
        try {
            Thread.sleep(1000);
        } catch (InterruptedException ex) {
            Logger.getLogger(GLFieldThread.class.getName()).log(Level.SEVERE, null, ex);
        }

		while (!stop) {
                        
			glp.refreshAbs();
			try {
				Thread.sleep(500); // 2 Hz.
			} catch (InterruptedException ex) {
				Logger.getLogger(GLFieldThread.class.getName()).log(Level.SEVERE, null, ex);
			}
		}
	}

	public void stopThread() {
		stop = true;
	}
}
