package jmanager.VICODE;

import edu.umd.cs.piccolo.event.PDragEventHandler;
import edu.umd.cs.piccolo.event.PInputEvent;
import edu.umd.cs.piccolo.nodes.PImage;
import edu.umd.cs.piccolo.nodes.PText;
import edu.umd.cs.piccolo.util.PDimension;

/**
 *
 * @author jesus
 */
public class TextNode extends PText {

	public TextNode(String id, final PImage parent) {
		super(id);

		PDragEventHandler myPDragEventHandler = new PDragEventHandler() {

			@Override
			public void mouseDragged(PInputEvent event) {

				//NO hace falta llamar al drag del popio nodo de TEXTO con super.drag(event).
				//porque EL PADRE YA MUEVE A TODOS SUS HIJOS.
				
				final PDimension delta = event.getDelta();

				//ESTO SIRVE PARA QUE AL ARRASTRAR EL TEXTO SE MUEVA TAMBIÉN EL NODO PADRE:
				parent.translate(delta.width, delta.height);

				MouseDraggingController.dragControl(parent); //OK

				event.setHandled(true); //para que no use el mouseDragged del círculo padre.

				//TOOD: establecer el texto de las transiciones como un TextNode hijo de ellas.
				if (parent instanceof NodeItem) {
					NodeItem parentNode = (NodeItem) parent;
					parentNode.updateRelations();
				}
			}
		};

		addInputEventListener(myPDragEventHandler);
	}
}

