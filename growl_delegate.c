#include "growl_delegate.h"

@implementation GrowlDelegate
@synthesize applicationName, callbackProc;

-(id)init {
  if((self = [super init])) {
    applicationName = [@"Notification" retain];
    return self;
  }
  
  return nil;
}

-(NSString *)applicationNameForGrowl {
  return applicationName;
}

-(NSDictionary *)registrationDictionaryForGrowl {
  NSArray *notifications = [NSArray arrayWithObject:GROWL_NOTIFICATION];
  return [NSDictionary dictionaryWithObjectsAndKeys: [self applicationName], GROWL_APP_ID, [self applicationName], GROWL_APP_NAME, notifications, GROWL_NOTIFICATIONS_ALL, notifications, GROWL_NOTIFICATIONS_DEFAULT, nil];
}

-(void)growlNotificationWasClicked:(id)clickContext {
  if(callbackProc != Qnil) {
    // TODO: This needs to call a C++ function
    rb_funcall(callbackProc, rb_intern("call"), 0);
  }
}

-(void)dealloc {
  [applicationName release];
  [super dealloc];
}

@end