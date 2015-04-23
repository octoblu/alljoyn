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

#import "AJCPSCPSButtonCell.h"
#import "AJCPSAction.h"

@implementation CPSButtonCell

- (id)initWithStyle:(UITableViewCellStyle)style reuseIdentifier:(NSString *)reuseIdentifier
{
    self = [super initWithStyle:style reuseIdentifier:reuseIdentifier];
    if (self) {
        self.cpsButton = [UIButton buttonWithType:UIButtonTypeSystem];
        
        [self.cpsButton setFrame:CGRectMake(10,0,300,60)];
        [self.cpsButton.titleLabel setFont:[UIFont systemFontOfSize:13]];
        [self.cpsButton addTarget:self action:@selector(touchUpInsideAction:) forControlEvents:UIControlEventTouchUpInside];
        [self.contentView addSubview:self.cpsButton];
        [self reloadInputViews];
    }
    return self;
}

- (void)setSelected:(BOOL)selected animated:(BOOL)animated
{
    [super setSelected:selected animated:animated];
}

- (void)touchUpInsideAction:(id)sender {
    
    NSLog(@"Pressed %@",[self.cpsButton titleLabel].text);
    
    QStatus status = [((AJCPSAction *)self.actionWidget) executeAction];
    if (status != ER_OK) {
        NSLog(@"execute Action returned error %d, %@",status, [AJNStatus descriptionForStatusCode:status]);
    }
}

-(void)setActionWidget:(AJCPSAction *)actionWidget
{
    _actionWidget = actionWidget;
    [self.cpsButton setTitle:[self.actionWidget getLabel] forState:UIControlStateNormal];
    [self.cpsButton setEnabled:[self.actionWidget getIsEnabled]?YES:NO];
    // We do not use [self.actionWidget getBgColor] so the iOS look and feel remain the same
    
}

@end
