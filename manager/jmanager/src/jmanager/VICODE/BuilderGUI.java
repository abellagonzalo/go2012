package jmanager.VICODE;

import jmanager.VICODE.dialogs.RemoveStateAsker;
import jmanager.VICODE.dialogs.NewStateDialog;
import jmanager.VICODE.dialogs.RemoveComponentAsker;
import jmanager.VICODE.dialogs.RemoveTransitionAsker;
import jmanager.VICODE.dialogs.RemoveConnectionAsker;
import jmanager.VICODE.dialogs.NewComponentDialog;
import edu.umd.cs.piccolo.PCanvas;
import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.event.PBasicInputEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;
import java.util.ArrayList;
import java.util.Iterator;

/**
 *
 * @author paco
 */
public class BuilderGUI extends PCanvas {

	private ArrayList<RegularState> states;
	private ArrayList<Transition> transitions;
	private ArrayList<Component> components;
	private String auxcodeIH;
	private String auxcodeIC;
	private String auxcodeH;
	private String auxcodeC;
	private PLayer stateLayer, nodeTransitionLayer, transitionLayer, textLayer, componentLayer, connectionLayer;
	private InitialState initState;
	private NewStateDialog newStateDialog;
	private NewComponentDialog newComponentDialog;
	private RemoveStateAsker removeStateAsker;
	private RemoveComponentAsker removeComponentAsker;
	private RemoveTransitionAsker removeTransitionAsker;
	private RemoveConnectionAsker removeConnectionAsker;
	private Transition tempTransition = null;
	private Modulation tempModulation = null;
	private Requirement tempRequirement = null;
	private boolean autoRestart = false;
	private double lastClickX = 0.0, lastClickY = 0.0;
	private int mode = NORMAL;

	public final static int NORMAL = 0;
	public final static int ADD_STATE = 1;
	public final static int REMOVE_STATE = 2;
	public final static int ADD_TRANSITION_SOURCE = 3;
	public final static int ADD_TRANSITION_TARGET = 4;
	public final static int REMOVE_TRANSITION = 5;
	public final static int ADD_COMPONENT = 6;
	public final static int REMOVE_COMPONENT = 7;
	public final static int ADD_CONNECTION_SOURCE = 8;
	public final static int ADD_CONNECTION_TARGET = 9;
	public final static int REMOVE_CONNECTION = 10;

	//Ojo que BEFORE_STATE_CODE y CODE_END llevan un ESPACIO al final.
	public final static String BEFORE_STATE_CODE = ("//BUILDER COMMENT. DO NOT REMOVE. begin ");
	public final static String CODE_END = ("//BUILDER COMMENT. DO NOT REMOVE. end ");
	public final static String BEFORE_TRANSITION_CODE = BEFORE_STATE_CODE;
	public final static String AUXCODE_BEGIN = "//BUILDER COMMENT. DO NOT REMOVE. auxcode begin";
	public final static String AUXCODE_END = "//BUILDER COMMENT. DO NOT REMOVE. auxcode end";
	public final static String AUXINCLUDE_BEGIN = "//BUILDER COMMENT. DO NOT REMOVE. auxinclude begin";
	public final static String AUXINCLUDE_END = "//BUILDER COMMENT. DO NOT REMOVE. auxinclude end";

	//public final static String WORKING_DIRECTORY = System.getProperty("user.dir");
	
	private static BuilderGUI instance;

	
	/* CLASE interna para los eventos del ratón del panel canvas BuilderGUI */
	private class BuilderGUIMouseEventHandler extends PBasicInputEventHandler {

		public BuilderGUIMouseEventHandler() {
		}

		@Override
		public void mouseClicked(PInputEvent e) {

			if (e.isRightMouseButton()) {
				
				removeTempTargets();

				if (mode == BuilderGUI.ADD_TRANSITION_TARGET) {
					instance.setMode(BuilderGUI.ADD_TRANSITION_SOURCE);

				} else if (mode == BuilderGUI.ADD_CONNECTION_TARGET) {
					instance.setMode(BuilderGUI.ADD_CONNECTION_SOURCE);
				}
				
			} else if (e.isLeftMouseButton()) {
				if (mode == BuilderGUI.ADD_STATE) {
					lastClickX = e.getPosition().getX();
					lastClickY = e.getPosition().getY();
					newStateDialog.setVisible(true);
				}
				if (mode == BuilderGUI.ADD_COMPONENT) {
					lastClickX = e.getPosition().getX();
					lastClickY = e.getPosition().getY();
					newComponentDialog.setVisible(true);
				}
			}
		}
	}

	
	/* Constructor */
	public BuilderGUI() {
		instance = this;
		states = new ArrayList<RegularState>();
		transitions = new ArrayList<Transition>();
		components = new ArrayList<Component>();
		states.clear();
		transitions.clear();

		textLayer = new PLayer();
		stateLayer = new PLayer();
		componentLayer = new PLayer();
		nodeTransitionLayer = new PLayer();
		transitionLayer = new PLayer();
		connectionLayer = new PLayer();

		getRoot().addChild(stateLayer);
		getRoot().addChild(componentLayer);
		getRoot().addChild(nodeTransitionLayer);
		getRoot().addChild(transitionLayer);
		getRoot().addChild(connectionLayer);

		//NOTA: Las capas con MAYOR nº de secuencia se pintan por encima de las anteriores.
		getCamera().addLayer(0, componentLayer);
		getCamera().addLayer(1, stateLayer);
		getCamera().addLayer(2, nodeTransitionLayer);
		getCamera().addLayer(3, transitionLayer);
		getCamera().addLayer(4, connectionLayer);
		getCamera().addLayer(5, textLayer);

		//invalida los manejadores que vienen establecidos por defecto
		setPanEventHandler(null);
		setZoomEventHandler(null);

		initState = new InitialState();

		newStateDialog = new NewStateDialog();
		newComponentDialog = new NewComponentDialog();
		newStateDialog.setVisible(false); //por defecto ya está a false, pero lo dejo por claridad.

		//Los diálogos de eliminar se crean al inicio, y permanecen ocultos hasta que se activan.
		//Otra opción sería que se creen cada vez que se necesiten.
		removeStateAsker = new RemoveStateAsker();
		removeComponentAsker = new RemoveComponentAsker();
		removeTransitionAsker = new RemoveTransitionAsker();
		removeConnectionAsker = new RemoveConnectionAsker();

		resetAuxcodeStrings();

		addInputEventListener(new BuilderGUIMouseEventHandler());
	}

	public static BuilderGUI getInstance() {
		return instance;
	}

	public PLayer getStateLayer() {
		return stateLayer;
	}

	public PLayer getTextLayer() {
		return textLayer;
	}

	public PLayer getTransitionLayer() {
		return transitionLayer;
	}

	public PLayer getConnectionLayer() {
		return connectionLayer;
	}

	public PLayer getComponentLayer() {
		return componentLayer;
	}

	public PLayer getNodeTransitionLayer() {
		return nodeTransitionLayer;
	}

	/**
	 * Crea y añade un estado nuevo
	 * 
	 * @param id - Nombre del State a crear.
	 * @return el State creado
	 */
	public RegularState addState(String id) {
		RegularState aux = new RegularState(id);
		aux.setX(lastClickX - aux.getWidth() / 2.0);
		aux.setY(lastClickY - aux.getHeight() / 2.0);

		//Coloca el texto en el centro
		aux.centerIdText();

		//Hace visible el State. Y ahí además es donde se añade como hijo
		//a la capa de estados.
		aux.setVisible(true);

		//Añade el estado a la lista de estados propia.
		states.add(aux);
		return aux;
	}

	/**
	 * Crea y añade una Dependencia nueva (Component)
	 *
	 * @param id - Nombre de la dependencia a crear.
	 * @return el component creado
	 */
	public Component addComponent(String id) {
		Component aux = new Component(id);

		aux.setX(lastClickX - aux.getWidth() / 2.0);
		aux.setY(lastClickY - aux.getHeight() / 2.0);

		//Coloca el texto en el centro
		aux.centerIdText();

		aux.setVisible(true);
		components.add(aux);
		return aux;
	}

	/**
	 * Crea y añade una transición nueva
	 *
	 * @param id - Nombre de la transición a crear.
	 * @return la transición creada
	 */
	public Transition addTransition(State src, State trg) {
		Transition t = new Transition(src, trg);
		transitions.add(t);
		trg.addIncomingTransition(t);
		src.addOutComingTransition(t);
		t.setVisible(true);
		return t;
	}

	public Component getComponent(String cid) {
		Component c = null;
		Iterator it = components.iterator();
		while (it.hasNext()) {
			c = (Component) it.next();
			if (c.getId().equals(cid)) {
				break;
			}
		}
		return c;
	}

	public State getState(String id) {

		if (id.equals("Initial")) {
			return initState;
		}
		Iterator it = states.iterator();
		RegularState s;
		while (it.hasNext()) {
			s = (RegularState) it.next();
			if (s.getId().equals(id)) {
				return s;
			}
		}
		return null;
	}

	public Transition getTransition(String id) {
		Iterator it = transitions.iterator();
		Transition t;
		while (it.hasNext()) {
			t = (Transition) it.next();
			if (t.getId().equals(id)) {
				return t;
			}
		}
		return null;
	}

	public void removeState(RegularState state) {
		states.remove(state);
		state.selfDestroy();
	}

	public void removeTransition(Transition transition) {
		transitions.remove(transition);
		transition.selfDestroy();
	}

	public void setMode(int newMode) {
		mode = newMode;
	}

	public int getMode() {
		return mode;
	}

	public void setTempTransition(Transition transition) {
		tempTransition = transition;
	}

	public void setTempModulation(Modulation modulation) {
		tempModulation = modulation;
	}

	public Transition getTempTransition() {
		return tempTransition;
	}

	public Modulation getTempModulation() {
		return tempModulation;
	}

	public RemoveTransitionAsker getRemoveTransitionAsker() {
		return removeTransitionAsker;
	}

	public RemoveStateAsker getRemoveStateAsker() {
		return removeStateAsker;
	}

	public State getInitialState() {
		return initState;
	}

	public ArrayList<Transition> getTransitions() {
		return transitions;
	}

	public ArrayList<RegularState> getStates() {
		return states;
	}

	public RemoveComponentAsker getRemoveComponentAsker() {
		return removeComponentAsker;
	}

	public ArrayList<Component> getComponentDependences() {
		return components;
	}

	public RemoveConnectionAsker getRemoveConnectionAsker() {
		return removeConnectionAsker;
	}

	public void removeComponent(Component component) {
		components.remove(component);
		component.selfDestroy();
	}

	public void removeModulation(Modulation modulation) {
		modulation.selfDestroy();
	}

	public void removeRequirement(Requirement requirement) {
		requirement.selfDestroy();
	}

	public void setTempRequirement(Requirement requirement) {
		tempRequirement = requirement;
	}

	public Requirement getTempRequirement() {
		return tempRequirement;
	}

	private void removeTempTargets() {
		if (mode == BuilderGUI.ADD_TRANSITION_TARGET) {
			//Hace falta este 'if' porque se puede estar en modo TARGET pero
			//pinchar sobre sí mismo con el derecho.
			if (instance.getTempTransition() != null) {
				instance.getTempTransition().selfDestroy();
			}
			instance.setTempTransition(null);

		} else if (mode == BuilderGUI.ADD_CONNECTION_TARGET) {
			if (instance.getTempModulation() != null) {
				instance.getTempModulation().selfDestroy();
			}
			if (instance.getTempRequirement() != null) {
				instance.getTempRequirement().selfDestroy();
			}
			instance.setTempModulation(null);
			instance.setTempRequirement(null);
		}
	}

	public boolean attemptChangeToNormalMode() {
		Builder builder = Builder.getInstance();

		if (builder.getHoldCheckBox().isSelected() == false) {

			builder.unselectAllToggleButtons();
			this.setMode(BuilderGUI.NORMAL);
			return true;

		} else {
			return false;
		}
	}

	private void resetAuxcodeStrings() {
		auxcodeIH = AUXINCLUDE_BEGIN + "\n" + AUXINCLUDE_END + "\n";
		auxcodeIC = AUXINCLUDE_BEGIN + "\n" + AUXINCLUDE_END + "\n";
		auxcodeC = AUXCODE_BEGIN + "\n" + AUXCODE_END + "\n";
		auxcodeH = AUXCODE_BEGIN + "\n" + AUXCODE_END + "\n";
	}

	public void reset() {
		stateLayer.removeAllChildren();
		nodeTransitionLayer.removeAllChildren();
		transitionLayer.removeAllChildren();
		connectionLayer.removeAllChildren();
		textLayer.removeAllChildren();
		componentLayer.removeAllChildren();

		Iterator it = states.iterator();
		RegularState s;
		while (it.hasNext()) {
			s = (RegularState) it.next();
			s.getModulations().clear();
			s.getRequirements().clear();
		}
		states.clear();
		transitions.clear();
		components.clear();

		//Crea el estado inicial.
		initState = new InitialState();
		stateLayer.addChild(initState);

		resetAuxcodeStrings();
	}

	public void setAuxIHCode(String IHcode) {
		auxcodeIH = AUXINCLUDE_BEGIN + "\n" + IHcode + AUXINCLUDE_END + "\n";
	}

	public void setAuxICCode(String ICcode) {
		auxcodeIC = AUXINCLUDE_BEGIN + "\n" + ICcode + AUXINCLUDE_END + "\n";
	}

	public void setAuxCCode(String Ccode) {
		auxcodeC = AUXCODE_BEGIN + "\n" + Ccode + AUXCODE_END + "\n";
	}

	public void setAuxHCode(String Hcode) {
		auxcodeH = AUXCODE_BEGIN + "\n" + Hcode + AUXCODE_END + "\n";
	}

	public String getAuxcodeC() {
		return auxcodeC;
	}

	public String getAuxcodeH() {
		return auxcodeH;
	}

	public String getAuxcodeIC() {
		return auxcodeIC;
	}

	public String getAuxcodeIH() {
		return auxcodeIH;
	}

	public void closeAllFrames() {
		Iterator it = states.iterator();
		RegularState s;
		while (it.hasNext()) {
			s = (RegularState) it.next();
			s.closeFrame();
		}

		it = components.iterator();
		Component c;
		while (it.hasNext()) {
			c = (Component) it.next();
			c.closeFrame();
		}

		it = transitions.iterator();
		Transition t;
		while (it.hasNext()) {
			t = (Transition) it.next();
			t.closeFrame();
		}
	}

	public void setAutoRestart(boolean autoR) {
		autoRestart = autoR;
	}

	public boolean isAutoRestart() {
		return autoRestart;
	}
}
