
package jmanager.VICODE;

import java.awt.geom.Point2D;

/**
 *
 * @author paco
 */
public class InitialState extends State {
    
    //private final static String IMAGE_PATH = BuilderGUI.WORKING_DIRECTORY + "/src/jmanager/figs/InitialState.png";

    public InitialState() {
        super(State.class.getResource("resources/InitialState.png"), new String("Initial"));
        stateframe.getIdTextField().setEditable(false);

		//Centra el texto del estado inicial.
		Point2D pos = this.getGlobalBounds().getCenter2D();
		idTextNode.setX(pos.getX() - (idTextNode.getWidth() / 2.0));
		idTextNode.setY(pos.getY() - (idTextNode.getHeight() / 2.0));
    }
}
