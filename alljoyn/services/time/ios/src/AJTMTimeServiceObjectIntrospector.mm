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

#import <Foundation/Foundation.h>
#import "AJTMTimeServiceObjectIntrospector.h"

@interface AJTMTimeServiceObjectIntrospector()

@property ajn::services::TimeServiceObjectIntrospector* handle;

@end

@implementation AJTMTimeServiceObjectIntrospector 

/* Constructor
 * @param handle A handle to a cpp object
 */
- (id)initWithHandle:(ajn::services::TimeServiceObjectIntrospector*)handle
{
    self = [super initWithTimeServiceClient:[[AJTMTimeServiceClient alloc]initWithHandle:(ajn::services::TimeServiceClient*)&handle->getTsClient()] objectPath:[NSString stringWithUTF8String:handle->getObjectPath().c_str()] ];
    if (self) {
        self.handle = (ajn::services::TimeServiceObjectIntrospector *)handle;
    }
    return self;
}

/**
 * Constructor
 *
 * @param tsClient TimeServiceClient managing this object
 * @param objectPath Object path of the Time Service server side object
 */
-(id)initWithClient:(AJTMTimeServiceClient *) tsClient objectPath:(NSString *)objectPath
{
    self = [super init];
    if (self) {
        self.handle = new ajn::services::TimeServiceObjectIntrospector(*[tsClient getHandle],[objectPath UTF8String]);
    }
    return self;
}

/**
 * Retrieves array of description languages supported by the introspected object.
 *
 * @return vector of the description languages.
 * If the returned vector is empty, it means that the introspected object has no description.
 */
//const std::vector<qcc::String>&
-(NSArray *)retrieveDescriptionLanguages
{
    std::vector<qcc::String> langs = self.handle->retrieveDescriptionLanguages();

    std::vector<qcc::String>::iterator itr;

    NSMutableArray *arr = [[NSMutableArray alloc]init];

    for(itr = langs.begin();itr!=langs.end();itr++)
    {
        [arr addObject:[NSString stringWithUTF8String:(*itr).c_str()]];
    }

    return arr;
}

/**
 * Retrieve description of the introspected object.
 * The description is retrieved in the requested language. The language should be
 * one of those returned by the retrieveDescriptionLanguages().
 *
 * @param language Language of the retrieved object description
 *
 * @return Object description or empty string if no description was found
 */
-(NSString *)retrieveObjectDescription:(NSString *)language
{
    return [NSString stringWithUTF8String:self.handle->retrieveObjectDescription([language UTF8String]).c_str()];
}

@end