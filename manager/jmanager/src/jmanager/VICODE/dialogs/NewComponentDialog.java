
package jmanager.VICODE.dialogs;

import java.awt.event.ActionEvent;
import java.util.ListIterator;
import jmanager.VICODE.Component;

/**
 *
 * @author jesus
 */
public class NewComponentDialog extends NewGenericNodeDialog {

	public NewComponentDialog() {
		super();
		newTypeNodeLabel.setText("New Component");
	}

	@Override
	protected void createButtonActionPerformed(ActionEvent evt) {

		ListIterator it = builderGUI.getComponentLayer().getChildrenIterator();
		String name = idTextField.getText().trim();

		Component c;
		while (it.hasNext()) {
			c = (Component) it.next();

			if (c.getId().equals(name)) {
				msgLabel.setText("Already exists");
				return;
			}
		}
		builderGUI.addComponent(name);

		//Después de crear el nuevo componente, si no está pulsado el botón de 'hold',
		//vuelve al modo normal.
		builderGUI.attemptChangeToNormalMode();
		setVisible(false);
	}
}
