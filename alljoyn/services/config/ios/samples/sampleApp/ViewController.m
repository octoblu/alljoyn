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

#import "ViewController.h"
#import "AJNStatus.h"
#import "alljoyn/about/AJNAboutDataConverter.h"
#import "alljoyn/about/AJNAnnouncement.h"
#import "alljoyn/about/AJNAnnouncementReceiver.h"
#import "AJNPasswordManager.h"
#import "alljoyn/services_common/AJSVCGenericLoggerDefaultImpl.h"
#import "alljoyn/config/AJCFGConfigClient.h"
#import "alljoyn/config/AJCFGConfigLogger.h"
#import "ConfigSetupViewController.h"
#import "AnnounceTextViewController.h"
#import "GetAboutCallViewController.h"
#import "ClientInformation.h"
#import "AppDelegate.h"

static bool ALLOWREMOTEMESSAGES = true; // About Client -  allow Remote Messages flag
static NSString *const APPNAME = @"AboutClientMain";  //About Client - default application name
static NSString *const DAEMON_QUIET_PREFIX = @"quiet@";    //About Client - quiet advertising
static NSString *const ABOUT_CONFIG_OBJECT_PATH = @"/Config";  //About Service - Config
static NSString *const ABOUT_CONFIG_INTERFACE_NAME = @"org.alljoyn.Config";   //About Service - Config
static NSString *const DEFAULT_REALM_BUS_NAME = @"org.alljoyn.BusNode.configClient";

static NSString *const DEFAULT_PASSCODE = @"000000";
static NSString *const KEYSTORE_FILE_PATH = @"Documents/alljoyn_keystore/s_central.ks";
static NSString *const  AUTH_MECHANISM = @"ALLJOYN_SRP_KEYX ALLJOYN_PIN_KEYX ALLJOYN_ECDHE_PSK";

@interface ViewController ()

// About Client properties
@property (strong, nonatomic) AJNBusAttachment *clientBusAttachment;
@property (strong, nonatomic) AJNAnnouncementReceiver *announcementReceiver;
@property (strong, nonatomic) NSString *realmBusName;
@property (nonatomic) bool isAboutClientConnected;
@property (strong, nonatomic) NSMutableDictionary *clientInformationDict; // Store the client related information
@property (strong, nonatomic) AJSVCGenericLoggerDefaultImpl *logger;

@property (strong, nonatomic) NSMutableDictionary *peersPasscodes; // Store the peers passcodes

// Announcement
@property (strong, nonatomic) NSString *announcementButtonCurrentTitle; // The pressed button's announcementUniqueName
@property (strong, nonatomic) dispatch_queue_t annBtnCreationQueue;

// About Client strings
@property (strong, nonatomic) NSString *ajconnect;
@property (strong, nonatomic) NSString *ajdisconnect;
@property (strong, nonatomic) NSString *annSubvTitleLabelDefaultTxt;

// About Client alerts
@property (strong, nonatomic) UIAlertView *announcementOptionsAlert;
@property (strong, nonatomic) UIAlertView *announcementOptionsAlertNoConfig;

/* ConfigClient */
@property (strong, nonatomic) AJCFGConfigClient *configClient;

/* Security */
@property (strong, nonatomic) UIAlertView *setPassCodeAlert;
@property (strong, nonatomic) NSString *passCodeText;
@property (strong, nonatomic) NSString *peerName;
@property (nonatomic, strong) NSString *password;

@end

@implementation ViewController

#pragma mark - Built In methods
- (void)viewDidLoad
{
	[super viewDidLoad];
	[self loadNewSession];
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

    [self.navigationController setToolbarHidden: YES];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];

    [self.navigationController setToolbarHidden: NO];
}

// Get the user's input from the alert dialog
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
	switch (alertView.tag) {
            
		case 3: // Announcement options alert
		{
			[self performAnnouncemnetAction:buttonIndex];
		}
            break;
            
		case 4: //announcementOptionsAlertNoConfig
		{
			[self performAnnouncemnetAction:buttonIndex];
		}
            break;
            
		case 5: // passcode alert
		{
			[self.setPassCodeAlert dismissWithClickedButtonIndex:buttonIndex animated:NO];
            
			if (buttonIndex == 1) { // User pressed OK
				// get the input pass
				self.passCodeText = [self.setPassCodeAlert textFieldAtIndex:0].text;
				[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Passcode is: %@",  self.passCodeText]];
				bool foundPeer = false;
                
				// check that peername is not empty
				if ([self.peerName length]) {
					if (![self.passCodeText length]) {
						// set the pass to default if input is empty
						self.passCodeText = DEFAULT_PASSCODE;
					}
					// Iterate over the dictionary and add/update
					for (NSString *key in self.peersPasscodes.allKeys) {
						if ([key isEqualToString:self.peerName]) {
							// Update passcode for key
							(self.peersPasscodes)[self.peerName] = self.passCodeText;
							[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Update peer %@ with passcode %@",  self.peerName, self.passCodeText]];
							// Set flag
							foundPeer = true;
							break;
						}
					}
					if (!foundPeer) {
						// Add new set of key/value
						[self.peersPasscodes setValue:self.passCodeText forKey:self.peerName];
						[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"add new peers %@ %@", self.peerName, self.passCodeText]];
					}
                    
					[[NSNotificationCenter defaultCenter] postNotificationName:@"passcodeForBus" object:self.peerName];
				}
			}
			else {     // User pressed Cancel
			}
		}
            break;
            
		default:
			[self.logger errorTag:[[self class] description] text:@"alertView.tag is wrong"];
			break;
	}
}

- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender
{
	if ([segue.destinationViewController isKindOfClass:[GetAboutCallViewController class]]) {
		GetAboutCallViewController *getAboutCallView = segue.destinationViewController;
		getAboutCallView.clientInformation = self.clientInformationDict[self.announcementButtonCurrentTitle];
		getAboutCallView.clientBusAttachment = self.clientBusAttachment;
	}
	else if ([segue.destinationViewController isKindOfClass:[ConfigSetupViewController class]]) {
		ConfigSetupViewController *getConfigCallView = segue.destinationViewController;
		getConfigCallView.clientInformation = self.clientInformationDict[self.announcementButtonCurrentTitle];
		getConfigCallView.clientBusAttachment = self.clientBusAttachment;
		getConfigCallView.configClient = self.configClient;
		getConfigCallView.realmBusName = self.realmBusName;
		getConfigCallView.peersPasscodes = self.peersPasscodes;
	}
	else if ([segue.destinationViewController isKindOfClass:[AnnounceTextViewController class]]) {
		AnnounceTextViewController *announceTextViewController = segue.destinationViewController;
		announceTextViewController.ajnAnnouncement = [(ClientInformation *)(self.clientInformationDict)[self.announcementButtonCurrentTitle] announcement];
	}
} /* prepareForSegue: sender: */

#pragma mark - IBAction Methods
- (IBAction)connectButtonDidTouchUpInside:(id)sender
{
	// Connect to the bus with the default realm bus name
	if (!self.isAboutClientConnected) {
		[self startAboutClient];
	} else {
        [self stopAboutClient];
	}
}

// IBAction triggered by a dynamic announcement button
- (void)didTouchAnnSubvOkButton:(id)subvOkButton
{
	[[subvOkButton superview] removeFromSuperview];
}

#pragma mark - AJNAnnouncementListener protocol method
// Here we receive an announcement from AJN and add it to the client's list of services avaialble
- (void)announceWithVersion:(uint16_t)version port:(uint16_t)port busName:(NSString *)busName objectDescriptions:(NSMutableDictionary *)objectDescs aboutData:(NSMutableDictionary **)aboutData
{
	NSString *announcementUniqueName; // Announcement unique name in a format of <busName DeviceName>
	ClientInformation *clientInformation = [[ClientInformation alloc] init];
    
	// Save the announcement in a AJNAnnouncement
	clientInformation.announcement = [[AJNAnnouncement alloc] initWithVersion:version port:port busName:busName objectDescriptions:objectDescs aboutData:aboutData];
    
	// Generate an announcement unique name in a format of <busName DeviceName>
	announcementUniqueName = [NSString stringWithFormat:@"%@ %@", [clientInformation.announcement busName], [AJNAboutDataConverter messageArgumentToString:[clientInformation.announcement aboutData][@"DeviceName"]]];
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Announcement unique name [%@]", announcementUniqueName]];
    
	AJNMessageArgument *annObjMsgArg = [clientInformation.announcement aboutData][@"AppId"];
	uint8_t *appIdBuffer;
	size_t appIdNumElements;
	QStatus status;
	status = [annObjMsgArg value:@"ay", &appIdNumElements, &appIdBuffer];
    
	// Add the received announcement
	if (status != ER_OK) {
		[self.logger fatalTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to read appId for key [%@] :%@", announcementUniqueName, [AJNStatus descriptionForStatusCode:status]]];
		return;
	}
    
	// Dealing with announcement entries should be syncronized, so we add it to a queue
	dispatch_sync(self.annBtnCreationQueue, ^{
	    bool isAppIdExists = false;
	    uint8_t *tmpAppIdBuffer;
	    size_t tmpAppIdNumElements;
	    QStatus tStatus;
	    int res;
        
	    // Iterate over the announcements dictionary
	    for (NSString *key in self.clientInformationDict.allKeys) {
	        ClientInformation *clientInfo = [self.clientInformationDict valueForKey:key];
	        AJNAnnouncement *announcement = [clientInfo announcement];
	        AJNMessageArgument *tmpMsgrg = [announcement aboutData][@"AppId"];
            
	        tStatus = [tmpMsgrg value:@"ay", &tmpAppIdNumElements, &tmpAppIdBuffer];
	        if (tStatus != ER_OK) {
	            [self.logger fatalTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to read appId for key [%@] :%@", key, [AJNStatus descriptionForStatusCode:tStatus]]];
	            return;
			}
            
	        res = 1;
	        if (appIdNumElements == tmpAppIdNumElements) {
	            res = memcmp(appIdBuffer, tmpAppIdBuffer, appIdNumElements);
			}
            
	        // Found a matched appId - res=0
	        if (!res) {
	            [self.logger errorTag:[[self class] description] text:@"Got an announcement from a known device[appID"];
	            isAppIdExists = true;
	            // Same AppId and the same announcementUniqueName
	            if ([key isEqualToString:announcementUniqueName]) {
	                // Update only announcements dictionary
	                (self.clientInformationDict)[announcementUniqueName] = clientInformation;
				}
	            else {
	                // Same AppId but *different* Bus name/Device name
	                [self.logger debugTag:[[self class] description] text:@"Same AppId but *different* Bus name/Device name - updating the announcement object and UI"];
	                NSString *prevBusName = [announcement busName];
	                // Check if bus name has changed
	                if (!([busName isEqualToString:prevBusName])) {
	                    [self.logger debugTag:[[self class] description] text:@"Bus name has changed - calling cancelFindAdvertisedName"];
	                    // Cancel advertise name
	                    tStatus = [self.clientBusAttachment cancelFindAdvertisedName:prevBusName];
	                    if (status != ER_OK) {
	                        [self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to cancelAdvertisedName for %@ :%@", prevBusName, [AJNStatus descriptionForStatusCode:tStatus]]];
						}
					}
	                // Remove existed record from the announcements dictionary
	                [self.clientInformationDict removeObjectForKey:key];
	                // Add new record to the announcements dictionary
	                [self.clientInformationDict setValue:clientInformation forKey:announcementUniqueName];
	                // Update UI
	                [self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Update UI with the announcementUniqueName: %@", announcementUniqueName]];
                    
                    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
				}
	            break;
			} //if
		} //for
        
	    //appId doesn't exist and  there is no match announcementUniqueName
	    if (!(self.clientInformationDict)[announcementUniqueName] && !isAppIdExists) {
	        // Add new pair with this AboutService information (version,port,bus name, object description and about data)
	        [self.clientInformationDict setValue:clientInformation forKey:announcementUniqueName];
	        [self addNewAnnouncemetEntry];
		}
	});
    
	// Register interest in a well-known name prefix for the purpose of discovery (didLoseAdertise)
	[self.clientBusAttachment enableConcurrentCallbacks];
	status = [self.clientBusAttachment findAdvertisedName:busName];
	if (status != ER_OK) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to findAdvertisedName for %@ :%@", busName, [AJNStatus descriptionForStatusCode:status]]];
	}
}

#pragma mark AJNBusListener protocol methods
- (void)didFindAdvertisedName:(NSString *)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString *)namePrefix
{
	[self.logger debugTag:[[self class] description] text:@"didFindAdvertisedName has been called"];
}

- (void)didLoseAdvertisedName:(NSString *)name withTransportMask:(AJNTransportMask)transport namePrefix:(NSString *)namePrefix
{
	[self.logger debugTag:[[self class] description] text:@"didLoseAdvertisedName has been called"];
	QStatus status;
	// Find the button title that should be removed
	for (NSString *key in[self.clientInformationDict allKeys]) {
		if ([[[[self.clientInformationDict valueForKey:key] announcement] busName] isEqualToString:name]) {
			// Cancel advertise name for that bus
			status = [self.clientBusAttachment cancelFindAdvertisedName:name];
			if (status != ER_OK) {
				[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to cancelFindAdvertisedName for %@. status:%@", name, [AJNStatus descriptionForStatusCode:status]]];
			}
			// Remove the anouncement from the dictionary
			[self.clientInformationDict removeObjectForKey:key];
		}
	}
    
    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
}

#pragma mark - util methods
- (void)loadNewSession
{
	// About Client flags
	self.isAboutClientConnected  = false;
    
	self.annBtnCreationQueue = dispatch_queue_create("org.alljoyn.announcementbuttoncreationQueue", NULL);
    
	// Set About Client strings
	self.ajconnect = @"Connect to AllJoyn";
	self.ajdisconnect = @"Disconnect from AllJoyn";
	self.realmBusName = DEFAULT_REALM_BUS_NAME;
	self.annSubvTitleLabelDefaultTxt = @"Announcement of ";
	// Set About Client connect button
	self.connectButton.backgroundColor = [UIColor darkGrayColor]; //button bg color
	[self.connectButton setTitleColor:[UIColor whiteColor] forState:UIControlStateNormal]; //button font color
	[self.connectButton setTitle:self.ajconnect forState:UIControlStateNormal]; //default text
    
	[self prepareAlerts];
    
	[AJCFGConfigLogger sharedInstance];
	[[AJCFGConfigLogger sharedInstance]setLogger:nil];
	self.logger = [[AJCFGConfigLogger sharedInstance] logger];
}

// Initialize alerts
- (void)prepareAlerts
{
	// announcementOptionsAlert.tag = 3
	self.announcementOptionsAlert = [[UIAlertView alloc] initWithTitle:@"Choose option:" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Show Announce", @"About", @"Config", nil];
	self.announcementOptionsAlert.alertViewStyle = UIAlertViewStyleDefault;
	self.announcementOptionsAlert.tag = 3;
    
	// announcementOptionsAlert.tag = 4
	self.announcementOptionsAlertNoConfig = [[UIAlertView alloc] initWithTitle:@"Choose option:" message:@"" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"Show Announce", @"About", nil];
	self.announcementOptionsAlertNoConfig.alertViewStyle = UIAlertViewStyleDefault;
	self.announcementOptionsAlertNoConfig.tag = 4;
    
	// setPassCodeAlert.tag = 5
	self.setPassCodeAlert = [[UIAlertView alloc] initWithTitle:@"" message:@"Enter device password" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
	self.setPassCodeAlert.alertViewStyle = UIAlertViewStylePlainTextInput;
	self.setPassCodeAlert.tag = 5;
}

- (void)performAnnouncemnetAction:(NSInteger)opt
{
	switch (opt) {
		case 0: // "Cancel"
			break;
            
		case 1: // "Show Announce"
		{
			[self performSegueWithIdentifier:@"AboutShowAnnounceSegue" sender:self];
		}
            break;
            
		case 2: // "About"
		{
			[self performSegueWithIdentifier:@"AboutClientSegue" sender:self]; // get the announcment object
		}
            break;
            
		case 3: // "Config"
		{
			[self performSegueWithIdentifier:@"ConfigSetupSegue" sender:self];
		}
            break;
            
		default:
			break;
	}
}

#pragma mark - AboutClient
#pragma mark start AboutClient

- (void)startAboutClient
{
	QStatus status;
    
    // Create a dictionary to contain announcements using a key in the format of: "announcementUniqueName + announcementObj"
	self.clientInformationDict = [[NSMutableDictionary alloc] init];
    
	[self.logger debugTag:[[self class] description] text:@"Start About Client"];
    //    [[AJCFGConfigLogger sharedInstance] debugTag:[[self class] description] text:@"Start About Client"];
    
	// Init AJNBusAttachment
	self.clientBusAttachment = [[AJNBusAttachment alloc] initWithApplicationName:APPNAME allowRemoteMessages:(ALLOWREMOTEMESSAGES)];
    
	// Start AJNBusAttachment
	status = [self.clientBusAttachment start];
	if (status != ER_OK) {
        [AppDelegate alertAndLog:@"Failed AJNBusAttachment start" status:status];
        [self stopAboutClient];
        return;
	}
    
	// Connect AJNBusAttachment
	status = [self.clientBusAttachment connectWithArguments:@""];
	if (status != ER_OK) {
        [AppDelegate alertAndLog:@"Failed AJNBusAttachment connectWithArguments" status:status];
        [self stopAboutClient];
        return;
	}
    
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"aboutClientListener"]];
    
	[self.clientBusAttachment registerBusListener:self];
    
	self.announcementReceiver = [[AJNAnnouncementReceiver alloc] initWithAnnouncementListener:self andBus:self.clientBusAttachment];
    const char* interfaces[] = { [ABOUT_CONFIG_INTERFACE_NAME UTF8String] };
	status = [self.announcementReceiver registerAnnouncementReceiverForInterfaces:interfaces withNumberOfInterfaces:1];
	if (status != ER_OK) {
		[AppDelegate alertAndLog:@"Failed to registerAnnouncementReceiver" status:status];
        [self stopAboutClient];
        return;
	}
    
    NSUUID *UUID = [NSUUID UUID];
    NSString *stringUUID = [UUID UUIDString];
    self.realmBusName = [NSString stringWithFormat:@"%@-%@", DEFAULT_REALM_BUS_NAME, stringUUID];
    
	// Advertise Daemon for tcl
	status = [self.clientBusAttachment requestWellKnownName:self.realmBusName withFlags:kAJNBusNameFlagDoNotQueue];
	if (status == ER_OK) {
		// Advertise the name with a quite prefix for TC to find it
		status = [self.clientBusAttachment advertiseName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, self.realmBusName] withTransportMask:kAJNTransportMaskAny];
		if (status != ER_OK) {
            [AppDelegate alertAndLog:@"Failed to advertise name" status:status];
            [self stopAboutClient];
            return;
		}
		else {
			[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Successfully advertised: %@%@", DAEMON_QUIET_PREFIX, self.realmBusName]];
		}
	}
	else {
        [AppDelegate alertAndLog:@"Failed to requestWellKnownName" status:status];
        [self stopAboutClient];
        return;
	}
    
	[self.connectButton setTitle:self.ajdisconnect forState:UIControlStateNormal]; //change title to "Disconnect from AllJoyn"
    
	// Start Config Client
	self.configClient = [[AJCFGConfigClient alloc] initWithBus:self.clientBusAttachment];
	if (!self.configClient) {
        [AppDelegate alertAndLog:@"Failed to start Config Client" status:status];
        [self stopAboutClient];
        return;
	}
    
	// Set logger
	self.logger = [[AJSVCGenericLoggerDefaultImpl alloc] init];
	[self.configClient setLogger:self.logger];
    
	// Create NSMutableDictionary dictionary of peers passcodes
	self.peersPasscodes = [[NSMutableDictionary alloc] init];
    
	// Enable Client Security
	status = [self enableClientSecurity];
	if (ER_OK != status) {
		[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to enable security on the bus. %@", [AJNStatus descriptionForStatusCode:status]]];
	} else {
		[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Successfully enabled security for the bus"]];
	}
    
	self.isAboutClientConnected = true;
}

- (QStatus)enableClientSecurity
{
	QStatus status;
	status = [self.clientBusAttachment enablePeerSecurity:AUTH_MECHANISM authenticationListener:self keystoreFileName:KEYSTORE_FILE_PATH sharing:YES];
    
    if (status != ER_OK) { //try to delete the keystore and recreate it, if that fails return failure
        NSError *error;
        NSString *keystoreFilePath = [NSString stringWithFormat:@"%@/alljoyn_keystore/s_central.ks", [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0]];
        [[NSFileManager defaultManager] removeItemAtPath:keystoreFilePath error:&error];
        if (error) {
            NSLog(@"ERROR: Unable to delete keystore. %@", error);
            return ER_AUTH_FAIL;
        }
        
        status = [self.clientBusAttachment enablePeerSecurity:AUTH_MECHANISM authenticationListener:self keystoreFileName:KEYSTORE_FILE_PATH sharing:YES];
    }
    
	return status;
}

- (void)addNewAnnouncemetEntry
{
    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
}

// announcementGetMoreInfo is an IBAction triggered by pressing a dynamic announcement button
- (void)announcementGetMoreInfo:(NSInteger)requestedRow
{
    // set the announcementButtonCurrentTitle
    self.announcementButtonCurrentTitle = [self.clientInformationDict allKeys][requestedRow];   // set the announcementButtonCurrentTitle
    
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Getting data for [%@]", self.announcementButtonCurrentTitle]];
    
	if ([self announcementHasConfig:self.announcementButtonCurrentTitle]) {
		[self.announcementOptionsAlert show]; // Event is forward to alertView: clickedButtonAtIndex:
	}
	else {
		[self.announcementOptionsAlertNoConfig show]; // Event is forward to alertView: clickedButtonAtIndex:
	}
}

// Return true if an announcement supports icon interface
- (bool)announcementHasConfig:(NSString *)announcementKey
{
	bool hasIconInterface = false;
	AJNAnnouncement *announcement = [(ClientInformation *)[self.clientInformationDict valueForKey:announcementKey] announcement];
	NSMutableDictionary *announcementObjDecs = [announcement objectDescriptions]; //Dictionary of ObjectDescriptions NSStrings
    
	// iterate over the object descriptions dictionary
	for (NSString *key in announcementObjDecs.allKeys) {
		if ([key isEqualToString:ABOUT_CONFIG_OBJECT_PATH]) {
			// iterate over the NSMutableArray
			for (NSString *intfc in[announcementObjDecs valueForKey:key]) {
				if ([intfc isEqualToString:ABOUT_CONFIG_INTERFACE_NAME]) {
					hasIconInterface = true;
				}
			}
		}
	}
	return hasIconInterface;
}

#pragma mark stop AboutClient
- (void)stopAboutClient
{
	QStatus status;
	[self.logger debugTag:[[self class] description] text:@"Stop About Client"];
    
	// Bus attachment cleanup
	status = [self.clientBusAttachment cancelAdvertisedName:[NSString stringWithFormat:@"%@%@", DAEMON_QUIET_PREFIX, self.realmBusName] withTransportMask:kAJNTransportMaskAny];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully cancel advertised name"];
	}
	status = [self.clientBusAttachment releaseWellKnownName:self.realmBusName];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully release WellKnownName"];
	}
	status = [self.clientBusAttachment removeMatchRule:@"sessionless='t',type='error'"];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully remove MatchRule"];
	}
    
	// Cancel advertise name for each announcement bus
	for (NSString *key in[self.clientInformationDict allKeys]) {
		ClientInformation *clientInfo = (self.clientInformationDict)[key];
		status = [self.clientBusAttachment cancelFindAdvertisedName:[[clientInfo announcement] busName]];
		if (status != ER_OK) {
			[self.logger errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to cancelAdvertisedName for %@: %@", key, [AJNStatus descriptionForStatusCode:status]]];
		}
	}
	self.clientInformationDict = nil;
    
    const char* interfaces[] = { [ABOUT_CONFIG_INTERFACE_NAME UTF8String] };
	status = [self.announcementReceiver unRegisterAnnouncementReceiverForInterfaces:interfaces withNumberOfInterfaces:1];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully unregistered AnnouncementReceiver"];
	}
    
	self.announcementReceiver = nil;
    
	// Stop bus attachment
	status = [self.clientBusAttachment stop];
	if (status == ER_OK) {
		[self.logger debugTag:[[self class] description] text:@"Successfully stopped bus"];
	}
	self.clientBusAttachment = nil;
    
	self.peersPasscodes = nil;
    
	// Set flag
	self.isAboutClientConnected  = false;
    
	// UI cleanup
	[self.connectButton setTitle:self.ajconnect forState:UIControlStateNormal];
    
    [self.servicesTable performSelectorOnMainThread:@selector(reloadData) withObject:nil waitUntilDone:NO];
    
	[self.logger debugTag:[[self class] description] text:@"About Client is stopped"];
    
	self.logger = nil;
}

#pragma mark UITableView delegates
- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
	return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
	return [self.clientInformationDict count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView
         cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
	static NSString *MyIdentifier = @"AnnouncementCell";
    
	UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:MyIdentifier];
    
	cell.selectionStyle = UITableViewCellSelectionStyleNone;
    
	if (cell == nil) {
		cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
		                              reuseIdentifier:MyIdentifier];
	}
    
	// Here we use the provided setImageWithURL: method to load the web image
	// Ensure you use a placeholder image otherwise cells will be initialized with no image
	cell.textLabel.text = [self.clientInformationDict allKeys][indexPath.row];
	return cell;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
	[self announcementGetMoreInfo:indexPath.row];
}

- (void)setPassCode:(NSString *)passCode
{
	self.password = passCode;
}

#pragma mark - AJNAuthenticationListener protocol methods
- (AJNSecurityCredentials *)requestSecurityCredentialsWithAuthenticationMechanism:(NSString *)authenticationMechanism peerName:(NSString *)peerName authenticationCount:(uint16_t)authenticationCount userName:(NSString *)userName credentialTypeMask:(AJNSecurityCredentialType)mask
{
	AJNSecurityCredentials *creds = nil;
	bool credFound = false;
    
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"requestSecurityCredentialsWithAuthenticationMechanism:%@ forRemotePeer%@ userName:%@", authenticationMechanism, peerName, userName]];
    
	if ([authenticationMechanism isEqualToString:@"ALLJOYN_SRP_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_PIN_KEYX"] || [authenticationMechanism isEqualToString:@"ALLJOYN_ECDHE_PSK"]) {
		if (mask & kAJNSecurityCredentialTypePassword) {
			if (authenticationCount <= 3) {
				creds = [[AJNSecurityCredentials alloc] init];
                
				// Check if the password stored in peersPasscodes
				for (NSString *key in self.peersPasscodes.allKeys) {
					if ([key isEqualToString:peerName]) {
						creds.password = (self.peersPasscodes)[key];
						[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Found password %@ for peer %@", creds.password, key]];
						credFound = true;
						break;
					}
				}
				// Use the default password
				if (!credFound) {
					creds.password = DEFAULT_PASSCODE;
					[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"Using default password %@ for peer %@", DEFAULT_PASSCODE, peerName]];
				}
			}
		}
	}
	return creds;
}

- (void)authenticationUsing:(NSString *)authenticationMechanism forRemotePeer:(NSString *)peerName didCompleteWithStatus:(BOOL)success
{
	NSString *status;
	status = (success == YES ? @"was successful" : @"failed");
    
	[self.logger debugTag:[[self class] description] text:[NSString stringWithFormat:@"authenticationUsing:%@ forRemotePeer%@ %@", authenticationMechanism, peerName, status]];
    
	//get the passcpde for this bus
	if (!success) {
		self.peerName = peerName;
		self.passCodeText = nil;
		dispatch_async(dispatch_get_main_queue(), ^{
		    [self.setPassCodeAlert show];
		});
	}
}

//- (BOOL)alertViewShouldEnableFirstOtherButton:(UIAlertView *)alertView
//{
//    return YES;
//}


@end
