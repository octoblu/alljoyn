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

#import "MainViewController.h"
#import <alljoyn/Status.h>
#import "AJNBus.h"
#import "AJNBusAttachment.h"
#import "alljoyn/services_common/AJSVCGenericLoggerDefaultImpl.h"
#import "ProducerViewController.h"
#import "ConsumerViewController.h"
#import "NotificationUtils.h"

static NSString * const DEFAULT_APP_NAME = @"DISPLAY_ALL";
static NSString * const DAEMON_QUIET_PREFIX = @"quiet@"; // For tcl
static NSString * const DAEMON_NAME = @"org.alljoyn.BusNode.IoeService"; // For tcl

@interface MainViewController ()

@property (strong, nonatomic) AJNBusAttachment *busAttachment;
@property (strong, nonatomic) AJSVCGenericLoggerDefaultImpl *logger;
@property (strong, nonatomic) ProducerViewController *producerVC;
@property (strong, nonatomic) ConsumerViewController *consumerVC;
@property (strong, nonatomic) NSString *appName;
@property (nonatomic) bool isProducerOn;
@property (nonatomic) bool isConsumerOn;
@property (nonatomic) bool hasAppName;

@end

@implementation MainViewController

#pragma mark - Built In methods
- (void)viewDidLoad
{
	[super viewDidLoad];
    self.appModeSegmentController.momentary = true;
    
    // Set flags
    self.isConsumerOn = false;
    self.isProducerOn = false;
    self.hasAppName = false;
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
	if ([segue.destinationViewController isKindOfClass:[ProducerViewController class]]) {
		self.producerVC = segue.destinationViewController;
	}
    
	if ([segue.destinationViewController isKindOfClass:[ConsumerViewController class]]) {
		self.consumerVC = segue.destinationViewController;
	}
}

// Autorotation support
- (BOOL)shouldAutorotate
{
	return YES;
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
	[textField resignFirstResponder];
	return YES;
}

// Limit app rotation to portrait
- (NSUInteger)supportedInterfaceOrientations
{
	return UIInterfaceOrientationMaskPortrait;
}

#pragma mark - IBAction Methods

// application mode (consumer/producer)
- (IBAction)appModeSegmentCtrlValueChanged:(id)sender
{
    QStatus status;
    
    if (!self.isConsumerOn && !self.isProducerOn) {
        [self.logger debugTag:[[self class] description] text:@"loadNewSession"];
        status = [self loadNewSession];
        if (ER_OK != status) {
            [[[UIAlertView alloc] initWithTitle:@"Startup Error" message:[NSString stringWithFormat:@"%@ (%@)",@"Failed to prepare AJNBusAttachment", [AJNStatus descriptionForStatusCode:status]] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
            return;
        }
    }
    
    // If application name hasn't been set - use the default
    if (!self.hasAppName) {
        self.appName  = DEFAULT_APP_NAME;
        self.hasAppName = true;
        [self.appNameTextField setEnabled:NO];
    }
    
	int appMode = self.appModeSegmentController.selectedSegmentIndex;
    
    switch (appMode) {
        case 0: // Producer
            if (!self.isProducerOn) {  // Start producer
                [self.logger debugTag:[[self class] description] text:@"Starting producer...."];
                // Forward shared properties
                self.producerVC.busAttachment = self.busAttachment;
                self.producerVC.logger = self.logger;
                self.producerVC.appName = self.appName;
                // Present view
                [self.producerVC.view setHidden:NO];
                // Set service flag
                self.isProducerOn = true;
                status = [self.producerVC startProducer];
                if (ER_OK != status) {
                    [self.logger debugTag:[[self class] description] text:@"Failed to startProducer"];
                    [self producerCleanup];
                }
            } else {
                [self producerCleanup];
            }
            break;
            
        case 1: // Consumer
            if (!self.isConsumerOn) { //Start consumer
                [self.logger debugTag:[[self class] description] text:@"Starting consumer..."];
                // Forward shared properties
                self.consumerVC.busAttachment = self.busAttachment;
                self.consumerVC.logger = self.logger;
                self.consumerVC.appName = self.appName;
                // Present view
                [self.consumerVC.view setHidden:NO];
                // Set service flag
                self.isConsumerOn = true;
                status = [self.consumerVC startConsumer];
                if (ER_OK != status) {
                    [self.logger debugTag:[[self class] description] text:@"Failed to startConsumer"];
                    [[[UIAlertView alloc] initWithTitle:@"Startup Error" message:[NSString stringWithFormat:@"%@ (%@)",@"Failed to startConsumer", [AJNStatus descriptionForStatusCode:status]] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles: nil] show];
                    [self consumerCleanup];
                }
            } else {
                [self consumerCleanup];
            }
            break;
    }
    
    if (self.isConsumerOn || self.isProducerOn) {
        // Disable application name editing
        [self.logger debugTag:[[self class] description] text:@"Application has started - disable application name editing"];
        [self.appNameTextField setEnabled:NO];
    }
    
    // Both services are off - closing session
    if (!self.isConsumerOn && !self.isProducerOn) {
        [self closeSession];
    }
}



- (IBAction)appNameChange:(UITextField *)sender
{
	if ([NotificationUtils textFieldIsValid:self.appNameTextField.text]) {
		self.appName = self.appNameTextField.text;
		self.hasAppName = true;
	} else {
		self.appName = DEFAULT_APP_NAME;
		self.appNameTextField.text = @"";
		self.hasAppName = false;
	}
}

#pragma mark - Util methods
- (QStatus)loadNewSession
{
    QStatus status = ER_OK;
    
    // Set a default logger
    self.logger = [[AJSVCGenericLoggerDefaultImpl alloc] init];
    // Set a bus
    if (!self.busAttachment) {
        status = [self prepareBusAttachment:nil];
        if (ER_OK != status) {
            [self.logger debugTag:@"" text:@"Failed to prepareBusAttachment"];
            return status;
        } else {
            [self.logger debugTag:[[self class] description] text:@"Bus is ready to use"];
        }
    }
    return status;
}

- (void)closeSession
{
    QStatus status;
    [self.logger debugTag:[[self class] description] text:@"Both services are off"];
    // reset the application Text view
    [self.appNameTextField setEnabled:YES];
    self.appNameTextField.text = @"";
    self.hasAppName = false;
    
    // stop bus attachment
    [self.logger debugTag:[[self class] description] text:@"Stopping the bus..."];
    
    if (self.busAttachment) {
        status = [self.busAttachment stop];
        if (status != ER_OK) {
            [self.logger debugTag:[[self class] description] text:@"failed to stop the bus"];
        }
        self.busAttachment = nil;
    }
    
    // destroy logger
    if (self.logger) {
        self.logger = nil;
    }
}

- (QStatus)prepareBusAttachment:(id <AJNAuthenticationListener> )authListener
{
	self.busAttachment = [[AJNBusAttachment alloc] initWithApplicationName:@"CommonServiceApp" allowRemoteMessages:true];
    
	// Start the BusAttachment
	QStatus status = [self.busAttachment start];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:@"Failed to start AJNBusAttachment"];
		[self.busAttachment destroy];
		return ER_FAIL;
	}
	// Connect to the daemon using address provided
	status = [self.busAttachment connectWithArguments:nil];
	if (status != ER_OK) {
		[self.busAttachment destroy];
		return ER_FAIL;
	}
    
	if (authListener) {
		status = [self.busAttachment enablePeerSecurity:@"ALLJOYN_PIN_KEYX ALLJOYN_SRP_KEYX ALLJOYN_ECDHE_PSK" authenticationListener:authListener];
		if (status != ER_OK) {
			[self.busAttachment destroy];
			return ER_FAIL;
		}
	}
    
	// advertise Daemon
	status = [self.busAttachment requestWellKnownName:DAEMON_NAME withFlags:kAJNBusNameFlagDoNotQueue];
	if (status == ER_OK) {
		//advertise the name with a quite prefix for TC to find it
		status = [self.busAttachment advertiseName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, DAEMON_NAME] withTransportMask:kAJNTransportMaskAny];
		if (status != ER_OK) {
            [self.busAttachment releaseWellKnownName:DAEMON_NAME];
			[self.logger errorTag:[[self class] description] text:@"Failed to advertise daemon name"];
		} else {
			[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Succefully advertised daemon name %@", DAEMON_NAME]];
		}
	}
	return status;
}

- (void)producerCleanup
{
    [self.logger debugTag:[[self class] description] text:@"Stopping producer...."];
    [self.producerVC.view setHidden:YES];
    [self.producerVC stopProducer:self.isConsumerOn];
    self.isProducerOn = false;
}

- (void)consumerCleanup
{
    [self.logger debugTag:[[self class] description] text:@"Stopping consumer..."];
    [self.consumerVC.view setHidden:YES];
    [self.consumerVC stopConsumer:self.isProducerOn];
    self.isConsumerOn = false;
}

@end
