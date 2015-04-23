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

package org.alljoyn.ns.commons;

/**
 * The exception is thrown if there was a failure in loading and instantiating the class 
 * implementing platform dependent logic
 */
public class NativePlatformFactoryException extends Exception {

	/**
	 * Serializable id
	 */
	private static final long serialVersionUID = 8554726459608620712L;

	public NativePlatformFactoryException() {
		super();
	}

	public NativePlatformFactoryException(String message, Throwable throwable) {
		super(message, throwable);
	}

	public NativePlatformFactoryException(String message) {
		super(message);
	}

	public NativePlatformFactoryException(Throwable throwable) {
		super(throwable);
	}
	
}
