/******************************************************************************
 * Copyright (c) 2013, AllSeen Alliance. All rights reserved.
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

package org.alljoyn.ns.transport;

import org.alljoyn.bus.annotation.Position;
import org.alljoyn.ns.RichAudioUrl;

/* 
 * The utility class used to wrap RichAudioUrl object into format of TransportRichAudioUrl that 
 * is sent over AJ network
 */
public class TransportRichAudioUrl {
	
	/**
	 * Url language
	 */
	@Position(0)
	public String language;
	
	/**
	 * Message url
	 */
	@Position(1)
	public String url;

	/**
	 * Constructor
	 */
	public TransportRichAudioUrl() {}
	
	/**
	 * Constructor
	 * @param language
	 * @param url
	 */
	public TransportRichAudioUrl(String language, String url) {
		this.language = language;
		this.url      = url;
	}

	/**
	 * Creates object of TransportRichAudioUrl from RichAudioUrl object
	 * @param richAudioUrl reference to TransportRichAudioUrl object
	 * @return TransportRichAudioUrl
	 */
	public static TransportRichAudioUrl buildInstance(RichAudioUrl richAudioUrl) {
		return new TransportRichAudioUrl(richAudioUrl.getLanguage(),
		     							 richAudioUrl.getUrl()
		);		
	}//buildInstance

}
