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

#import <UIKit/UIKit.h>
#import "AJNBusAttachment.h"
#import "alljoyn/services_common/AJSVCGenericLoggerDefaultImpl.h"

@interface ConsumerViewController : UIViewController <UITableViewDataSource, UITableViewDelegate>


@property (weak, nonatomic) UIViewController *mainVC;

@property (weak, nonatomic) IBOutlet UIButton *consumerLangButton;

@property (weak, nonatomic) IBOutlet UIButton *dismissChosen;

@property (weak, nonatomic) IBOutlet UIButton *actionOnChosen;

@property (weak, nonatomic) IBOutlet UITableView *notificationTableView;



// Shared
@property (strong, nonatomic) AJNBusAttachment *busAttachment;

@property (strong, nonatomic) AJSVCGenericLoggerDefaultImpl *logger;

@property (strong, nonatomic) NSString *appName;

@property (strong, nonatomic) NSString *consumerLang;

- (QStatus)startConsumer;

- (void)stopConsumer:(bool) isProducerOn;

@end
