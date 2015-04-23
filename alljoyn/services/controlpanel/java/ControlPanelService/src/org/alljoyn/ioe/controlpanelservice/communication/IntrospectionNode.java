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

package org.alljoyn.ioe.controlpanelservice.communication;

import java.io.ByteArrayInputStream;
import java.io.IOException;
import java.io.StringReader;
import java.util.LinkedList;
import java.util.List;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;

import org.alljoyn.bus.BusAttachment;
import org.alljoyn.bus.BusException;
import org.alljoyn.bus.ProxyBusObject;
import org.alljoyn.bus.ifaces.Introspectable;
import org.alljoyn.ioe.controlpanelservice.ControlPanelException;
import org.xml.sax.Attributes;
import org.xml.sax.EntityResolver;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.XMLReader;
import org.xml.sax.helpers.DefaultHandler;

import android.util.Log;

public class IntrospectionNode {

	class NoOpEntityResolver implements EntityResolver {
			public InputSource resolveEntity(String publicId, String systemId) 
										throws SAXException, java.io.IOException {
				return new InputSource(new ByteArrayInputStream("".getBytes()));
			}

	}
	
	//=============================================//

	class IntrospectionParser extends DefaultHandler{
		
		private XMLReader xmlReader = null;
		private SAXParser saxParser = null;

		private IntrospectionNode currentNode = null;
		private boolean sawRootNode = false;

		public IntrospectionParser() throws IOException, ParserConfigurationException, SAXException {
			SAXParserFactory spf = SAXParserFactory.newInstance();
			spf.setNamespaceAware(false);
			saxParser = spf.newSAXParser();
			xmlReader = saxParser.getXMLReader();
			xmlReader.setContentHandler(this);
			xmlReader.setEntityResolver(new NoOpEntityResolver());
		}

		public void parse(IntrospectionNode node, String xml) throws SAXException {
			this.currentNode = node;
			sawRootNode = false;
			try{
				xmlReader.parse(new InputSource(new StringReader(xml)));
			}catch(IOException cantReallyHappen) {
				Log.e("IntrospectionNode", "Failed to read the XML: '" + cantReallyHappen.getMessage() + "', ", cantReallyHappen);
			}
			this.currentNode = null;
		}

		public void startElement(String namespaceURI, String localName, 
						String qName, Attributes attrs) throws SAXException {
			if(qName.equals("node")) {
				if(!sawRootNode) {
						sawRootNode = true;
						return;
				}
				currentNode.addChild(getNameAttr(attrs));
			}else if(qName.equals("interface")){
				if(null == currentNode) throw new SAXException("interface not in node");
				currentNode.interfaces.add(getNameAttr(attrs));
			}
			
		}

		private String getNameAttr(Attributes attrs) throws SAXException {
			int i = attrs.getIndex("name");
			if(-1 == i) throw new SAXException("inner node without a name");
			return attrs.getValue(i);
		}
	}

	//================================================//
	//                END OF NESTED CLASSES           //
	//================================================//
	
	private boolean parsed = false;
	private String path = null;
	private IntrospectionParser parser = null;

	private List<IntrospectionNode> children = new LinkedList<IntrospectionNode>();
	private List<String> interfaces = new LinkedList<String>();

	public IntrospectionNode(String path) throws ParserConfigurationException, IOException, SAXException { 
		this.path = path;
		this.parser = new IntrospectionParser();
	}

	private IntrospectionNode(String path, IntrospectionParser parser){
		this.path = path;
		this.parser = parser;
	}

	protected void addChild(String name) {
		StringBuilder sb = new StringBuilder(path);
		if(!name.endsWith("/")) sb.append('/');
		sb.append(name);
		children.add(new IntrospectionNode(sb.toString(), parser));
	}

	public String getPath() {
		return path;
	}

	public boolean isParsed() {
		return parsed;
	}

	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append(path);
		sb.append('\n');

		if(!parsed) {
			sb.append(" Not parsed\n");
			return sb.toString();
		}

		for(String ifc : interfaces) {
			sb.append(' ');
			sb.append(ifc);
			sb.append('\n');
		}

		for(IntrospectionNode node : children ) {
			sb.append(node.toString());
		}

		return sb.toString();
	}

	public void parse(BusAttachment bus, String busName, int sessionId) throws SAXException, ControlPanelException {
		String xml = getInstrospection(bus, busName, path, sessionId);
		parse(xml);
		for (IntrospectionNode childNode : children) {
			childNode.parse(bus, busName, sessionId);
		}
	}//parse

	public void parseOneLevel(BusAttachment bus, String busName, int sessionId) throws SAXException, ControlPanelException {
		String xml = getInstrospection(bus, busName, path, sessionId);
		parse(xml);
	}
	
	public void parse(String xml) throws SAXException {
		parser.parse(this, xml);
		parsed = true;
	}//parse

	public List<IntrospectionNode> getChidren() {
		return children;
	}

	public List<String> getInterfaces() {
		return interfaces;
	}

	/**
	 * Get introspection of the object path
	 * @param bus BusAttachment 
	 * @param busName
	 * @param objPath
	 * @param sessionId
	 * @return Introspection XML
	 * @throws ControlPanelException If failed to get the introspection
	 */
	public static String getInstrospection(BusAttachment bus, String busName, String objPath, int sessionId) throws ControlPanelException {
		
		Log.v("IntrospectionNode", "Introspecting the Object: '" + objPath + "', BusUniqueName: '" + busName + "', sessionId: '" + sessionId + "'");
		
		ProxyBusObject proxyObj = bus.getProxyBusObject(busName, objPath, sessionId, new Class<?>[]{Introspectable.class});
		Introspectable introObj = proxyObj.getInterface(Introspectable.class);
		
		String introXML;
		try {
			introXML = introObj.Introspect(); 
		}
		catch (BusException be) {
			throw new ControlPanelException("Failed to get introspection of: '" + objPath + "', Error: '" + be.getMessage() + "'");
		}
		
		proxyObj.release();
		
		return introXML;
	}//getIntrospection
	
	/*
	public static void main(String[] args) throws Exception {
		IntrospectionNode n = new IntrospectionNode("/josh");
		n.parse(blob);
		System.out.println(n.toString());
	}
	static private String blob = "<!DOCTYPE node PUBLIC \"-//freedesktop//DTD D-BUS Object Introspection 1.0//EN\" \"http://standards.freedesktop.org/dbus/introspect-1.0.dtd\"> <node> <node name=\"Child\"/> <node name=\"Action\"/> <interface name=\"org.alljoyn.Control.TV.Dev\"> <method name=\"DownChannel\"> <arg type=\"(qq)\" direction=\"out\"/> </method> <method name=\"DownVolume\"> <arg type=\"q\" direction=\"out\"/> </method> <method name=\"GetChannelList\"> <arg type=\"a((qq)s)\" direction=\"out\"/> </method> <signal name=\"PropertyChanged\"> <arg type=\"s\" direction=\"out\"/> <arg type=\"v\" direction=\"out\"/> </signal> <method name=\"UpChannel\"> <arg type=\"(qq)\" direction=\"out\"/> </method> <method name=\"UpVolume\"> <arg type=\"q\" direction=\"out\"/> </method> <property name=\"ChannelNumber\" type=\"(qq)\" access=\"readwrite\"/> <property name=\"InputSource\" type=\"q\" access=\"readwrite\"/> <property name=\"Mute\" type=\"b\" access=\"readwrite\"/> <property name=\"Version\" type=\"q\" access=\"read\"/> <property name=\"Volume\" type=\"q\" access=\"readwrite\"/> </interface> <interface name=\"org.alljoyn.Control.Mouse.Dev\"> <method name=\"ClickMouse\"> </method> <method name=\"MoveMouse\"> <arg type=\"q\" direction=\"in\"/> <arg type=\"(ii)\" direction=\"out\"/> </method> <property name=\"MousePosition\" type=\"(ii)\" access=\"readwrite\"/> <property name=\"Version\" type=\"q\" access=\"read\"/> </interface> <interface name=\"org.freedesktop.DBus.Introspectable\"> <method name=\"Introspect\"> <arg name=\"data\" type=\"s\" direction=\"out\"/> </method> </interface> <interface name=\"org.freedesktop.DBus.Properties\"> <method name=\"Get\"> <arg name=\"interface\" type=\"s\" direction=\"in\"/> <arg name=\"propname\" type=\"s\" direction=\"in\"/> <arg name=\"value\" type=\"v\" direction=\"out\"/> </method> <method name=\"GetAll\"> <arg name=\"interface\" type=\"s\" direction=\"in\"/> <arg name=\"props\" type=\"a{sv}\" direction=\"out\"/> </method> <method name=\"Set\"> <arg name=\"interface\" type=\"s\" direction=\"in\"/> <arg name=\"propname\" type=\"s\" direction=\"in\"/> <arg name=\"value\" type=\"v\" direction=\"in\"/> </method> </interface> </node>"; 
	*/
}
