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

#import "AJCPSLanguageSet.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJCPSLanguageSet ()
@property (nonatomic) ajn::services::LanguageSet *handle;
@end


@implementation AJCPSLanguageSet

- (id)initWithHandle:(ajn::services::LanguageSet *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

- (NSString *)getLanguageSetName
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getLanguageSetName()];
}

- (size_t)getNumLanguages
{
	return self.handle->getNumLanguages();
}

- (void)addLanguage:(NSString *)language
{
	return self.handle->addLanguage([AJNConvertUtil convertNSStringToQCCString:language]);
}

- (NSArray *)getLanguages
{
    if (!self.handle)
        return nil;
    
	const std::vector <qcc::String> cpp_languages = self.handle->getLanguages();
    
	NSMutableArray *languages = [[NSMutableArray alloc]init];
    
	for (int i = 0; i != cpp_languages.size(); i++) {
		[languages addObject:[AJNConvertUtil convertQCCStringtoNSString:cpp_languages.at(i)]];
	}
    
	return languages;
}

@end
