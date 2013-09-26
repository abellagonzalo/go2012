
package jmanager.VICODE.dialogs;

import jmanager.VICODE.Modulation;
import jmanager.VICODE.Requirement;

/**
 *
 * @author jesus
 */
public class RemoveConnectionAsker extends RemoverAskerGeneric {

	private Modulation modulation2remove = null;
	private Requirement requirement2remove = null;

	public RemoveConnectionAsker() {
		super();
	}

	protected void yesOptionClicked() {
		if (modulation2remove != null) {
			builderGUI.removeModulation(modulation2remove);
		}
		if (requirement2remove != null) {
			builderGUI.removeRequirement(requirement2remove);
		}
		//Después de borrar la flecha de modulación o de requerimiento,
		//vuelve al modo normal si no está pulsado el botón de 'hold'.
		builderGUI.attemptChangeToNormalMode();
	}

	public void setRemoveModulation(Modulation modulation) {
		requirement2remove = null;
		modulation2remove = modulation;
		msg = "¿Esta seguro de que desea borrar esta modulación?";
	}

	public void setRemoveRequirement(Requirement requirement) {
		modulation2remove = null;
		requirement2remove = requirement;
		msg = "¿Esta seguro de que desea borrar este requerimiento?";
	}
}
