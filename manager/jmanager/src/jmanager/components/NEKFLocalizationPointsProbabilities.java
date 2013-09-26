/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components;

/**
 *
 * @author jmom
 */
public class NEKFLocalizationPointsProbabilities {
    private float posX;
    private float posY;
    private float posZ;
    private int angle;
    private float prob;

    public NEKFLocalizationPointsProbabilities() {
    }


    public NEKFLocalizationPointsProbabilities(float posX, float posY, int angle, float prob) {
        this.posX = posX;
        this.posY = posY;
        this.posZ = 0.0f;
        this.angle = angle;
        this.prob = prob;
    }



    public NEKFLocalizationPointsProbabilities(float posX, float posY, float prob) {
        this.posX = posX;
        this.posY = posY;
        this.posZ = 0.0f;
        this.prob = prob;
    }

    public NEKFLocalizationPointsProbabilities(float posX, float posY) {
        this.posX = posX;
        this.posY = posY;
        this.posZ = 0.0f;
        this.prob = 0.5f;
    }

    public NEKFLocalizationPointsProbabilities(float posX, float posY, float posZ, float prob) {
        this.posX = posX;
        this.posY = posY;
        this.posZ = posZ;
        this.prob = prob;
    }

    public float getX(){
        return this.posX;
    }

    public float getY(){
        return this.posY;
    }

    public float getZ(){
        return this.posZ;
    }
    public float getProb(){
        return this.prob;
    }
    public int getAngle(){
        return this.angle;
    }

}
