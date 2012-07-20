#include <Foundation/Foundation.h>
#include <Growl/Growl.h>

#define GROWL_NOTIFICATION XSTR("GrowlNotification")

@interface GrowlDelegate : NSObject<GrowlApplicationBridgeDelegate>
{
  NSString *applicationName;
  VALUE callbackProc;
}

-(NSDictionary *) registrationDictionaryForGrowl;
-(void) growlNotificationWasClicked:(id)clickContext;
-(NSString *)applicationNameForGrowl;

@property(nonatomic, assign) VALUE callbackProc;
@property(nonatomic, retain) NSString *applicationName;
@end