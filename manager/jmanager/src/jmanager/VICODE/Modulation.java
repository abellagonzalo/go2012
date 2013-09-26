
package jmanager.VICODE;

import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.event.PBasicInputEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;

/**
 *
 * @author paco
 */
public class Modulation extends Interaction {

	private Modulation instance = null;

	public Modulation(State src, Component trg) {
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
						builderGUI.getRemoveConnectionAsker().setRemoveModulation(instance);
						builderGUI.getRemoveConnectionAsker().showDialog();
					}
				}
			}
		});
	}

	public State getSource() {
		return (State) source;
	}

	public Component getTarget() {
		if (target == null) {
			System.err.println("NULL modulation from " + source.getId());
		}
		return (Component) target;
	}

	public void selfDestroy() {
		source.removeModulation(this);

		if (target != null) {
			target.removeModulation(this);
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
