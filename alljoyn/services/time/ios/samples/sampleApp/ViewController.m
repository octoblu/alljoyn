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

#import "ViewController.h"
#import "AJNAboutObject.h"
#import "alljoyn/time/AJTMTimeServiceServer.h"
#import "AJNVersion.h"
#import "AppDelegate.h"
#import "CommonBusListener.h"
#import "AuthenticationListenerImpl.h"
#import "SampleClock.h"
#import "SampleAlarm.h"
#import "SampleTimer.h"
#import "alljoyn/time/AJTMTimeServiceConstants.h"
#import "alljoyn/time/AJTMTimeServiceClient.h"
#import "alljoyn/time/AJTMTimeServiceClientClock.h"


static bool ALLOWREMOTEMESSAGES = true; // About Client -  allow Remote Messages flag
static NSString *const APPNAME = @"TimeServiceApp";  //About Client - default application name
static NSString *const KEYSTORE_FILE_PATH = @"Documents/alljoyn_keystore/s_central.ks";
static NSString * const AUTH_MECHANISM = @"ALLJOYN_SRP_KEYX ALLJOYN_PIN_KEYX ALLJOYN_ECDHE_PSK";
static NSString *const DAEMON_QUIET_PREFIX = @"quiet@";    //About Client - quiet advertising
static NSString *const DEFAULT_PASSCODE = @"000000";
static AJNSessionPort SERVICE_PORT = 900; // About Service - service port
static NSString *const TIME_INTERFACES = @"org.allseen.Time*"; // The interfaces that implement the time service
static NSString *const DEVICEID = @"1231232145667745675477";


@interface ViewController () <AJTMTimeServiceSessionListener, AJNAboutDataListener>

- (IBAction)startServiceButtonDidTouchUpInside:(id)sender;
@property AJNBusAttachment* bus;

@property (strong, nonatomic) NSMutableDictionary *peersPasscodes; // Store the peers passcodes
@property (strong, nonatomic) NSString *realmBusName;
@property (strong, nonatomic) CommonBusListener *aboutSessionPortListener;
@property (strong, nonatomic) NSString *uniqueID;
@property (strong, nonatomic) AJNAboutObject *aboutObject;
@property (strong, nonatomic) AuthenticationListenerImpl *authenticationListenerImpl;

// About Client properties
@property (strong, nonatomic) AJNBusAttachment *clientBusAttachment;
@property (nonatomic) bool isAboutClientConnected;
// NOT USED @property (strong, nonatomic) NSMutableDictionary *clientInformationDict; // Store the client related information

@property (strong,nonatomic) AJTMTimeServiceClient *timeClient;

@property (nonatomic) BOOL active;

@end

@implementation ViewController

- (void)viewDidLoad
{
    [super viewDidLoad];

    NSError *error = nil;
    NSString *keystoreFilePath = [NSString stringWithFormat:@"%@/alljoyn_keystore/s_central.ks", [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]];

    if ([[NSFileManager defaultManager] fileExistsAtPath:keystoreFilePath]) {

        [[NSFileManager defaultManager] removeItemAtPath:keystoreFilePath error:&error];
        if (error) {
            NSLog(@"ERROR: Unable to delete keystore. %@", error);
        }
    }

    self.uniqueID = [[NSUUID UUID] UUIDString];
    self.realmBusName = @"org.alljoyn.BusNode.timeService";

    QStatus status = [self startAboutClient];
    if ((status != ER_OK) || (error)) {
        [self.startServiceButton setTitle:@"error in start about client" forState:UIControlStateNormal];
        [self.startServiceButton setEnabled:FALSE];

    }

    _active = NO;
}

- (void)stopAboutService
{
    QStatus status;

    // BusAttachment cleanup
    status = [self.bus cancelAdvertisedName:[self.bus uniqueName] withTransportMask:kAJNTransportMaskAny];
    if (status == ER_OK) {
        NSLog(@"Successfully cancel advertised nam");
    }

    status = [self.bus unbindSessionFromPort:SERVICE_PORT];
    if (status == ER_OK) {
        NSLog(@"Successfully unbind Session");
    }

    // Delete AboutSessionPortListener
    [self.bus unregisterBusListener:self.aboutSessionPortListener];
    self.aboutSessionPortListener = nil;

    // Stop bus attachment
    status = [self.bus stop];
    if (status == ER_OK) {
        NSLog(@"Successfully stopped bus");
    }
    self.bus = nil;
} /* stopAboutService */


- (void)announce
{
	NSLog(@"[%@] [%@] Calling announceForSessionPort", @"DEBUG", [[self class] description]);
	QStatus status = [self.aboutObject announceForSessionPort:SERVICE_PORT withAboutDataListener:self];
	if (status == ER_OK) {
		NSLog(@"[%@] [%@] Successfully announced", @"DEBUG", [[self class] description]);
	}
}

- (IBAction)startServiceButtonDidTouchUpInside:(id)sender {

    if (_active == YES) {
        [self stopAboutClient];
        [self stopAboutService];

        [_startServiceButton setTitle :@"Stopped Server" forState:(UIControlStateNormal)];

        [_startServiceButton setEnabled:FALSE];
        return;

    }





    [self startAboutService];

    AJTMTimeServiceServer* server = [AJTMTimeServiceServer sharedInstance];

    NSLog(@"Server is %@", [server isStarted] ? @"started" : @"stopped");

    QStatus status = [server initWithBus:self.bus];

    NSLog(@"Server is %@", [server isStarted] ? @"started" : @"stopped");

    if (status != ER_OK) {
        NSLog(@"Failed to start the TimeServiceServer: %@", [AJNStatus descriptionForStatusCode:status]);
        return;
    } else {
        [_startServiceButton setTitle :@"Stop Server" forState:(UIControlStateNormal)];
        NSLog(@"Got AJTMTimeServiceServer!");
    }

    //Clock
    AJTMTimeServiceServerClockBase *clock = [[SampleClock alloc] init];
    NSLog(@"Clock objectPath: %@", [clock objectPath]);

    status = [server createClock:clock];
    if (status != ER_OK) {
        NSLog(@"Failed to create clock: %@", [AJNStatus descriptionForStatusCode:status]);
        return;
    }

    /*
    AJTMTimeServiceServerAuthorityClock* authClock = [[AJTMTimeServiceServerAuthorityClock alloc] init]; // object should conformsToProtocol AJTMTimeServiceServerClock
    AJTMClockAuthorityType authType = CELLULAR;
    status = [server createTimeAuthorityClock:authClock authorityType:authType];
    if (status != ER_OK) {
        NSLog(@"Failed to create time authority clock: %@", [AJNStatus descriptionForStatusCode:status]);
        return;
    } else {
        status = [authClock timeSync];
        if (status != ER_OK) {
            NSLog(@"Failed to run time sync: %@", [AJNStatus descriptionForStatusCode:status]);
            return;
        }
    }
     
     */

    //Alarm
    AJTMTimeServiceServerAlarmBase *alarm = [[SampleAlarm alloc] init];
    status = [server createAlarm:alarm];
    if (status != ER_OK) {
        NSLog(@"Failed to create alarm: %@", [AJNStatus descriptionForStatusCode:status]);
        return;
    }

    //    -(QStatus)createAlarm:(AJTMTimeServiceServerAlarmBase*) alarm description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;
    //    -(QStatus)registerCustomAlarm:(AJTMTimeServiceServerAlarmBase*) alarmBusObj alarm:(id<AJTMTimeServiceServerAlarm>) alarm notAnnounced:(NSArray*) notAnnounced;
    //    -(QStatus)registerCustomAlarm:(id<AJTMTimeServiceAlarmBusObj>) alarmBusObj alarm:(AJTMTimeServiceServerAlarmBase*) alarm notAnnounced:(NSArray*) notAnnounced description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;

    //Timer
    AJTMTimeServiceServerTimerBase *timer = [[SampleTimer alloc] init];
    status = [server createTimer:timer];
    if (status != ER_OK) {
        NSLog(@"Failed to create timer: %@", [AJNStatus descriptionForStatusCode:status]);
        return;
    }

    /*  Server API's
     //Factory
     -(QStatus) createAlarmFactory:(AJTMTimeServiceServerAlarmFactoryBase*) alarmFactory;
     -(QStatus) createAlarmFactory:(AJTMTimeServiceServerAlarmFactoryBase*) alarmFactory description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;
     -(QStatus) createTimerFactory:(AJTMTimeServiceServerTimerFactoryBase*) timerFactory;
     -(QStatus) createTimerFactory:(AJTMTimeServiceServerTimerFactoryBase*) timerFactory description:(NSString*) description language:(NSString*) language translator:(id<AJNTranslator>) translator;


     -(QStatus)generateObjectPath:(NSString*) objectPath;
     */

    [self announce];

    _active = YES;
}

- (void)startAboutService
{
    QStatus status;

    NSLog(@"[%@] [%@] Start About Service", @"DEBUG", [[self class] description]);

    NSLog(@"[%@] [%@] AboutService using port %hu", @"DEBUG", [[self class] description], SERVICE_PORT);

    // Create message bus

    NSLog(@"[%@] [%@] Create a message bus", @"DEBUG", [[self class] description]);

    self.bus = [[AJNBusAttachment alloc] initWithApplicationName:@"AboutServiceName" allowRemoteMessages:ALLOWREMOTEMESSAGES];
    if (!self.bus) {

        NSLog(@"[%@] [%@] Failed to create a message bus - exiting application", @"DEBUG", [[self class] description]);

        status = ER_OUT_OF_MEMORY;
        exit(status);
    }
    // Start the bus
    NSLog(@"[%@] [%@] Start bus", @"DEBUG", [[self class] description]);

    status = [self.bus start];
    if (status != ER_OK) {

        NSLog(@"[%@] [%@] Failed to start bus - exiting application", @"FATAL", [[self class] description]);

        exit(status);
    }

    NSLog(@"[%@] [%@] Connect to an AllJoyn daemon", @"DEBUG", [[self class] description]);

    status = [self.bus connectWithArguments:@""];
    if (status != ER_OK) {
        NSLog(@"[%@] [%@] Failed to start bus - exiting application", @"FATAL", [[self class] description]);

        exit(status);
    }

    NSLog(@"[%@] [%@] Create AboutSessionPortListener", @"DEBUG", [[self class] description]);

    self.aboutSessionPortListener = [[CommonBusListener alloc] initWithServicePort:SERVICE_PORT];

    if (self.aboutSessionPortListener) {
        NSLog(@"[%@] [%@] Register AboutSessionPortListener", @"DEBUG", [[self class] description]);

        [self.bus registerBusListener:self.aboutSessionPortListener];
    }

	//start AboutObject
	self.aboutObject = [[AJNAboutObject alloc] initWithBusAttachment:self.bus withAnnounceFlag:ANNOUNCED];

    // Bind session port
    AJNSessionOptions *opt = [[AJNSessionOptions alloc] initWithTrafficType:kAJNTrafficMessages supportsMultipoint:false proximity:kAJNProximityAny transportMask:kAJNTransportMaskAny];

    NSLog(@"[%@] [%@] Bind session", @"DEBUG", [[self class] description]);


    status = [self.bus bindSessionOnPort:SERVICE_PORT withOptions:opt withDelegate:self.aboutSessionPortListener];

    if (status == ER_ALLJOYN_BINDSESSIONPORT_REPLY_ALREADY_EXISTS) {
        NSLog(@"[%@] [%@] SessionPort already exists", @"DEBUG", [[self class] description]);
    }


    self.authenticationListenerImpl = [[AuthenticationListenerImpl alloc] init];
    status = [self enableServiceSecurity];

    if (ER_OK != status) {
        NSLog(@"Failed to enable security on the bus. %@", [AJNStatus descriptionForStatusCode:status]);
    }
    else {
        NSLog(@"Successfully enabled security for the bus");
    }
}

- (QStatus)startAboutClient
{
    QStatus status;

    //    [[AJCFGConfigLogger sharedInstance] debugTag:[[self class] description] text:@"Start About Client"];

    // Init AJNBusAttachment
    self.clientBusAttachment = [[AJNBusAttachment alloc] initWithApplicationName:APPNAME allowRemoteMessages:(ALLOWREMOTEMESSAGES)];

    // Start AJNBusAttachment
    status = [self.clientBusAttachment start];
    if (status != ER_OK) {
        [AppDelegate alertAndLog:@"Failed AJNBusAttachment start" status:status];
        [self stopAboutClient];
        return status;
    }

    // Connect AJNBusAttachment
    status = [self.clientBusAttachment connectWithArguments:@""];
    if (status != ER_OK) {
        [AppDelegate alertAndLog:@"Failed AJNBusAttachment connectWithArguments" status:status];
        [self stopAboutClient];
        return status;
    }


    [self.clientBusAttachment registerBusListener:self];

	// registering for Announcements has two steps:
	// (1) registerAboutListener - register the listener to the announce callback
	// (2) whoImplementsInterfaces - filters which announcements will be received by stating the interfaces
	[self.clientBusAttachment registerAboutListener:self];
	NSArray* interfaces = [NSArray arrayWithObject:[NSString stringWithString:TIME_INTERFACES]];
	status = [self.clientBusAttachment whoImplementsInterfaces:interfaces numberOfInterfaces:[interfaces count]];
	if (status != ER_OK) {
        [AppDelegate alertAndLog:@"Failed whoImplementsInterfaces" status:status];
        [self stopAboutClient];
        return status;
    }

     // Advertise Daemon for tcl
    status = [self.clientBusAttachment requestWellKnownName:self.realmBusName withFlags:kAJNBusNameFlagDoNotQueue];
    if (status == ER_OK) {
        // Advertise the name with a quite prefix for TC to find it
        status = [self.clientBusAttachment advertiseName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, self.realmBusName] withTransportMask:kAJNTransportMaskAny];
        if (status != ER_OK) {
            [AppDelegate alertAndLog:@"Failed to advertise name" status:status];
            [self stopAboutClient];
            return status;
        }
        else {
            NSLog(@"Successfully advertised: %@%@", DAEMON_QUIET_PREFIX, self.realmBusName);
        }
    }
    else {
        [AppDelegate alertAndLog:@"Failed to requestWellKnownName" status:status];
        [self stopAboutClient];
        return status;
    }

//    [self.connectButton setTitle:self.ajdisconnect forState:UIControlStateNormal]; //change title to "Disconnect from AllJoyn"

    // Enable Client Security
    status = [self enableClientSecurity];
    if (ER_OK != status) {
        NSLog(@"Failed to enable security on the bus. %@", [AJNStatus descriptionForStatusCode:status]);
    } else {
         NSLog(@"Successfully enabled security for the bus");
    }

    self.isAboutClientConnected = true;

    return status;
}


- (void)stopAboutClient
{
    NSLog(@"Stop About Client");

    QStatus status;
    status = [self.clientBusAttachment enablePeerSecurity:nil authenticationListener:self keystoreFileName:KEYSTORE_FILE_PATH sharing:FALSE];
    if (status == ER_OK) {
        NSLog(@"Successfully disabled peer security");
    }

     // Bus attachment cleanup
     status = [self.clientBusAttachment cancelAdvertisedName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, self.realmBusName] withTransportMask:kAJNTransportMaskAny];
     if (status == ER_OK) {
     NSLog(@"Successfully cancel advertised name");
     }
     status = [self.bus releaseWellKnownName:self.realmBusName];
     if (status == ER_OK) {
     NSLog(@"Successfully release WellKnownName");
     }
     status = [self.bus removeMatchRule:@"sessionless='t',type='error'"];
     if (status == ER_OK) {
     NSLog(@"Successfully remove MatchRule");
     }

     // Cancel listening to advertisements
	 [self.bus unregisterAllAboutListeners];

	 // Cancel listening to annoucements
	[self.bus cancelWhoImplements:TIME_INTERFACES];

     // Stop bus attachment
     status = [self.bus stop];
     if (status == ER_OK) {
     NSLog(@"Successfully stopped bus");
     }
     self.bus = nil;

     self.peersPasscodes = nil;


     // Set flag
     self.isAboutClientConnected  = false;

     NSLog(@"About Client is stopped");


}

#pragma mark AJNBusListener protocol methods
- (void)didFindAdvertisedName:(NSString *)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString *)namePrefix
{
    NSLog(@"didFindAdvertisedName has been called");
}

- (void)didLoseAdvertisedName:(NSString *)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString *)namePrefix
{
    NSLog(@"didLoseAdvertisedName has been called");
    //QStatus status;
    //    // Find the button title that should be removed
    //    for (NSString *key in[self.clientInformationDict allKeys]) {
    //        if ([[[[self.clientInformationDict valueForKey:key] announcement] busName] isEqualToString:name]) {
    //            // Cancel advertise name for that bus
    //            status = [self.bus cancelFindAdvertisedName:name];
    //            if (status != ER_OK) {
    //                NSLog(@"Failed to cancelFindAdvertisedName for %@. status:%@", name, [AJNStatus descriptionForStatusCode:status]);
    //            }
    //            // Remove the anouncement from the dictionary
    //            [self.clientInformationDict removeObjectForKey:key];
    //            [self.servicesTable reloadData];
    //        }
    //    }
}

- (QStatus)enableServiceSecurity
{
    QStatus status;

    status = [self.bus enablePeerSecurity:@"ALLJOYN_SRP_KEYX ALLJOYN_PIN_KEYX ALLJOYN_ECDHE_PSK" authenticationListener:self keystoreFileName:@"Documents/alljoyn_keystore/s_central.ks" sharing:FALSE];
    return status;
}

- (QStatus)enableClientSecurity
{
    QStatus status;
    status = [self.clientBusAttachment enablePeerSecurity:AUTH_MECHANISM authenticationListener:self keystoreFileName:KEYSTORE_FILE_PATH sharing:FALSE];

    return status;
}

#pragma mark - AJNAuthenticationListener protocol methods
- (AJNSecurityCredentials *)requestSecurityCredentialsWithAuthenticationMechanism:(NSString *)authenticationMechanism peerName:(NSString *)peerName authenticationCount:(uint16_t)authenticationCount userName:(NSString *)userName credentialTypeMask:(AJNSecurityCredentialType)mask
{
    AJNSecurityCredentials *creds = nil;
    bool credFound = false;

    NSLog(@"requestSecurityCredentialsWithAuthenticationMechanism:%@ forRemotePeer%@ userName:%@", authenticationMechanism, peerName, userName);

    if ([authenticationMechanism isEqualToString:@"ALLJOYN_SRP_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_PIN_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_ECDHE_PSK"]) {
        if (mask & kAJNSecurityCredentialTypePassword) {
            if (authenticationCount <= 3) {
                creds = [[AJNSecurityCredentials alloc] init];

                // Check if the password stored in peersPasscodes
                for (NSString *key in self.peersPasscodes.allKeys) {
                    if ([key isEqualToString:peerName]) {
                        creds.password = (self.peersPasscodes)[key];
                        NSLog(@"Found password %@ for peer %@", creds.password, key);
                        credFound = true;
                        break;
                    }
                }
                // Use the default password
                if (!credFound) {
                    creds.password = DEFAULT_PASSCODE;
                    NSLog(@"Using default password %@ for peer %@", DEFAULT_PASSCODE, peerName);
                }
            }
        }
    }
    return creds;
}

#pragma mark - AJNAboutListener protocol method
// Here we receive an announcement from AJN and add it to the client's list of services avaialble
-(void)didReceiveAnnounceOnBus:(NSString *)busName withVersion:(uint16_t)version withSessionPort:(AJNSessionPort)port withObjectDescription:(AJNMessageArgument *)objectDescriptionArg withAboutDataArg:(AJNMessageArgument *)aboutDataArg
{
	// Start Config Client
    self.timeClient = [[AJTMTimeServiceClient alloc] init ];
    QStatus status = [self.timeClient populateWithBus:self.clientBusAttachment serverBusName:busName deviceId:DEVICEID appId:self.uniqueID objDescArgs:objectDescriptionArg];
    if (status != ER_OK) {
        [AppDelegate alertAndLog:@"Failed to start Config Client" status:status];
        [self stopAboutClient];
        return;
    }

    [self.timeClient joinSessionAsyncWithListener:self];
}

#pragma mark - AJNAuthenticationListener protocol methods

- (void)authenticationUsing:(NSString *)authenticationMechanism forRemotePeer:(NSString *)peerName didCompleteWithStatus:(BOOL)success
{
    NSString *status;
    status = (success == YES ? @"was successful" : @"failed");

    NSLog(@"authenticationUsing:%@ forRemotePeer%@ %@", authenticationMechanism, peerName, status);
}

-(void)sessionLost:(AJTMTimeServiceClient*)timeServiceClient SessionListener:(AJNSessionLostReason)reason
{
    NSLog(@"client lost session");
}

-(void)sessionJoined:(AJTMTimeServiceClient*)timeServiceClient staus:(QStatus)status
{
        NSLog(@"client joined session");

    NSArray *announcedClockList = [self.timeClient getAnnouncedClockList];

    AJTMTimeServiceClientClock *firstClock = announcedClockList[0];

    AJTMTimeServiceDate *ldate = [[AJTMTimeServiceDate alloc]init ];
    [ldate populateWithYear:2010 month:10 day:12];
    AJTMTimeServiceTime *ltime = [[AJTMTimeServiceTime alloc]init];
    [ltime populateWithHour:14 minute:25 second:12 millisecond:80];

    AJTMTimeServiceDateTime *dateTime = [[AJTMTimeServiceDateTime alloc]init ];
    [dateTime populateWithDate:ldate time:ltime offsetMinutes:0];
     status = [firstClock setDateTime:dateTime];

    if (status != ER_OK) {
        return;
    }

    AJTMTimeServiceDateTime *gotDateTime;
    [firstClock retrieveDateTime:&gotDateTime];
    AJTMTimeServiceDate *date = [gotDateTime date];
    AJTMTimeServiceTime *time = [gotDateTime time];

    NSLog(@"%d/%d/%d %d:%d:%d.%d",date.year,date.month,date.day,time.hour,time.minute,time.second,time.millisecond);

}

#pragma mark - AJNAboutListener delegate methods
- (QStatus)getAboutDataForLanguage:(NSString *)language usingDictionary:(NSMutableDictionary **)aboutData
{
    QStatus status = ER_OK;
    *aboutData = [[NSMutableDictionary alloc] initWithCapacity:16];

	AJNMessageArgument *appID = [[AJNMessageArgument alloc] init];
    uint8_t originalAppId[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    [appID setValue:@"ay", sizeof(originalAppId) / sizeof(originalAppId[0]), originalAppId];
    [appID stabilize];
    [*aboutData setValue:appID forKey:@"AppId"];

    AJNMessageArgument *defaultLang = [[AJNMessageArgument alloc] init];
    [defaultLang setValue:@"s", "en"];
    [defaultLang stabilize];
    [*aboutData setValue:defaultLang forKey:@"DefaultLanguage"];

    AJNMessageArgument *deviceName = [[AJNMessageArgument alloc] init];
    [deviceName setValue:@"s", "TimeService"];
    [deviceName stabilize];
    [*aboutData setValue:deviceName forKey:@"DeviceName"];

    AJNMessageArgument *deviceId = [[AJNMessageArgument alloc] init];
    [deviceId setValue:@"s", DEVICEID];
    [deviceId stabilize];
    [*aboutData setValue:deviceId forKey:@"DeviceId"];

    AJNMessageArgument *appName = [[AJNMessageArgument alloc] init];
    [appName setValue:@"s", "Timer Sample App"];
    [appName stabilize];
    [*aboutData setValue:appName forKey:@"AppName"];

    AJNMessageArgument *manufacturer = [[AJNMessageArgument alloc] init];
    [manufacturer setValue:@"s", "Company"];
    [manufacturer stabilize];
    [*aboutData setValue:manufacturer forKey:@"Manufacturer"];

    AJNMessageArgument *modelNo = [[AJNMessageArgument alloc] init];
    [modelNo setValue:@"s", "Wxfy388i"];
    [modelNo stabilize];
    [*aboutData setValue:modelNo forKey:@"ModelNumber"];

    AJNMessageArgument *supportedLang = [[AJNMessageArgument alloc] init];
    const char *supportedLangs[] = {"en"};
    [supportedLang setValue:@"as", 1, supportedLangs];
    [supportedLang stabilize];
    [*aboutData setValue:supportedLang forKey:@"SupportedLanguages"];

    AJNMessageArgument *description = [[AJNMessageArgument alloc] init];
    [description setValue:@"s", "This is an Alljoyn Application"];
    [description stabilize];
    [*aboutData setValue:description forKey:@"Description"];

    AJNMessageArgument *dateOfManufacture = [[AJNMessageArgument alloc] init];
    [dateOfManufacture setValue:@"s", "1-1-2099"];
    [dateOfManufacture stabilize];
    [*aboutData setValue:dateOfManufacture forKey:@"DateOfManufacture"];

    AJNMessageArgument *softwareVersion = [[AJNMessageArgument alloc] init];
    [softwareVersion setValue:@"s", "1.0"];
    [softwareVersion stabilize];
    [*aboutData setValue:softwareVersion forKey:@"SoftwareVersion"];

    AJNMessageArgument *ajSoftwareVersion = [[AJNMessageArgument alloc] init];
    [ajSoftwareVersion setValue:@"s", [AJNVersion versionInformation]];
    [ajSoftwareVersion stabilize];
    [*aboutData setValue:ajSoftwareVersion forKey:@"AJSoftwareVersion"];

    AJNMessageArgument *hwSoftwareVersion = [[AJNMessageArgument alloc] init];
    [hwSoftwareVersion setValue:@"s", [AJNVersion versionInformation]];
    [hwSoftwareVersion stabilize];
    [*aboutData setValue:hwSoftwareVersion forKey:@"HardwareVersion"];

    AJNMessageArgument *supportURL = [[AJNMessageArgument alloc] init];
    [supportURL setValue:@"s", "http://www.alljoyn.org"];
    [supportURL stabilize];
    [*aboutData setValue:supportURL forKey:@"SupportUrl"];

    return status;
}

-(QStatus)getDefaultAnnounceData:(NSMutableDictionary **)aboutData
{
    QStatus status = ER_OK;
    *aboutData = [[NSMutableDictionary alloc] initWithCapacity:16];

    AJNMessageArgument *appID = [[AJNMessageArgument alloc] init];
    uint8_t originalAppId[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };
    [appID setValue:@"ay", sizeof(originalAppId) / sizeof(originalAppId[0]), originalAppId];
    [appID stabilize];
    [*aboutData setValue:appID forKey:@"AppId"];

    AJNMessageArgument *defaultLang = [[AJNMessageArgument alloc] init];
    [defaultLang setValue:@"s", "en"];
    [defaultLang stabilize];
    [*aboutData setValue:defaultLang forKey:@"DefaultLanguage"];

    AJNMessageArgument *deviceName = [[AJNMessageArgument alloc] init];
    [deviceName setValue:@"s", "TimeService"];
    [deviceName stabilize];
    [*aboutData setValue:deviceName forKey:@"DeviceName"];

    AJNMessageArgument *deviceId = [[AJNMessageArgument alloc] init];
    [deviceId setValue:@"s", DEVICEID];
    [deviceId stabilize];
    [*aboutData setValue:deviceId forKey:@"DeviceId"];

    AJNMessageArgument *appName = [[AJNMessageArgument alloc] init];
    [appName setValue:@"s", "Timer Sample App"];
    [appName stabilize];
    [*aboutData setValue:appName forKey:@"AppName"];

    AJNMessageArgument *manufacturer = [[AJNMessageArgument alloc] init];
    [manufacturer setValue:@"s", "Company"];
    [manufacturer stabilize];
    [*aboutData setValue:manufacturer forKey:@"Manufacturer"];

    AJNMessageArgument *modelNo = [[AJNMessageArgument alloc] init];
    [modelNo setValue:@"s", "Wxfy388i"];
    [modelNo stabilize];
    [*aboutData setValue:modelNo forKey:@"ModelNumber"];

    AJNMessageArgument *supportedLang = [[AJNMessageArgument alloc] init];
    const char *supportedLangs[] = {"en"};
    [supportedLang setValue:@"as", 1, supportedLangs];
    [supportedLang stabilize];
    [*aboutData setValue:supportedLang forKey:@"SupportedLanguages"];

    AJNMessageArgument *description = [[AJNMessageArgument alloc] init];
    [description setValue:@"s", "This is an Alljoyn Application"];
    [description stabilize];
    [*aboutData setValue:description forKey:@"Description"];

    AJNMessageArgument *dateOfManufacture = [[AJNMessageArgument alloc] init];
    [dateOfManufacture setValue:@"s", "1-1-2099"];
    [dateOfManufacture stabilize];
    [*aboutData setValue:dateOfManufacture forKey:@"DateOfManufacture"];

    AJNMessageArgument *softwareVersion = [[AJNMessageArgument alloc] init];
    [softwareVersion setValue:@"s", "1.0"];
    [softwareVersion stabilize];
    [*aboutData setValue:softwareVersion forKey:@"SoftwareVersion"];

    AJNMessageArgument *ajSoftwareVersion = [[AJNMessageArgument alloc] init];
    [ajSoftwareVersion setValue:@"s", [AJNVersion versionInformation]];
    [ajSoftwareVersion stabilize];
    [*aboutData setValue:ajSoftwareVersion forKey:@"AJSoftwareVersion"];

    AJNMessageArgument *hwSoftwareVersion = [[AJNMessageArgument alloc] init];
    [hwSoftwareVersion setValue:@"s", [AJNVersion versionInformation]];
    [hwSoftwareVersion stabilize];
    [*aboutData setValue:hwSoftwareVersion forKey:@"HardwareVersion"];

    AJNMessageArgument *supportURL = [[AJNMessageArgument alloc] init];
    [supportURL setValue:@"s", "http://www.alljoyn.org"];
    [supportURL stabilize];
    [*aboutData setValue:supportURL forKey:@"SupportUrl"];

	return status;
}


@end
