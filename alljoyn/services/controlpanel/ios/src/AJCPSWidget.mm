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

#import "AJCPSWidget.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "AJCPSControlPanelDevice.h"

@interface AJCPSWidget ()
@property (nonatomic) ajn::services::Widget *handle;
@end


@implementation AJCPSWidget

- (id)init
{
	return Nil; // This is an abstract class. Do Not Instantiate.
}

- (id)initWithHandle:(ajn::services::Widget *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
        
		if (!self.handle) {
			NSLog(@"Failed getting a pointer to a Widget subclass");
			return Nil;
		}
	}
	return self;
}

- (void)dealloc
{
    // it is very important not to delete anything using widget, this is all taken care of by lower layers
}

- (AJCPSWidgetType)getWidgetType
{
	return self.handle->getWidgetType();
}

- (NSString *)getWidgetName
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getWidgetName()];
}

- (AJCPSControlPanelMode)getControlPanelMode
{
	return (AJCPSControlPanelMode)self.handle->getControlPanelMode();
}

- (const AJCPSWidget *)getRootWidget
{
	return [[AJCPSWidget alloc]initWithHandle:self.handle->getRootWidget()];
}

- (const AJCPSControlPanelDevice *)getDevice
{
	return [[AJCPSControlPanelDevice alloc] initWithHandle:self.handle->getDevice()];
}

- (const uint16_t)getInterfaceVersion
{
	return self.handle->getInterfaceVersion();
}

- (bool)getIsSecured
{
	return self.handle->getIsSecured();
}

- (bool)getIsEnabled
{
	return self.handle->getIsEnabled();
}

- (bool)getIsWritable
{
	return self.handle->getIsWritable();
}

- (uint32_t)getStates
{
	return self.handle->getStates();
}

- (uint32_t)getBgColor
{
	return self.handle->getBgColor();
}

- (NSString *)getLabel
{
	return [AJNConvertUtil convertQCCStringtoNSString:self.handle->getLabel()];
}

- (NSArray *)getHints
{
	const std::vector <uint16_t> cpp_hints = self.handle->getHints();
	NSMutableArray *hints = [[NSMutableArray alloc]init];
    
    if (cpp_hints.size()) {
        for (int i = 0; i != cpp_hints.size(); i++) {
            [hints addObject:[NSNumber numberWithShort:cpp_hints.at(i)]];
        }
    }
	return hints;
}

- (QStatus)registerObjects:(AJNBusAttachment *)bus atObjectPath:(NSString *)objectPath
{
	ajn::BusAttachment *cpp_bus = (ajn::BusAttachment *)[bus handle];
    
	return self.handle->registerObjects(cpp_bus, [AJNConvertUtil convertNSStringToQCCString:objectPath]);
}

- (QStatus)refreshObjects:(AJNBusAttachment *)bus
{
	ajn::BusAttachment *cpp_bus = (ajn::BusAttachment *)[bus handle];
    
	return self.handle->refreshObjects(cpp_bus);
}

- (QStatus)unregisterObjects:(AJNBusAttachment *)bus
{
	ajn::BusAttachment *cpp_bus = (ajn::BusAttachment *)[bus handle];
    
	return self.handle->unregisterObjects(cpp_bus);
}

@end
