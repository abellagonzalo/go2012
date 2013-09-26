
package jmanager.VICODE.xmlparsers;

import java.util.Iterator;
import java.util.Map;

//import javax.swing.tree.DefaultMutableTreeNode;
import jmanager.VICODE.BuilderGUI;
import jmanager.VICODE.State;
import jmanager.VICODE.Transition;

import org.xml.sax.Attributes;
import org.xml.sax.ContentHandler;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
//import lejos.nxt.LCD;

/**
 * <b><code>JTreeContentHandler</code></b> implements the SAX
 *   <code>ContentHandler</code> interface and defines callback
 *   behavior for the SAX callbacks associated with an XML
 *   document's content, bulding up JTree nodes.
 */
public class TransitionsLoaderContentHandler implements ContentHandler {

	private boolean endProgram = false;
	/** Hold onto the locator for location information */
	private Locator locator;
	/** Store URI to prefix mappings */
	private Map namespaceMappings;
	private int waituntilTimeCounter = 0;

	public TransitionsLoaderContentHandler() {
	}

	/**
	 * <p>
	 *  Provide reference to <code>Locator</code> which provides
	 *    information about where in a document callbacks occur.
	 * </p>
	 *
	 * @param locator <code>Locator</code> object tied to callback
	 *        process
	 */
	public void setDocumentLocator(Locator locator) {
		// Save this for later use
		this.locator = locator;
	}

	/**
	 * <p>
	 *  This indicates the start of a Document parse-this precedes
	 *    all callbacks in all SAX Handlers with the sole exception
	 *    of <code>{@link #setDocumentLocator}</code>.
	 * </p>
	 *
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void startDocument() throws SAXException {
		//No se emplea
	}

	/**
	 * <p>
	 *  This indicates the end of a Document parse-this occurs after
	 *    all callbacks in all SAX Handlers.</code>.
	 * </p>
	 *
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void endDocument() throws SAXException {
		//No se emplea
	}

	/**
	 * <p>
	 *   This indicates that a processing instruction (other than
	 *     the XML declaration) has been encountered.
	 * </p>
	 *
	 * @param target <code>String</code> target of PI
	 * @param data <code>String</code containing all data sent to the PI.
	 *               This typically looks like one or more attribute value
	 *               pairs.
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void processingInstruction(String target, String data) throws SAXException {
		//Empty
	}

	/**
	 * <p>
	 *   This indicates the beginning of an XML Namespace prefix
	 *     mapping. Although this typically occurs within the root element
	 *     of an XML document, it can occur at any point within the
	 *     document. Note that a prefix mapping on an element triggers
	 *     this callback <i>before</i> the callback for the actual element
	 *     itself (<code>{@link #startElement}</code>) occurs.
	 * </p>
	 *
	 * @param prefix <code>String</code> prefix used for the namespace
	 *                being reported
	 * @param uri <code>String</code> URI for the namespace
	 *               being reported
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void startPrefixMapping(String prefix, String uri) {
		// No visual events occur here.
		namespaceMappings.put(uri, prefix);
	}

	/**
	 * <p>
	 *   This indicates the end of a prefix mapping, when the namespace
	 *     reported in a <code>{@link #startPrefixMapping}</code> callback
	 *     is no longer available.
	 * </p>
	 *
	 * @param prefix <code>String</code> of namespace being reported
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void endPrefixMapping(String prefix) {
		// No visual events occur here.
		for (Iterator i = namespaceMappings.keySet().iterator();
			i.hasNext();) {

			String uri = (String) i.next();
			String thisPrefix = (String) namespaceMappings.get(uri);
			if (prefix.equals(thisPrefix)) {
				namespaceMappings.remove(uri);
				break;
			}
		}
	}

	/**
	 * <p>
	 *   This reports the occurrence of an actual element. It includes
	 *     the element's attributes, with the exception of XML vocabulary
	 *     specific attributes, such as
	 *     <code>xmlns:[namespace prefix]</code> and
	 *     <code>xsi:schemaLocation</code>.
	 * </p>
	 *
	 * @param namespaceURI <code>String</code> namespace URI this element
	 *               is associated with, or an empty <code>String</code>
	 * @param localName <code>String</code> name of element (with no
	 *               namespace prefix, if one is present)
	 * @param qName <code>String</code> XML 1.0 version of element name:
	 *                [namespace prefix]:[localName]
	 * @param atts <code>Attributes</code> list for this element
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void startElement(String namespaceURI, String localName,
		String qName, Attributes atts)
		throws SAXException {

		BuilderGUI gui = BuilderGUI.getInstance();

		if (localName.equals("transition")) {
			String id = new String();
			String srcid = new String();
			String trgid = new String();
			double x = 0, y = 0;
			int priority = 1;
			boolean timer = false;

			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("id")) {
					id = atts.getValue(i).toString();
				}
			}

			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("x")) {
					x = Double.parseDouble(atts.getValue(i).toString());
				}
			}
			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("y")) {
					y = Double.parseDouble(atts.getValue(i).toString());
				}
			}
			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("source")) {
					srcid = atts.getValue(i).toString();
				}
			}
			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("target")) {
					trgid = atts.getValue(i).toString();
				}
			}
			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("priority")) {
					priority = Integer.parseInt(atts.getValue(i).toString());
				}
			}

			for (int i = 0; i < atts.getLength(); i++) {
				if (atts.getLocalName(i).equals("timer")) {
					timer = Boolean.parseBoolean(atts.getValue(i).toString());
				}
			}

			State src = gui.getState(srcid);
			State trg = gui.getState(trgid);

			if (src == null) {
				System.err.println("error " + srcid + " is null");
			}
			if (trg == null) {
				System.err.println("error " + trgid + " is null");
			}

			Transition t = gui.addTransition(src, trg);
			t.setId(id);
			t.setXYpos(x, y);
			t.setPriority(priority);
			t.setTimer(timer);
		}
	}

	/**
	 * <p>
	 *   Indicates the end of an element
	 *     (<code>&lt;/[element name]&gt;</code>) is reached. Note that
	 *     the parser does not distinguish between empty
	 *     elements and non-empty elements, so this occurs uniformly.
	 * </p>
	 *
	 * @param namespaceURI <code>String</code> URI of namespace this
	 *                element is associated with
	 * @param localName <code>String</code> name of element without prefix
	 * @param qName <code>String</code> name of element in XML 1.0 form
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void endElement(String namespaceURI, String localName,
		String qName) throws SAXException {

		if (endProgram == false) {
			//System.out.println(localName);
		}
		//System.out.println(qName);

	}

	/**
	 * <p>
	 *   This reports character data (within an element).
	 * </p>
	 *
	 * @param ch <code>char[]</code> character array with character data
	 * @param start <code>int</code> index in array where data starts.
	 * @param length <code>int</code> index in array where data ends.
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void characters(char[] ch, int start, int length)
		throws SAXException {

		/*
		String s = new String(ch, start, length);
		DefaultMutableTreeNode data =
		new DefaultMutableTreeNode("Character Data: '" + s + "'");
		current.add(data);*/
	}

	/**
	 * <p>
	 * This reports whitespace that can be ignored in the
	 * originating document. This is typically invoked only when
	 * validation is ocurring in the parsing process.
	 * </p>
	 *
	 * @param ch <code>char[]</code> character array with character data
	 * @param start <code>int</code> index in array where data starts.
	 * @param end <code>int</code> index in array where data ends.
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void ignorableWhitespace(char[] ch, int start, int length)
		throws SAXException {
		// This is ignorable, so don't display it
	}

	/**
	 * <p>
	 *   This reports an entity that is skipped by the parser. This
	 *     should only occur for non-validating parsers, and then is still
	 *     implementation-dependent behavior.
	 * </p>
	 *
	 * @param name <code>String</code> name of entity being skipped
	 * @throws <code>SAXException</code> when things go wrong
	 */
	public void skippedEntity(String name) throws SAXException {
		//DefaultMutableTreeNode skipped =
		//    new DefaultMutableTreeNode("Skipped Entity: '" + name + "'");
		//current.add(skipped);
	}
}
