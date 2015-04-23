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

#import "AJCPSControlPanel.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJCPSControlPanel ()

@property (nonatomic) ajn::services::ControlPanel *handle;

@end

@implementation AJCPSControlPanel


- (id)initWithHandle:(ajn ::services ::ControlPanel *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

// original cpp constructor: ControlPanel(LanguageSet const& languageSet, qcc::String objectPath, ControlPanelDevice* device);

- (NSString *)getPanelName
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getPanelName()];
}

- (QStatus)registerObjects:(AJNBusAttachment *)bus
{
	return self.handle->registerObjects((ajn::BusAttachment *)[bus handle]);
}

- (QStatus)unregisterObjects:(AJNBusAttachment *)bus
{
	return self.handle->unregisterObjects((ajn::BusAttachment *)[bus handle]);
}

- (AJCPSLanguageSet *)getLanguageSet
{
	return [[AJCPSLanguageSet alloc]initWithHandle:(ajn::services::LanguageSet *)&self.handle->getLanguageSet()];
}

- (AJCPSControlPanelDevice *)getDevice
{
	return [[AJCPSControlPanelDevice alloc]initWithHandle:self.handle->getDevice()];
}

- (NSString *)getObjectPath
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getObjectPath()];
}

- (AJCPSContainer *)getRootWidget:(NSString *)Language
{
    return [[AJCPSContainer alloc]initWithHandle:self.handle->getRootWidget([AJNConvertUtil convertNSStringToQCCString:Language])];
}

@end
