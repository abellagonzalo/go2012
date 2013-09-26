package jmanager.VICODE;

import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.event.PBasicInputEventHandler;
import edu.umd.cs.piccolo.event.PDragEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;
import edu.umd.cs.piccolo.nodes.PImage;
import edu.umd.cs.piccolo.nodes.PPath;
import edu.umd.cs.piccolo.nodes.PText;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.geom.Point2D;
import java.util.ListIterator;

/**
 *
 * @author paco
 */
public class Transition implements Comparable<Transition> {

	private boolean timer = false;
	private String id = new String();
	private PImage node = null;
	private PPath first = null, second = null;
	private PText idText = null;
	private Transition instance = null;
	private TransitionCodeFrame transitionCodeFrame = new TransitionCodeFrame();
	private State source = null, target = null;
	private BuilderGUI builderGUI;
	private boolean visible = false;
	private int priority = -1;
	
	//Distancia en píxeles desde el nombre hasta el círculo transición.
	private final int Y_DISTANCE_TO_TEXTLABEL = 30;


	/* CLASE interna para los eventos del ratón en el círculo de una Transición */
	private class TransitionMouseEventHandler extends PBasicInputEventHandler {

		private PDragEventHandler dragEventHandler;

		public TransitionMouseEventHandler() {

			dragEventHandler = new PDragEventHandler() {

				@Override
				public void mouseDragged(PInputEvent event) {
					super.drag(event); //PRODUCE el ARRASTRE
					MouseDraggingController.dragControl(node);
					instance.update(); //Actualiza las flechas de la transición
				}
			};
		}

		public PDragEventHandler getDragEventHandler() {
			return dragEventHandler;
		}

		@Override
		public void mousePressed(PInputEvent e) {
			if (e.isRightMouseButton()) {
				if (builderGUI.getMode() == BuilderGUI.NORMAL) {
					transitionCodeFrame.setVisible(true);
				}
			}
		}

		@Override
		public void mouseClicked(PInputEvent e) {
			if (e.isLeftMouseButton()) {
				if (builderGUI.getMode() == BuilderGUI.REMOVE_TRANSITION) {
					builderGUI.getRemoveTransitionAsker().setRemoveTransition(instance);
					builderGUI.getRemoveTransitionAsker().showDialog();
				}
				e.setHandled(true);
			}
		}
	}


	/* Constructor */
	public Transition(State src, State trg) {
		instance = this;
		builderGUI = BuilderGUI.getInstance();

		idText = new PText("");
		node = new PImage(Transition.class.getResource("resources/NodeTransitionSmall.png"));
		first = new PPath();
		second = new PPath();

		//Estados origen y destino
		source = src;
		target = trg;

		this.setVisible(false);
		this.update();

		transitionCodeFrame.setVisible(false);
		transitionCodeFrame.setTransition(this);
		transitionCodeFrame.setSourceState(source);

		TransitionMouseEventHandler transitionEvents = new TransitionMouseEventHandler();

		node.addInputEventListener(transitionEvents);

		//OJO Añadir el evento de arrastrar el ÚLTIMO.
		node.addInputEventListener(transitionEvents.getDragEventHandler());
	}

	public void update() {

		first.reset();
		second.reset();

		if (target != null) {
			double x1, y1, x2, y2, x3, y3;
			Color color;

			Point2D start = source.getGlobalBounds().getCenter2D();
			Point2D end = target.getGlobalBounds().getCenter2D();
			Point2D medium = node.getGlobalBounds().getCenter2D();

			double ang = Math.atan2(medium.getX() - start.getX(), medium.getY() - start.getY());

			x1 = start.getX() + 46.0 * Math.sin(ang);
			y1 = start.getY() + 46.0 * Math.cos(ang);

			ang = Math.atan2(end.getX() - medium.getX(), end.getY() - medium.getY());

			x2 = end.getX() - 46.0 * Math.sin(ang);
			y2 = end.getY() - 46.0 * Math.cos(ang);

			//First line
			ang = Math.atan2(x1 - medium.getX(), y1 - medium.getY());
			x3 = medium.getX() + 15.0 * Math.sin(ang);
			y3 = medium.getY() + 15.0 * Math.cos(ang);

			first.setStroke(new BasicStroke(4.0f));
			color = Color.RED;
			first.setStrokePaint(color);

			first.moveTo((float) x1, (float) y1);
			first.lineTo((float) x3, (float) y3);

			//Flecha
			ang = Math.atan2(x3 - x1, y3 - y1);

			//150º para el primer segmento de la punta de la flecha.
			double p1a = ang - Math.toRadians(150.0);
			double p1x = 25 * Math.sin(p1a);
			double p1y = 25 * Math.cos(p1a);

			first.moveTo((float) x3, (float) y3);
			first.lineTo((float) (x3 + p1x), (float) (y3 + p1y));

			first.moveTo((float) x3, (float) y3);

			//200º para el otro segmento de la flecha.
			p1a = ang - Math.toRadians(200.0);
			p1x = 25 * Math.sin(p1a);
			p1y = 25 * Math.cos(p1a);
			first.lineTo((float) (x3 + p1x), (float) (y3 + p1y));

			//Second line
			ang = Math.atan2(x2 - medium.getX(), y2 - medium.getY());
			x3 = medium.getX() + 15.0 * Math.sin(ang);
			y3 = medium.getY() + 15.0 * Math.cos(ang);

			second.setStroke(new BasicStroke(4.0f));
			color = Color.RED;
			second.setStrokePaint(color);

			second.moveTo((float) x3, (float) y3);
			second.lineTo((float) x2, (float) y2);

			//Flecha
			ang = Math.atan2(x2 - x3, y2 - y3);

			p1a = ang - Math.toRadians(150.0);
			p1x = 25 * Math.sin(p1a);
			p1y = 25 * Math.cos(p1a);

			second.moveTo((float) x2, (float) y2);
			second.lineTo((float) (x2 + p1x), (float) (y2 + p1y));

			second.moveTo((float) x2, (float) y2);

			p1a = ang - Math.toRadians(200.0);
			p1x = 25 * Math.sin(p1a);
			p1y = 25 * Math.cos(p1a);
			second.lineTo((float) (x2 + p1x), (float) (y2 + p1y));

			//TEXTO
			//double rot = idText.getRotation();
			Point2D pos = node.getGlobalBounds().getCenter2D();

			//idText.rotateInPlace(-ang+(Math.toRadians(90.0))-rot);
			idText.setX(pos.getX() - (idText.getWidth() / 2.0));

			//La etiqueta con el nombre de la transición se pone encima
			//a una distancia de 'YDistanceToLabel' píxeles.
			idText.setY(pos.getY() - (idText.getHeight() / 2.0) - Y_DISTANCE_TO_TEXTLABEL);
		}
	}

	public State getSource() {
		return source;
	}

	public void setTarget(State targetState) {
		target = targetState;

		if (targetState != null) {
			Integer cont = 0;
			id = source.getId() + "2" + target.getId() + cont.toString();

			while (!this.setId(id)) {
				id = source.getId() + "2" + target.getId() + (++cont).toString();
			}
			setVisible(true);
			transitionCodeFrame.setIdTextField(id);

		} else {
			id = source.getId() + "2any";
			transitionCodeFrame.setIdTextField(id);
			idText.setText(id);
		}
	}

	public boolean setId(String newid) {
		ListIterator it = builderGUI.getTransitions().listIterator();

		Transition t;
		while (it.hasNext()) {
			t = (Transition) it.next();

			if (t.getId().equals(newid)) {
				return false;
			}
		}
		id = newid;
		idText.setText(id);
		transitionCodeFrame.setIdTextField(id);

		this.update();
		return true;
	}

	public State getTarget() {
		return target;
	}

	public void selfDestroy() {
		source.removeOutcomingTransition(this);

		if (target != null) {
			target.removeIncomingTransition(this);
		}

		this.setVisible(false);
	}

	public String getId() {
		return id;
	}

	public PText getText() {
		return idText;
	}

	public void setVisible(boolean vis) {

		if (visible != vis) {
			visible = vis;

			PLayer nodetransLayer = builderGUI.getNodeTransitionLayer();
			PLayer transLayer = builderGUI.getTransitionLayer();
			PLayer textLayer = builderGUI.getTextLayer();

			if (visible) {
				if (!textLayer.isAncestorOf(idText)) {
					textLayer.addChild(idText);
				}
				if (!transLayer.isAncestorOf(first)) {
					transLayer.addChild(first);
				}
				if (!transLayer.isAncestorOf(second)) {
					transLayer.addChild(second);
				}
				if (!nodetransLayer.isAncestorOf(node)) {
					nodetransLayer.addChild(node);
				}

				Point2D start = source.getGlobalBounds().getCenter2D();
				Point2D end = target.getGlobalBounds().getCenter2D();

				node.setX((start.getX() + end.getX()) / 2);
				node.setY((start.getY() + end.getY()) / 2);

				update();

			} else { //visible es FALSE
				if (textLayer.isAncestorOf(idText)) {
					textLayer.removeChild(idText);
				}
				if (transLayer.isAncestorOf(first)) {
					transLayer.removeChild(first);
				}
				if (transLayer.isAncestorOf(second)) {
					transLayer.removeChild(second);
				}
				if (nodetransLayer.isAncestorOf(node)) {
					nodetransLayer.removeChild(node);
				}
			}
		}
	}

	public PImage getNode() {
		return node;
	}

	public TransitionCodeFrame getStateFrame() {
		return transitionCodeFrame;
	}

	public void setXYpos(double posx, double posy) {
		node.setX(posx);
		node.setY(posy);

		this.update();
	}

	public int getPriority() {
		return priority;
	}

	public void setPriority(int p) {
		priority = p;
		transitionCodeFrame.setPriorityLabel();
	}

	public void setTimer(boolean value) {
		timer = value;
		transitionCodeFrame.setTimerCheckBox();
	}

	public boolean getTimer() {
		return timer;
	}

	public int compareTo(Transition arg0) {
		if (getPriority() == arg0.getPriority()) {
			return 0;
		} else if (getPriority() < arg0.getPriority()) {
			return -1;
		} else {
			return 1;
		}
	}

	@Override
	public String toString() {
		return id + "(" + priority + ")";
	}

	public void closeFrame() {
		if (transitionCodeFrame.isVisible()) {
			transitionCodeFrame.setVisible(false);
		}

	}
}
