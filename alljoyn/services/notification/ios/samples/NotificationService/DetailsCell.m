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

#import "DetailsCell.h"

@implementation DetailsCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {

    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
    
    if ([self.notificationEntry dismissed]) {
        [self setAccessoryType:UITableViewCellAccessoryNone];
        [self.notificationEntry setChosen:NO];
        return;
    }

    if(selected)
    {
        if (self.accessoryType == UITableViewCellAccessoryNone) {
            [self setAccessoryType:UITableViewCellAccessoryCheckmark];
            [self.notificationEntry setChosen:YES];
        } else {
            [self setAccessoryType:UITableViewCellAccessoryNone];
            [self.notificationEntry setChosen:NO];
        }
    }
}



-(void)setNotificationEntry:(NotificationEntry *)notificationEntry
{
    _notificationEntry = notificationEntry;
    
    self.detailsLabel.text = notificationEntry.text;
    
    if([self.notificationEntry chosen])
    {
        [self setAccessoryType:UITableViewCellAccessoryCheckmark];
    } else {
        [self setAccessoryType:UITableViewCellAccessoryNone];
    }
    
    if([self.notificationEntry dismissed])
    {
        [self setBackgroundColor:[UIColor yellowColor]];
    } else {
        [self setBackgroundColor:[UIColor whiteColor]];
    }
}

@end
