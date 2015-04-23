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

package org.alljoyn.services.android.security;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import org.alljoyn.bus.AuthListener;
import org.alljoyn.bus.BusAttachment;
import org.alljoyn.services.common.DefaultGenericLogger;
import org.alljoyn.services.common.utils.GenericLogger;

/**
 * A default implementation of alljoyn AuthListener.
 * The application will register this listener with the bus, passing itself as a password handler.
 * When the bus requires authentication with a remote device, it will let the password handler (the application) handle it.
 * When the bus receives a result of an authentication attempt with a remote device, it will let the password handler (the application) handle it.
 * @see AuthPasswordHandler
 */
public class SrpAnonymousKeyListener implements AuthListener
{

	private String TAG = "SrpAnonymousKeyListener";
	public static String KEY_STORE_FINE_NAME;
	public static final char [] DEFAULT_PINCODE = new char[]{'0','0','0','0','0','0'};
	
	AuthPasswordHandler m_passwordHandler;
	private GenericLogger m_logger;

	/**
	 * Supported authentication mechanisms
	 */
	private List<String> authMechanisms;
	
	/**
	 * Constructor
	 * @param passwordHandler
	 * @param logger
	 */
	public SrpAnonymousKeyListener(AuthPasswordHandler passwordHandler, GenericLogger logger)
	{
		m_logger = logger;
		if (m_logger == null)
		{
			m_logger =  new DefaultGenericLogger();
		}
		m_passwordHandler = passwordHandler;
		
		authMechanisms = new ArrayList<String>(3);
		authMechanisms.add("ALLJOYN_PIN_KEYX");
		authMechanisms.add("ALLJOYN_SRP_KEYX");
		authMechanisms.add("ALLJOYN_ECDHE_PSK");
	}
	
	/**
	 * Constructor
	 * @param passwordHandler
	 * @param logger
	 * @param authMechanisms Array of authentication mechanisms
	 */
	public SrpAnonymousKeyListener(AuthPasswordHandler passwordHandler, GenericLogger logger, String[] authMechanisms)
	{
		this(passwordHandler, logger);
		if ( authMechanisms == null ) {
			
			throw new IllegalArgumentException("authMechanisms is undefined");
		}
		
		this.authMechanisms = Arrays.asList(authMechanisms);
		m_logger.debug(TAG, "Supported authentication mechanisms: '" + this.authMechanisms + "'");
	}
	

	@Override
	public boolean requested(String mechanism, String peer, int count, String userName,  AuthRequest[] requests) 
	{
		m_logger.info(TAG, " ** " + "requested, mechanism = " + mechanism + " peer = " + peer);
		if ( !this.authMechanisms.contains(mechanism) )
		{
			return false;
		}
		else
		{
			if (!(requests[0] instanceof PasswordRequest)) 
			{
				return false;
			}
			char [] pinCode = DEFAULT_PINCODE;
			// if pincode not set for this peer, the function will return null, at that case, use the default one.
			if (m_passwordHandler != null && m_passwordHandler.getPassword(peer)!= null)
			{
				pinCode = m_passwordHandler.getPassword(peer);
			}
			((PasswordRequest) requests[0]).setPassword(pinCode);
			return true;
		}
	}
   
	@Override
	public void completed(String mechanism, String authPeer, boolean authenticated) 
	{
		/*if (! authenticated)
		{
			m_logger.info(TAG, " ** " + authPeer + " failed to authenticate");
			return;
		}
		m_logger.info(TAG, " ** " + authPeer + " successfully authenticated");*/
		
		m_passwordHandler.completed(mechanism, authPeer, authenticated);
	}

	
	/**
	 * @return AuthMechanisms used by the class
	 */
	public String[] getAuthMechanisms() {
		
		return authMechanisms.toArray(new String[authMechanisms.size()]);
	}
	
	/**
	 * @return Returns AuthMechanisms used by the class as a String required by the 
	 * {@link BusAttachment#registerAuthListener(String, AuthListener)}
	 */
	public String getAuthMechanismsAsString() {
		
		final String separator = " ";
		StringBuilder sb       = new StringBuilder();
		for (String mech : authMechanisms ) {
			
			sb.append(mech).append(separator);
		}
		
		int length = sb.length();
		if ( length >= 1 ) {
			sb.deleteCharAt(length - 1); //remove the last added separator
		}
		
		return sb.toString();
	}
}
