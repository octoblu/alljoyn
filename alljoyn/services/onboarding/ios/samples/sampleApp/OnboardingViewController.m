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

#import "OnboardingViewController.h"
#import "alljoyn/onboarding/AJOBSOnboardingClient.h"
#import "SystemConfiguration/CaptiveNetwork.h"



@interface OnboardingViewController () <AJOBOnboardingClientListener>
@property (strong, nonatomic) AJOBSOnboardingClient *onboardingClient;
@property (nonatomic) AJNSessionId sessionId;
@property (strong, nonatomic) NSString *onboardeeBus;
@end

@implementation OnboardingViewController


- (void)viewDidLoad
{
    [super viewDidLoad];
    // UI
    [self.statusLbl setText:@"  "];
    [self.instructLbl setText:@"  "];
    [self displayPreOnbordingElements:0];
    self.offBoardingBtn.alpha = 0;
    
    [self updateStatusLabel:@"Loading Onboarding client"];
    QStatus status = [self startOnboardingClient];
    if (ER_OK != status) {
        [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to start onboarding client" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil] show];
    }
}

-(void)displayPreOnbordingElements:(CGFloat) alpha
{
    //text
    self.ssidTextField.alpha = alpha;
    self.ssidPassTextField.alpha = alpha;
    
    //buttons
    self.connectBtn.alpha = alpha;
    self.configureBtn.alpha = alpha;
    
    //labels
    self.onboardTitleLbl.alpha = alpha;
    self.ssidLbl.alpha = alpha;
    self.ssidPassLbl.alpha = alpha;
}

-(QStatus)startOnboardingClient
{
    self.onboardeeBus = [self.clientInformation.announcement busName];
    QStatus status = ER_FAIL;
    if (!self.onboardeeBus) {
        [self updateStatusLabel:@"Bus attachment hasn't been initialized"];
        return status;
    }
    
    self.onboardingClient =  [[AJOBSOnboardingClient alloc] initWithBus:self.clientBusName listener:self];
    
    if (!self.sessionId)
    {
        status = [self createNewSession];
        if (ER_OK != status) {
            return ER_FAIL;
        }
    }
    
    NSString *model = [[UIDevice currentDevice] model];
    //using a simulator
    if ([model isEqualToString:@"iPhone Simulator"] || [model isEqualToString:@"iPad Simulator"]) {
        [self updateStatusLabel:[NSString stringWithFormat:@"NOTE: Using %@ does not support network detection - all possible actions are displayed",model]];
        [self displayPreOnbordingElements:1];
        self.offBoardingBtn.alpha = 1;
        self.connectBtn.enabled = NO;
    } else {
        if ([self isOnSAPNetwork])
        {
            [self.instructLbl setText:@"Press Configure after filling in the WiFi network details"];
            [self displayPreOnbordingElements:1];
            self.connectBtn.enabled = NO;
            [self.ssidTextField setText:[[NSUserDefaults standardUserDefaults] objectForKey:@"lastVisitedNetwork"]];
        } else {
            [self.instructLbl setText:@"To offboard the device - press Offboard"];
            [self displayPreOnbordingElements:0];
            //device already onboarded
            self.offBoardingBtn.alpha = 1;
        }
    }
    
    return status;
}

-(QStatus)createNewSession
{
    //create sessionOptions
    [self updateStatusLabel:[NSString stringWithFormat:@"Create a new session with %@", [self.clientInformation.announcement busName]]];
    AJNSessionOptions *opt = [[AJNSessionOptions alloc] initWithTrafficType:kAJNTrafficMessages supportsMultipoint:false proximity:kAJNProximityAny transportMask:kAJNTransportMaskAny];
    
    //call joinSession
    self.sessionId = [self.clientBusName
                      joinSessionWithName:[self.clientInformation.announcement busName]
                      onPort:[self.clientInformation.announcement port]
                      withDelegate:(nil) options:opt];
    
    if (self.sessionId == 0 || self.sessionId == -1) {
        [self updateStatusLabel:[NSString stringWithFormat:@"Failed to join session. sid=%u",self.sessionId]];
        return ER_FAIL;
    }
    return ER_OK;
}

-(bool)isOnSAPNetwork
{
    NSString *currentSSID;
    // this code does not work in the simulator
    NSArray *supportedInterfaces = (__bridge_transfer id)CNCopySupportedInterfaces();
    id interfaceInformation = nil;
    for (NSString *interfaceName in supportedInterfaces) {
        interfaceInformation = (__bridge_transfer id)CNCopyCurrentNetworkInfo((__bridge CFStringRef)interfaceName);
        NSDictionary *dict = interfaceInformation;
        currentSSID = dict[@"SSID"];
        NSLog(@"Current SSID: %@", currentSSID);
    }
    return [currentSSID hasPrefix:AJ_AP_PREFIX] | [currentSSID hasSuffix:AJ_AP_SUFFIX];
}

-(void)updateStatusLabel:(NSString *)status
{
    NSLog(@"%@",status);
    [self.statusLbl setText:status];
}

-(void)stopOnboardingClient
{
    QStatus status;
    
    NSLog(@"Calling leaveSession");
    status = [self.clientBusName leaveSession:self.sessionId];
    if (ER_OK != status) {
        NSLog(@"Failed to leave session %u, %@",self.sessionId, [AJNStatus descriptionForStatusCode:status]);
    }
    
    self.onboardingClient = nil;
}

#pragma marks - event methods
- (IBAction)configureBtnDidTouchUpInside:(id)sender {
    QStatus status;
    [self updateStatusLabel:@"Calling ConfigureWiFi"];
    
    AJOBInfo obInfo;
   obInfo.SSID = self.ssidTextField.text;
   obInfo.passcode = self.ssidPassTextField.text;
   obInfo.authType = ANY;
    
    NSLog(@"input SSID:%@ passcode:%@",obInfo.SSID,obInfo.passcode);
    
    if (![obInfo.SSID length]) {
        [self updateStatusLabel:@"Error: SSID is empty"];
        [[[UIAlertView alloc] initWithTitle:@"Error" message:@"SSID can't be empty" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil] show];
        return;
    }
    
    bool shouldConvert = false;
    if ([AJOBSOnboarding isValidWPAKey:obInfo.passcode]) {
        [self updateStatusLabel:@"Input passcode is a valid WPA - will convert to Hex"];
        shouldConvert = true;
    } else if([AJOBSOnboarding isValidWEPKey:obInfo.passcode]) {
        if ([obInfo.passcode length] % 2) {
            [self updateStatusLabel:@"Input passcode is valid WEP - converting to Hex"];
            shouldConvert = true;
        } else {
            [self updateStatusLabel:@"Input passcode is valid WEP - Hex conversion is not required"];
        }
    } else {
        [self updateStatusLabel:@"Input passcode is none of the following: WPA, WEP"];
    }
    
    if (shouldConvert) {
        NSString *tPasscode = [AJOBSOnboarding passcodeToHex:obInfo.passcode];
       obInfo.passcode = tPasscode;
        NSLog(@"Passcode has been converted to:%@",obInfo.passcode);
    }
    
    short resultStatus;
    status = [self.onboardingClient configureWiFi:self.onboardeeBus obInfo:obInfo resultStatus:resultStatus sessionId:self.sessionId];
    if (status == ER_OK) {
        [self updateStatusLabel:[NSString stringWithFormat:@"Call to configureWiFi succeeded.\nResult status is %hd",resultStatus]];
        [self.instructLbl setText:@"Press Connect to complete the onboarding."];
        
        if (resultStatus == 1)
        {
            self.connectBtn.enabled = YES;
        }
        
        if (resultStatus == 2) {
            [self updateStatusLabel:@"Waiting for connectionResult signal"];
        }
        
    } else {
        [[[UIAlertView alloc] initWithTitle:@"Error" message:[NSString stringWithFormat:@"Call to configureWiFi failed: %@ ", [AJNStatus descriptionForStatusCode:status]] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil] show];
        [self updateStatusLabel:[NSString stringWithFormat:@"Call to configureWiFi failed: %@ ", [AJNStatus descriptionForStatusCode:status]]];
    }
    
    self.configureBtn.enabled = NO;
    self.ssidTextField.enabled = NO;
    self.ssidPassTextField.enabled = NO;
}

- (IBAction)connectBtnDidTouchUpInside:(id)sender {
    QStatus status;
    
    [self updateStatusLabel:@"Calling connect"];
    status = [self.onboardingClient connectTo:self.onboardeeBus sessionId:self.sessionId];
    if (status == ER_OK) {
        [self updateStatusLabel:@"Call to connect succeeded"];
        [[[UIAlertView alloc] initWithTitle:@"Onboarding succeeded" message:[NSString stringWithFormat:@"Check that your device connects to the '%@' network.\nGo to Settings -> Wi-Fi", self.ssidTextField.text] delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil, nil] show];
    } else {
        [self updateStatusLabel:[NSString stringWithFormat:@"Call to connect failed: %@", [AJNStatus descriptionForStatusCode:status]]];
    }
    
    [self.connectBtn setEnabled:NO];
    [self.instructLbl setText:@"  "];
}
- (IBAction)offBoardingBtnDidTouchUpInside:(id)sender {
    
    [self updateStatusLabel:@"Calling offboard"];
    QStatus status = [self.onboardingClient offboardFrom:self.onboardeeBus sessionId:self.sessionId];
    if (status == ER_OK) {
        [self updateStatusLabel:@"Call to offboard succeeded"];
    } else {
        [self updateStatusLabel:[NSString stringWithFormat:@"Call to offboard failed %@", [AJNStatus descriptionForStatusCode:status]]];
    }
    [self.offBoardingBtn setEnabled:NO];
    [self.instructLbl setText:@"  "];
}

//  Get the user's input from the alert dialog
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    
}

- (void)viewWillDisappear:(BOOL)animated
{
    [self stopOnboardingClient];
    
    [super viewWillDisappear:animated];
}

-(void)connectionResultSignalReceived:(int) connectionResultCode connectionResultMessage:(NSString*) connectionResultMessage
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self updateStatusLabel:[NSString stringWithFormat:@"connectionResultSignal has been received with code:%d message:%@", connectionResultCode,connectionResultMessage]];
        if(connectionResultCode == VALIDATED) {
        [self.connectBtn setEnabled:YES];
        }
	});
}

@end
