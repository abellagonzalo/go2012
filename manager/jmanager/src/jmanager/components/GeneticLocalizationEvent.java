package jmanager.components;

import jmanager.field.GLFieldEvent;
import java.util.ArrayList;
import javax.media.opengl.GL;
import javax.media.opengl.GLAutoDrawable;

/**
 * author: Eduardo Perdices
 */
public class GeneticLocalizationEvent extends GLFieldEvent {

    private ArrayList<GeneticLocalizationRace> races;
    private float winx;
    private float winy;
    private float wintheta;
    private boolean hasReal;
    private GeneticLocalizationParticle real;
    private float reliability;
    private float escale;

    public GeneticLocalizationEvent() {
        this.hasReal = false;
        this.real = new GeneticLocalizationParticle(0,0,0,0,0,0,0);
        this.escale = (float) 1000.0;
    }

     /*Obtenemos las particuals a dibujar*/
    public void setRaces(ArrayList<GeneticLocalizationRace> races) {
        this.races = races;
    }

    /*Obtenemos el ganador*/
    public void setWinner(float x, float y, float theta) {
        this.winx = x;
        this.winy = y;
        this.wintheta = theta;
    }

    public void setReal(float x, float y, float theta) {
        this.hasReal = true;
        this.real.setX(x);
        this.real.setY(y);
        this.real.setTheta(theta);
    }

    public void setReliability(float r) {
        this.reliability = r;
    }

    public float getReliability() {
        return this.reliability;
    }

    public float getLocationError() {

        if(this.hasReal) {
            return (float) Math.sqrt(Math.pow(this.winx - this.real.getX(), 2) + Math.pow(this.winy - this.real.getY(), 2));
        }

        return 0;
    }

    public float getLocationErrorAngle() {
        float error1, error2;

        if(this.hasReal) {
            error1 = (float) Math.abs(this.real.getTheta() - this.wintheta);
            if(this.wintheta > 0)
                error2 = (float) Math.abs(this.real.getTheta() - (this.wintheta - 2*Math.PI));
            else
                error2 = (float) Math.abs(this.real.getTheta() - (this.wintheta + 2*Math.PI));
            if(error1 < error2)
                return error1;
            else
                return error2;
        }

        return 0;
    }

    public void showCurrentLocalization() {
        System.out.println("CALCULATED LOCALIZATION");
        System.out.println(this.winx + " " + this.winy + " " + this.wintheta);
        if(this.hasReal) {
            System.out.println("REAL LOCALIZATION");
            System.out.println(this.real.getX() + " " + this.real.getY() + " " + this.real.getTheta());
        }
    }

    @Override
    protected void drawOpenGL(GLAutoDrawable drawable) {

        float prob;

        GL gl = drawable.getGL();

        /*Draw calculated races*/
        if (this.races != null) {
            for (int i = 0; i < this.races.size(); i++) {
                prob = this.races.get(i).getRace().getProb();
                if (prob > 0.9) {
                    gl.glColor3f(1.0f, 0.0f, 0.0f);
                } else if (prob > 0.8) {
                    gl.glColor3f(0.0f, 0.0f, 1.0f);
                } else if (prob > 0.7) {
                    gl.glColor3f(1.0f, 1.0f, 0.0f);
                } else {
                    gl.glColor3f(0.3f, 0.3f, 0.3f);
                }

                /*Debug explorers*/
                if(prob<0.1) {
                    gl.glColor3f(0.85f, 0.85f, 0.85f);
                }

                this.drawArrow(gl, this.races.get(i).getRace());
            }
            gl.glColor3f(1.0f, 0.0f, 0.0f);
            this.drawWinner(gl, this.winx, this.winy, true);
        }
        /*Draw real location*/
        if(this.hasReal) {
            gl.glColor3f(1.0f, 0.0f, 1.0f);
            this.drawArrow(gl, this.real);
            this.drawWinner(gl, this.real.getX(), this.real.getY(), false);
        }
    }

    private void drawArrow(GL gl, GeneticLocalizationParticle race) {
        float ang, dist, dist2, angarrow;
        float z = 10.0f;

        dist = 0.15f;
        dist2 = 0.08f;
        angarrow = (float) (30 * Math.PI / 180f);

        ang = (float) -race.getTheta();
        gl.glBegin(gl.GL_LINES);
        gl.glVertex3f((float) (race.getX() / this.escale), race.getY() / this.escale, z / this.escale);
        gl.glVertex3f((float) (race.getX() / this.escale + dist * -Math.cos(ang)), (float) (race.getY() / this.escale + dist * Math.sin(ang)), z / this.escale);
        gl.glVertex3f((float) (race.getX() / this.escale), race.getY() / this.escale, z / this.escale);
        gl.glVertex3f((float) (race.getX() / this.escale + dist2 * -Math.cos(ang - angarrow)), (float) (race.getY() / this.escale + dist2 * Math.sin(ang - angarrow)), z / this.escale);
        gl.glVertex3f((float) (race.getX() / this.escale), race.getY() / this.escale, z / this.escale);
        gl.glVertex3f((float) (race.getX() / this.escale + dist2 * -Math.cos(ang + angarrow)), (float) (race.getY() / this.escale + dist2 * Math.sin(ang + angarrow)), z / this.escale);
        gl.glEnd();
    }

    private void drawCircle(GL gl, float cx, float cy, float cz, float r, int num_segments) {
        int ii;
        float x, y, theta;

        gl.glColor3f(1.0f, 0.0f, 0.0f);
        gl.glBegin(gl.GL_LINE_LOOP);
        for (ii = 0; ii < num_segments; ii++) {
            theta = 2.0f * 3.1415926f * ((float) ii) / ((float) num_segments);//get the current angle

            x = (float) (r * Math.cos(theta));//calculate the x component
            y = (float) (r * Math.sin(theta));//calculate the y component

            gl.glVertex3f((x + cx), (y + cy), cz); // output vertex

        }
        gl.glEnd();
    }

    private void drawWinner(GL gl, float x, float y, boolean drawrel) {
        float rad;

        this.drawCircle(gl, x / this.escale, y / this.escale, 10.0f / this.escale, 0.15f, 12);

        /*Reliability*/
        if(drawrel) {
            rad = 0.15f + (1.0f - this.reliability)*2;
            this.drawCircle(gl, x / this.escale, y / this.escale, 10.0f / this.escale, rad, 12);
        }
    }
}

