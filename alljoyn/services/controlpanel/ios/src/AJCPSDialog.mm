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

#import "AJCPSDialog.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJCPSDialog ()

@end


@implementation AJCPSDialog

- (id)initWithHandle:(ajn::services::Dialog *)handle
{
	self = [super initWithHandle:handle];
	if (self) {
	}
	return self;
}

- (uint16_t)getNumActions
{
	return ((ajn::services::Dialog *)self.handle)->getNumActions();
}

- (NSString *)getMessage
{
	return [AJNConvertUtil convertQCCStringtoNSString:((ajn::services::Dialog *)self.handle)->getMessage()];
}

- (NSString *)getLabelAction1
{
	return [AJNConvertUtil convertQCCStringtoNSString:((ajn::services::Dialog *)self.handle)->getLabelAction1()];
}

- (NSString *)getLabelAction2
{
	return [AJNConvertUtil convertQCCStringtoNSString:((ajn::services::Dialog *)self.handle)->getLabelAction2()];
}

- (NSString *)getLabelAction3
{
	return [AJNConvertUtil convertQCCStringtoNSString:((ajn::services::Dialog *)self.handle)->getLabelAction3()];
}

- (QStatus)executeAction1
{
	return ((ajn::services::Dialog *)self.handle)->executeAction1();
}

- (QStatus)executeAction2
{
	return ((ajn::services::Dialog *)self.handle)->executeAction2();
}

- (QStatus)executeAction3
{
	return ((ajn::services::Dialog *)self.handle)->executeAction3();
}

@end
