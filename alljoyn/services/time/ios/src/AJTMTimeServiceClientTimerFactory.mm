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

/**
 * This class is experimental, and as such has not been tested.
 * Please help make it more robust by contributing fixes if you find issues
 **/

#import "AJTMTimeServiceClientTimerFactory.h"

@interface AJTMTimeServiceClientTimerFactory()

@property ajn::services::TimeServiceClientTimerFactory* handle;

@end


@implementation AJTMTimeServiceClientTimerFactory
/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceClientTimerFactory*)handle
{
    self = [super initWithHandle:handle];
    if (self) {
        self.handle = (ajn::services::TimeServiceClientTimerFactory *)handle;
    }
    return self;
}

-(void)releaseObject
{
    self.handle->release();
}

-(QStatus)newTimer:(NSString**)objectPath
{
    qcc::String nativeObjectPath;
    QStatus status = self.handle->newTimer(&nativeObjectPath);

    if (status == ER_OK) {
        *objectPath = [NSString stringWithUTF8String:nativeObjectPath.c_str()];
    }

    return status;
}

-(QStatus)deleteTimer:(NSString *)objectPath
{
    return self.handle->deleteTimer([objectPath UTF8String]);
}

//std::vector<qcc::String>* objectPathList
-(QStatus)retrieveTimerList:(NSArray**)objectPathList
{
    std::vector<qcc::String> nativeObjectPathList;

    QStatus status = self.handle->retrieveTimerList(&nativeObjectPathList);

    if (status == ER_OK) {
        NSMutableArray *list = [[NSMutableArray alloc]init];
        std::vector<qcc::String>::iterator itr;

        for (itr = nativeObjectPathList.begin(); itr!=nativeObjectPathList.end(); itr++) {
            [list addObject:[NSString stringWithUTF8String:(*itr).c_str()]];
        }

        *objectPathList = list;

    }
    return status;
}


@end
