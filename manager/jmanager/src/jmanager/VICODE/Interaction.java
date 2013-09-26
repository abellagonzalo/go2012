package jmanager.VICODE;

import edu.umd.cs.piccolo.PLayer;
import edu.umd.cs.piccolo.nodes.PPath;
import java.awt.BasicStroke;
import java.awt.Color;
import java.awt.geom.Point2D;

/**
 * Clase padre de Modulation y Requirement que contiene
 * el código común a ambas.
 *
 * @author jesus
 */
public abstract class Interaction extends PPath {

	protected NodeItem source = null;
	protected NodeItem target = null;
	protected BuilderGUI builderGUI;
	protected boolean visible = false;

	protected Interaction() {
		super();
		builderGUI = BuilderGUI.getInstance();
	}

	public void setTarget(NodeItem trg) {
		target = trg;
	}

	public void update() {

		this.reset();

		if (target != null) {
			double x1, y1, x2, y2;
			Color color;

			//Valen: getFullBoundsReference(), getGlobalBounds() (y por extensión getGlobalFullBounds) y getFullBounds().
			//La mejor es getGlobalBounds() ya que proporciona independencia respecto de los hijos que pueda tener la figura.
			Point2D start = source.getGlobalBounds().getCenter2D();
			Point2D end = target.getGlobalBounds().getCenter2D();

			double ang = Math.atan2(end.getX() - start.getX(), end.getY() - start.getY());

			x1 = start.getX() + 46.0 * Math.sin(ang);
			y1 = start.getY() + 46.0 * Math.cos(ang);

			x2 = end.getX() - 46.0 * Math.sin(ang);
			y2 = end.getY() - 46.0 * Math.cos(ang);

			//Set line width and color
			setStroke(new BasicStroke(2.0f));
			color = Color.BLUE;
			setStrokePaint(color);

			moveTo((float) x1, (float) y1);
			lineTo((float) x2, (float) y2);

			//Flecha
			ang = Math.atan2(x2 - x1, y2 - y1);

			double p1a = ang - Math.toRadians(150.0);
			double p1x = 20 * Math.sin(p1a);
			double p1y = 20 * Math.cos(p1a);

			moveTo((float) x2, (float) y2);
			lineTo((float) (x2 + p1x), (float) (y2 + p1y));

			moveTo((float) x2, (float) y2);

			p1a = ang - Math.toRadians(200.0);
			p1x = 20 * Math.sin(p1a);
			p1y = 20 * Math.cos(p1a);
			lineTo((float) (x2 + p1x), (float) (y2 + p1y));
		}
	}

	public abstract void selfDestroy();

	@Override
	public void setVisible(boolean vis) {

		if (visible != vis) {
			visible = vis;

			PLayer connectLayer = builderGUI.getConnectionLayer();

			if (visible) {
				if (!connectLayer.isAncestorOf(this)) {
					connectLayer.addChild(this);
				}
				update();

			} else {
				if (connectLayer.isAncestorOf(this)) {
					connectLayer.removeChild(this);
				}
			}
		}
	}
}
