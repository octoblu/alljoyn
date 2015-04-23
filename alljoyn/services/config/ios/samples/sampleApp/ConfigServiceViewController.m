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

#import "ConfigServiceViewController.h"
#import "AJNVersion.h"
#import "AJNPasswordManager.h"
#import "ClientInformation.h"
#import "alljoyn/about/AJNAboutServiceApi.h"
#import "alljoyn/config/AJCFGPropertyStoreImpl.h"
#import "alljoyn/config/AJCFGConfigService.h"
#import "alljoyn/config/AJCFGConfigServiceListenerImpl.h"
#import "alljoyn/services_common/AJSVCGenericLoggerDefaultImpl.h"
#import "alljoyn/config/AJCFGConfigLogger.h"
#import "ConfigServiceListener.h"
#import "CommonBusListener.h"
#import "AppDelegate.h"



static NSString *const DEFAULTPASSCODE = @"000000";

static NSString *const DAEMON_QUIET_PREFIX = @"quiet@";  // About Client - quiet advertising
static NSString *const ABOUT_CONFIG_OBJECT_PATH = @"/Config";  // Config Service
static NSString *const ABOUT_CONFIG_INTERFACE_NAME = @"org.alljoyn.Config";  //Config Service
static bool ALLOWREMOTEMESSAGES = true; // About Client -  allow Remote Messages flag
static AJNSessionPort SERVICE_PORT; // About Service - service port

@interface ConfigServiceViewController ()

@property (strong, nonatomic) AJNBusAttachment *busAttachment;
@property (strong, nonatomic) AJCFGConfigServiceListenerImpl *configServiceListenerImpl;
@property (strong, nonatomic) AJCFGConfigService *configService;
@property (strong, nonatomic) AJCFGPropertyStoreImpl *propertyStore;
@property (strong, nonatomic) CommonBusListener *aboutSessionPortListener;
@property (strong, nonatomic) ConfigServiceListener *configServiceListener;
@property (strong, nonatomic) AJNAboutServiceApi *aboutServiceApi;
@property (strong, nonatomic) AJSVCGenericLoggerDefaultImpl *logger;

@property (nonatomic) bool isServiceOn;
@property (strong, nonatomic) NSString *uniqueID;
@property (nonatomic, strong) NSString *password;
@property (weak, nonatomic) IBOutlet UIButton *btnStartStopService;

@end

@implementation ConfigServiceViewController

- (void)viewDidLoad
{
	[super viewDidLoad];
    
	// create unique ID*/
	self.uniqueID = [[NSUUID UUID] UUIDString];
	self.isServiceOn = NO;
    
	self.logger = [[AJCFGConfigLogger sharedInstance] logger];
}

- (QStatus)startAboutService
{
	QStatus serviceStatus;
    
	// Set port
	int port = 900;
	SERVICE_PORT = (AJNSessionPort)port;
    
	// Create message bus
	self.busAttachment = [[AJNBusAttachment alloc] initWithApplicationName:@"ConfigService" allowRemoteMessages:ALLOWREMOTEMESSAGES];
	if (!self.busAttachment) {
		[self.logger errorTag:[[self class] description] text:@"Failed to create a message bus"];
		serviceStatus = ER_OUT_OF_MEMORY;
		return serviceStatus;
	}
    
	//start the bus
	serviceStatus = [self.busAttachment start];
	if (serviceStatus != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to start bus: %@", [AJNStatus descriptionForStatusCode:serviceStatus]]];
		return serviceStatus;
	}
    
	// Allocate and fill property store
	self.propertyStore = [[AJCFGPropertyStoreImpl alloc] initPointerToFactorySettingFile:[[NSBundle mainBundle] pathForResource:@"FactoryProperties" ofType:@"plist"]];
    
	serviceStatus = [self fillAboutPropertyStoreImplData];
    
	if (serviceStatus != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to fill propertyStore: %@", [AJNStatus descriptionForStatusCode:serviceStatus]]];
		return serviceStatus;
	}
    
	serviceStatus = [self.busAttachment connectWithArguments:@""];
	if (serviceStatus != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to connectWithArguments: %@", [AJNStatus descriptionForStatusCode:serviceStatus]]];
	}
    
	self.aboutSessionPortListener = [[CommonBusListener alloc] initWithServicePort:(SERVICE_PORT)];
    
	if (self.aboutSessionPortListener) {
		[self.busAttachment registerBusListener:self.aboutSessionPortListener];
	}
    
	[self.logger debugTag:[[self class] description] text:@"Create aboutServiceApi"];
	self.aboutServiceApi = [AJNAboutServiceApi sharedInstance];
	if (!self.aboutServiceApi) {
		serviceStatus =  ER_BUS_NOT_ALLOWED;
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to create aboutServiceApi: %@", [AJNStatus descriptionForStatusCode:serviceStatus]]];
		return serviceStatus;
	}
    
	[self.logger debugTag:[[self class] description] text:@"Start aboutServiceApi"];
	[self.aboutServiceApi startWithBus:self.busAttachment andPropertyStore:self.propertyStore];
    
	//Register Port
	[self.logger debugTag:[[self class] description] text:@"Register the AboutService on the AllJoyn bus"];
	if (self.aboutServiceApi.isServiceStarted) {
		serviceStatus = [self.aboutServiceApi registerPort:(SERVICE_PORT)];
	}
    
	if (serviceStatus != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed register port: %@", [AJNStatus descriptionForStatusCode:serviceStatus]]];
		return serviceStatus;
	}
    
	// bind session port
	AJNSessionOptions *opt = [[AJNSessionOptions alloc] initWithTrafficType:(kAJNTrafficMessages) supportsMultipoint:(false) proximity:(kAJNProximityAny) transportMask:(kAJNTransportMaskAny)];
	[self.logger debugTag:[[self class] description] text:@"Bind session"];
	serviceStatus = [self.busAttachment bindSessionOnPort:SERVICE_PORT withOptions:opt withDelegate:self.aboutSessionPortListener];
    
	if (serviceStatus == ER_ALLJOYN_BINDSESSIONPORT_REPLY_ALREADY_EXISTS) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"SessionPort already exists: %@", [AJNStatus descriptionForStatusCode:serviceStatus]]];
	}
	return serviceStatus;
}

- (QStatus)startConfigService
{
	QStatus status;
    
	status = [self startAboutService];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"About service failed to start: %@", [AJNStatus descriptionForStatusCode:status]]];
		return status;
	}
    
	status = [self enableServiceSecurity];
    
	if (ER_OK != status) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to enable security on the bus: %@", [AJNStatus descriptionForStatusCode:status]]];
	}
	else {
		[self.logger debugTag:[[self class] description] text:@"Successfully enabled security for the bus"];
	}
    
	self.configServiceListener = [[ConfigServiceListener alloc] initWithPropertyStore:self.propertyStore andBus:self.busAttachment];
    
	self.configServiceListenerImpl = [[AJCFGConfigServiceListenerImpl alloc] initWithConfigServiceListener:self.configServiceListener];
    
	self.configService = [[AJCFGConfigService alloc] initWithBus:self.busAttachment propertyStore:self.propertyStore listener:self.configServiceListenerImpl];
    
	// Set logger
	self.logger = [[AJSVCGenericLoggerDefaultImpl alloc] init];
	[self.configService setLogger:self.logger];
    
	NSMutableArray *interfaces = [[NSMutableArray alloc] init];
	[interfaces addObject:ABOUT_CONFIG_INTERFACE_NAME];
	NSString *path = ABOUT_CONFIG_OBJECT_PATH;
	status = [self.aboutServiceApi addObjectDescriptionWithPath:path andInterfaceNames:interfaces];
    
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to addObjectDescription: %@", [AJNStatus descriptionForStatusCode:status]]];
		return status;
	}
    
	status = [self.configService registerService];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to register configService: %@", [AJNStatus descriptionForStatusCode:status]]];
		return status;
	}
    
	status = [self.busAttachment registerBusObject:self.configService];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to register registerBusObject: %@", [AJNStatus descriptionForStatusCode:status]]];
		return status;
	}
    
	status = [self.busAttachment advertiseName:[self.busAttachment uniqueName] withTransportMask:kAJNTransportMaskAny];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to advertiseName [%@]: %@", [self.busAttachment uniqueName], [AJNStatus descriptionForStatusCode:status]]];
		return status;
	}
    
	if (status == ER_OK) {
		if (self.aboutServiceApi.isServiceStarted) {
			[self.logger debugTag:[[self class] description] text:@"Calling Announce"];
			status = [self.aboutServiceApi announce];
		}
		if (status == ER_OK) {
			[self.logger debugTag:[[self class] description] text:@"Successfully announced"];
		}
	}
    
	return status;
}

- (QStatus)enableServiceSecurity
{
	QStatus status;
    
	status = [self.busAttachment enablePeerSecurity:@"ALLJOYN_SRP_KEYX ALLJOYN_PIN_KEYX ALLJOYN_ECDHE_PSK" authenticationListener:self keystoreFileName:@"Documents/alljoyn_keystore/s_central.ks" sharing:YES];
	return status;
}

- (void)stopAboutService
{
	QStatus status;
    
	[self.logger debugTag:[[self class] description] text:@"Stop About Service"];
	// Delete AboutPropertyStoreImpl
	self.propertyStore = nil;
    
	// BusAttachment cleanup
	status = [self.busAttachment cancelAdvertisedName:[self.busAttachment uniqueName] withTransportMask:kAJNTransportMaskAny];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully cancel advertised nam"];
	}
    
	status = [self.busAttachment unbindSessionFromPort:SERVICE_PORT];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully unbind Session"];
	}
    
	// Delete AboutSessionPortListener
	[self.busAttachment unregisterBusListener:self.aboutSessionPortListener];
	self.aboutSessionPortListener = nil;
    
	// Stop bus attachment
	status = [self.busAttachment stop];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully stopped bus"];
	}
	self.busAttachment = nil;
} /* stopAboutService */

- (void)stopConfigService
{
	// Delete AboutServiceApi
	[self.aboutServiceApi destroyInstance];
    
	self.aboutServiceApi = nil;
    
	self.configService = nil;
    
	self.configServiceListenerImpl = nil;
    
	self.configServiceListener = nil;
    
	[self stopAboutService];
}

- (IBAction)touchUpInsideStartStopBtn:(UIButton *)sender
{
	QStatus status;
	if (self.isServiceOn == NO) {
		status = [self startConfigService];
		if (status == ER_OK) {
			[sender setTitle:@"Stop Service" forState:UIControlStateNormal];
			self.isServiceOn = YES;
		} else {
            [AppDelegate alertAndLog:@"Start Config Service Failed" status:status];
		}
	} else {
		[self stopConfigService];
		[sender setTitle:@"Start Service" forState:UIControlStateNormal];
		self.isServiceOn = NO;
	}
}

- (QStatus)fillAboutPropertyStoreImplData
{
	QStatus status;
    
	// AppId
	status = [self.propertyStore setAppId:self.uniqueID];
	if (status != ER_OK) return status;
    
	// AppName
	status = [self.propertyStore setAppName:@"AboutConfig"];
	if (status != ER_OK) return status;
    
	// DeviceId
	status = [self.propertyStore setDeviceId:@"1231232145667745675477"];
	if (status != ER_OK) return status;
    
	// DeviceName
	NSString *value = [self.propertyStore getPersistentValue:DEVICE_NAME_STR forLanguage:@"en"];
    
	value = (value ? value : @"Device Name(en)"); // check that we don't have a persistent value
    
	status = [self.propertyStore setDeviceName:value language:@"en"];
	if (status != ER_OK) return status;

    // DeviceName
	value = [self.propertyStore getPersistentValue:DEVICE_NAME_STR forLanguage:@"fr"];
    
	value = (value ? value : @"Device Name(fr)"); // check that we don't have a persistent value
    
	status = [self.propertyStore setDeviceName:value language:@"fr"];
	if (status != ER_OK) return status;

    // DeviceName
	value = [self.propertyStore getPersistentValue:DEVICE_NAME_STR forLanguage:@"sp"];
    
	value = (value ? value : @"Device Name(sp)"); // check that we don't have a persistent value
    
	status = [self.propertyStore setDeviceName:value language:@"sp"];
	if (status != ER_OK) return status;
    
	// SupportedLangs
	NSArray *languages = @[@"en", @"sp", @"fr"];
	status = [self.propertyStore setSupportedLangs:languages];
	if (status != ER_OK) return status;
    
	// DefaultLang
	value = [self.propertyStore getPersistentValue:DEFAULT_LANGUAGE_STR forLanguage:@""];
    
	value = (value ? value : @"en"); // check that we don't have a persistent value
    
	status = [self.propertyStore setDefaultLang:value];
	if (status != ER_OK) return status;
    
	// ModelNumber
	status = [self.propertyStore setModelNumber:@"Wxfy388i"];
	if (status != ER_OK) return status;
    
	// DateOfManufacture
	status = [self.propertyStore setDateOfManufacture:@"10/1/2199"];
	if (status != ER_OK) return status;
    
	// SoftwareVersion
	status = [self.propertyStore setSoftwareVersion:@"12.20.44 build 44454"];
	if (status != ER_OK) return status;
    
	// AjSoftwareVersion
	status = [self.propertyStore setAjSoftwareVersion:[AJNVersion versionInformation]];
	if (status != ER_OK) return status;
    
	// HardwareVersion
	status = [self.propertyStore setHardwareVersion:@"355.499. b"];
	if (status != ER_OK) return status;
    
	// Description
	status = [self.propertyStore setDescription:@"This is an Alljoyn Application" language:@"en"];
	if (status != ER_OK) return status;
    
	status = [self.propertyStore setDescription:@"Esta es una Alljoyn aplicación" language:@"sp"];
	if (status != ER_OK) return status;
    
	status = [self.propertyStore setDescription:@"C'est une Alljoyn application"  language:@"fr"];
	if (status != ER_OK) return status;
    
	// Manufacturer
	status = [self.propertyStore setManufacturer:@"Company" language:@"en"];
	if (status != ER_OK) return status;
    
	status = [self.propertyStore setManufacturer:@"Empresa" language:@"sp"];
	if (status != ER_OK) return status;
    
	status = [self.propertyStore setManufacturer:@"Entreprise" language:@"fr"];
	if (status != ER_OK) return status;
    
	status = [self.propertyStore setPasscode:DEFAULTPASSCODE];
	if (status != ER_OK) return status;
    
	// SupportedUrl
	status = [self.propertyStore setSupportUrl:@"http://www.alljoyn.org"];
	if (status != ER_OK) return status;
    
	return status;
}

#pragma mark - AJNAuthenticationListener protocol methods
- (AJNSecurityCredentials *)requestSecurityCredentialsWithAuthenticationMechanism:(NSString *)authenticationMechanism peerName:(NSString *)peerName authenticationCount:(uint16_t)authenticationCount userName:(NSString *)userName credentialTypeMask:(AJNSecurityCredentialType)mask
{
	AJNSecurityCredentials *creds = nil;
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"RequestSecurityCredentialsWithAuthenticationMechanism:%@ forRemotePeer%@ userName:%@", authenticationMechanism, peerName, userName]];
    
	if ([authenticationMechanism isEqualToString:@"ALLJOYN_SRP_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_PIN_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_ECDHE_PSK"]) {
		if (mask & kAJNSecurityCredentialTypePassword) {
			if (authenticationCount <= 3) {
				creds = [[AJNSecurityCredentials alloc] init];
				creds.password = [self.propertyStore getPasscode];
			}
		}
	}
	return creds;
}

- (void)authenticationUsing:(NSString *)authenticationMechanism forRemotePeer:(NSString *)peerName didCompleteWithStatus:(BOOL)success
{
	NSString *status;
	status = (success == YES ? @"was successful" : @"failed");
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Authentication using:%@ for remote peer%@ %@", authenticationMechanism, peerName, status]];
}

@end
