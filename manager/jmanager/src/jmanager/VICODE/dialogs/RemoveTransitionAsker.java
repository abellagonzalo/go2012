
package jmanager.VICODE.dialogs;

import jmanager.VICODE.Transition;

/**
 *
 * @author jesus
 */
public class RemoveTransitionAsker extends RemoverAskerGeneric {

	private Transition transition2remove = null;

	public RemoveTransitionAsker() {
		super();
	}

	protected void yesOptionClicked() {
		builderGUI.removeTransition(transition2remove);
		//Después de borrar la transición, vuelve al modo normal.
		builderGUI.attemptChangeToNormalMode();
	}

	public void setRemoveTransition(Transition transition) {
		transition2remove = transition;
		msg = "¿Esta seguro de que desea borrar esta transición?";
	}
}
