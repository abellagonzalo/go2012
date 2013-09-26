package jmanager.VICODE;

import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.event.PBasicInputEventHandler;
import edu.umd.cs.piccolo.event.PDragEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;
import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.ListIterator;

/**
 *
 * @author paco
 */
public class State extends NodeItem {

	private ArrayList<Transition> incoming_transitions = new ArrayList<Transition>();
	private ArrayList<Transition> outcoming_transitions = new ArrayList<Transition>();
	private State instance = null;
	protected StateCodeFrame stateframe;


	/* CLASE interna para los eventos del ratón en el nodo State */
	private class StateMouseEventHandler extends PBasicInputEventHandler {
		
		//Variable para almacenar el evento de arrastrar.
		//El evento de arrastrar está implementado en la clase PDragEventHandler,
		//por tanto hay que instanciar una clase de este tipo.
		private PDragEventHandler dragEventHandler;

		/* Constructor */
		public StateMouseEventHandler() {

			dragEventHandler = new PDragEventHandler() {
				//Si los nodos hijos no redefinen otro, usarán este método para arrastrar.
				@Override
				public void mouseDragged(PInputEvent event) {
					super.drag(event); //PRODUCE el ARRASTRE
					MouseDraggingController.dragControl(instance);
					instance.updateRelations(); //Actualiza las transiciones y modulaciones
				}
			};
		}

		public PDragEventHandler getDragEventHandler() {
			return dragEventHandler;
		}

		@Override
		public void mouseEntered(PInputEvent e) {

			if (builderGUI.getMode() == BuilderGUI.ADD_TRANSITION_TARGET) {
				Transition transition = builderGUI.getTempTransition();
				if (transition.getSource() != instance) {
					transition.setTarget(instance);
					transition.setVisible(true);
				}
			} else if (builderGUI.getMode() == BuilderGUI.ADD_CONNECTION_TARGET) {

				if (builderGUI.getTempRequirement() != null) {
					Requirement requirement = builderGUI.getTempRequirement();
					requirement.setTarget(instance);
					requirement.setVisible(true);
				}
			}
		}

		@Override
		public void mouseExited(PInputEvent e) {

			if (builderGUI.getMode() == BuilderGUI.ADD_TRANSITION_TARGET) {
				Transition transition = builderGUI.getTempTransition();
				if ((transition != null) && (transition.getSource() != instance)) {
					transition.setTarget(null);
					transition.setVisible(false);
				}
			} else if (builderGUI.getMode() == BuilderGUI.ADD_CONNECTION_TARGET) {
				Requirement requirement = builderGUI.getTempRequirement();
				if (requirement != null) {
					requirement.setTarget(null);
					requirement.setVisible(false);
				}
			}
		}

		@Override
		public void mouseClicked(PInputEvent e) {
			if (e.isLeftMouseButton()) {

				if (builderGUI.getMode() == BuilderGUI.REMOVE_STATE) {
					if (instance != builderGUI.getInitialState()) {
						builderGUI.getRemoveStateAsker().setRemoveStateID((RegularState) instance);
						builderGUI.getRemoveStateAsker().showDialog();
					}
				}

				//Para que dé por despachado el evento y no haga NADA
				//al hacer clic IZQUIERDO ENCIMA DE UNA FIGURA.
				//Así no se permite por ejemplo crear una figura clicando justo
				//encima de otra existente, de acuerdo al evento mouseClicked de BuilderGUI.
				//Si no se pone, el evento pasa al 'mouseClicked' del nodo padre (BuilderGUI).
				e.setHandled(true);
			}
		}


		//NOTA sobre mousePressed:
		//Con mousePressed() permite establecer el nodo destino
		//de una transición presionando en el nodo y no importa
		//que a la vez, sin haberlo soltado, se arrastre el nodo.
		//Con mouseClicked en lugar de mousePressed ocurría que si se
		//pinchaba en un nodo para poner una transición, y por casualidad se arrastraba
		//el nodo sin haberlo soltado, la flecha de la transición se mantenía fija
		//y no se actualizaba hasta que no se soltara el nodo.
		@Override
		public void mousePressed(PInputEvent e) {

			if (e.isRightMouseButton()) {
				//EL RESTO DE FUNCIONALIDAD DEL BOTON DERECHO es COMÚN
				//a todos y lo gestiona el evento mouseCLICKED del panel padre (BuilderGUI).

				if ((builderGUI.getMode() == BuilderGUI.NORMAL)) {
					stateframe.setVisible(true);
				}

			} else if (e.isLeftMouseButton()) {

				int currentMode = builderGUI.getMode();
				Transition transition;

				switch (currentMode) {
					case BuilderGUI.ADD_CONNECTION_SOURCE:
						//Crea una modulación temporal que puede ser usada a través de BuilderGUI.
						Modulation modulation = new Modulation(instance, null);
						builderGUI.setTempModulation(modulation);
						builderGUI.setMode(BuilderGUI.ADD_CONNECTION_TARGET);
						modulations.add(modulation);
						break;

					case BuilderGUI.ADD_CONNECTION_TARGET:
						if (builderGUI.getTempRequirement() != null) {
							Requirement requirement = builderGUI.getTempRequirement();
							boolean exist = false;

							Iterator it = requirements.iterator();
							Requirement r;
							while (it.hasNext()) {
								r = (Requirement) it.next();
								if (r.getSource() == requirement.getSource()) {
									exist = true;
								}
							}
							if (!exist) {
								builderGUI.setTempRequirement(null);
								requirement.setTarget(instance);
								requirements.add(requirement);
								builderGUI.setMode(BuilderGUI.ADD_CONNECTION_SOURCE);
								requirement.update();

								//Al acabar vuelve al modo normal si no está pulsado el botón de 'hold'.
								builderGUI.attemptChangeToNormalMode();
							}
						}
						break;

					case BuilderGUI.ADD_TRANSITION_SOURCE:
						//Crea una transición temporal que puede ser usada a través de BuilderGUI.
						transition = new Transition(instance, null);
						builderGUI.setTempTransition(transition);
						builderGUI.setMode(BuilderGUI.ADD_TRANSITION_TARGET);
						outcoming_transitions.add(transition);
						transition.setPriority(outcoming_transitions.size());
						break;

					case BuilderGUI.ADD_TRANSITION_TARGET:
						transition = builderGUI.getTempTransition();

						if (transition.getSource() != instance) {
							builderGUI.setTempTransition(null);
							transition.setTarget(instance);
							incoming_transitions.add(transition);
							builderGUI.setMode(BuilderGUI.ADD_TRANSITION_SOURCE);
							builderGUI.getTransitions().add(transition);
							transition.update();

							//Al acabar vuelve al modo normal si no está pulsado el botón de 'hold'.
							builderGUI.attemptChangeToNormalMode();
						}
						break;
				}
			}//End e.isLeftMouseButton()
		}
	}//End Class StateMouseEventHandler.


	/* Constructor de State */
	protected State(URL imageResource, String id) {
		super(imageResource, id);
		instance = this;

		stateframe = new StateCodeFrame();
		stateframe.setState(this);
		stateframe.setVisible(false);
		stateframe.setIdTextField(id);

		idTextNode = new TextNode(id, this);

		//Añade el nodo texto como hijo.
		this.addChild(idTextNode);

		//Crea el manejador de eventos.
		StateMouseEventHandler stateEvents = new StateMouseEventHandler();

		//Añade el listener para los botones del ratón.
		this.addInputEventListener(stateEvents);

		//Añade al final el evento de arrastrar, para que
		//se añada DETRÁS del de mousePressed, y así no da conflicto.
		this.addInputEventListener(stateEvents.getDragEventHandler());
	}

	@Override
	public void setVisible(boolean vis) {
		if (visible != vis) {
			visible = vis;

			PLayer stateLayer = builderGUI.getStateLayer();

			if (visible) {
				if (!stateLayer.isAncestorOf(this)) {
					stateLayer.addChild(0, this); //AQUÍ SE LE AÑADEN LOS state HIJOS
				}
				setTransitionsVisible(true);
				updateRelations();

			} else {
				//Borra este estado de la lista de hijos de la capa padre.
				//El estado desaparece de la imagen, justo con su nodo hijo (el texto con el nombre).
				if (stateLayer.isAncestorOf(this)) {
					stateLayer.removeChild(this);
				}
				setTransitionsVisible(false);
			}
		}
	}

	private void setTransitionsVisible(boolean vis) {
		Iterator it = incoming_transitions.iterator();
		Transition t;
		while (it.hasNext()) {
			t = (Transition) it.next();
			t.setVisible(vis);
		}

		it = outcoming_transitions.iterator();
		while (it.hasNext()) {
			t = (Transition) it.next();
			t.setVisible(vis);
		}
	}

	public void selfDestroy() {
		ArrayList<Transition> aux = (ArrayList<Transition>) incoming_transitions.clone();

		Iterator it = aux.iterator();
		Transition t;
		while (it.hasNext()) {
			t = (Transition) it.next();
			builderGUI.removeTransition(t);
		}

		ArrayList<Transition> aux2 = (ArrayList<Transition>) outcoming_transitions.clone();

		it = aux2.iterator();
		while (it.hasNext()) {
			t = (Transition) it.next();
			builderGUI.removeTransition(t);
		}

		ArrayList<Modulation> aux3 = (ArrayList<Modulation>) modulations.clone();

		it = aux3.iterator();
		Modulation m;
		while (it.hasNext()) {
			m = (Modulation) it.next();
			builderGUI.removeModulation(m);
		}
		modulations.clear();

		ArrayList<Requirement> aux4 = (ArrayList<Requirement>) requirements.clone();

		it = aux4.iterator();
		Requirement r;
		while (it.hasNext()) {
			r = (Requirement) it.next();
			builderGUI.removeRequirement(r);
		}
		requirements.clear();

		this.setVisible(false);
	}

	public boolean setId(String newId) {
		String oldId = this.getId();

		ListIterator it = builderGUI.getStates().listIterator();

		State s;
		while (it.hasNext()) {
			s = (State) it.next();

			if (s.getId().equals(newId)) {
				return false;
			}
		}
		id = newId;
		idTextNode.setText(id);
		this.centerIdText();

		//Se cambia también el nombre de las transiciones asociadas,
		//siempre y cuando tengan el formato de nombre original.
		Iterator<Transition> itr = outcoming_transitions.iterator();
		Transition t;
		String tId;
		while (itr.hasNext()) {
			t = itr.next();
			tId = t.getId();
			if (tId.startsWith(oldId + "2")) {
				t.setId(tId.replaceFirst(oldId, newId));
			}
		}

		itr = incoming_transitions.iterator();
		while (itr.hasNext()) {
			t = itr.next();
			tId = t.getId();

			int posEndOld = tId.lastIndexOf("2" + oldId) + oldId.length();

			if (posEndOld != -1) {
				t.setId(tId.substring(0, tId.lastIndexOf(oldId))
					+ tId.substring(tId.lastIndexOf(oldId), tId.length()).replaceFirst(oldId, newId));
			}
		}
		return true;
	}

	public void setXYpos(double posx, double posy) {
		this.setX(posx);
		this.setY(posy);

		this.updateRelations();
	}

	public void updateRelations() {
		Iterator<Transition> itr = incoming_transitions.iterator();
		Transition trs;

		while (itr.hasNext()) {
			trs = itr.next();
			trs.update();
		}

		itr = outcoming_transitions.iterator();
		while (itr.hasNext()) {
			trs = itr.next();
			trs.update();
		}

		Requirement r;
		Iterator<Requirement> ir = requirements.iterator();
		while (ir.hasNext()) {
			r = ir.next();
			r.update();
		}

		Modulation m;
		Iterator<Modulation> im = modulations.iterator();
		while (im.hasNext()) {
			m = im.next();
			m.update();
		}
	}

	public void removeOutcomingTransition(Transition transition) {
		outcoming_transitions.remove(transition);
	}

	public void removeIncomingTransition(Transition transition) {
		incoming_transitions.remove(transition);
	}

	public StateCodeFrame getStateFrame() {
		return stateframe;
	}

	public void addIncomingTransition(Transition t) {
		incoming_transitions.add(t);
		this.updateRelations();
	}

	public void addOutComingTransition(Transition t) {
		outcoming_transitions.add(t);
		this.updateRelations();
	}

	public ArrayList<Requirement> getRequirements() {
		return requirements;
	}

	public ArrayList<Modulation> getModulations() {
		return modulations;
	}

	public void addModulation(State nowState, Component modulation) {
		Modulation mod = new Modulation(nowState, modulation);
		modulations.add(mod);
		mod.setVisible(true);
		modulation.addModulation(mod);
	}

	public void addRequirement(Component comp, State nowState) {
		Requirement req = new Requirement(comp, nowState);
		requirements.add(req);
		req.setVisible(true);
		comp.addRequirement(req);
	}

	public boolean isTimerDriven() {
		boolean timer = false;

		Iterator<Transition> otr = outcoming_transitions.iterator();
		Transition t;
		while (otr.hasNext()) {
			t = otr.next();
			if (t.getStateFrame().isTimerDriven()) {
				timer = true;
			}
		}
		return timer;
	}

	public void changeTransitionPriorities(int oldp, int newp) {
		if (newp > outcoming_transitions.size()) {
			return;
		}
		if (newp < 1) {
			return;
		}

		Transition t = outcoming_transitions.get(oldp - 1);
		outcoming_transitions.remove(oldp - 1);
		outcoming_transitions.add(newp - 1, t);

		int counter = 1;
		Transition t2;
		Iterator<Transition> otr = outcoming_transitions.iterator();
		while (otr.hasNext()) {
			t2 = otr.next();
			t2.setPriority(counter++);
		}
	}

	public void closeFrame() {
		if (stateframe.isVisible()) {
			stateframe.setVisible(false);
		}
	}
}
