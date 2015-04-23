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

#import "AJCPSGetControlPanelViewController.h"
#import "alljoyn/Status.h"
#import "AJCPSControlPanelService.h"
#import "AJCPSControlPanelController.h"
#import "AJCPSContainer.h"
#import "AJCPSControllerUpdateEvents.h"
#import "AJCPSCPSButtonCell.h"
#import "AJCPSCPSGeneralCell.h"

const float GENERAL_CELL_HEIGHT = 115.00;
const float BUTTON_CELL_HEIGHT = 65.00;
static NSString * const CLIENTDEFAULTLANG=@"";
static NSString * const CPS_BUTTON_CELL = @"CPSButtonCell";
static NSString * const CPS_GENERAL_CELL = @"CPSGeneralCell";


@interface GetControlPanelViewController () <ControllerUpdateEvents,UIAlertViewDelegate>
@property (strong, nonatomic) ControllerModel *controllerModel;
@property (strong, nonatomic) AJCPSControlPanelService *controlPanelService;
@property (strong, nonatomic) AJCPSControlPanelController *controlPanelController;
@property (strong, nonatomic) AJCPSControlPanelDevice *controlPanelDevice;
@property (strong, nonatomic) UITextField *alertChooseLanguage;
@property (strong, nonatomic) UIBarButtonItem *chooseLangButton;

@property (strong, nonatomic) UITableView* tableView;

@property (weak, nonatomic) AJNBusAttachment *clientBusAttachment;

@property (weak, nonatomic) AJNAnnouncement *announcement;
@property (nonatomic) bool isAnnouncementMode;
@property (strong, nonatomic) NSString *notificationSenderBusName;
@property (strong, nonatomic) NSString *notificationCPSObjectPath;
@property (nonatomic) bool isNotificationMode;
@property (strong, nonatomic) AJCPSNotificationAction* notificationAction;

@property (strong, atomic) UIAlertView *loadingAV;

@end


@implementation GetControlPanelViewController

- (id)initWithNotificationSenderBusName:(NSString*) senderBusName cpsObjectPath:(NSString*) cpsObjectPath bus:(AJNBusAttachment*) bus
{
    if (self = [super init]) {
        self.notificationSenderBusName = senderBusName;
        self.notificationCPSObjectPath = cpsObjectPath;
        self.clientBusAttachment = bus;
        self.isNotificationMode = true;
    }
    return self;
}

- (id)initWithAnnouncement:(AJNAnnouncement*) announcement bus:(AJNBusAttachment*) bus
{
    if (self = [super init]) {
        self.announcement = announcement;
        self.clientBusAttachment = bus;
        self.isAnnouncementMode = true;
    }
    return self;
}

- (void)viewDidLoad
{
    QStatus status;
	[super viewDidLoad];
    
    self.navigationItem.hidesBackButton = YES;
    [self showLoadingAlert:@"Loading..."];

    // Add language button
    if (self.isAnnouncementMode) {
        [self addLanguageButton];
    }
    
    CGRect tableViewFrame = self.view.bounds;
    

        self.tableView = [[UITableView alloc] initWithFrame:tableViewFrame style:UITableViewStylePlain];
        self.tableView.delegate = self;
        self.tableView.dataSource = self;
        
        // Add two cells to UITableView :
        [self.tableView registerClass:[CPSButtonCell class] forCellReuseIdentifier:CPS_BUTTON_CELL];
        [self.tableView registerClass:[CPSGeneralCell class] forCellReuseIdentifier:CPS_GENERAL_CELL];
        
        [self.view addSubview:self.tableView];
    
    if (!self.controllerModel) {
        status = [self startService];
        if (ER_OK != status) {
            NSLog(@"Failed to start control panel");
            [[[UIAlertView alloc] initWithTitle:@"Error" message:@"Failed to start control panel." delegate:self cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
            
            [self loadEnded];
        }
    }
}

-(QStatus)startService
{
    QStatus status;
    
    if (!self.clientBusAttachment) {
        NSLog(@"Bus attachment is nil.");
        return ER_FAIL;
    }
    
    if (self.isNotificationMode && self.isAnnouncementMode) {
        NSLog(@"Using both Annnouncement and Notification mode is not supported");
        return ER_FAIL;
    }
    // Get a controllable device using senderBusName (Notification1.1)
    self.controlPanelController = [[AJCPSControlPanelController alloc] init];
    if (self.isNotificationMode) {
        self.controlPanelDevice  = [self.controlPanelController getControllableDevice:self.notificationSenderBusName];
        if (!self.controlPanelDevice) {
            NSLog(@"Could not initialize control panel device.");
            return ER_FAIL;
        }
    }
    
    self.controllerModel = [[ControllerModel alloc] init];
    if (!self.controllerModel) {
        NSLog(@"Could not initialize controller model.");
        return ER_FAIL;
    }
    self.controllerModel.delegate = self;
    
    
    self.controlPanelService = [AJCPSControlPanelService getInstance];
    status = [self.controlPanelService initController:self.clientBusAttachment
                               controlPanelController:self.controlPanelController
                                 controlPanelListener:self.controllerModel];
    
    if (status != ER_OK) {
        NSLog(@"Could not initialize Controller.");
        return ER_FAIL;
    } else {
        NSLog(@"Successfully initialize Controller.");
    }
    
    
    if (self.isAnnouncementMode) {
        // Create a controllable device using the announcement bus name - this will trigger a listener method
        self.controlPanelDevice = [self.controlPanelController createControllableDevice:self.announcement.busName
                                                                            ObjectDescs:self.announcement.objectDescriptions];
        if (!self.controlPanelDevice) {
            NSLog(@"Could not initialize control panel device.");
            return ER_FAIL;
        } else {
            NSLog(@"Successfully initialize control panel device.");

        }
        return status;
    }
    
    
    if (self.isNotificationMode) {
        status = [self loadNewSessionForNotificationWithAction];
        if (ER_OK != status) {
            NSLog(@"Failed to load session for notification with action");
        } else {
            NSLog(@"Successfully load a session for notification with action");
        }
    }
    return status;
}

-(QStatus)loadNewSessionForNotificationWithAction
{
    QStatus status;
    
    status = [self.controlPanelDevice startSession];
    // Triggers sessionEstablished
    if (ER_OK != status) {
        NSLog(@"Failed to start a session. ERROR: %@", [AJNStatus descriptionForStatusCode:status]);
        //            if (status != ER_ALLJOYN_JOINSESSION_REPLY_ALREADY_JOINED) {
        return ER_FAIL;
        //            }
    } else {
        NSLog(@"Successfully start session with device %@",self.notificationSenderBusName);
    }
    
    // Add notification action
    self.notificationAction = [self.controlPanelDevice addNotificationAction:self.notificationCPSObjectPath];
    if (!self.notificationAction) {
        NSLog(@"Failed to add notification action.");
        return ER_FAIL;
    } else {
        NSLog(@"Successfully added a notification action.");
    }
    
    [self.controllerModel setSupportedLanguagesForNotificationAction:self.notificationAction];
    
    NSArray *languages= [self.controllerModel supportedLanguages];
    if (![languages count]) {
        NSLog(@"notification action languages is empty");
        return ER_FAIL;
    } else {
       
        [self addLanguageButton];
    
        NSLog(@"Get rootWidget for the first language: %@",languages[0]);
        AJCPSRootWidget *rootWidget = [self.notificationAction getRootWidget:languages[0]]; // Take the rootWidget first language
        NSLog(@"Finished loading widget: %@", [rootWidget getWidgetName]);
        [self.controllerModel  loadRootWidget:rootWidget];
    }
    
    return ER_OK;
}

-(void)addLanguageButton
{
    _chooseLangButton = [[UIBarButtonItem alloc] initWithTitle:@"Language" style:UIBarButtonItemStyleBordered target:self action:@selector(chooseLanguageAction)];
    [[self navigationItem] setRightBarButtonItem:_chooseLangButton];
    self.navigationItem.rightBarButtonItem.enabled = NO;
}

- (void)chooseLanguageAction
{
    NSString *supportedLangs;
    
    supportedLangs = [[self.controllerModel supportedLanguages] componentsJoinedByString:@" "];
    
    NSLog(@"Supported languages: %@",supportedLangs);
    
    UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Choose Language" message:supportedLangs delegate:self cancelButtonTitle:@"Cancel" otherButtonTitles:@"OK",nil];
    
    alert.alertViewStyle = UIAlertViewStylePlainTextInput;
    
    self.alertChooseLanguage = [alert textFieldAtIndex:0]; //connect the UITextField with the alert
    
    [alert show];
}

- (void)stopControlPanel
{
    
    QStatus status;
    
    if (self.isNotificationMode) {
        status = [self.controlPanelDevice removeNotificationAction:self.notificationAction];
        if (ER_OK != status) {
            NSLog(@"Failed to remove notification action.");
        }
    }
    self.controlPanelDevice = nil;
    
    self.controllerModel = nil;
    
    self.controlPanelController = nil;
    
    // shutdown will handle the self.controlPanelDevice endSession/shutdownDevice
    status = [self.controlPanelService shutdownController];
    if (ER_OK != status) {
        NSLog(@"Failed to shutdown control panel controller");
    } else {
        NSLog(@"Successfully shutdown control panel controller");
    }
    self.controlPanelService = nil;
    
}

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (buttonIndex == 1) { //user pressed OK
        
        QStatus status = ER_OK;
        if (self.isAnnouncementMode) {
            status = [self.controllerModel switchLanguage:self.alertChooseLanguage.text];
        }
        
        if (self.isNotificationMode) {
            status = [self.controllerModel switchLanguageForNotificationAction:[self.notificationAction getRootWidget:self.alertChooseLanguage.text]];
        }
        
        if (status != ER_OK) {
            [[[UIAlertView alloc]initWithTitle:@"Invalid Language" message:@"" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil] show];
        }
        
    }
    
}

- (void)viewWillAppear:(BOOL)animated
{
    self.controllerModel.delegate = self;
    // enable BackButton for all other containers but the top container(handeled by the AJCPSControlPanelListener protocol methods)
    if ([self.controllerModel childContainerPosition]) {
        self.navigationItem.hidesBackButton = NO;
        [self dismissLoadingAlert];
    }
}


- (void)viewWillDisappear:(BOOL)animated
{
    if ([self.navigationController.viewControllers indexOfObject:self]==NSNotFound) {
        // the UINavigation controller's back button pressed. the current viewcontroller is no longer in the view controller's list
        
        NSInteger pos = [self.controllerModel popChildContainer];
        
        if(pos == 0) { //this is the top most container
            [self stopControlPanel];
        }
    }
        
    
    [super viewWillDisappear:animated];
}


#pragma mark - ControllerUpdateEvents
- (void)refreshEntries
{
    dispatch_async(dispatch_get_main_queue(), ^{
        [self.tableView reloadData];
    });
}

-(void)loadEnded
{
    self.navigationItem.hidesBackButton = NO;
    self.navigationItem.rightBarButtonItem.enabled = YES;
    [self dismissLoadingAlert];
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
    NSInteger count;
    
    @synchronized(self.controllerModel){
                   count  = [[self.controllerModel widgetsContainer]count];
    };
    return count;
    
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    AJCPSWidget *widget = [self.controllerModel widgetsContainer][indexPath.row];
    
    switch ([widget getWidgetType]) {
        case AJCPS_ACTION:
        {
            NSArray *hints = [widget getHints];
            for (NSNumber *hint  in hints) {
                if(hint.shortValue == AJCPS_ACTIONBUTTON) {
                    
                    CPSButtonCell *cell = (CPSButtonCell *)[tableView dequeueReusableCellWithIdentifier:CPS_BUTTON_CELL forIndexPath:indexPath];
                    
                    cell.actionWidget = (AJCPSAction *)widget;
                    return cell;
                }
            }
            
            break;
        }
    }
    
    CPSGeneralCell *cell;
    cell = (CPSGeneralCell *)[tableView dequeueReusableCellWithIdentifier:CPS_GENERAL_CELL forIndexPath:indexPath];
    
    cell.widget = widget;
    
    
    
    // Configure the cell...
    
    cell.selectionStyle = UITableViewCellSelectionStyleNone;
    
    return cell;
}


- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section

{
    return self.controllerModel.unit;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    AJCPSWidget *widget = [self.controllerModel widgetsContainer][indexPath.row];
    
    switch ([widget getWidgetType]) {
        case AJCPS_ACTION:
            return BUTTON_CELL_HEIGHT;
            break;
    }
    
    return GENERAL_CELL_HEIGHT;
    
}


#pragma mark - Navigation

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    AJCPSWidget *widget = [self.controllerModel widgetsContainer][indexPath.row];
    
    if ([widget getWidgetType] == AJCPS_CONTAINER) {
        GetControlPanelViewController *viewController = [[GetControlPanelViewController alloc] init];
        
        // prepare the view we load to show the child container widgets
       
        viewController.controllerModel = self.controllerModel;
        viewController.controllerModel.delegate = viewController;
        viewController.navigationItem.rightBarButtonItem.enabled = NO;
        [viewController.controllerModel pushChildContainer:(AJCPSContainer *)widget];
   
        // show the table
        [self.navigationController pushViewController:viewController animated:YES];
    }
    else {
        NSLog(@"tried to segue into a non container of type %d name %@",[widget getWidgetType], [widget getWidgetName]);
        return;
    }
}

-(void)showLoadingAlert:(NSString *)message
{
    self.loadingAV = [[UIAlertView alloc] initWithTitle:@"Please wait" message:message delegate:nil cancelButtonTitle:nil otherButtonTitles:nil];
    UIActivityIndicatorView *activityIV = [[UIActivityIndicatorView alloc] initWithFrame:CGRectMake(0, 0, 80, 40)];
    activityIV.activityIndicatorViewStyle = UIActivityIndicatorViewStyleGray;
    [activityIV startAnimating];
    [self.loadingAV setValue:activityIV forKey:@"accessoryView"];
    [self.loadingAV show];
}

-(void)dismissLoadingAlert
{
    [self.loadingAV dismissWithClickedButtonIndex:0 animated:YES];
}

@end
