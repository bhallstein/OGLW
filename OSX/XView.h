#import <Cocoa/Cocoa.h>

@interface XView : NSView

@property BOOL clickThrough;

-(instancetype)initWithFrame:(NSRect)f clickThrough:(BOOL)c windowID:(void*)winID;
-(void)convertNSWindowToWCoords:(NSPoint*)p;

@end
