/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * GLFieldPanel.java
 *
 * Created on 28-mar-2011, 20:04:37
 */
package jmanager.field;

import com.sun.opengl.util.Animator;
import java.awt.event.WindowEvent;
import java.awt.event.WindowListener;
import java.util.Arrays;
import java.util.Observable;
import java.util.Observer;
import java.util.Vector;
import javax.swing.JOptionPane;
import jmanager.GrDebug.GrPrimitivesAbs;
import jmanager.JmanagerView;
import jmanager.Robot;
import jmanager.groundtruth.GroundtruthController;
import jmanager.groundtruth.GroundtruthPanel;

/**
 *
 * @author paco
 */
public class GLFieldPanel extends javax.swing.JPanel implements WindowListener, Observer {

    private GrPrimitivesAbs grPrimitives = null;
    private FieldDrawerEvent myopengl;
    private Animator animator;
    private GLFieldThread glthread = null;
    private Ice.Communicator ic = null;

    /** Creates new form GLFieldPanel */
    public GLFieldPanel(GrPrimitivesAbs grPrimitives) {

        initComponents();

        this.grPrimitives = grPrimitives;
        myopengl = new FieldDrawerEvent();
        gLField1.addGLEventListener(myopengl);
        animator = new Animator(gLField1);
        grPrimitives.addObserver(this);

        ic = Ice.Util.initialize();
        Vector<Robot> robots = new Vector(Arrays.asList(JmanagerView.root.getConnectionPanel().getRobots()));
        new GroundtruthController(ic, groundtruthPanel1, robots);

    }

    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        gLField1 = new jmanager.field.GLField();
        groundtruthPanel1 = new jmanager.groundtruth.GroundtruthPanel();

        gLField1.setName("gLField1"); // NOI18N

        groundtruthPanel1.setName("groundtruthPanel1"); // NOI18N

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(this);
        this.setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addComponent(groundtruthPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, 503, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(gLField1, javax.swing.GroupLayout.PREFERRED_SIZE, 340, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addContainerGap()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(gLField1, javax.swing.GroupLayout.DEFAULT_SIZE, 523, Short.MAX_VALUE)
                    .addComponent(groundtruthPanel1, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addContainerGap())
        );
    }// </editor-fold>//GEN-END:initComponents
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private jmanager.field.GLField gLField1;
    public jmanager.groundtruth.GroundtruthPanel groundtruthPanel1;
    // End of variables declaration//GEN-END:variables

    public void windowOpened(WindowEvent we) {
       
       glthread = new GLFieldThread(this);
        glthread.start();
        animator.start();
        myopengl.initField();
        System.out.println("Field Inited");
    }

    public void windowClosing(WindowEvent we) {
        glthread.stopThread();
        animator.stop();
        try {
            ic.destroy();
        } catch (Ice.LocalException ex) {
            JOptionPane.showMessageDialog(this,
                        "Error al destruir el Communicator.",
                        "Error",
                        JOptionPane.ERROR_MESSAGE);
        }
    }

    public void windowClosed(WindowEvent we) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void windowIconified(WindowEvent we) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void windowDeiconified(WindowEvent we) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void windowActivated(WindowEvent we) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void windowDeactivated(WindowEvent we) {
        //throw new UnsupportedOperationException("Not supported yet.");
    }

    public void update(Observable arg0, Object arg1) {

        if (arg0 == grPrimitives) {
            myopengl.SetDebugAbs(grPrimitives.getGrPrimitives());

        }
    }

    void refreshAbs() {
        if (JmanagerView.root.getConnection() != null) {
            bica.Shape[] shapeList = JmanagerView.root.getConnection().debugPrx.getGrDebugAbs();
            grPrimitives.setGrPrimitives(shapeList);
        }
    }
}
