package jmanager.VICODE.dialogs;

import javax.swing.JOptionPane;
import jmanager.VICODE.BuilderGUI;

/**
 *
 * @author jesus
 */
public abstract class RemoverAskerGeneric {

	protected String msg;
	protected BuilderGUI builderGUI;

	protected RemoverAskerGeneric() {
		builderGUI = BuilderGUI.getInstance();
		msg = "";
	}

	protected abstract void yesOptionClicked();

	public void showDialog() {
		int selection = JOptionPane.showOptionDialog(null, msg, "Aviso", JOptionPane.YES_NO_OPTION,
			JOptionPane.INFORMATION_MESSAGE, null, new Object[]{"YES", "NO"}, "NO");

		if (selection == 0) { //Se eligió "YES"
			this.yesOptionClicked();

		} else {
			//si no está pulsado el botón de 'hold', vuelve al modo normal
			//y desactiva cualquier botón que hubiera.
			builderGUI.attemptChangeToNormalMode();
		}
	}
}
