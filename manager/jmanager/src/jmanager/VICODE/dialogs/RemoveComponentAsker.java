
package jmanager.VICODE.dialogs;

import jmanager.VICODE.Component;

/**
 *
 * @author jesus
 */
public class RemoveComponentAsker extends RemoverAskerGeneric {

	private Component component2remove = null;
	
	public RemoveComponentAsker() {
		super();
	}
	
	protected void yesOptionClicked() {
		builderGUI.removeComponent(component2remove);

		//Después de borrar el estado, vuelve al modo normal.
		builderGUI.attemptChangeToNormalMode();
	}

	public void setRemoveComponentID(Component component) {
		component2remove = component;
		msg = "¿Esta seguro de que desea borrar el componente " + component.getId() + "?";
	}
}
