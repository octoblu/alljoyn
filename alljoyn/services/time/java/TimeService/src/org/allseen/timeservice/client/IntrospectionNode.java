 /******************************************************************************
  * Copyright AllSeen Alliance. All rights reserved.
  *
  *    Permission to use, copy, modify, and/or distribute this software for any
  *    purpose with or without fee is hereby granted, provided that the above
  *    copyright notice and this permission notice appear in all copies.
  *
  *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
  ******************************************************************************/

package org.allseen.timeservice.client;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.ifaces.AllSeenIntrospectable;
import org.alljoyn.bus.ifaces.Introspectable;
import org.xml.sax.Attributes;
import org.xml.sax.EntityResolver;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import android.util.Log;

/**
 * Utility class for introspection
 */
class IntrospectionNode {
    private static final String TAG = "ajts" + IntrospectionNode.class.getSimpleName();

    private static class NoOpEntityResolver implements EntityResolver {

        @Override
        public InputSource resolveEntity(String publicId, String systemId) throws SAXException, java.io.IOException {
            return new InputSource(new ByteArrayInputStream("".getBytes()));
        }
    }

    // =============================================//

    /**
     * Parses the introspected XMLs
     */
    class IntrospectionParser extends DefaultHandler {

        private XMLReader xmlReader           = null;
        private SAXParser saxParser           = null;

        private IntrospectionNode currentNode = null;
        private boolean sawRootNode           = false;
        private boolean searchRootNodeDesc    = false;
        private boolean foundRootNodeDesc     = false;

        public IntrospectionParser() throws IOException, ParserConfigurationException, SAXException {

            SAXParserFactory spf = SAXParserFactory.newInstance();
            spf.setNamespaceAware(false);
            saxParser = spf.newSAXParser();
            xmlReader = saxParser.getXMLReader();
            xmlReader.setContentHandler(this);
            xmlReader.setEntityResolver(new NoOpEntityResolver());
        }

        /**
         * Parses the {@link IntrospectionNode}
         * @param node
         * @param xml
         * @throws SAXException
         */
        public void parse(IntrospectionNode node, String xml) throws SAXException {

            this.currentNode = node;
            sawRootNode      = false;

            try {

                xmlReader.parse(new InputSource(new StringReader(xml)));
            } catch (IOException cantReallyHappen) {

                Log.e(TAG, "Failed to read the XML: '" + cantReallyHappen.getMessage() + "', ", cantReallyHappen);
            }

            this.currentNode = null;
        }

        @Override
        public void startElement(String namespaceURI, String localName, String qName, Attributes attrs) throws SAXException {

            if ( qName.equals("node") ) {

                if ( !sawRootNode ) {

                    //Currently on Root Node
                    sawRootNode = true;

                    //Search for the Root Node Description
                    searchRootNodeDesc = true;
                    return;
                }

                //Found child node
                currentNode.addChild(getNameAttr(attrs));
            }
            else if ( qName.equals("description") && searchRootNodeDesc ) {

                foundRootNodeDesc  = true;
                searchRootNodeDesc = false;
            }
            else {

                foundRootNodeDesc  = false;
                searchRootNodeDesc = false;
            }
        }

        @Override
        public void characters(char[] ch, int start, int length) throws SAXException {

            if ( foundRootNodeDesc ) {

                currentNode.appendRootObjDescription(ch, start, length);
            }
        }

        /**
         * Retrieve the name attribute from the node tag
         * @param attrs
         * @return
         * @throws SAXException
         */
        private String getNameAttr(Attributes attrs) throws SAXException {

            int i = attrs.getIndex("name");
            if (-1 == i) {

                throw new SAXException("inner node without a name");
            }

            return attrs.getValue(i);
        }
    }

    // ================================================//
    // END OF NESTED CLASSES                           //
    // ================================================//

    /**
     * Object path to parse
     */
    private String path                  = null;

    /**
     * Whether the object was parsed
     */
    private boolean parsed               = false;

    /**
     * {@link IntrospectionParser} object that was used for the parsing
     */
    private IntrospectionParser parser   = null;

    /**
     * Child objects of the introspected object
     */
    private final List<IntrospectionNode> children = new ArrayList<IntrospectionNode>();

    /**
     * {@link StringBuilder} of the root node description
     */
    private final StringBuilder rootObjectDescription = new StringBuilder();

    /**
     * Provide object path to be introspected and parsed
     * @param path Object path to introspect
     * @throws ParserConfigurationException
     * @throws IOException
     * @throws SAXException
     */
    public IntrospectionNode(String path) throws ParserConfigurationException, IOException, SAXException {

        this.path              = path;
        this.parser            = new IntrospectionParser();
    }

    /**
     * Constructor
     * @param path
     * @param parser
     */
    private IntrospectionNode(String path, IntrospectionParser parser) {

        this.path   = path;
        this.parser = parser;
    }

    /**
     * Get object path
     * @return Object Path
     */
    public String getPath() {

        return path;
    }

    /**
     * Whether the object is parsed
     * @return
     */
    public boolean isParsed() {

        return parsed;
    }

    /**
     * Parse the object using {@link AllSeenIntrospectable} interface.
     * @param bus {@link BusAttachment}
     * @param busName Bus unique name hosting the parsed object
     * @param sessionId Session connected to the busName
     * @param language Introspection language. Introspection with unknown language has the same result as {@link Introspectable}
     * @throws Exception Is throws if failed to introspect the object, or to parse the introspected XML
     */
    public void parse(BusAttachment bus, String busName, int sessionId, String language) throws Exception {

        String xml = getInstrospection(bus, busName, path, sessionId, language);
        parse(xml);
    }

    /**
     * Parse the introspection XML
     * @param xml
     * @throws SAXException
     */
    public void parse(String xml) throws SAXException {

        parser.parse(this, xml);
        parsed = true;
    }// parse

    /**
     * Get child objects of this object
     * @return {@link List} of the introspected child objects
     */
    public List<IntrospectionNode> getChidren() {

        return children;
    }

    /**
     * Returns content of the <i>description</i> tag of the introspected object
     * @return Description or empty string if the description tag wasn't found
     */
    public String getObjectDescription() {

        return rootObjectDescription.toString().trim();
    }

    /**
     * Append the child object path to the parent one.
     * @param name Child object path
     */
    private void addChild(String name) {

        StringBuilder sb = new StringBuilder(path);
        if ( !name.endsWith("/") ) {
            sb.append('/');
        }

        sb.append(name);
        children.add(new IntrospectionNode(sb.toString(), parser));
    }

    /**
     * Appends to the rootObjectDescription the received characters, starting from the given
     * start index and the length offset
     * @param chars Array of characters to append
     * @param start Starting index
     * @param length length of the appended characters
     */
    private void appendRootObjDescription(char[] chars, int start, int length) {

        rootObjectDescription.append(chars, start, length);
    }

    /**
     * Get introspection of the object path
     * @param bus {@link BusAttachment}
     * @param busName The bus unique name hosting the introspected object
     * @param objPath Object to be introspected
     * @param sessionId Session connected to the bus name
     * @param language The language to be used in introspection
     * @return Introspection XML
     * @throws Exception Is thrown if failed to introspect
     */
    private String getInstrospection(BusAttachment bus, String busName, String objPath, int sessionId, String language)
                                        throws Exception {

        ProxyBusObject proxyObj = bus.getProxyBusObject(busName, objPath, sessionId, new Class<?>[] { AllSeenIntrospectable.class });
        AllSeenIntrospectable introObj = proxyObj.getInterface(AllSeenIntrospectable.class);

        String introXML;
        try {

            introXML = introObj.IntrospectWithDescription(language);
        } catch (BusException be) {

            throw new Exception("Failed to introspect Object: '" + objPath + "'", be);
        }

        return introXML;
    }// getIntrospection

    /**
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {

        StringBuilder sb = new StringBuilder();
        sb.append(path);
        sb.append('\n');

        if (!parsed) {

            sb.append(" Not parsed\n");
            return sb.toString();
        }

        for (IntrospectionNode node : children) {

            sb.append(node.toString());
        }

        return sb.toString();
    }
}
