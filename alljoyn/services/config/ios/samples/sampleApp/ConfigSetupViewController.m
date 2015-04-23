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

#import "ConfigSetupViewController.h"
#import "qcc/String.h"
#import "AJNMessageArgument.h"
#import "alljoyn/about/AJNAboutIconClient.h"
#import "alljoyn/about/AJNConvertUtil.h"
#import "alljoyn/config/AJCFGConfigLogger.h"
#import "alljoyn/about/AJNAboutDataConverter.h"


@interface ConfigSetupViewController  () <UITextFieldDelegate, AJNSessionListener, UIAlertViewDelegate>

@property (nonatomic) AJNSessionId sessionId;
@property (strong, nonatomic) NSMutableDictionary *writableElements;
@property (strong, nonatomic) NSString *annBusName;
@property (nonatomic) UIAlertView *setPasswordAlert;
@property (nonatomic) UIAlertView *alertNoSession;

-(void)hasPasscodeInput:(NSNotification *)notification;
-(void)prepareAlerts;
-(void)loadSession;
-(void)updateWritableDictionary;
-(void)updateUI;
-(NSString *)getConfigurableValueForKey:(NSString *)key;
-(void)resetButtonTouchUpInside:(UIButton *)resetButton;

@end

@implementation ConfigSetupViewController

@synthesize clientBusAttachment = _clientBusAttachment;
@synthesize clientInformation = _clientInformation;
@synthesize configClient = _configClient;
@synthesize realmBusName = _realmBusName;
@synthesize peersPasscodes = _peersPasscodes;
@synthesize btnFactoryReset = _btnFactoryReset;
@synthesize btnRestart = _btnRestart;
@synthesize btnSetPassword = _btnSetPassword;
@synthesize sessionId = _sessionId;
@synthesize writableElements = _writableElements;
@synthesize annBusName = _annBusName;
@synthesize setPasswordAlert = _setPasswordAlert;
@synthesize alertNoSession = _alertNoSession;

- (void)viewDidLoad
{
    [super viewDidLoad];

    [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(hasPasscodeInput:) name:@"passcodeForBus" object:nil];

    [self prepareAlerts];
    [self loadSession];

    int ver = 0;
    [self.configClient versionWithBus:[self.clientInformation.announcement busName] version:ver];
    NSLog(@"Version: %d",ver);
}

- (void)viewWillDisappear: (BOOL)animated
{
    [super viewWillDisappear:animated];

    //Leave current AllJoyn session
    QStatus status = [self.clientBusAttachment leaveSession:self.sessionId];
    if (status == ER_OK) {
        NSLog(@"Left AJ session successfully");
    }
}

/*
 * "passcodeForBus" Notification Handler
 */
- (void)hasPasscodeInput:(NSNotification *)notification
{
	if ([notification.name isEqualToString:@"passcodeForBus"]) {
		if ([notification.object isEqualToString:self.annBusName]) {
			[[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:@"Successfully received PasscodeInput notification!"];
			[self updateWritableDictionary];
			[self updateUI];
		}
	}
}

/*
 * Private Functions
 */
- (void)prepareAlerts
{
	/* setPasswordAlert.tag = 1 */
	self.setPasswordAlert = [[UIAlertView alloc] initWithTitle:@"" message:@"Enter device password" delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK", nil];
	self.setPasswordAlert.alertViewStyle = UIAlertViewStylePlainTextInput;
	self.setPasswordAlert.tag = 1;
    
	//  alertNoSession.tag = 2
	self.alertNoSession = [[UIAlertView alloc] initWithTitle:@"Error" message:@"Session is not connected, check the connection and reconnect." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil];
	self.alertNoSession.alertViewStyle = UIAlertViewStyleDefault;
	self.alertNoSession.tag = 2;
}

- (void)loadSession
{
    if (!self.sessionId) {
        //create sessionOptions
        AJNSessionOptions *opt = [[AJNSessionOptions alloc] initWithTrafficType:(kAJNTrafficMessages) supportsMultipoint:(false) proximity:(kAJNProximityAny) transportMask:(kAJNTransportMaskAny)];

        //call joinSession
        self.sessionId = [self.clientBusAttachment joinSessionWithName:[self.clientInformation.announcement busName] onPort:[self.clientInformation.announcement port] withDelegate:self options:opt];
    }

    // Session is not connected
    if (self.sessionId == 0 || self.sessionId == -1) {
        [[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to join session. sid = %u", self.sessionId]];
        [self.alertNoSession show];
    }

    [self updateWritableDictionary];
    [self updateUI];
}

- (void)updateWritableDictionary
{
	QStatus status;
    
	self.annBusName = [self.clientInformation.announcement busName];

    NSMutableDictionary *configDict = [[NSMutableDictionary alloc] init];
	status = [self.configClient configurationsWithBus:self.annBusName languageTag:@"" configs:&configDict sessionId:self.sessionId];

    if (status != ER_OK) {
		[[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to get configuration from bus: %@", [AJNStatus descriptionForStatusCode:status]]];

        //[[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to get configuration" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];

        // disable buttons
		[self.btnFactoryReset setEnabled:NO];
		[self.btnRestart setEnabled:NO];
		[self.btnSetPassword setEnabled:NO];
        
        self.writableElements = nil;
	}
	else {
        // enable buttons
		[self.btnFactoryReset setEnabled:YES];
		[self.btnRestart setEnabled:YES];
		[self.btnSetPassword setEnabled:YES];
        
        self.writableElements = configDict;
        [self.clientInformation setCurrLang:[self getConfigurableValueForKey:DEFAULT_LANGUAGE_STR]];
        NSLog(@"updateWritableDictionary count %d",[self.writableElements count]);
	}
}

- (void)updateUI
{
    [self.tableView reloadData];
}

- (NSString *)getConfigurableValueForKey:(NSString *)key
{
    AJNMessageArgument *msgArg = [self.writableElements valueForKey:key];
    return [AJNAboutDataConverter messageArgumentToString:msgArg];
}

- (void)resetButtonTouchUpInside:(UIButton *)resetButton
{
    QStatus status;
    
	// Get the property name
	NSString *key = [self.writableElements allKeys][resetButton.tag];
    NSLog(@"Resetting Key = %@", key);

    NSMutableArray *configNames = [[NSMutableArray alloc]init];
    [configNames addObject:key];
    
    status = [self.configClient resetConfigurationsWithBus:self.annBusName languageTag:[self.clientInformation currLang] configNames:configNames sessionId:self.sessionId];

	if (status != ER_OK) {
		[[[UIAlertView alloc] initWithTitle:@"Reset Property Store Failed" message:[NSString stringWithFormat:@"Error occured:%@", [AJNStatus descriptionForStatusCode:status]] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
        
		[[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to reset Property Store for key '%@': %@", key, [AJNStatus descriptionForStatusCode:status]]];
	}
	else {
		[[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:[NSString stringWithFormat:@"Successfully reset Property Store for key '%@'", key]];
        
        [self updateWritableDictionary];
        [self updateUI];
	}
}

/*
 * UITableViewDataSource Protocol Implementation
 */
-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return [self.writableElements count];
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSString *fieldKey = [[self.writableElements allKeys] objectAtIndex: [indexPath row]];

    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier: @"WritableElementCell" forIndexPath:indexPath];

    for (UIView *view in cell.contentView.subviews) {
        if ([view isKindOfClass: [UILabel class]]) {
            UILabel *fieldNameLabel = (UILabel *)view;
            fieldNameLabel.text = fieldKey;
            fieldNameLabel.tag = [indexPath row];
        }
        else if ([view isKindOfClass: [UITextField class]]) {
            UITextField *fieldTextField = (UITextField *)view;
            fieldTextField.text = [self getConfigurableValueForKey: fieldKey];
            fieldTextField.delegate = self;
            fieldTextField.tag = [indexPath row];
        }
        else if ([view isKindOfClass: [UIButton class]]) {
            UIButton *resetFieldButton = (UIButton *)view;
            [resetFieldButton setTitle: [NSString stringWithFormat:@"Reset %@", fieldKey] forState: UIControlStateNormal];
            [resetFieldButton setContentHorizontalAlignment: UIControlContentHorizontalAlignmentCenter];
            [resetFieldButton addTarget: self action: @selector(resetButtonTouchUpInside:) forControlEvents: UIControlEventTouchUpInside];
            resetFieldButton.tag = [indexPath row];
        }
        else {
            NSLog(@"Got view of unknown type from the content view subviews array");
        }
    }

    return cell;
}

/*
 * UIAlertViewDelegate implementation
 */
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    QStatus status;
    switch (alertView.tag) {
        case 1: // setPasswordAlert
        {
            if (buttonIndex == 1) { //user pressed OK
                // Get the password input
                NSString *pass = [self.setPasswordAlert textFieldAtIndex:0].text;
                [[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:[NSString stringWithFormat:@"Trying to set Passcode to %@", pass]];
                // Validate

                // Prepare password for sending

                NSString *guid = [self.clientBusAttachment guidForPeerNamed:self.annBusName];
                status = [self.clientBusAttachment clearKeysForRemotePeerWithId:guid];

                if (ER_OK == status) {
                    [[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:@"Successfully clearKeysForRemotePeer"];
                }
                else {
                    [[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to clearKeysForRemotePeer: %@", [AJNStatus descriptionForStatusCode:status]]];
                }

                NSData *passcodeData = [pass dataUsingEncoding:NSUTF8StringEncoding];
                const void *bytes = [passcodeData bytes];
                int length = [passcodeData length];

                // Set new password
                status = [self.configClient setPasscodeWithBus:self.annBusName daemonRealm:self.realmBusName newPasscodeSize:length newPasscode:(const uint8_t *)bytes sessionId:self.sessionId];

                if (ER_OK == status) {
                    [[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:@"Successfully setPasscodeWithBus"];
                    // add passcode to peersPass
                    (self.peersPasscodes)[self.annBusName] = pass;
                    [[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:[NSString stringWithFormat:@"update peer %@ with passcode %@", self.annBusName, pass]];
                }
                else {
                    [[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:[NSString stringWithFormat:@"Failed to setPasscodeWithBus: %@", [AJNStatus descriptionForStatusCode:status]]];
                }
            }
        }
            break;
        case 2: //NoSessionAlert
            break;
        default:
            [[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:@"alertView.tag is wrong"];
            break;
    }
}

/*
 * UITextFieldDelegate implementation
 */
- (void)textFieldDidEndEditing:(UITextField *)textField
{
    QStatus status;

    NSMutableDictionary *configElements = [[NSMutableDictionary alloc] init];

    // Get the property name
    NSString *key = [self.writableElements allKeys][textField.tag];
    // Get the property value
    AJNMessageArgument *msgArgValue = [[AJNMessageArgument alloc] init];
    const char *char_str_value = [AJNConvertUtil convertNSStringToConstChar:textField.text];
    [msgArgValue setValue:@"s", char_str_value];

    // Add the property name/value
    configElements[key] = msgArgValue;


    NSString *useLang;
    if ([key isEqualToString:DEFAULT_LANGUAGE_STR]) {
        useLang = @"";
    } else {
        useLang = [self.clientInformation currLang];
    }

    status = [self.configClient updateConfigurationsWithBus:self.annBusName languageTag:useLang configs:&configElements sessionId:self.sessionId];

    if (status != ER_OK) {
        NSString *str= [NSString stringWithFormat:@"Failed to update property '%@' to '%s'", key, char_str_value ];

        [[[UIAlertView alloc] initWithTitle:@"Update Property Store Failed" message:str delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];

        [[[AJCFGConfigLogger sharedInstance] logger] errorTag:[[self class] description] text:str];
    }
    else {
        [[[AJCFGConfigLogger sharedInstance] logger] debugTag:[[self class] description] text:[NSString stringWithFormat:@"Successfully update Property Store with %@ = %s for tag[%d]", key, char_str_value, textField.tag]];

        [self updateWritableDictionary];
        [self updateUI];
        
    }
    
}

- (BOOL)textFieldShouldReturn:(UITextField *)textField
{
    [textField resignFirstResponder];
    return YES;
}

/*
 * IBAction Button Handler Methods
 */
- (IBAction)factoryResetPressed:(id)sender
{
	QStatus status = [self.configClient factoryResetWithBus:self.annBusName sessionId:self.sessionId];
    
    if (status!=ER_OK) {
        [[[UIAlertView alloc]initWithTitle:@"Factory reset failed" message:[NSString stringWithFormat:@"Factory reset failed with error:%@",[AJNStatus descriptionForStatusCode:status]] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    } else {
        [[[UIAlertView alloc]initWithTitle:@"Factory reset success" message:@"Factory reset done. Your wifi connection may have changed." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
    }
    
    [self.navigationController popViewControllerAnimated:YES];
}

- (IBAction)restartPressed:(id)sender
{
	[self.configClient restartWithBus:self.annBusName sessionId:self.sessionId];
}

- (IBAction)setPasswordPressed:(id)sender
{
    [self.setPasswordAlert show];
}

/*
 * AJNSessionListener Delegate implementation
 */
- (void)sessionWasLost:(AJNSessionId)sessionId forReason:(AJNSessionLostReason)reason
{
    NSLog(@"session on bus %@ lost. reason:%d",self.annBusName,reason);
    [self.navigationController popViewControllerAnimated:YES];
}

@end
