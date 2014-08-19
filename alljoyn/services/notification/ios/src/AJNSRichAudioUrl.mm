/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#import "AJNSRichAudioUrl.h"
#import "alljoyn/about/AJNConvertUtil.h"

@implementation AJNSRichAudioUrl

/**
 * Constructor for RichAudioUrl
 * @param language Language of Audio Content
 * @param text     Text of Audio Content
 */
- (AJNSRichAudioUrl *)initRichAudioUrlWithLang:(NSString *)language andUrl:(NSString *)url
{
	self = [super init];
	if (self) {
		self.richAudioUrlHandler = new ajn::services::RichAudioUrl([AJNConvertUtil convertNSStringToQCCString:language], [AJNConvertUtil convertNSStringToQCCString:(url)]);
	}
	return self;
}

- (void)setRichAudioUrlHandler:(ajn::services::RichAudioUrl *)richAudioUrlHandler
{
	_richAudioUrlHandler = richAudioUrlHandler;
	_language = [AJNConvertUtil convertQCCStringtoNSString:richAudioUrlHandler->getLanguage()];
	_url = [AJNConvertUtil convertQCCStringtoNSString:richAudioUrlHandler->getUrl()];
}

/**
 * Set Language for Audio Content
 * @param language
 */
- (void)setLanguage:(NSString *)language
{
	self.richAudioUrlHandler->setLanguage([AJNConvertUtil convertNSStringToQCCString:language]);
}

/**
 * Set URL for Audio Content
 * @param url
 */
- (void)setUrl:(NSString *)url
{
	self.richAudioUrlHandler->setUrl([AJNConvertUtil convertNSStringToQCCString:url]);
}

@end
