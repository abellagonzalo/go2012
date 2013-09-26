package jmanager.VICODE.dialogs;

import java.awt.event.ActionEvent;
import java.util.ListIterator;
import jmanager.VICODE.State;

/**
 *
 * @author jesus
 */
public class NewStateDialog extends NewGenericNodeDialog {

	public NewStateDialog() {
		super();
		newTypeNodeLabel.setText("New State");
	}

	@Override
	protected void createButtonActionPerformed(ActionEvent evt) {

		ListIterator it = builderGUI.getStateLayer().getChildrenIterator();
		String name = idTextField.getText().trim();

		State s;
		while (it.hasNext()) {
			s = (State) it.next();

			if (s.getId().equals(name)) {
				msgLabel.setText("Already exists");
				return;
			}
		}
		//Crea y añade el nuevo estado.
		builderGUI.addState(name);

		//Después de crear el nuevo estado, si no está pulsado el botón de 'hold',
		//vuelve al modo normal.
		builderGUI.attemptChangeToNormalMode();
		setVisible(false);
	}

}
