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

#import "ProducerViewController.h"
#import <alljoyn/Status.h>
#import "AJNVersion.h"
#import "alljoyn/about/AJNAboutServiceApi.h"
#import "alljoyn/samples_common/AJSCCommonBusListener.h"
#import "AJNPasswordManager.h"
#import "alljoyn/notification/AJNSNotificationSender.h"
#import "alljoyn/notification/AJNSNotificationService.h"
#import "alljoyn/services_common/AJSVCGenericLoggerUtil.h"
#import "NotificationUtils.h"

static NSString * const DEVICE_ID_PRODUCER= @"ProducerBasic";
static NSString * const DEVICE_NAME_PRODUCER= @"ProducerBasic";
static NSString * const DEFAULT_LANG_PRODUCER= @"en";
static NSString * const RICH_ICON_OBJECT_PATH= @"rich/Icon/Object/Path";
static NSString * const RICH_AUDIO_OBJECT_PATH= @"rich/Audio/Object/Path";
static short const SERVICE_PORT= 900;

static const uint16_t TTL_MIN = 30;
static const uint16_t TTL_MAX = 43200;
static NSString *const DEFAULT_TTL = @"40000";
static NSString *const DEFAULT_MSG_TYPE = @"INFO";


@interface ProducerViewController () <UIAlertViewDelegate>

@property (weak, nonatomic) AJNSNotificationSender *Sender;
@property (strong, nonatomic) AJNSNotificationService *producerService;
@property (strong, nonatomic) AJSCCommonBusListener *commonBusListener;
@property (weak, nonatomic) AJNAboutServiceApi *aboutService;
@property (strong, nonatomic) AJNAboutPropertyStoreImpl *aboutPropertyStoreImpl;
@property (strong, nonatomic) UIAlertView *selectLanguage;
@property (strong, nonatomic) UIAlertView *selectMessageType;
@property (strong, nonatomic) AJNSNotification *notification;
@property (strong, nonatomic) NSMutableArray *notificationTextArr;
@property (strong, nonatomic) NSMutableDictionary *customAttributesDictionary;
@property (strong, nonatomic) NSMutableArray *richAudioUrlArray;

@property (strong, nonatomic) NSString *defaultTTL; // Hold the default ttl as set in the UI
@property (strong, nonatomic) NSString *defaultMessageType; // Hold the default message type as set in the UI

@property (nonatomic) AJNSNotificationMessageType messageType;
@property (strong, nonatomic) NSString *otherLang;


@end

@implementation ProducerViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
	[self.view setHidden:YES];
}

- (QStatus)startProducer
{
	// Set TextField.delegate to enable dissmiss keyboard
	self.notificationEnTextField.delegate = self;
	self.notificationLangTextField.delegate = self;
	self.ttlTextField.delegate = self;
	self.audioTextField.delegate = self;
	self.iconTextField.delegate = self;
    
	// Set default ttl
	self.defaultTTL = DEFAULT_TTL;
	self.ttlTextField.text = DEFAULT_TTL;
	[self.ttlTextField setKeyboardType:UIKeyboardTypeNumberPad];
    
	// Set default messageType
	self.defaultMessageType = DEFAULT_MSG_TYPE;
	self.messageType = [self convertMessageType:self.defaultMessageType];
	self.messageTypeButton.titleLabel.text = DEFAULT_MSG_TYPE;
    
	// Set switch to off
	self.audioSwitch.on = false;
	self.iconSwitch.on = false;
    
	// Initialize a AJNSNotificationService object
	self.producerService =  [[AJNSNotificationService alloc] init];
    
	// Set logger (see a logger implementation example in  ConsumerViewController.m)
	[self.producerService setLogLevel:QLEVEL_DEBUG];
    
	// Confirm that bus is valid
	if (!self.busAttachment) {
		[self.logger fatalTag:[[self class] description] text:@"BusAttachment is nil"];
		return ER_FAIL;
	}
    
	// Prepare propertyStore
	[self.logger debugTag:[[self class] description] text:@"preparePropertyStore."];
	self.aboutPropertyStoreImpl = [[AJNAboutPropertyStoreImpl alloc] init];
    
	QStatus status = [self fillAboutPropertyStoreImplData:[[NSUUID UUID] UUIDString]
	                                      appName:self.appName
	                                     deviceId:DEVICE_ID_PRODUCER
	                                   deviceName:DEVICE_NAME_PRODUCER
	                              defaultLanguage:DEFAULT_LANG_PRODUCER];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:@"Could not fill PropertyStore."];
		return ER_FAIL;
	}
    
    // Prepare AJNSBusListener
    self.commonBusListener = [[AJSCCommonBusListener alloc] init];
    
    // Prepare AboutService
    [self.logger debugTag:[[self class] description] text:@"prepareAboutService"];
    status = [self prepareAboutService:self.commonBusListener servicePort:SERVICE_PORT];
    if (status != ER_OK) {
        [self.logger fatalTag:[[self class] description] text:@"Could not prepareAboutService."];
        return ER_FAIL;
    }
    
    // Call initSend
    self.Sender = [self.producerService startSendWithBus:self.busAttachment andPropertyStore:self.aboutPropertyStoreImpl];
    if (!self.Sender) {
        [self.logger fatalTag:[[self class] description] text:@"Could not initialize Sender"];
        return ER_FAIL;
    }
    
    // Call announce
    status = [self.aboutService announce];
    if (status != ER_OK) {
        [self.logger fatalTag:[[self class] description] text:@"Could not announce"];
        return ER_FAIL;
    } else {
        [self.logger debugTag:[[self class] description] text:@"Announce..."];
    }
    
	return ER_OK;
}

- (void)stopProducer:(bool) isConsumerOn
{
	QStatus status;
    
	[self.logger debugTag:[[self class] description] text:@"Stop Producer service"];
    
    if (self.Sender) {
        self.Sender = nil;
    }
    
    // AboutService destroy
    if (self.aboutService) {
        // AboutService destroyInstance
        [self.aboutService destroyInstance]; //isServiceStarted = false, call [self.aboutService unregister]
        status = [self.busAttachment unbindSessionFromPort:SERVICE_PORT];
        if (status != ER_OK) {
            [self.logger errorTag:[[self class] description] text:@"Failed to unbindSessionFromPort"];
        }
        self.aboutService = nil;
    }
    
    if (self.aboutPropertyStoreImpl) {
        self.aboutPropertyStoreImpl = nil;
    }
    
    // Shutdown producer
    if (self.producerService && isConsumerOn) {
        [self.logger debugTag:[[self class] description] text:@"calling shutdownSender"];
        [self.producerService shutdownSender];
    }
    else {
        [self.logger debugTag:[[self class] description] text:@"calling shutdown"];
        [self.producerService shutdown];
    }
    self.producerService = nil;
    
	// Unregister bus listener from the common
	if (self.busAttachment && self.commonBusListener) {
		[self.busAttachment unregisterBusListener:self.commonBusListener];
		[self.busAttachment unbindSessionFromPort:([self.commonBusListener sessionPort])];
		self.commonBusListener = nil;
	}
}

// ENUM convertMessageType
- (AJNSNotificationMessageType)convertMessageType:(NSString *)tMsgType
{
	if ([tMsgType isEqualToString:@"INFO"]) {
		return INFO;
	}
	else if ([tMsgType isEqualToString:@"WARNING"]) {
		return WARNING;
	}
	else if ([tMsgType isEqualToString:@"EMERGENCY"]) {
		return EMERGENCY;
	}
	else
		return UNSET;
}

- (void)producerPostSendView
{
	self.notificationEnTextField.text = nil;
	self.notificationLangTextField.text = nil;
    
	// Lang field-save perv selection
	// Color save perv selection
	self.ttlTextField.text = self.defaultTTL;
	if (self.iconSwitch.on) {
		self.iconSwitch.on = false;
		[self didChangeIconSwitchValue:nil];
	}
	if (self.audioSwitch.on) {
		self.audioSwitch.on = false;
		[self didChangeAudioSwitchValue:nil];
	}
	[self.messageTypeButton setTitle:self.defaultMessageType forState:(UIControlStateNormal)];
    
	self.messageType = [self convertMessageType:DEFAULT_MSG_TYPE];
    
	// Clear sent parameters:
	[self.notificationTextArr removeAllObjects];
}

-(void)logNotification:(AJNSNotification *)ajnsNotification ttl:(uint16_t)ttl
{
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Sending message with message type: '%@'",[AJNSNotificationEnums AJNSMessageTypeToString:[ajnsNotification messageType]]]];
    
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"TTL: %hu", ttl]];
    
    for (AJNSNotificationText *notificationText in self.notificationTextArr) {
        [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"message: '%@'",[notificationText getText]]];
    }
    
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"richIconUrl: '%@'",[ajnsNotification richIconUrl]]];
    
    NSMutableArray *array = [[NSMutableArray alloc]init];
    
    [ajnsNotification richAudioUrl:array];
    
    for (AJNSRichAudioUrl *richAudioURL in array) {
        [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"RichAudioUrl: '%@'",[richAudioURL url]]];
    }
    
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"richIconObjPath: '%@'",[ajnsNotification richIconObjectPath]]];
    
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"RichAudioObjPath: '%@'",[ajnsNotification richAudioObjectPath]]];
    
    [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Sending notification message for messageType '%d'",[ajnsNotification messageType]]];
    
}

#pragma mark - IBActions

// Send a notification
- (IBAction)didTouchSendNotificationButton:(id)sender
{
    uint16_t nttl;
    NSString *nsender = @"iosTestApp";
    NSString *richIconUrl;
    
	// Dismiss keyboard
	[self touchesBegan:sender withEvent:nil];
    
	// Build notification object
    
	// Set flags
	bool hasEnNotificationText = false;
	bool hasOtherLangNotificationText = false;
	bool ttlIsValid = false;
    
	// Create containers
	self.notificationTextArr = [[NSMutableArray alloc] init];
	self.customAttributesDictionary = [[NSMutableDictionary alloc] init];  // this dictionary holds custom attributes that are sent in notification. see below for an example on how to send the color custom attribute
	self.richAudioUrlArray = [[NSMutableArray alloc] init];
    
	// Set enNotificationText
	if ([NotificationUtils textFieldIsValid:self.notificationEnTextField.text]) {
		NSString *enNotificationText;
		enNotificationText = self.notificationEnTextField.text;
		// Insert into the array text and lang
		[self.notificationTextArr addObject:[[AJNSNotificationText alloc] initWithLang:@"en" andText:enNotificationText]];
		hasEnNotificationText = true;
	}
	else {
		// Create UIAlertView alert
		[[[UIAlertView alloc] initWithTitle:@"Error" message:@"At least one of the messages should be sent in english" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil] show];
        
		return;
	}
    
	// Set other lang message
	if ([NotificationUtils textFieldIsValid:self.notificationLangTextField.text] && self.otherLang) {
		NSString *langNotificationText;
		langNotificationText = self.notificationLangTextField.text;
		[self.notificationTextArr addObject:[[AJNSNotificationText alloc] initWithLang:self.otherLang andText:langNotificationText]];
		hasOtherLangNotificationText = true;
	}
    
	// Set ttl
	unsigned long rcv_ttl;
	rcv_ttl = [[self.ttlTextField text] intValue];
	if ((rcv_ttl >= TTL_MIN) && (rcv_ttl <= TTL_MAX)) {
		ttlIsValid = true;
		nttl = rcv_ttl;
	}
	else {
		NSString *ttlErrReport = [NSString stringWithFormat:@"ttl range is %hu - %hu. %lu is invalid", TTL_MIN, TTL_MAX, rcv_ttl];
		[[[UIAlertView alloc] initWithTitle:@"Error" message:ttlErrReport delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil] show];
		
	}
    
	// Set audio
	if (self.audioSwitch.on == true && ([NotificationUtils textFieldIsValid:(self.audioTextField.text)])) {
		AJNSRichAudioUrl *richAudioUrlObj = [[AJNSRichAudioUrl alloc] initRichAudioUrlWithLang:@"en" andUrl:self.audioTextField.text];
		[self.richAudioUrlArray addObject:richAudioUrlObj];
	}
    
	// Set icon
	if (self.iconSwitch.on == true && ([NotificationUtils textFieldIsValid:(self.iconTextField.text)])) {
		richIconUrl = self.iconTextField.text;
	}
    
	// Set color example - this is how to set the color
	// [customAttributesDictionary setValue:@"Blue" forKey:@"color"];
    
	NSString *curr_controlPanelServiceObjectPath = @"";
    
	// Set the Notification object
	if ((hasEnNotificationText) && ttlIsValid && (self.messageType != UNSET)) {
        
        self.notification = [[AJNSNotification alloc] initWithMessageType:self.messageType andNotificationText:self.notificationTextArr];
        
        // This is an exaple of using AJNSNotification setters :
        [self.notification setMessageId:-1];
        [self.notification setDeviceId:nil];
        [self.notification setDeviceName:nil];
        [self.notification setAppId:nil];
        [self.notification setAppName:self.appName];
        [self.notification setSender:nsender];
        [self.notification setCustomAttributes:self.customAttributesDictionary];
        [self.notification setRichIconUrl:richIconUrl];
        [self.notification setRichAudioUrl:self.richAudioUrlArray];
        [self.notification setRichIconObjectPath:RICH_ICON_OBJECT_PATH];
        [self.notification setRichAudioObjectPath:RICH_AUDIO_OBJECT_PATH];
        [self.notification setControlPanelServiceObjectPath:curr_controlPanelServiceObjectPath];
        
        
        [self logNotification:self.notification ttl:nttl];
        
        [self.richAudioUrlArray removeAllObjects];
        
		// Call send
		QStatus sendStatus = [self.Sender send:self.notification ttl:nttl];
		if (sendStatus != ER_OK) {
			[self.logger infoTag:[[self class] description] text:[NSString stringWithFormat:@"Send has failed"]];
		}
		else {
			[self.logger infoTag:[[self class] description] text:[NSString stringWithFormat:@"Successfully sent!"]];
		}
		[self producerPostSendView];
	}
	else {
		[self.logger infoTag:[[self class] description] text:[NSString stringWithFormat:@"Invalid Notification input:\n EnNotificationText=%u\n OtherLangNotificationText=%u\n ttlIsValid=%u\n hasMessageType=%u", hasEnNotificationText, hasOtherLangNotificationText, ttlIsValid, self.messageType]];
	}
	[self producerPostSendView];
}

- (IBAction)didChangeAudioSwitchValue:(id)sender
{
	self.audioTextField.alpha = self.audioSwitch.on;
}

- (IBAction)didChangeIconSwitchValue:(id)sender
{
	self.iconTextField.alpha = self.iconSwitch.on;
}

- (IBAction)didTouchDeleteButton:(id)sender
{
	[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"message type is %u", self.messageType]];
    
	if (self.messageType == UNSET)
		return;
    
	QStatus deleteStatus = [self.Sender deleteLastMsg:self.messageType];
    
	if (deleteStatus != ER_OK) {
		[self.logger errorTag:[[self class] description] text:@"Failed to delete a message"];
	}
}

#pragma mark - About Service methods

- (QStatus)prepareAboutService:(AJSCCommonBusListener *)busListener servicePort:(AJNSessionPort)port
{
	if (!self.busAttachment)
		return ER_BAD_ARG_1;
    
	if (!busListener)
		return ER_BAD_ARG_2;
    
    if (!self.aboutPropertyStoreImpl)
    {
        [self.logger errorTag:[[self class] description] text:@"PropertyStore is empty"];
        return ER_FAIL;
    }
    
    // Prepare About Service
    self.aboutService = [AJNAboutServiceApi sharedInstance];
    
    if (!self.aboutService)
    {
        return ER_BUS_NOT_ALLOWED;
    }
    
    [self.aboutService startWithBus:self.busAttachment andPropertyStore:self.aboutPropertyStoreImpl]; //isServiceStarted = true
    
    
    
    [self.logger debugTag:[[self class] description] text:@"registerBusListener"];
    [busListener setSessionPort:port];
    [self.busAttachment registerBusListener:busListener];
    
	AJNSessionOptions *opt = [[AJNSessionOptions alloc] initWithTrafficType:(kAJNTrafficMessages) supportsMultipoint:(false) proximity:(kAJNProximityAny) transportMask:(kAJNTransportMaskAny)];
	QStatus aboutStatus = [self.busAttachment bindSessionOnPort:SERVICE_PORT withOptions:opt withDelegate:busListener];
    
	if (aboutStatus == ER_ALLJOYN_BINDSESSIONPORT_REPLY_ALREADY_EXISTS)
		[self.logger infoTag:[[self class] description] text:([NSString stringWithFormat:@"bind status: ER_ALLJOYN_BINDSESSIONPORT_REPLY_ALREADY_EXISTS"])];
	if (aboutStatus != ER_OK)
		return aboutStatus;
	return [self.aboutService registerPort:(SERVICE_PORT)];
}


-(QStatus)fillAboutPropertyStoreImplData:(NSString*) appId appName:(NSString*) appName deviceId:(NSString*)deviceId deviceName:(NSString*) deviceName defaultLanguage:(NSString*) defaultLang
{
	QStatus status;
    
	// AppId
	status = [self.aboutPropertyStoreImpl setAppId:appId];
    
	if (status != ER_OK) {
		return status;
	}
    
	// AppName
	status = [self.aboutPropertyStoreImpl setAppName:appName];
	if (status != ER_OK) {
		return status;
	}
    
	// DeviceId
	status = [self.aboutPropertyStoreImpl setDeviceId:deviceId];
	if (status != ER_OK) {
		return status;
	}
    
	// DeviceName
	status = [self.aboutPropertyStoreImpl setDeviceName:deviceName];
    
	if (status != ER_OK) {
		return status;
	}
    
	// SupportedLangs
	NSArray *languages = @[@"en", @"sp", @"fr"];
	status = [self.aboutPropertyStoreImpl setSupportedLangs:languages];
	if (status != ER_OK) {
		return status;
	}
    
	// DefaultLang
	status = [self.aboutPropertyStoreImpl setDefaultLang:defaultLang];
	if (status != ER_OK) {
		return status;
	}
    
	// ModelNumber
	status = [self.aboutPropertyStoreImpl setModelNumber:@"Wxfy388i"];
	if (status != ER_OK) {
		return status;
	}
    
	// DateOfManufacture
	status = [self.aboutPropertyStoreImpl setDateOfManufacture:@"10/1/2199"];
	if (status != ER_OK) {
		return status;
	}
    
	// SoftwareVersion
	status = [self.aboutPropertyStoreImpl setSoftwareVersion:@"12.20.44 build 44454"];
	if (status != ER_OK) {
		return status;
	}
    
	// AjSoftwareVersion
	status = [self.aboutPropertyStoreImpl setAjSoftwareVersion:[AJNVersion versionInformation]];
	if (status != ER_OK) {
		return status;
	}
    
	// HardwareVersion
	status = [self.aboutPropertyStoreImpl setHardwareVersion:@"355.499. b"];
	if (status != ER_OK) {
		return status;
	}
    
	// Description
	status = [self.aboutPropertyStoreImpl setDescription:@"This is an Alljoyn Application" language:@"en"];
	if (status != ER_OK) {
		return status;
	}
    
	status = [self.aboutPropertyStoreImpl setDescription:@"Esta es una Alljoyn aplicación" language:@"sp"];
	if (status != ER_OK) {
		return status;
	}
    
	status = [self.aboutPropertyStoreImpl setDescription:@"C'est une Alljoyn application"  language:@"fr"];
	if (status != ER_OK) {
		return status;
	}
    
	// Manufacturer
	status = [self.aboutPropertyStoreImpl setManufacturer:@"Company" language:@"en"];
	if (status != ER_OK) {
		return status;
	}
    
	status = [self.aboutPropertyStoreImpl setManufacturer:@"Empresa" language:@"sp"];
	if (status != ER_OK) {
		return status;
	}
    
    status = [self.aboutPropertyStoreImpl setManufacturer:@"Entreprise" language:@"fr"];
	if (status != ER_OK) {
		return status;
	}
    
	// SupportedUrl
	status = [self.aboutPropertyStoreImpl setSupportUrl:@"http://www.alljoyn.org"];
	if (status != ER_OK) {
		return status;
	}
    
	return status;
}

- (void)viewWillDisappear:(BOOL)animated
{
	[super viewWillDisappear:animated];
}

- (IBAction)selectLanguagePressed:(UIButton *)sender
{
	[self showSelectLanguageAlert];
}

- (IBAction)selectMessageTypePressed:(UIButton *)sender
{
	[self showMessageTypeAlert];
}

- (void)showSelectLanguageAlert
{
	NSArray *langArray = [[NSMutableArray alloc] initWithObjects:@"English", @"Hebrew", @"Russian", nil];
    
	self.selectLanguage = [[UIAlertView alloc] initWithTitle:@"Select Language" message:nil delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
    
	for (NSString *str in langArray) {
		[self.selectLanguage addButtonWithTitle:str];
	}
    
	[self.selectLanguage show];
}

- (void)showMessageTypeAlert
{
	NSArray *messageTypeArray = [[NSMutableArray alloc] initWithObjects:@"INFO", @"WARNING", @"EMERGENCY", nil];
    
	self.selectMessageType = [[UIAlertView alloc] initWithTitle:@"Select Language" message:nil delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:nil];
    
	for (NSString *str in messageTypeArray) {
		[self.selectMessageType addButtonWithTitle:str];
	}
    
	[self.selectMessageType show];
}

#pragma mark - UIAlertView delegate function

- (void)alertView:(UIAlertView *)alertView didDismissWithButtonIndex:(NSInteger)buttonIndex
{
	if (buttonIndex == 0) {
		return; //Cancel was pressed
	}
	if (alertView == self.selectLanguage) {
		self.otherLang = [self convertToLangCode:[alertView buttonTitleAtIndex:buttonIndex]];
		[self.langButton setTitle:[alertView buttonTitleAtIndex:buttonIndex] forState:UIControlStateNormal];
	}
	else if (alertView == self.selectMessageType) {
		self.messageType = [self convertMessageType:[alertView buttonTitleAtIndex:buttonIndex]];
		[self.messageTypeButton setTitle:[alertView buttonTitleAtIndex:buttonIndex] forState:UIControlStateNormal];
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

// Set dismiss keyboard for each UITextField
- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
	for (UIView *aSubview in[self.view subviews]) {
		if ([aSubview isKindOfClass:[UITextField class]]) {
			[(UITextField *)aSubview resignFirstResponder];
		}
	}
}

@end
