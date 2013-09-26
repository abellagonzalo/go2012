
package jmanager.VICODE;

import java.io.File;

/**
 *
 * @author paco
 */
public class XMLFileFilter extends javax.swing.filechooser.FileFilter {
	
    public boolean accept(File file) {
        String filename = file.getName();
        return ( filename.endsWith(".xml") || file.isDirectory() );
    }
    public String getDescription() {
        return "*.xml";
    }
}