
package jmanager.VICODE;

import java.awt.geom.Point2D;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

/**
 *
 * @author jesus
 */
public class CodeGenerator {
	private BuilderGUI builderGUI;

	private State initState;
	private ArrayList<RegularState> states;
	private ArrayList<Transition> transitions;
	private ArrayList<Component> components;
	private String auxcodeIH;
	private String auxcodeIC;
	private String auxcodeH;
	private String auxcodeC;
	private boolean autoRestart;

	public CodeGenerator() {
		builderGUI  = BuilderGUI.getInstance();

		initState   = builderGUI.getInitialState();
		states      = builderGUI.getStates();
		transitions = builderGUI.getTransitions();
		components  = builderGUI.getComponentDependences();
		auxcodeIH   = builderGUI.getAuxcodeIH();
		auxcodeIC   = builderGUI.getAuxcodeIC();
		auxcodeH    = builderGUI.getAuxcodeH();
		auxcodeC    = builderGUI.getAuxcodeC();
		autoRestart = builderGUI.isAutoRestart();
	}

	public String generateHPPCode(String componentId) {
		String ret = new String("");

		ret = ret + "#ifndef " + componentId + "_H\n";
		ret = ret + "#define " + componentId + "_H\n\n";
		ret = ret + "#include \"Component.h\"\n";

		Requirement r;
		Modulation m;
		RegularState s;
		Component c;
		Transition t;

		ArrayList<Component> comps = new ArrayList();

		//#include of the required components (initial state)
		ArrayList<Requirement> requirements = initState.getRequirements();
		Iterator reqsIter = requirements.iterator();
		while (reqsIter.hasNext()) {
			r = (Requirement) reqsIter.next();
			if (! comps.contains(r.getSource())) {
				comps.add(r.getSource());
			}
		}

		//#include of the moduled components (initial state)
		ArrayList<Modulation> modulations = initState.getModulations();
		Iterator modsIter = modulations.iterator();
		while (modsIter.hasNext()) {
				m = (Modulation) modsIter.next();
			if (! comps.contains(m.getTarget())) {
				comps.add(m.getTarget());
			}
		}

		Iterator it = states.iterator();
		while (it.hasNext()) {
			s = (RegularState) it.next();

			//#include of the required components
			requirements = s.getRequirements();
			reqsIter = requirements.iterator();
			while (reqsIter.hasNext()) {
				r = (Requirement) reqsIter.next();
				if (! comps.contains(r.getSource())) {
					comps.add(r.getSource());
				}
			}

			//#include of the moduled components
			modulations = s.getModulations();
			modsIter = modulations.iterator();
			while (modsIter.hasNext()) {
				m = (Modulation) modsIter.next();
				if (!comps.contains(m.getTarget())) {
					comps.add(m.getTarget());
				}
			}
		}

		it = comps.iterator();
		while (it.hasNext()) {
			c = (Component) it.next();
			ret = ret + "#include \"" + c.getId() + ".h\"\n";
		}

		ret = ret + "#include \"Singleton.h\"\n\n";

		ret = ret + auxcodeIH + "\n";

		ret = ret + "class " + componentId + " : public Component, public Singleton<" + componentId + ">\n";
		ret = ret + "{\n" + "public:\n\n\t" + componentId + "();\n\t" + "~" + componentId + "();\n\n";
		ret = ret + "\tvoid step();\n";
		if (autoRestart) {
			ret = ret + "\tvoid restart();\n";
		}
		ret = ret + "private:\n\n";

		int counter = 0;

		ret = ret + "\tstatic const int " + initState.getId() + "\t= " + counter + ";\n";
		counter++;

		it = states.iterator();
		while (it.hasNext()) {
			s = (RegularState) it.next();
			ret = ret + "\tstatic const int " + s.getId() + "\t= " + counter + ";\n";
			counter++;
		}
		ret = ret + "\tint state;\n\n";

		//Declaration of the pointer to each component
		it = comps.iterator();
		while (it.hasNext()) {
			c = (Component) it.next();
			ret = ret + "\t" + c.getId() + " *_" + c.getId() + ";\n";
		}

		ret = ret + "\n\tvoid " + initState.getId() + "_state_code(void);\n";

		it = states.iterator();
		while (it.hasNext()) {
			s = (RegularState) it.next();
			ret = ret + "\tvoid " + s.getId() + "_state_code(void);\n";
		}

		it = transitions.iterator();
		while (it.hasNext()) {
			t = (Transition) it.next();
			ret = ret + "\tbool " + t.getId() + "_transition_code(void);\n";
		}

		if (autoRestart) {
			ret = ret + "\tbool testRestart();\n\tlong lastExecTimestamp;\n";
		}

		ret = ret + auxcodeH + "};\n\n" + "#endif\n\n";

		return ret;
	}


	public String generateCPPCode(String componentId) {
		String ret = new String("");

		ret = ret + "#include \"" + componentId + ".h\"\n\n";

		ret = ret + auxcodeIC + "\n";

		// Constructor
		ret = ret + componentId + "::" + componentId + "()\n{\n";

		ArrayList<Component> comps = new ArrayList();

		RegularState s;
		Requirement r;
		Modulation m;
		Component c;
		Transition t;

		//#include of the required components (initial state)
		ArrayList<Requirement> requirements = initState.getRequirements();
		Iterator reqsIter = requirements.iterator();
			while (reqsIter.hasNext()) {
				r = (Requirement) reqsIter.next();
			if (! comps.contains(r.getSource())) {
				comps.add(r.getSource());
			}
		}

		//#include of the moduled components (initial state)
		ArrayList<Modulation> modulations = initState.getModulations();
		Iterator modsIter = modulations.iterator();
			while (modsIter.hasNext()) {
				m = (Modulation) modsIter.next();
			if (! comps.contains(m.getTarget())) {
				comps.add(m.getTarget());
			}
		}

		Iterator it = states.iterator();
		while (it.hasNext()) {
			s = (RegularState) it.next();

			//#include of the required components
			requirements = s.getRequirements();
			reqsIter = requirements.iterator();
			while (reqsIter.hasNext()) {
				r = (Requirement) reqsIter.next();
				if (! comps.contains(r.getSource())) {
					comps.add(r.getSource());
				}
			}

			//#include of the moduled components
			modulations = s.getModulations();
			modsIter = modulations.iterator();
			while (modsIter.hasNext()) {
				m = (Modulation) modsIter.next();
				if (! comps.contains(m.getTarget())) {
					comps.add(m.getTarget());
				}
			}
		}

		it = comps.iterator();
		while (it.hasNext()) {
			c = (Component) it.next();
			ret = ret + "\t_" + c.getId() + " = " + c.getId() + "::getInstance();\n";
		}

		ret = ret + "\tstate = Initial;\n";
		if (initState.isTimerDriven()) {
			ret = ret + "\tresetStopWatch();\n";
		}
		ret = ret + "}\n\n";

		// Destructor
		ret = ret + componentId + "::~" + componentId + "()\n{\n\n}\n\n";

		ret = ret + "void " + componentId + "::" + initState.getId() + "_state_code(void)\n{\n";
		ret = ret + BuilderGUI.BEFORE_STATE_CODE + initState.getId() + "_state_code\n";

		//Coge el código que se le haya puesto en el editor de código de VICODE (botón derecho en el estado o transición),
		//o que ya hubiera en el .cpp (que se obtuvo al cargar el xml, en la llamada al método 'parseCPP()').
		ret = ret + initState.getStateFrame().getCode();

		ret = ret + BuilderGUI.CODE_END + initState.getId() + "_state_code\n";
		ret = ret + "}\n\n";


		it = states.iterator();
		while (it.hasNext()) {
			s = (RegularState) it.next();
			ret = ret + "void " + componentId + "::" + s.getId() + "_state_code(void)\n{\n";
			ret = ret + BuilderGUI.BEFORE_STATE_CODE + s.getId() + "_state_code\n";

			ret = ret + s.getStateFrame().getCode();

			ret = ret + BuilderGUI.CODE_END + s.getId() + "_state_code\n";
			ret = ret + "}\n\n";
		}

		it = transitions.iterator();
		while (it.hasNext()) {
			t = (Transition) it.next();
			ret = ret + "bool " + componentId + "::" + t.getId() + "_transition_code(void)\n{\n";
			ret = ret + BuilderGUI.BEFORE_TRANSITION_CODE + t.getId() + "_transition_code\n";

			ret = ret + t.getStateFrame().getCode();

			ret = ret + BuilderGUI.CODE_END + t.getId() + "_transition_code\n";
			ret = ret + "}\n\n";
		}

		ret = ret + "void\n" + componentId + "::step(void)\n{\n";

		if (autoRestart) {
			ret = ret + "\tif(testRestart())\n\t\trestart();\n\n";
		}

		ret = ret + "\tswitch (state)\n\t{\n";

		ret = ret + "\tcase " + initState.getId() + ":\n";

		//Step() of the required components (initial state)
		requirements = initState.getRequirements();
		reqsIter = requirements.iterator();
		while (reqsIter.hasNext()) {
			r = (Requirement) reqsIter.next();
			ret = ret + "\t\t_" + r.getSource().getId() + "->step();\n";
		}

		ret = ret + "\n\t\tif (isTime2Run()) {\n";
		ret = ret + "\t\t\t" + initState.getId() + "_state_code();\n\n";


		//Transition to other states from initial
		Collections.sort(transitions);

		it = transitions.iterator();
		int nTransitions = 0;
		while (it.hasNext()) {
			t = (Transition) it.next();
			if (t.getSource().getId().equals(initState.getId())) {
				nTransitions++;
			}
		}

		int countTransitions = 0;
		it = transitions.iterator();
		while (it.hasNext()) {
			t = (Transition) it.next();

			if (t.getSource().getId().equals(initState.getId())) {
				countTransitions++;
				if (countTransitions == 1) {
					ret = ret + "\t\t\tif (" + t.getId() + "_transition_code()) {\n";
				} else {
					ret = ret + "if (" + t.getId() + "_transition_code()) {\n";
				}

				ret = ret + "\t\t\t\tstate = " + t.getTarget().getId() + ";\n";
				if (t.getTarget().isTimerDriven()) {
					ret = ret + "\t\t\t\tresetStopWatch();\n";
				}

				ret = ret + "\t\t\t}\n";

				if (countTransitions < nTransitions) {
					ret = ret + "\t\t\telse ";
				}
			}
		}

		ret = ret + "\t\t}\n\n";

		//Step() of the moduled components (initial state)
		modulations = initState.getModulations();
		modsIter = modulations.iterator();
		while (modsIter.hasNext()) {
			m = (Modulation) modsIter.next();
			ret = ret + "\t\t_" + m.getTarget().getId() + "->step();\n";
		}

		ret = ret + "\t\tbreak;\n";

		it = states.iterator();
		while (it.hasNext()) {
			s = (RegularState) it.next();
			ret = ret + "\tcase " + s.getId() + ":\n";

			//Step() of the required components
			requirements = s.getRequirements();
			reqsIter = requirements.iterator();
			while (reqsIter.hasNext()) {
				Requirement req = (Requirement) reqsIter.next();
				ret = ret + "\t\t_" + req.getSource().getId() + "->step();\n";
			}

			ret = ret + "\n\t\tif (isTime2Run()) {\n";
			ret = ret + "\t\t\t" + s.getId() + "_state_code();\n\n";

			//Transition to other states
			Collections.sort(transitions);

			Iterator it2 = transitions.iterator();
			nTransitions = 0;
			while (it2.hasNext()) {
				t = (Transition) it2.next();
				if (t.getSource().getId().equals(s.getId())) {
					nTransitions++;
				}
			}

			countTransitions = 0;

			it2 = transitions.iterator();
			while (it2.hasNext()) {
				t = (Transition) it2.next();

				if (t.getSource().getId().equals(s.getId())) {
					countTransitions++;
					if (countTransitions == 1) {
						ret = ret + "\t\t\tif (" + t.getId() + "_transition_code()) {\n";
					} else {
						ret = ret + "if (" + t.getId() + "_transition_code()) {\n";
					}
					ret = ret + "\t\t\t\tstate = " + t.getTarget().getId() + ";\n";

					if (t.getTarget().isTimerDriven()) {
						ret = ret + "\t\t\t\tresetStopWatch();\n";
					}

					ret = ret + "\t\t\t}\n";

					if (countTransitions < nTransitions) {
						ret = ret + "\t\t\telse ";
					}
				}
			}

			ret = ret + "\t\t}\n\n";

			//Step() of the moduled components
			modulations = s.getModulations();
			modsIter = modulations.iterator();
			while (modsIter.hasNext()) {
				m = (Modulation) modsIter.next();
				ret = ret + "\t\t_" + m.getTarget().getId() + "->step();\n";
			}

			ret = ret + "\t\tbreak;\n";
		}
		ret = ret + "\tdefault:\n\t\tcout << \"[" + componentId + "::step()] Invalid state\\n\";\n\t}\n";

		ret = ret + "}\n\n";

		if (autoRestart) {
			ret = ret + "bool\n" + componentId + "::testRestart()\n{\n";
			ret = ret + "\tlong elapsed;\n";
			ret = ret + "\tlong now;\n\n";
			ret = ret + "\tnow = getCurrentTime();\n";
			ret = ret + "\telapsed = getCurrentTime() - lastExecTimestamp;\n";
			ret = ret + "\tlastExecTimestamp = now;\n\n";
			ret = ret + "\treturn elapsed > (freq_time*10.0);\n}\n\n";
			ret = ret + "void\n" + componentId + "::restart()\n{\n";
			ret = ret + "\tstate = Initial;\n\n\tresetStopWatch();\n}\n\n";
		}

		//Aquí es donde le enchufa el código auxiliar (auxcode) que ya hubiera en el .cpp cargado.
		ret = ret + auxcodeC + "\n";

		return ret;
	}


	public String generateXML(String componentId) {
		String ret = new String("");

		ret = ret + "<component id=\"" + componentId + "\">\n";
		ret = ret + "<conf autorestart=\"";
		if (autoRestart) {
			ret = ret + "1";
		} else {
			ret = ret + "0";
		}
		ret = ret + "\"/>\n";

		Point2D pos;

		Iterator itc = components.iterator();
		Component c;
		while (itc.hasNext()) {
			c = (Component) itc.next();
			//getGlobalBounds() coge sólo los bordes del propio círculo nada más,
			//sin incluir a los de su hijo (el texto).
			pos = c.getGlobalBounds().getOrigin();
			ret = ret + "\t<dependenceComponent id=\"" + c.getId() + "\" x=\"" + pos.getX() + "\" y=\"" + pos.getY()
				+ "\"/>\n";
		}

		Point2D initpos = initState.getGlobalBounds().getOrigin();
		ret = ret + "\t<state id=\"initial\" x=\"" + initpos.getX() + "\" y=\"" + initpos.getY()
			+ "\" code=\"" + initState.getId() + "_state_code\" >\n";

		/*Iterator itir = initState.getRequirements().iterator();
		while (itir.hasNext()) {
		Requirement r = (Requirement) itir.next();
		ret = ret + "\t\t<requirement id=\"" + r.getSource().getId() + "\"/>\n";
		}
		Iterator itim = initState.getModulations().iterator();
		while (itim.hasNext()) {
		Modulation m = (Modulation) itim.next();
		ret = ret + "\t\t<modulation id=\"" + m.getTarget().getId() + "\"/>\n";
		}*/
		ret = ret + "\t</state>\n";

		Iterator it = states.iterator();
		RegularState s;
		while (it.hasNext()) {
			s = (RegularState) it.next();

			pos = s.getGlobalBounds().getOrigin();
			ret = ret + "\t<state id=\"" + s.getId() + "\" x=\"" + pos.getX() + "\" y=\"" + pos.getY()
				+ "\" code=\"" + s.getId() + "_state_code\">\n";

			Iterator itr = s.getRequirements().iterator();
			Requirement r;
			while (itr.hasNext()) {
				r = (Requirement) itr.next();
				ret = ret + "\t\t<requirement id=\"" + r.getSource().getId() + "\"/>\n";
			}
			Iterator itm = s.getModulations().iterator();
			Modulation m;
			while (itm.hasNext()) {
				m = (Modulation) itm.next();
				if (m != null) {
					ret = ret + "\t\t<modulation id=\"" + m.getTarget().getId() + "\"/>\n";
				} else {
					s.getModulations().remove(m);
				}
			}

			ret = ret + "\t</state>\n";
		}

		it = transitions.iterator();
		Transition t;
		while (it.hasNext()) {
			t = (Transition) it.next();

			pos = t.getNode().getGlobalBounds().getOrigin();
			ret = ret + "\t<transition id=\"" + t.getId() + "\" x=\"" + pos.getX() + "\" y=\"" + pos.getY()
				+ "\" code=\"" + t.getId() + "_transition_code\" source=\""
				+ t.getSource().getId() + "\" target=\"" + t.getTarget().getId() + "\" priority=\"" + t.getPriority()
				+ "\" timer=\"" + t.getTimer() + "\"/>\n";
			//ret = ret+"\t\t<source id=\""+t.getSource().getId()+"\"/>\n";
			//ret = ret+"\t\t<target id=\""+t.getTarget().getId()+"\"/>\n";
			//ret = ret+"\t</transition>\n";
		}

		ret = ret + "</component>\n";

		return ret;
	}

}
