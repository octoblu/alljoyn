/******************************************************************************
 * Copyright (c) 2013-2014, AllSeen Alliance. All rights reserved.
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

#import <UIKit/UIKit.h>
#import "AJNBusAttachment.h"
#import "alljoyn/services_common/AJSVCGenericLoggerDefaultImpl.h"

@interface ProducerViewController : UIViewController <UITextFieldDelegate>

@property (weak, nonatomic) UIViewController *mainVC;

@property (weak, nonatomic) IBOutlet UITextField *notificationEnTextField;
@property (weak, nonatomic) IBOutlet UITextField *notificationLangTextField;
@property (weak, nonatomic) IBOutlet UITextField *ttlTextField;
@property (weak, nonatomic) IBOutlet UITextField *audioTextField;
@property (weak, nonatomic) IBOutlet UITextField *iconTextField;

@property (weak, nonatomic) IBOutlet UILabel *defaultLangLabel;
@property (weak, nonatomic) IBOutlet UILabel *ttlLabel;
@property (weak, nonatomic) IBOutlet UILabel *audioLabel;
@property (weak, nonatomic) IBOutlet UILabel *iconLabel;
@property (weak, nonatomic) IBOutlet UILabel *messageTypeLabel;

@property (weak, nonatomic) IBOutlet UISwitch *audioSwitch;
@property (weak, nonatomic) IBOutlet UISwitch *iconSwitch;

@property (weak, nonatomic) IBOutlet UIButton *messageTypeButton;
@property (weak, nonatomic) IBOutlet UIButton *sendNotificationButton;
@property (weak, nonatomic) IBOutlet UIButton *deleteButton;
@property (weak, nonatomic) IBOutlet UIButton *langButton;

// Shared properties
@property (strong, nonatomic) AJNBusAttachment *busAttachment;
@property (strong, nonatomic) AJSVCGenericLoggerDefaultImpl *logger;
@property (strong, nonatomic) NSString *appName;

- (IBAction)didTouchSendNotificationButton:(id)sender;

- (IBAction)didChangeAudioSwitchValue:(id)sender;

- (IBAction)didChangeIconSwitchValue:(id)sender;

- (IBAction)didTouchDeleteButton:(id)sender;

- (QStatus)startProducer;

- (void)stopProducer:(bool) isConsumerOn;
@end
