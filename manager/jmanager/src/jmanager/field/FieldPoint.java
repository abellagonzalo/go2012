package jmanager.field;

/*
 * Point of the field.
 */

/**
 *
 * @author Eduardo Perdices
 */
public class FieldPoint {

    private float x, y, z, h;

    public FieldPoint(float x, float y, float z, float h) {
        this.x = x;
        this.y = y;
        this.z = z;
        this.h = h;
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

        public float getH() {
        return h;
    }

    public void setH(float h) {
        this.h = h;
    }

    public String toString() {
        return "(" + this.x + ", " + this.y + ", " + this.z + ")";
    }

    public boolean equals(Object object) {
        return this.x == ((FieldPoint) object).x && this.y == ((FieldPoint) object).y &&
                this.z == ((FieldPoint) object).z && this.h == ((FieldPoint) object).h;
    }
}
