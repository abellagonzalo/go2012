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
public class GrPrimitivesAbs extends Observable{

    private bica.Shape[] shapeList;

    public GrPrimitivesAbs() {

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
