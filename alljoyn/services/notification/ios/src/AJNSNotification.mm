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

#import "AJNSNotification.h"
#import "alljoyn/about/AJNConvertUtil.h"

@interface AJNSNotification ()

/** notificationHandler */
@property (nonatomic) ajn::services::Notification *handle;

@end

@implementation AJNSNotification
- (id)initWithHandle:(const ajn::services::Notification *)handle
{
	self = [super init];
	if (self) {
		self.handle = (ajn::services::Notification *)handle;
	}
	return self;
}

/**
 * Constructor for Notification
 * @param messageId
 * @param messageType
 * @param deviceId
 * @param deviceName
 * @param appId
 * @param appName
 * @param sender
 * @param customAttributes
 * @param notificationText
 * @param richIconUrl
 * @param richAudioUrl
 * @param richIconObjectPath
 * @param richAudioObjectPath
 * @param controlPanelServiceObjectPath
 */

- (AJNSNotification *)initWithMessageId:(int32_t)messageId
                           messageType:(AJNSNotificationMessageType)messageType
                              deviceId:(NSString *)deviceId
                            deviceName:(NSString *)deviceName
                                 appId:(NSString *)appId
                               appName:(NSString *)appName
                                sender:(NSString *)sender
                      customAttributes:(NSMutableDictionary *)customAttributes
                      notificationText:(NSMutableArray *)notificationText
                           richIconUrl:(NSString *)richIconUrl
                          richAudioUrl:(NSMutableArray *)richAudioUrl
                    richIconObjectPath:(NSString *)richIconObjectPath
                   richAudioObjectPath:(NSString *)richAudioObjectPath
         controlPanelServiceObjectPath:(NSString *)controlPanelServiceObjectPath
{
	self = [super init];
	if (self) {
		self.handle = new ajn::services::Notification::Notification(messageId,
		                                                            [self convertAJNSNMessageType:messageType],
		                                                            [AJNConvertUtil convertNSStringToConstChar:deviceId],
		                                                            [AJNConvertUtil convertNSStringToConstChar:deviceName],
		                                                            [AJNConvertUtil convertNSStringToConstChar:appId],
		                                                            [AJNConvertUtil convertNSStringToConstChar:appName],
		                                                            [AJNConvertUtil convertNSStringToConstChar:sender],
		                                                            [self convertAJNSCustomAttributes:customAttributes],
		                                                            [self convertAJNSNotificationText:notificationText],
		                                                            [AJNConvertUtil convertNSStringToConstChar:richIconUrl],
		                                                            [self convertAJNSRichAudioUrl:richAudioUrl],
		                                                            [AJNConvertUtil convertNSStringToConstChar:richIconObjectPath],
		                                                            [AJNConvertUtil convertNSStringToConstChar:richAudioObjectPath],
		                                                            [AJNConvertUtil convertNSStringToConstChar:controlPanelServiceObjectPath], NULL);
	}
	return self;
}

/**
 * Construct for Notification
 * @param messageType
 * @param notificationText
 */
- (AJNSNotification *)initWithMessageType:(AJNSNotificationMessageType)messageType andNotificationText:(NSMutableArray *)notificationText
{
	//covert parameters from obj-c to cpp
	NSMutableDictionary *tmpCustomAttributesDictionary = [[NSMutableDictionary alloc] init];
	NSMutableArray *tmpRichAudioUrlArr = [[NSMutableArray alloc] init];
    
	self = [self           initWithMessageId:-1
	                             messageType:messageType
	                                deviceId:@""
	                              deviceName:@""
	                                   appId:@""
	                                 appName:@""
	                                  sender:@""
	                        customAttributes:tmpCustomAttributesDictionary
	                        notificationText:notificationText
	                             richIconUrl:@""
	                            richAudioUrl:tmpRichAudioUrlArr
	                      richIconObjectPath:@""
	                     richAudioObjectPath:@""
	           controlPanelServiceObjectPath:@""];
    
	return self;
}

//Retreival methods
/**
 * Get the Version
 * @return version
 */
- (int16_t)version
{
	return (self.handle->getVersion());
}

/**
 * Get the device Id
 * @return deviceId
 */
- (NSString *)deviceId
{
	return self.handle->getDeviceId() ? ([AJNConvertUtil convertConstCharToNSString:(self.handle->getDeviceId())]) : nil;
}

/**
 * Get the Device Name
 * @return deviceName
 */
- (NSString *)deviceName
{
	return self.handle->getDeviceName() ? ([AJNConvertUtil convertConstCharToNSString:(self.handle->getDeviceName())]) : nil;
}

/**
 * Get the app Id
 * @return appId
 */
- (NSString *)appId
{
	return self.handle->getAppId() ? ([AJNConvertUtil convertConstCharToNSString:self.handle->getAppId()]) : nil;
}

/**
 * Get the app Name
 * @return appName
 */
- (NSString *)appName
{
	return [AJNConvertUtil convertConstCharToNSString:self.handle->getAppName()];
}

/**
 * Get the map of customAttributes
 @return customAttributes
 */
- (NSMutableDictionary *)customAttributes
{
	std::map <qcc::String, qcc::String> customAttributesMap;
	std::map <qcc::String, qcc::String>::iterator cIt;
	NSMutableDictionary *customAttributesDictionary = [[NSMutableDictionary alloc] init];
    
	//call cpp getCustomAttributes(returns map)
	//returns const std::map<qcc::String, qcc::String>
	customAttributesMap = self.handle->getCustomAttributes();
	//translate cpp returned value to obj-c (map to dictionary)
	for (cIt = customAttributesMap.begin(); cIt != customAttributesMap.end(); ++cIt) {
		//insert key/value into the dictionary
		[customAttributesDictionary setValue:[AJNConvertUtil convertQCCStringtoNSString:cIt->second] forKey:[AJNConvertUtil convertQCCStringtoNSString:cIt->first]];
	}
	return customAttributesDictionary;
}

/**
 * Get the Message Id
 * @return notificationId
 */
- (int32_t)messageId
{
	return self.handle->getMessageId();
}

/**
 * Get the Sender
 * @return Sender
 */
- (NSString *)senderBusName
{
	return self.handle->getSenderBusName() ? [AJNConvertUtil convertConstCharToNSString:self.handle->getSenderBusName()] : nil;
}

/**
 * Get the MessageType
 * @return MessageType
 */
- (AJNSNotificationMessageType)messageType
{
	return (AJNSNotificationMessageType)(self.handle->getMessageType());
}

/**
 * Get the Notification Text
 * @return notificationText
 */
- (NSArray *)text
{
	return self.ajnsntArr;
}

/**
 * Get the Rich Icon Url
 * @return RichIconUrl
 */
- (NSString *)richIconUrl
{
	return self.handle->getRichIconUrl() ? [AJNConvertUtil convertConstCharToNSString:self.handle->getRichIconUrl()] : nil;
}

/**
 * Get the Rich Icon Object Path
 * @return richIconObjectPath
 */
- (NSString *)richIconObjectPath
{
	return self.handle->getRichIconUrl() ? [AJNConvertUtil convertConstCharToNSString:self.handle->getRichIconObjectPath()] : nil;
}

/**
 * Get the Rich Audio Object Path
 * @return richAudioObjectPath
 */
- (NSString *)richAudioObjectPath
{
	return self.handle->getRichIconUrl() ? [AJNConvertUtil convertConstCharToNSString:self.handle->getRichAudioObjectPath()] : nil;
}

/**
 * Get the Rich Audio Urls
 * @return RichAudioUrl - NSMutableArray of AJNSRichAudioUrl objects
 */
- (void)richAudioUrl:(NSMutableArray *)inputArray
{
	std::vector <ajn::services::RichAudioUrl> nRichAudioUrlVect;
    
	//call cpp getRichAudioUrl (returns std::vector<RichAudioUrl>)
	nRichAudioUrlVect = self.handle->getRichAudioUrl();
	//for ( size_t &i : nRichAudioUrl )
    
	for (size_t i = 0; i < nRichAudioUrlVect.size(); i++) {
		AJNSRichAudioUrl *ajnsRau = [[AJNSRichAudioUrl alloc] init];
        
		ajnsRau.richAudioUrlHandler = &nRichAudioUrlVect.at(i);
        
		[inputArray addObject : (ajnsRau)];
	}
}

/**
 * Get the ControlPanelService object path
 * @return ControlPanelServiceObjectPath
 */
- (NSString *)controlPanelServiceObjectPath
{
	return self.handle->getControlPanelServiceObjectPath() ? [AJNConvertUtil convertConstCharToNSString:self.handle->getControlPanelServiceObjectPath()] : nil;
}

//Methods which set information

/**
 * Set the App Id of the Notification
 * @param appId
 */
- (void)setAppId:(NSString *)appId
{
	self.handle->setAppId([AJNConvertUtil convertNSStringToConstChar:appId]);
}

/**
 * Set the App Name of the Notification
 * @param appName
 */
- (void)setAppName:(NSString *)appName
{
	self.handle->setAppName([AJNConvertUtil convertNSStringToConstChar:appName]);
}

/**
 * Set the Control Panel Service Object Path of the Notification
 * @param controlPanelServiceObjectPath
 */
- (void)setControlPanelServiceObjectPath:(NSString *)controlPanelServiceObjectPath
{
	self.handle->setControlPanelServiceObjectPath([AJNConvertUtil convertNSStringToConstChar:controlPanelServiceObjectPath]);
}

/**
 * Set the Custom Attributed of the Notification
 * @param customAttributes
 */
- (void)setCustomAttributes:(NSMutableDictionary *)customAttributes
{
	self.handle->setCustomAttributes([self convertAJNSCustomAttributes:customAttributes]);
}

/**
 * Set the deviceId of the Notification
 * @param deviceId
 */
- (void)setDeviceId:(NSString *)deviceId
{
	self.handle->setDeviceId([AJNConvertUtil convertNSStringToConstChar:deviceId]);
}

/**
 * Set the deviceName of the Notification
 * @param deviceName
 */
- (void)setDeviceName:(NSString *)deviceName
{
	self.handle->setDeviceName([AJNConvertUtil convertNSStringToConstChar:deviceName]);
}

/**
 * Set the messageId of the Notification
 * @param messageId
 */
- (void)setMessageId:(int32_t)messageId
{
	self.handle->setMessageId(messageId);
}

/**
 * Set the richAudioUrl of the Notification
 * @param richAudioUrl
 */
- (void)setRichAudioUrl:(NSMutableArray *)richAudioUrl
{
	self.handle->setRichAudioUrl([self convertAJNSRichAudioUrl:richAudioUrl]);
}

/**
 * Set the richIconUrl of the Notification
 * @param richIconUrl
 */
- (void)setRichIconUrl:(NSString *)richIconUrl
{
	self.handle->setRichIconUrl([AJNConvertUtil convertNSStringToConstChar:richIconUrl]);
}

/**
 * Set the richIconObjectPath of the Notification
 * @param richIconObjectPath
 */
- (void)setRichIconObjectPath:(NSString *)richIconObjectPath
{
	self.handle->setRichIconObjectPath([AJNConvertUtil convertNSStringToConstChar:richIconObjectPath]);
}

/**
 * Set the richAudioObjectPath of the Notification
 * @param richAudioObjectPath
 */
- (void)setRichAudioObjectPath:(NSString *)richAudioObjectPath
{
	self.handle->setRichAudioObjectPath([AJNConvertUtil convertNSStringToConstChar:richAudioObjectPath]);
}

/**
 * Set the sender of the Notification
 * @param sender
 */
- (void)setSender:(NSString *)sender
{
	self.handle->setSender([AJNConvertUtil convertNSStringToConstChar:sender]);
}

/**
 * Convert AJNSNMessageType to NotificationMessageType
 * @param ajnsnMessageType
 */
- (ajn::services::NotificationMessageType)convertAJNSNMessageType:(AJNSNotificationMessageType)ajnsnMessageType
{
	return (ajn::services::NotificationMessageType)ajnsnMessageType;
}

/**
 * Convert AJNSCustomAttributes NSMutableDictionary to CustomAttributes std::map
 * @param ajnsCustomAttributes
 */
- (std::map <qcc::String, qcc::String> )convertAJNSCustomAttributes:(NSMutableDictionary *)ajnsCustomAttributes
{
	std::map <qcc::String, qcc::String> nmap;
	NSEnumerator *enumerator = [ajnsCustomAttributes keyEnumerator];  //use  NSFastEnumeration?
	id key;
	while ((key = [enumerator nextObject])) {
		/* put key and value in the map */
		//        nmap.insert(std::make_pair([key UTF8String],[[ajnsCustomAttributes objectForKey:(key)] UTF8String]));
		nmap.insert(std::make_pair([key cStringUsingEncoding:NSUTF8StringEncoding], [[ajnsCustomAttributes objectForKey:key] cStringUsingEncoding:NSUTF8StringEncoding]));
	}
	return nmap;
}

/**
 * Convert AJNSNotificationText NSMutableArray to NotificationText std::vector
 * @param ajnsnTextArr
 */
- (std::vector <ajn::services::NotificationText> )convertAJNSNotificationText:(NSMutableArray *)ajnsnTextArr
{
	std::vector <ajn::services::NotificationText> nNotificationTextVect;
	//for ( auto &i : ajnsnTextArr )
    
	for (AJNSNotificationText *t_AJNSNotificationText in ajnsnTextArr) {
		nNotificationTextVect.push_back(*(t_AJNSNotificationText.handle));
	}
    
	return nNotificationTextVect;
}

/**
 * Convert AJNSRichAudioUrl NSMutableArray to RichAudioUrl std::vector
 * @param ajnsRichAudioUrlArr
 */
- (std::vector <ajn::services::RichAudioUrl> )convertAJNSRichAudioUrl:(NSMutableArray *)ajnsRichAudioUrlArr
{
	std::vector <ajn::services::RichAudioUrl> nRichAudioUrlVect;
    
	//convert ajnsRichAudioUrlArr(NSString)to AJNSRichAudioUrl object
	for (AJNSRichAudioUrl *t_AJNSRichAudioUrl in ajnsRichAudioUrlArr) {
		nRichAudioUrlVect.push_back(*(t_AJNSRichAudioUrl.richAudioUrlHandler));
	}
    
	return nRichAudioUrlVect;
}

- (void)createAJNSNotificationTextArray
{
	self.ajnsntArr = [[NSMutableArray alloc] init];
	std::vector <ajn::services::NotificationText> ntVect; //cpp vector
	ntVect = self.handle->getText();
    
	for (size_t i = 0; i < ntVect.size(); i++) {
		//        ajnsNt.notificationTextHandler = &(ntVect.at(i));         //cpp
        
		//get lang and text from the object at i
		qcc::String cppl = ntVect.at(i).getLanguage();
		NSString *ocl = [AJNConvertUtil convertQCCStringtoNSString:cppl];
        
		qcc::String cppt = ntVect.at(i).getText();
		NSString *oct = [AJNConvertUtil convertQCCStringtoNSString:cppt];
        
		AJNSNotificationText *ajnsNt = [[AJNSNotificationText alloc] initWithLang:ocl andText:oct];
        
		[self.ajnsntArr addObject:ajnsNt];
	}
}

-(QStatus) dismiss
{
    return self.handle->dismiss();
}

@end
