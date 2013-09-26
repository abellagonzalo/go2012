/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.field;

import bica.Shape;
import jmanager.components.*;
import java.util.ArrayList;
import java.util.List;
import java.util.Vector;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;
import jmanager.field.GLFieldEvent;
import Jama.Matrix;
import Jama.EigenvalueDecomposition;

/**
 *
 * @author jmom
 */
public class FieldDrawerEvent extends GLFieldEvent {

    private float escale;
    private float x = -1000.0f, y = 0.0f, theta = 0.0f;
    private float _QEKF = 0.0f;

    Matrix _s = new Matrix(3, 1);
    Matrix _P = new Matrix(3, 3);
    Vector _vobs = new Vector();

    public FieldDrawerEvent() {

        this.escale = (float) 1000.0;
    }

    void setFilter(Matrix s, Matrix P, float QEKF, Vector vobs) {

        
        this._s = s;
        this._P = P;
        this._vobs = vobs;
        this._QEKF = QEKF;

    }

    @Override
    protected void drawOpenGL(GLAutoDrawable drawable) {
        float prob;

        GL gl = drawable.getGL();

        //gl.glPushMatrix();

        gl.glEnable(gl.GL_BLEND);
        gl.glBlendFunc(gl.GL_SRC_ALPHA, gl.GL_ONE_MINUS_SRC_ALPHA);

        gl.glDisable(gl.GL_BLEND);
        gl.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        gl.glPointSize(5.0f);
        gl.glLineWidth(1.0f);
        gl.glPointSize(1.0f);
        //gl.glPopMatrix();


    }

   
}
