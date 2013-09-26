package jmanager.field;

import jmanager.field.FieldPoint;

/*
 * Line of the field.
 */


/**
 *
 * @author Eduardo Perdices
 */
public class FieldCircle {

    private FieldPoint p1, p2;
    private float r1, r2;
    private String color;

    public FieldCircle(FieldPoint p1, FieldPoint p2, float r1, float r2, String color) {
        this.p1 = p1;
        this.p2 = p2;
        this.r1 = r1;
        this.r2 = r2;
        this.color = color;
    }

    public String getColor() {
        return color;
    }

    public void setColor(String color) {
        this.color = color;
    }

    public FieldPoint getP1() {
        return p1;
    }

    public void setP1(FieldPoint p1) {
        this.p1 = p1;
    }

    public FieldPoint getP2() {
        return p2;
    }

    public void setP2(FieldPoint p2) {
        this.p2 = p2;
    }

    public float getR1() {
        return r1;
    }

    public void setR1(float r1) {
        this.r1 = r1;
    }

    public float getR2() {
        return r2;
    }

    public void setR2(float r2) {
        this.r2 = r2;
    }

    public String toString() {
        return this.p1.toString() + ", " + this.p2.toString() + ", " + this.r1 + ", " + this.r2;
    }
}
