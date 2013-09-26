
package jmanager.VICODE;

import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.event.PBasicInputEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;

/**
 *
 * @author paco
 */
public class Requirement extends Interaction {

	private Requirement instance = null;

	public Requirement(Component src, State trg) {
		super();

		instance = this;
		source = src;
		target = trg;

		this.update();

		this.addInputEventListener(new PBasicInputEventHandler() {

			@Override
			public void mouseClicked(PInputEvent e) {
				super.mouseClicked(e);
				
				if (e.isLeftMouseButton()) {
					if (builderGUI.getMode() == BuilderGUI.REMOVE_CONNECTION) {
						builderGUI.getRemoveConnectionAsker().setRemoveRequirement(instance);
						builderGUI.getRemoveConnectionAsker().showDialog();
					}
				}
			}
		});
	}

	public Component getSource() {
		return (Component) source;
	}

	public State getTarget() {
		return (State) target;
	}

	public void selfDestroy() {
		source.removeRequirement(this);

		if (target != null) {
			target.removeRequirement(this);
		}
		this.setVisible(false);
	}

	@Override
	public void setVisible(boolean vis) {

		if (visible != vis) {
			visible = vis;

			PLayer connectLayer = builderGUI.getConnectionLayer();

			if (visible) {
				if (! connectLayer.isAncestorOf(this)) {
					connectLayer.addChild(this);
				}
				update();

			} else {
				if (connectLayer.isAncestorOf(this)) {
					connectLayer.removeChild(this);
				}
			}
		}
	}
}
