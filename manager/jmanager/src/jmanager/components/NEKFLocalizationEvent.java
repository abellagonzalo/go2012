/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */
package jmanager.components;

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
public class NEKFLocalizationEvent extends GLFieldEvent {

    private float escale;
    private float x = -1000.0f, y = 0.0f, theta = 0.0f;
    private float _QEKF = 0.0f;

    Matrix _s = new Matrix(3, 1);
    Matrix _P = new Matrix(3, 3);
    Vector _vobs = new Vector();

    public NEKFLocalizationEvent() {

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

        DrawStateWithError(gl, _s, _P, _QEKF, 0.0f, 0.0f, 0.7f, 0.5f);
       
        DrawObs(gl, _s, _vobs);
        gl.glDisable(gl.GL_BLEND);
        gl.glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        gl.glPointSize(5.0f);
        gl.glLineWidth(1.0f);
        gl.glPointSize(1.0f);
        //gl.glPopMatrix();


    }

    private void DrawObs(GL gl, Matrix s, Vector vobs) {

        float nx, ny, nz;

        x = (float) s.get(0, 0);
        y = (float) s.get(1, 0);
        theta = (float) s.get(2, 0);

        nx = x / this.escale;
        ny = y / this.escale;
        nz = 500.0f / this.escale;


        gl.glPushMatrix();


        
        gl.glLineWidth(1.0f);
        gl.glBegin(gl.GL_LINES);

        for(int i=0; i<vobs.size(); i++)
        {
            double posxg, posyg;
            ObsDbg obs = (ObsDbg)vobs.elementAt(i);

            if(obs.color == ObsDbg.CYELLOW)
                gl.glColor4f(1.0f, 1.0f, 0.0f, 1.0f);
            if(obs.color == ObsDbg.CBLUE)
                gl.glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
            else
                gl.glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
            
            posxg = nx + ( obs.ox/ this.escale * Math.cos(-theta) - obs.oy/ this.escale * Math.sin(-theta));
            posyg = ny + ( obs.ox/ this.escale * Math.sin(-theta) + obs.oy/ this.escale * Math.cos(-theta));

            gl.glVertex3f(nx, ny, nz);
            gl.glVertex3f((float)posxg, (float) posyg, nz);
            gl.glVertex3f((float)posxg, (float) posyg, nz);
            gl.glVertex3f((float)obs.lx/ this.escale, (float) obs.ly/ this.escale, nz);
        }

        //gl.glVertex3f((float) (nx + dist * 0.75 * Math.cos(ang)), (float) (ny + dist * 0.75 * Math.sin(ang)), nz);
        //gl.glVertex3f((float) (nx + dist * Math.cos(ang-0.5)), (float) (ny + dist * Math.sin(ang-0.5)), nz);
        gl.glEnd();
        gl.glPopMatrix();
    }

    private void DrawStateWithError(GL gl, Matrix s, Matrix P, float r, float g, float b, float alpha, float Z) {


        float nx, ny, nz;
        x = (float) s.get(0, 0);
        y = (float) s.get(1, 0);
        theta = (float) s.get(2, 0);

        nx = x / this.escale;
        ny = y / this.escale;
        nz = Z;

        float ang, dist, dist2, angarrow;

        dist = 0.5f;
        angarrow = (float) (30 * Math.PI / 180f);

        ang = -theta;
        gl.glPushMatrix();
        gl.glColor4f(r, g, b, 1.0f);
        gl.glLineWidth(1.5f);
        gl.glBegin(gl.GL_LINES);

        gl.glVertex3f(nx, ny, nz);
        gl.glVertex3f((float) (nx + dist * Math.cos(ang)), (float) (ny + dist * Math.sin(ang)), nz);

        gl.glVertex3f((float) (nx + dist * Math.cos(ang)), (float) (ny + dist * Math.sin(ang)), nz);
        gl.glVertex3f((float) (nx + dist * 0.75 * Math.cos(ang + 0.3)), (float) (ny + dist * 0.75 * Math.sin(ang + 0.3)), nz);

        gl.glVertex3f((float) (nx + dist * Math.cos(ang)), (float) (ny + dist * Math.sin(ang)), nz);
        gl.glVertex3f((float) (nx + dist * 0.75 * Math.cos(ang - 0.3)), (float) (ny + dist * 0.75 * Math.sin(ang - 0.3)), nz);

        //gl.glVertex3f((float) (nx + dist * 0.75 * Math.cos(ang)), (float) (ny + dist * 0.75 * Math.sin(ang)), nz);
        //gl.glVertex3f((float) (nx + dist * Math.cos(ang-0.5)), (float) (ny + dist * Math.sin(ang-0.5)), nz);
        gl.glEnd();
        gl.glPopMatrix();



        double q, Su, Sv;
        double S2x, S2y, Sxy;


        S2x = P.get(0, 0);
        S2y = P.get(1, 1);
        Sxy = P.get(0, 1);

        if (S2x != S2y) {
            q = 0.5 * Math.atan2(-2.0 * Sxy, S2x - S2y);
        } else {
            q = 0.0;
        }

        Su = Math.sqrt(S2x * Math.pow(Math.sin(q), 2.0) + 2.0 * Sxy * Math.sin(q) * Math.cos(q) + S2y * Math.pow(Math.cos(q), 2.0));
        Sv = Math.sqrt(S2x * Math.pow(Math.cos(q), 2.0) + 2.0 * Sxy * Math.cos(q) * Math.sin(q) + S2y * Math.pow(Math.sin(q), 2.0));

        gl.glPushMatrix();
        gl.glColor4f(r, g, b, alpha);
        gl.glBegin(gl.GL_POLYGON);

        float yradius = (float) (Su / this.escale);
        float xradius = (float) (Sv / this.escale);

        for (double a = -Math.PI; a < Math.PI; a = a + 0.05) {
            //convert degrees into radians
            float px, py, ex, ey;

            ex = (float) (Math.cos(a) * xradius);
            ey = (float) (Math.sin(a) * yradius);

            px = (float) (ex * Math.cos(theta) - ey * Math.sin(theta));
            py = (float) (ex * Math.sin(theta) + ey * Math.cos(theta));

            px = px + nx;
            py = py + ny;

            gl.glVertex3f(px, py, nz);
        }

        gl.glEnd();
        gl.glPopMatrix();

    }
}
