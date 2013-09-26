
package jmanager.VICODE;

import edu.umd.cs.piccolo.nodes.PImage;
import java.awt.geom.Point2D;
import java.net.URL;
import java.util.ArrayList;

/**
 *
 * @author jesus
 */
public abstract class NodeItem extends PImage {

	protected BuilderGUI builderGUI;
	protected String id;
	protected TextNode idTextNode = null;
	protected boolean visible = false;
	protected ArrayList<Modulation> modulations = new ArrayList<Modulation>();
	protected ArrayList<Requirement> requirements = new ArrayList<Requirement>();

	public NodeItem(URL imageResource, String id) {
		super(imageResource);
		this.id = id;
		//builderGUI = Builder.getInstance().gethFSMBuilderGUI(); //<--NO
		builderGUI = BuilderGUI.getInstance();
	}

	public void centerIdText() {
		Point2D pos;
		//pos = this.getFullBoundsReference().getCenter2D(); <--NO

		pos = this.getBounds().getCenter2D();

		idTextNode.setX(pos.getX() - (idTextNode.getWidth() / 2.0));
		idTextNode.setY(pos.getY() - (idTextNode.getHeight() / 2.0));
	}

	public void removeModulation(Modulation mod) {
		if (modulations.contains(mod)) {
			modulations.remove(mod);
		}
	}

	public void removeRequirement(Requirement req) {
		if (requirements.contains(req)) {
			requirements.remove(req);
		}
	}

	public String getId() {
		return id;
	}

	public abstract void updateRelations();
}
