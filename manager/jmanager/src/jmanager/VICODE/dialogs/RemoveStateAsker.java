
package jmanager.VICODE.dialogs;

import jmanager.VICODE.RegularState;

/**
 *
 * @author jesus
 */
public class RemoveStateAsker extends RemoverAskerGeneric {

	private RegularState state2remove = null;

	public RemoveStateAsker() {
		super();
	}

	protected void yesOptionClicked() {
		builderGUI.removeState(state2remove);

		//Después de borrar el estado, vuelve al modo normal si no está
		//pulsado el botón de 'hold'.
		builderGUI.attemptChangeToNormalMode();
	}

	public void setRemoveStateID(RegularState state) {
		state2remove = state;
		msg = "¿Esta seguro de que desea borrar el estado " + state.getId() + "?";
	}
}
