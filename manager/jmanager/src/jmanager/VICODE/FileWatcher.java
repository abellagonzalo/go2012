/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.VICODE;

import java.io.File;
import java.util.TimerTask;

/**
 *
 * @author paco
 */
public class FileWatcher extends TimerTask {

	private long timeStamp;
	private File file, xmlfile;
	private Builder builder;
	private boolean enabled = true;

	public FileWatcher(File file, File xmlfile, Builder builder) {
		this.file = file;
		this.timeStamp = file.lastModified();
		this.builder = builder;
		this.xmlfile = xmlfile;
	}

	public final void run() {
		long currentTimeStamp = file.lastModified();
		if (this.timeStamp != currentTimeStamp) {
			this.timeStamp = currentTimeStamp;
			if (enabled) {
				builder.fileChanged(xmlfile);
			}
		}
	}

	public void enable() {
		enabled = true;
	}

	public void disabled() {
		enabled = true;
	}
}
