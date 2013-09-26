/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

package jmanager.GrDebug;

import bica.*;
import java.util.Observable;

/**
 *
 * @author carlos
 */
public class GrPrimitivesRel extends Observable{

    private bica.Shape[] shapeList;

    public GrPrimitivesRel() {

    }

    public void setGrPrimitives(bica.Shape[] newShapeList){
        this.shapeList = newShapeList;
        this.setChanged();
	this.notifyObservers();
    }

    public Shape[] getGrPrimitives(){
        return this.shapeList;
    }
}
