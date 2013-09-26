/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.field;

import java.awt.Dimension;
import java.awt.event.MouseEvent;
import java.awt.event.MouseWheelEvent;
import javax.media.opengl.GLCanvas;
import javax.media.opengl.GLEventListener;

/**
 *
 * @author eperdes
 */
public class GLField extends GLCanvas {

    private GLFieldEvent myopengl;
    float pressx, pressy;
    float releasex, releasey;
    float clickx, clicky;

    public GLField() {
        super();
        this.setMinimumSize(new Dimension());

        /*Add mouse events*/
        this.addMouseListener(new java.awt.event.MouseAdapter() {

            @Override
            public void mouseClicked(MouseEvent e) {
                HandlerMouseClicked(e);
            }

            @Override
            public void mousePressed(MouseEvent e) {
                HandlerMousePressed(e);
            }

            @Override
            public void mouseReleased(MouseEvent e) {
                HandlerMouseReleased(e);
            }
        });

        this.addMouseWheelListener(new java.awt.event.MouseWheelListener() {
            @Override
            public void mouseWheelMoved(MouseWheelEvent e) {
                HandlerMouseWheelMoved(e);
            }

        });
    }

    @Override
    public void addGLEventListener(GLEventListener listener) {
        super.addGLEventListener(listener);
        this.myopengl = (GLFieldEvent) listener;
    }

    private void HandlerMouseClicked(MouseEvent evt) {

        float cx, cy;
        float[] fv;

        cx = evt.getX();
        cy = evt.getY();

        this.myopengl.setProjection(cx, cy);

        /*Project...*/
        //this.clickx = fv[0];
        //this.clicky = fv[1];
    }

    private void HandlerMousePressed(MouseEvent evt) {
        this.pressx = evt.getX();
        this.pressy = evt.getY();
    }

    private void HandlerMouseReleased(MouseEvent evt) {

        float diffx, diffy;
        float currentx, currenty;
        float jump = 0.01f;

        this.releasex = evt.getX();
        this.releasey = evt.getY();

        if(this.releasex == this.pressx && this.releasey == this.pressx)
            return;

        diffx = this.releasex - this.pressx;
        diffy = this.releasey - this.pressy;
        currentx = diffx*jump;
        currenty = diffy*jump;

        this.myopengl.updateCameraPosition(currenty, currentx, 0.0f);
    }

    private void HandlerMouseWheelMoved(MouseWheelEvent evt) {

        float rotation;
        float jump = 0.1f;

        rotation = evt.getWheelRotation();
        this.myopengl.updateCameraPosition(0.0f, 0.0f, jump*rotation);
    }
}
