package jmanager.VICODE;

import edu.umd.cs.piccolo.PNode;
import edu.umd.cs.piccolo.util.PBounds;
import java.awt.Dimension;
import java.awt.Rectangle;

/**
 *
 * @author jesus
 */
public class MouseDraggingController {

	private static Builder builder = Builder.getInstance();

	private static BuilderGUI builderGUI = BuilderGUI.getInstance();

	//Vale cualquiera de las 3 siguientes, pero veo más clara la primera.
	private static Rectangle limitesGUI = new Rectangle(0, 0, builderGUI.getWidth(), builderGUI.getHeight());
	//private static Rectangle limitesGUI = builderGUI.getRoot().getFullBounds().getBounds();
	//private static Rectangle limitesGUI = builderGUI.getBounds();

	private static PBounds posAfterDrag;

	public MouseDraggingController() {
	}

	public static void dragControl(PNode figura) {

		//getGlobalBounds() sólo coge los límites del nodo en cuestión (el círculo), en
		//coordenadas globales, y no incluye los límites del hijo (el nodo de texto).
		posAfterDrag = figura.getGlobalBounds();

		//LÍMITE DERECHO
		if (posAfterDrag.getMaxX() > limitesGUI.getMaxX()) {
			/* setBounds() no valía para el lado derecho. Lo que hace es desplazarlo a la posición 500 en horizontal, es decir,
			le suma el nuevo limite en X, en lugar de establecerlo a 500. */
			//setBounds(500, 5, ultimaPosValida.getWidth(), ultimaPosValida.getHeight());

			//System.out.println("sale por la derecha");

			//Así valía cuando lo tenía para que no se saliera en lugar de agrandarse.
			//animateTransformToBounds(limitesGUI.getMaxX() - 101, posAfterDrag.getY(), posAfterDrag.getWidth(), posAfterDrag.getHeight(), 0);

			//Con setSize no hacía nada. Hay que usar setPreferedSize().
			///Tampoco resultaba con setBounds(). Sólo vale para TRASLADAR el panel blanco, pero no lo agranda.
			//builderGUI.setBounds(0, 0, 2000, 700);

			builderGUI.setPreferredSize(new Dimension((int) posAfterDrag.getMaxX(), builderGUI.getHeight()));
			//builderGUI.setBounds(0, 0, (int) posAfterDrag.getMaxX(), builderGUI.getHeight()); //<-- OJO Lo traslada pero no lo agranda.
			limitesGUI.setBounds(0, 0, (int) posAfterDrag.getMaxX(), builderGUI.getHeight());

			//REEMPLAZA el contenido del jScrollPane
			builder.getJScrollPane1().setViewportView(builderGUI);

			//Esto no hace falta porque ya se actualizan solos.
			//builderGUI.setBounds(0, 0, (int) posAfterDrag.getMaxX(), builderGUI.getHeight());
		}
		//LÍMITE IZQUIERDO
		if (posAfterDrag.getX() < limitesGUI.getX()) {

			//animateTransformToBounds() funciona bien siempre que los bordes se obtengan con getGlobalBounds(),
			//para que no coja los bordes del nodo texto hijo, ya que el texto puede salirse por fuera.
			//System.out.println(posAfterDrag.getWidth());
			figura.animateTransformToBounds(limitesGUI.getX(), posAfterDrag.getY(), posAfterDrag.getWidth(), posAfterDrag.getHeight(), 0);

			/*
			//Con 0.5 en ambos puntos para la figura, hace lo mismo que el animateTransformToBounds.
			//Y esta manera funciona tanto con el getGlobalBounds() como con el getFullBoundsReference().
			//Sólo que con el fullBounds coge los bordes del texto también, y por tanto
			//traslada la figura de acuerdo a los bordes del texto. El texto siempre va
			//a quedar dentro del panel.
			
			Point2D srcPt = new Point2D.Double(0.5, 0.0);
			Point2D destPt = new Point2D.Double(0.5, 0.0);
			PBounds destBounds = new PBounds(limitesGUI.getX(), posAfterDrag.getY(), posAfterDrag.getWidth(), posAfterDrag.getHeight());

			figura.position(srcPt, destPt, destBounds, 0)
			*/
		}
		//LÍMITE INFERIOR, incluido la ESQUINA DERECHA.
		//Como no puede estirar el panel en los dos direcciones a la vez,
		//por tema de que SERÍAN DOS TAREAS SIMULTÁNEAS, HAY QUE ELEGIR CUÁL VA A HACER PRIMERO.
		if (posAfterDrag.getMaxY() > limitesGUI.getMaxY()) {

			//Si está en la esquina, primero estira a la derecha y luego hacia abajo si seguimos moviendo hacia abajo.
			//Pero si estiramos primero abajo y movemos a la vez a la derecha, sólo estirará hacia abajo.
			if (posAfterDrag.getMaxX() > limitesGUI.getMaxX()) {
				builderGUI.setPreferredSize(new Dimension((int) posAfterDrag.getMaxX(), builderGUI.getHeight()));
			}
			builderGUI.setPreferredSize(new Dimension(builderGUI.getWidth(), (int) posAfterDrag.getMaxY()));
			limitesGUI.setBounds(0, 0, builderGUI.getWidth(), (int) posAfterDrag.getMaxY());

			//REEMPLAZA el contenido del jScrollPane
			builder.getJScrollPane1().setViewportView(builderGUI);

			//Para que no se saliese en lugar de agrandar usaba esta:
			//animateTransformToBounds(posAfterDrag.getX(), limitesGUI.getMaxY() - 101, posAfterDrag.getWidth(), posAfterDrag.getHeight(), 0);
		}
		//LÍMITE SUPERIOR
		if (posAfterDrag.getY() < limitesGUI.getY()) {
			//Con tiempo 0 milisegundos, la etiqueta de texto se actualiza correctamente siempre y de forma inmediata.
			figura.animateTransformToBounds(posAfterDrag.getX(), limitesGUI.getY(), posAfterDrag.getWidth(), posAfterDrag.getHeight(), 0);
		}
	}
}
