package jmanager.VICODE;

import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.event.PBasicInputEventHandler;
import edu.umd.cs.piccolo.event.PDragEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;

/**
 * 
 * @author paco
 */
public class Component extends NodeItem {

	private Component instance = null;
	private ComponentCodeFrame componentframe;


	/* CLASE interna para los eventos del ratón en el nodo Component */
	private class ComponentMouseEventHandler extends PBasicInputEventHandler {

		//Variable para almacenar el evento de arrastrar.
		//El evento de arrastrar está implementado en la clase PDragEventHandler,
		//por tanto hay que instanciar una clase de este tipo.
		private PDragEventHandler dragEventHandler;

		/* Constructor */
		public ComponentMouseEventHandler() {

			dragEventHandler = new PDragEventHandler() {

				@Override
				public void mouseDragged(PInputEvent event) {
					super.drag(event); //PRODUCE el ARRASTRE
					MouseDraggingController.dragControl(instance);
					instance.updateRelations(); //Actualiza las modulaciones
				}
			};
		}

		public PDragEventHandler getDragEventHandler() {
			return dragEventHandler;
		}

		@Override
		public void mouseEntered(PInputEvent e) {
			if (builderGUI.getMode() == BuilderGUI.ADD_CONNECTION_TARGET) {

				if (builderGUI.getTempModulation() != null) {
					Modulation modulation = builderGUI.getTempModulation();
					modulation.setTarget(instance);
					modulation.setVisible(true);
				}
			}
		}

		@Override
		public void mouseExited(PInputEvent e) {
			if (builderGUI.getMode() == BuilderGUI.ADD_CONNECTION_TARGET) {
				Modulation modulation = builderGUI.getTempModulation();
				if (modulation != null) {
					modulation.setTarget(null);
					modulation.setVisible(false);
				}
			}
		}

		@Override
		public void mouseClicked(PInputEvent e) {
			if (e.isLeftMouseButton()) {
				if (builderGUI.getMode() == BuilderGUI.REMOVE_COMPONENT) {
					builderGUI.getRemoveComponentAsker().setRemoveComponentID((Component) instance);
					builderGUI.getRemoveComponentAsker().showDialog();
				}
				e.setHandled(true);
			}
		}

		@Override
		public void mousePressed(PInputEvent e) {
			//super.mouseClicked(e); <--No se necesita.

			if (e.isRightMouseButton()) {
				//EL RESTO DE FUNCIONALIDAD DEL BOTON DERECHO es COMÚN
				//a todos y lo gestiona el evento mouseCLICKED del panel padre (BuilderGUI).
				if (builderGUI.getMode() == BuilderGUI.NORMAL) {
					componentframe.setVisible(true);
				}

			} else if (e.isLeftMouseButton()) {

				int currentMode = builderGUI.getMode();

				if (currentMode == BuilderGUI.ADD_CONNECTION_SOURCE) {
					//Crea un requerimiento temporal que puede ser usado a través de BuilderGUI.
					Requirement requirement = new Requirement(instance, null);
					builderGUI.setTempRequirement(requirement);
					builderGUI.setMode(BuilderGUI.ADD_CONNECTION_TARGET);
					requirements.add(requirement);

				} else if (currentMode == BuilderGUI.ADD_CONNECTION_TARGET) {

					if (builderGUI.getTempModulation() != null) {
						Modulation modulation = builderGUI.getTempModulation();
						boolean exist = false;

						Iterator it = modulations.iterator();
						Modulation m;
						while (it.hasNext()) {
							m = (Modulation) it.next();
							if (m.getSource() == modulation.getSource()) {
								exist = true;
							}
						}
						if (!exist) {
							builderGUI.setTempModulation(null);
							modulation.setTarget(instance);
							modulations.add(modulation);
							builderGUI.setMode(BuilderGUI.ADD_CONNECTION_SOURCE);

							modulation.update();
							//Al acabar vuelve al modo normal si no está pulsado el botón de 'hold'.
							builderGUI.attemptChangeToNormalMode();
						}
					}
				}
			}

			e.setHandled(true);
		}
	}//End Class ComponentMouseEventHandler.

	/* Constructor de Component */
	public Component(String id) {
		super(Component.class.getResource("resources/Component.png"), id);
		instance = this;
		idTextNode = new TextNode(id, this);

		//Añade el nodo texto como hijo.
		this.addChild(idTextNode);

		//Esto ya no va así porque ahora el nodo texto es hijo de cada círculo.
		//builderGUI.getTextLayer().addChild(idText);

		componentframe = new ComponentCodeFrame();
		componentframe.setComponent(this);
		componentframe.setVisible(false);
		componentframe.setIdTextField(id);

		//Crea el manejador de eventos.
		ComponentMouseEventHandler componentEvents = new ComponentMouseEventHandler();

		//Añade el listener para los botones del ratón.
		this.addInputEventListener(componentEvents);

		//Añade al final el evento de arrastrar, para que
		//se añada DETRÁS del de mousePressed, y así no da conflicto.
		this.addInputEventListener(componentEvents.getDragEventHandler());
	}

	@Override
	public void setVisible(boolean vis) {

		if (visible != vis) {
			visible = vis;

			PLayer compLayer = builderGUI.getComponentLayer();

			if (visible) {
				if (!compLayer.isAncestorOf(this)) {
					compLayer.addChild(this);
				}
				setConnectionsVisible(true);
				updateRelations();

			} else {
				if (compLayer.isAncestorOf(this)) {
					compLayer.removeChild(this);
				}
				setConnectionsVisible(false);
			}
		}
	}

	private void setConnectionsVisible(boolean vis) {
		Iterator it = modulations.iterator();
		Modulation m;
		while (it.hasNext()) {
			m = (Modulation) it.next();
			m.setVisible(vis);
		}

		Iterator it2 = requirements.iterator();
		Requirement r;
		while (it2.hasNext()) {
			r = (Requirement) it2.next();
			r.setVisible(vis);
		}
	}

	public void selfDestroy() {
		//Para evitar problemas de acceso simultáneo al mismo elemento,
		//usamos otra lista con referencias a la original.
		ArrayList<Modulation> aux = (ArrayList<Modulation>) modulations.clone();

		Iterator it = aux.iterator();
		Modulation m;
		while (it.hasNext()) {
			m = (Modulation) it.next();
			//m.selfDestroy(); //Ya lo hace la GUI en la llamada siguiente.
			builderGUI.removeModulation(m);
		}
		modulations.clear();

		ArrayList<Requirement> aux2 = (ArrayList<Requirement>) requirements.clone();

		Iterator it2 = aux2.iterator();
		Requirement r;
		while (it2.hasNext()) {
			r = (Requirement) it2.next();
			builderGUI.removeRequirement(r);
		}
		requirements.clear();

		this.setVisible(false);
	}

	public boolean setId(String newid) {
		ListIterator it = builderGUI.getComponentDependences().listIterator();

		Component c;
		while (it.hasNext()) {
			c = (Component) it.next();

			if (c.getId().equals(newid)) {
				return false;
			}
		}
		id = newid;
		idTextNode.setText(id);
		this.centerIdText();

		return true;
	}

	public void setXYpos(double posx, double posy) {
		this.setX(posx);
		this.setY(posy);

		this.updateRelations();
	}

	public void updateRelations() {
		Iterator<Modulation> itm = modulations.iterator();
		Modulation m;
		while (itm.hasNext()) {
			m = itm.next();
			m.update();
		}

		Iterator<Requirement> itr = requirements.iterator();
		Requirement r;
		while (itr.hasNext()) {
			r = itr.next();
			r.update();
		}
	}

	public void addModulation(Modulation mod) {
		if (!modulations.contains(mod)) {
			modulations.add(mod);
		}
	}

	public void addRequirement(Requirement req) {
		if (!requirements.contains(req)) {
			requirements.add(req);
		}
	}

	public void closeFrame() {
		if (componentframe.isVisible()) {
			componentframe.setVisible(false);
		}
	}
}
