package jmanager.VICODE.xmlparsers;

import java.io.IOException;
import java.util.logging.Level;
import java.util.logging.Logger;

//Packages for using a SAX XML Parser
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.XMLReaderFactory;

/**
 * Class designed to convert a XML file into a Java file.
 * @author fmartin
 *
 */
public class Loader {

    /** Default parser to use */
    private String vendorParserClass = "";//"org.apache.xerces.parsers.SAXParser";
    
    private InfoLoaderContentHandler lich;
    private ComponentsLoaderContentHandler lcch;
    private StatesLoaderContentHandler lsch;
    private TransitionsLoaderContentHandler ltch;
    private LoaderErrorHandler leh;
    private String xmlURI;

    private XMLReader infoReader = XMLReaderFactory.createXMLReader();
    private XMLReader componentsReader = XMLReaderFactory.createXMLReader();
    private XMLReader stateReader = XMLReaderFactory.createXMLReader();
    private XMLReader transitionReader = XMLReaderFactory.createXMLReader();

    public Loader(String uri) throws IOException, SAXException {
        xmlURI = uri;
        lich = new InfoLoaderContentHandler();
        lcch = new ComponentsLoaderContentHandler();
        lsch = new StatesLoaderContentHandler();
        ltch = new TransitionsLoaderContentHandler();
        leh = new LoaderErrorHandler();
        
        // Register content handler
        infoReader.setContentHandler(lich);
        componentsReader.setContentHandler(lcch);
        stateReader.setContentHandler(lsch);
        transitionReader.setContentHandler(ltch);

        // Register error handler
        componentsReader.setErrorHandler(leh);
        stateReader.setErrorHandler(leh);
        transitionReader.setErrorHandler(leh);
    }

    public void parse() throws IOException {
        InputSource inputSource = new InputSource(xmlURI);
        try {
            infoReader.parse(inputSource);
        } catch (SAXException ex) {
            Logger.getLogger(Loader.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            componentsReader.parse(inputSource);
        } catch (SAXException ex) {
            Logger.getLogger(Loader.class.getName()).log(Level.SEVERE, null, ex);
        }
        try {
            stateReader.parse(inputSource);
        } catch (SAXException ex) {
            Logger.getLogger(Loader.class.getName()).log(Level.SEVERE, null, ex);
        }
         try {
            transitionReader.parse(inputSource);
        } catch (SAXException ex) {
            Logger.getLogger(Loader.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

}
