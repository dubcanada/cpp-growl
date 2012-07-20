/* TODO: These have to be rewritten for C++ */

#include "growl_api.h"

VALUE rb_mGrowl;
VALUE rb_cGrowlApplication;

static NSString*
build_nsstring(VALUE string) {
  if(string == Qnil) {
    return nil;
  } else {
    return [NSString stringWithCString:StringValuePtr(string) encoding: NSASCIIStringEncoding];    
  }
}

static NSData*
get_icon_data(VALUE pathOrUrl) {
  NSString *nsPathOrUrl = build_nsstring(pathOrUrl);
  NSData *iconData = nil;
  
  if(nsPathOrUrl != nil) {
    iconData = [NSData dataWithContentsOfFile: nsPathOrUrl];
    
    if(iconData == nil) {
      iconData = [NSData dataWithContentsOfURL: [NSURL URLWithString: nsPathOrUrl]];
    }
  }
  
  return iconData;
}


static NSAutoreleasePool*
create_autorelease_pool() {
  return [[NSAutoreleasePool alloc] init];
}

static void
free_delegate(id delegate) {
  [delegate release];
}

static VALUE
alloc_delegate(VALUE klass) {
  return Data_Wrap_Struct(klass, 0, free_delegate, [[GrowlDelegate alloc] init]);  
}

VALUE
classMethod_installed(VALUE self) {
  NSAutoreleasePool *pool = create_autorelease_pool();
  BOOL installed = [GrowlApplicationBridge isGrowlInstalled];
  [pool drain];
  
  if(installed) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE
classMethod_running(VALUE self) {
  NSAutoreleasePool *pool = create_autorelease_pool();
  BOOL running = [GrowlApplicationBridge isGrowlRunning];
  [pool drain];
  
  if(running) {
    return Qtrue;
  } else {
    return Qfalse;
  }
}

VALUE
method_notify(VALUE self, VALUE options) {
  NSAutoreleasePool *pool = create_autorelease_pool();
  id delegate = NULL;
  Data_Get_Struct(self, GrowlDelegate, delegate);
  
  VALUE title             = rb_hash_aref(options, ID2SYM(rb_intern("title")));
  VALUE description       = rb_hash_aref(options, ID2SYM(rb_intern("description")));
  VALUE priority          = rb_Integer(rb_hash_aref(options, ID2SYM(rb_intern("priority"))));
  VALUE sticky            = rb_hash_aref(options, ID2SYM(rb_intern("sticky")));
  VALUE iconPath          = rb_hash_aref(options, ID2SYM(rb_intern("icon")));  
  VALUE click             = rb_hash_aref(options, ID2SYM(rb_intern("click")));
  
  BOOL nsSticky           = (sticky == Qtrue) ? YES : NO;
  NSData *data            = get_icon_data(iconPath);
  NSString *nsTitle       = build_nsstring(title);
  NSString *nsDescription = build_nsstring(description);
  NSNumber *clickContext  = [NSNumber numberWithUnsignedLong: self];
  
  [delegate setCallbackProc: click];
  [GrowlApplicationBridge setGrowlDelegate: delegate];  
  [GrowlApplicationBridge 
    notifyWithTitle: nsTitle 
    description: nsDescription 
    notificationName: REAL_GROWL_NOTIFICATION 
    iconData: data 
    priority: NUM2INT(priority) 
    isSticky:nsSticky 
    clickContext:clickContext];
  
  [pool drain];
  
  return Qnil;
}

VALUE 
method_init(VALUE self, VALUE applicationName) {
  NSAutoreleasePool *pool = create_autorelease_pool();
  id delegate = NULL;  
  
  Data_Get_Struct(self, GrowlDelegate, delegate);  
  NSString *nsAppName = build_nsstring(applicationName);
  [delegate setApplicationName: nsAppName];
  
  [pool drain];
  
  return self;
}

void
Init_real_growl_api() {  
  rb_mRealGrowl = rb_define_module("RealGrowl");
  rb_define_singleton_method(rb_mRealGrowl, "installed?", classMethod_installed, 0);
  rb_define_singleton_method(rb_mRealGrowl, "running?", classMethod_running, 0);
  
  rb_cRealGrowlApplication = rb_define_class_under(rb_mRealGrowl, "Application", rb_cObject);
  rb_define_alloc_func(rb_cRealGrowlApplication, alloc_delegate);
  rb_define_method(rb_cRealGrowlApplication, "initialize", method_init, 1);
  rb_define_method(rb_cRealGrowlApplication, "notify", method_notify, 1);  
}

