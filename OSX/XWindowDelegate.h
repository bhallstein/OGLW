#import <Cocoa/Cocoa.h>

@interface XWindowDelegate : NSObject <NSWindowDelegate>

-(instancetype)initWithWindowID:(void*)winID;

-(BOOL)windowShouldClose:(id)sender;
-(void)windowDidMiniaturize:(NSNotification*)n;
-(void)windowDidDeminiaturize:(NSNotification *)n;
-(void)windowDidBecomeKey:(NSNotification *)n;
-(void)windowDidResignKey:(NSNotification *)n;

@end
