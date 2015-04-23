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

#import "AnnounceTextViewController.h"
#import "alljoyn/about/AJNAboutDataConverter.h"

@interface AnnounceTextViewController ()

@property (weak, nonatomic) IBOutlet UITextView *announceInformation;

@end

@implementation AnnounceTextViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
    
	//  Retrive AJNAnnouncement by the  announcementButtonCurrentTitle unique name
	NSString *txt = [[NSString alloc] init];
    
	// Set Announcement title
	NSString *title = [self.ajnAnnouncement busName];
    
	txt = [txt stringByAppendingFormat:@"%@\n%@\n", title, [@"" stringByPaddingToLength :[title length] + 10 withString : @"-" startingAtIndex : 0]];
    
	// Set Announcement body
	txt = [txt stringByAppendingFormat:@"BusName: %@\n", [self.ajnAnnouncement busName]];
    
	txt = [txt stringByAppendingFormat:@"Port: %hu\n", [self.ajnAnnouncement port]];
    
    txt = [txt stringByAppendingFormat:@"Version: %u\n", [self.ajnAnnouncement version]];
    
	txt = [txt stringByAppendingString:@"\n\n"];
    
	// Set Announcement AboutMap data
	txt = [txt stringByAppendingFormat:@"About map:\n"];
    
	txt = [txt stringByAppendingString:[AJNAboutDataConverter aboutDataDictionaryToString:([self.ajnAnnouncement aboutData])]];
    
	txt = [txt stringByAppendingString:@"\n\n"];
    
	//  Set Announcement ObjectDesc data
	txt = [txt stringByAppendingFormat:@"Bus Object Description:\n"];
	txt = [txt stringByAppendingString:[AJNAboutDataConverter objectDescriptionsDictionaryToString:[self.ajnAnnouncement objectDescriptions]]];
    
	self.announceInformation.text = txt;
}

@end
