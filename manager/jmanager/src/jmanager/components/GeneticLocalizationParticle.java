/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components;

/**
 *
 * @author eperdes
 */
public class GeneticLocalizationParticle {

    private float x;
    private float y;
    private float z;
    private float theta;
    private float tilt;
    private float roll;
    private float prob;

    public GeneticLocalizationParticle(float x, float y, float z, float theta, float tilt, float roll, float prob) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.theta = theta;
        this.tilt = tilt;
        this.roll = roll;
        this.prob = prob;
    }

    public float getTheta() {
        return theta;
    }

    public void setTheta(float theta) {
        this.theta = theta;
    }

    public float getProb() {
        return prob;
    }

    public void setProb(float prob) {
        this.prob = prob;
    }

    public float getRoll() {
        return roll;
    }

    public void setRoll(float roll) {
        this.roll = roll;
    }

    public float getTilt() {
        return tilt;
    }

    public void setTilt(float tilt) {
        this.tilt = tilt;
    }

    public float getX() {
        return x;
    }

    public void setX(float x) {
        this.x = x;
    }

    public float getY() {
        return y;
    }

    public void setY(float y) {
        this.y = y;
    }

    public float getZ() {
        return z;
    }

    public void setZ(float z) {
        this.z = z;
    }
}
