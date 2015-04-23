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

#import "AJCPSControlPanelController.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJCPSControlPanelController ()
@property (nonatomic) ajn::services::ControlPanelController *handle;
@end

@implementation AJCPSControlPanelController

- (id)init {
    self = [super init];
	if (self) {
		self.handle = new ajn::services::ControlPanelController();
	}
	return self;
}

- (id)initWithHandle:(ajn::services::ControlPanelController *)handle
{
	self = [super init];
	if (self) {
		self.handle = handle;
	}
	return self;
}

- (AJCPSControlPanelDevice *)createControllableDevice:(NSString *)deviceBusName ObjectDescs:(NSDictionary *)objectDescs
{
	ajn::services::AnnounceHandler::ObjectDescriptions *cpp_ObjectDescs = new ajn::services::AnnounceHandler::ObjectDescriptions;
    
	for (NSString *key in objectDescs) {
		NSArray *strings = [objectDescs objectForKey:key];
        
		std::vector <qcc::String> cpp_strings;
        
		for (NSString *string in strings) {
			cpp_strings.push_back([AJNConvertUtil convertNSStringToQCCString:string]);
		}
        
		cpp_ObjectDescs->insert(std::make_pair([AJNConvertUtil convertNSStringToQCCString:key], cpp_strings));
	}
    
    
	return [[AJCPSControlPanelDevice alloc]initWithHandle:self.handle->createControllableDevice([AJNConvertUtil convertNSStringToQCCString:deviceBusName], *cpp_ObjectDescs)];
}

- (AJCPSControlPanelDevice *)getControllableDevice:(NSString *)deviceBusName
{
	return [[AJCPSControlPanelDevice alloc]initWithHandle:self.handle->getControllableDevice([AJNConvertUtil convertNSStringToQCCString:deviceBusName])];
}

- (QStatus)deleteControllableDevice:(NSString *)deviceBusName
{
	return self.handle->deleteControllableDevice([AJNConvertUtil convertNSStringToQCCString:deviceBusName]);
}

- (QStatus)deleteAllControllableDevices
{
	return self.handle->deleteAllControllableDevices();
}

- (NSDictionary *)getControllableDevices
{
	std::map <qcc::String, ajn::services::ControlPanelDevice *> cpp_ControllableDevices =  self.handle->getControllableDevices();
    
	NSMutableDictionary *controllableDevices = [[NSMutableDictionary alloc]init];
    
	for (std::map <qcc::String, ajn::services::ControlPanelDevice *>::const_iterator itr = cpp_ControllableDevices.begin(); itr != cpp_ControllableDevices.end(); itr++) {
		NSString *key = [AJNConvertUtil convertQCCStringtoNSString:itr->first];
		AJCPSControlPanelDevice *value = [[AJCPSControlPanelDevice alloc] initWithHandle:itr->second];
        
		[controllableDevices setObject:value forKey:key];
	}
    
	return controllableDevices;
}

@end