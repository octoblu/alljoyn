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

package org.alljoyn.onboarding.transport;

import org.alljoyn.bus.annotation.Position;

/**
 * A class that represents the last onboarding error: The last error code and error message
 */
public class OBLastError {

	/** 
	 * The error code of the last validation
	 * 0 - Validated
	 * 1 - Unreachable
	 * 2 - Unsupported_protocol
	 * 3 - Unauthorized
	 * 4 - Error_message
	 */
    @Position(0) public short m_errorCode;
    
    /** 
     * The error message of the last validation
     * Error_message is the error message received from the underlying Wifi layer. 
     */
    @Position(1) public String m_errorMsg;
	
    public OBLastError()
    {
    }
    
    /**
     * Get the last validation error code
     * @return the last error code
	 * 0 - Validated
	 * 1 - Unreachable
	 * 2 - Unsupported_protocol
	 * 3 - Unauthorized
	 * 4 - Error_message
     */
	public short getErrorCode()
	{
		return m_errorCode;
	}

	/**
	 * Set the last validation error code
	 * @param errorCode
	 */
	public void setErrorCode(short errorCode)
	{
		m_errorCode = errorCode;
	}

	/**
	 * Get the last validation error message
	 * @return the error message received from the underlying Wifi layer.
	 */
	public String getErrorMessage()
	{
		return m_errorMsg;
	}

	/**
	 * Set the last validation error message
	 * @param errorMsg
	 */
	public void setErrorMessage(String errorMsg)
	{
		m_errorMsg = errorMsg;
	}
	
}
