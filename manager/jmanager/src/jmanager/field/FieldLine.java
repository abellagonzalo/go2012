package jmanager.field;

import jmanager.field.FieldPoint;

/*
 * Line of the field.
 */
/**
 *
 * @author Eduardo Perdices
 */
public class FieldLine {

    private FieldPoint p1,  p2,  p3,  p4;
    private FieldPoint p5,  p6,  p7,  p8;
    private String color;

    public FieldLine(FieldPoint p1, FieldPoint p2, FieldPoint p3, FieldPoint p4, FieldPoint p5, FieldPoint p6, FieldPoint p7, FieldPoint p8, String color) {
        this.p1 = p1;
        this.p2 = p2;
        this.p3 = p3;
        this.p4 = p4;
        this.p5 = p5;
        this.p6 = p6;
        this.p7 = p7;
        this.p8 = p8;
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

    public FieldPoint getP3() {
        return p3;
    }

    public void setP3(FieldPoint p3) {
        this.p3 = p3;
    }

    public FieldPoint getP4() {
        return p4;
    }

    public void setP4(FieldPoint p4) {
        this.p4 = p4;
    }

    public FieldPoint getP5() {
        return p5;
    }

    public void setP5(FieldPoint p5) {
        this.p5 = p5;
    }

    public FieldPoint getP6() {
        return p6;
    }

    public void setP6(FieldPoint p6) {
        this.p6 = p6;
    }

    public FieldPoint getP7() {
        return p7;
    }

    public void setP7(FieldPoint p7) {
        this.p7 = p7;
    }

    public FieldPoint getP8() {
        return p8;
    }

    public void setP8(FieldPoint p8) {
        this.p8 = p8;
    }

    public String getColor() {
        return color;
    }

    public void setColor(String color) {
        this.color = color;
    }

    public String toString() {
        return this.p1.toString() + ", " + this.p2.toString() + ", " + this.p3.toString() + ", " + this.p4.toString()
                + ", " + this.p5.toString() + ", " + this.p6.toString() + ", " + this.p7.toString() + ", " + this.p8.toString();
    }
}
