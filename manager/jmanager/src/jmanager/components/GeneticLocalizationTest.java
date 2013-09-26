/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.components;

import java.util.logging.Level;
import java.util.logging.Logger;

/**
 *
 * @author eperdes
 */
public class GeneticLocalizationTest extends Thread {
    
    private GeneticLocalization component;
    private boolean stop;
    private int time;
    private int sleeptime;

    @Override
    public void run() {
        float reliability;
        float [] errors;

        if(this.component == null)
            return;

        System.out.println("Iniciamos la prueba");

        this.stop = false;
        this.time = 0;
        this.sleeptime = 1;

        //Reset location
        //this.component.resetLocation();

        while(!this.stop) {
            errors = this.component.getLocationError();
            System.out.println("ERROR POSITION");
            System.out.println(this.time + " " + errors[0]);
            System.out.println("ERROR ANGLE");
            System.out.println(this.time + " " + errors[1]);
            reliability = this.component.getLocationReliability();
            System.out.println("RELIABILITY");
            System.out.println(this.time + " " + reliability);

            try {
                Thread.sleep(this.sleeptime * 1000);
            } catch (InterruptedException ex) {
            }
            this.time += this.sleeptime;
        }
    }
    
    public void setComponent(GeneticLocalization component) {
        this.component = component;
    }

    public void stopTest() {
        this.stop = true;
    }
}
