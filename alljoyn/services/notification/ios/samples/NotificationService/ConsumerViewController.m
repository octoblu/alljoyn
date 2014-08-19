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

#import "ConsumerViewController.h"
#import "alljoyn/notification/AJNSNotificationService.h"
#import "alljoyn/notification/AJNSNotificationReceiver.h"
#import "alljoyn/services_common/AJSVCGenericLoggerUtil.h"
#import "AJNPasswordManager.h"
#import "DetailsCell.h"
#import "NotificationEntry.h"
#import "alljoyn/controlpanel/AJCPSGetControlPanelViewController.h"

static NSString *const DEFAULT_APP_NAME = @"DISPLAY_ALL";
static NSString *const CONSUMER_DEFAULT_LANG = @"en";

@interface ConsumerViewController () <AJNSNotificationReceiver>

@property (weak, nonatomic) AJNSNotificationService *consumerService;

@property (strong, nonatomic) UIAlertView *selectConsumerLang;
@property (strong, nonatomic) NSMutableArray *notificationEntries; // array of NotificationEntry objects

@end

@implementation ConsumerViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
	[self.view setHidden:YES];
   
    [self.notificationTableView setContentOffset:self.notificationTableView.contentOffset animated:NO];
    
    _notificationEntries = [[NSMutableArray alloc] init];
}

- (void) viewWillAppear:(BOOL)animated
{
     [self.navigationController setNavigationBarHidden:YES animated:NO];
    [super viewWillAppear:animated];
}


- (QStatus)startConsumer
{
	QStatus status;
    
    // Initialize Service object and send it Notification Receiver object
	self.consumerService = [AJNSNotificationService sharedInstance];
    
	/*   This is an example of using the AJNSNotificationService default Logger (QSCGenericLoggerDefaultImpl)
     
     // Get the default logger
     [self.consumerService logger];
     
     // Get the default logger log level
     [self.consumerService logLevel]
     
     // Set a new logger level
     [self.consumerService setLogLevel:QLEVEL_WARN];
     
     // Print the new log level
     //    NSString *newLogLevel = [NSString stringWithFormat:@"Consumer Logger has started in %@ mode", [AJSVCGenericLoggerUtil toStringQLogLevel:[self.consumerService logLevel]]];
     //    [[self.consumerService logger] debugTag:[[self class] description] text: newLogLevel];
     
     // Instead, you can use a self implementation logger as shown below:
	 */
    [self.consumerService setLogLevel:QLEVEL_DEBUG];
    
	if (!self.busAttachment) {
		[self.logger fatalTag:[[self class] description] text:@"BusAttachment is nil"];
		return ER_FAIL;
	}
    
	// Call "initReceive"
	status = [self.consumerService startReceive:self.busAttachment withReceiver:self];
	if (status != ER_OK) {
		[self.logger fatalTag:[[self class] description] text:@"Could not initialize receiver"];
		return ER_FAIL;
	}
    
	// Set Consumer UI
    [self.logger debugTag:[[self class] description] text:@"Waiting for notifications"];
	return ER_OK;
}

- (void)stopConsumer:(bool) isProducerOn
{
	[self.logger debugTag:[[self class] description] text:@"Stop Consumer service"];
    
  if (self.consumerService && isProducerOn) {
        [self.consumerService shutdownReceiver];
        [self.logger debugTag:[[self class] description] text:@"calling shutdownReceiver"];
    } else {
        [self.consumerService shutdown];
        [self.logger debugTag:[[self class] description] text:@"calling shutdown"];
    }
    self.consumerService = nil;

    [self.notificationEntries removeAllObjects];
    
    [self.notificationTableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];

}

// Add AJNSNotification text to the scroll view
- (void)addToUITableView:(AJNSNotification *)ajnsNotification
{

    NotificationEntry *notificationEntry= [[NotificationEntry alloc] initWithAJNSNotification:ajnsNotification andConsumerViewController:self ];
    
    notificationEntry.dismissed = NO;
    
    [self.notificationEntries addObject:notificationEntry];
    
    [self.notificationTableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
    
    [self scrollToLastRow];
    
}

-(void)logNotification:(AJNSNotification *)ajnsNotification
{
    NSMutableString *notificationContent = [[NSMutableString alloc] init];
    
    [notificationContent appendFormat:@"Application name: %@\n[current app name is %@]\n", [ajnsNotification appName], self.appName];
    
    [notificationContent appendFormat:@"Message id: '%d'\n",[ajnsNotification messageId]];
    
    [notificationContent appendFormat:@"MessageType: '%@'\n",[AJNSNotificationEnums AJNSMessageTypeToString:[ajnsNotification messageType]]];
    
    [notificationContent appendFormat:@"Device id: '%@'\n",[ajnsNotification deviceId]];
    
    [notificationContent appendFormat:@"Device Name: '%@'\n",[ajnsNotification deviceName]];
    
    [notificationContent appendFormat:@"Sender: '%@'\n",[ajnsNotification senderBusName]];

    [notificationContent appendFormat:@"AppId: '%@'\n",[ajnsNotification appId]];

//    [notificationContent appendFormat:@"CustomAttributes: '%@'\n",[ajnsNotification.customAttributes description]];
    
    [notificationContent appendFormat:@"richIconUrl: '%@'\n",[ajnsNotification richIconUrl]];
    
    
    NSMutableArray *array = [[NSMutableArray alloc]init];
    [ajnsNotification richAudioUrl:array];
    
    if ([array count]) {
        [notificationContent appendString:@"RichAudioUrl: "];
        for (AJNSRichAudioUrl *richAudioURL in array) {
            [notificationContent appendFormat:@"'%@'",[richAudioURL url]];
        }
        [notificationContent appendString:@"\n"];
    } else {
        [notificationContent appendFormat:@"RichAudioUrl is empty\n"];
    }

    [notificationContent appendFormat:@"richIconObjPath: '%@'\n",[ajnsNotification richIconObjectPath]];
    
    [notificationContent appendFormat:@"RichAudioObjPath: '%@'\n",[ajnsNotification richAudioObjectPath]];
    
    [notificationContent appendFormat:@"CPS Path: '%@'\n",[ajnsNotification controlPanelServiceObjectPath]];
    

    AJNSNotificationText *nt = ajnsNotification.ajnsntArr[0];
    
    [notificationContent appendFormat:@"First msg: '%@' [total: %lu]\n", [nt getText], (unsigned long)[ajnsNotification.ajnsntArr count]];

    [notificationContent appendFormat:@"Received message Lang: '%@'\n",[nt getLanguage]];
    
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Received new Notification:\n%@", notificationContent]];

}

#pragma mark - AJNSNotificationReceiver protocol methods

- (void)dismissMsgId:(const int32_t)msgId appId:(NSString*) appId
{
    // find the message:
    for (NotificationEntry *entry in self.notificationEntries) {
        if ([entry.ajnsNotification messageId] == msgId && [[entry.ajnsNotification appId] isEqualToString:appId]) {
            [entry setDismissed:YES];
            
            [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"msgId:%d appId:%@ dismissed",msgId,appId]];
        }
    }
    
    [self.notificationTableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
}

// Parse AJNSNotification into a string
- (void)receive:(AJNSNotification *)ajnsNotification
{
    [self logNotification:ajnsNotification];
    
    
	//show notification only if (appName is DISPLAY_ALL) || (received app name matches appName)
	if ((([self.appName isEqualToString:DEFAULT_APP_NAME]) || ([[ajnsNotification appName] isEqualToString:self.appName]))) {
        
        if (self.consumerLang == nil) {
            self.consumerLang = CONSUMER_DEFAULT_LANG;
        }
        
        [self addToUITableView:ajnsNotification];
	}
	else {
		[self.logger debugTag:[[self class] description] text:@"The received msg app name does not match the application name"];
		return;
	}
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (void)showSelectLanguageAlert
{
	NSArray *langArray = [[NSMutableArray alloc] initWithObjects:@"English", @"Hebrew", @"Russian", nil];
    
	self.selectConsumerLang = [[UIAlertView alloc] initWithTitle:@"Select Language" message:nil delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
    
	for (NSString *str in langArray) {
		[self.selectConsumerLang addButtonWithTitle:str];
	}
    
	[self.selectConsumerLang show];
}

#pragma mark - UIAlertView delegate function

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0) {
		return; // Cancel pressed
	}
	if (alertView == self.selectConsumerLang) {
		self.consumerLang = [self convertToLangCode:[alertView buttonTitleAtIndex:buttonIndex]];
		[self.consumerLangButton setTitle:[alertView buttonTitleAtIndex:buttonIndex] forState:UIControlStateNormal];
	}
}

#pragma mark - Application util methods

- (NSString *)convertToLangCode:(NSString *)value
{
	if ([value isEqualToString:@"English"]) {
		return @"en";
	}
	else if ([value isEqualToString:@"Hebrew"]) {
		return @"he";
	}
	else if ([value isEqualToString:@"Russian"]) {
		return @"ru";
	}
	return nil;
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections - Currently we use only 1 section
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [self.notificationEntries count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    
    DetailsCell *cell = (DetailsCell *)[tableView dequeueReusableCellWithIdentifier:@"detailsCell" forIndexPath:indexPath];
    
    cell.notificationEntry = self.notificationEntries[indexPath.row];
    
    // Configure the cell...
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
        
    return cell;
}

#pragma mark - IBActions
- (IBAction)consumerLangButtonPressed:(UIButton *)sender
{
	[self showSelectLanguageAlert];
}

- (IBAction)dismissChosenPressed:(UIButton *)sender {
    for (NotificationEntry *entry in self.notificationEntries) {
        if (entry.chosen) {
            NSLog(@"sending dismiss to %@",[entry.ajnsNotification text]);
            [entry.ajnsNotification dismiss];
        }
    }
}

- (IBAction)actionOnChosenPressed:(UIButton *)sender {
    
    __block NSInteger cnt = 0;
    
    [self.notificationEntries enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        NotificationEntry *entry = (NotificationEntry *)obj;
        
        if (entry.chosen) {
            cnt++;
        }
    }];
    
    if (cnt > 1) {
        [[[UIAlertView alloc]initWithTitle:@"More than one notification chosen" message:@"select single notification to get notification with action" delegate:Nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
        return;
    }
    
    
    for (NotificationEntry *entry in self.notificationEntries) {
        if (entry.chosen) {
            NSLog(@"Action requested.");
            // Checking if entry has CPS objectpath
            
            NSString *cpsObjectPath = [entry.ajnsNotification controlPanelServiceObjectPath];
            if ([cpsObjectPath length])
            {
                GetControlPanelViewController *controlPanelViewController = [[GetControlPanelViewController alloc] initWithNotificationSenderBusName:[entry.ajnsNotification senderBusName] cpsObjectPath:[entry.ajnsNotification controlPanelServiceObjectPath] bus:self.busAttachment];
                [self.navigationController setNavigationBarHidden:NO];
                [self.navigationController pushViewController:controlPanelViewController animated:YES];
            } else {
                NSLog(@"%@ has no CPS object path", [entry.ajnsNotification text]);
                [[[UIAlertView alloc] initWithTitle:@"Info" message:@"This notification doesn't have an action." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
                //deselect cell
                entry.chosen = NO;
                [self.notificationTableView performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:YES];
            }
        }
    }
}

-(void)scrollToLastRow
{
    [NSThread sleepForTimeInterval:1];
    int lastRowNumber = [self.notificationTableView numberOfRowsInSection:0] - 1;
    NSIndexPath* ip = [NSIndexPath indexPathForRow:lastRowNumber inSection:0];
    [self.notificationTableView scrollToRowAtIndexPath:ip atScrollPosition:UITableViewScrollPositionTop animated:YES];
}

@end
