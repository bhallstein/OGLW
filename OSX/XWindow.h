/*
 * W - simple cross-platform OpenGL windows
 *
 * =============
 *  XWindow.h
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import <Cocoa/Cocoa.h>

@interface XWindow : NSWindow

@property (strong, nonatomic) NSOpenGLContext *context;
@property NSPoint pos;
@property NSSize size;

-(instancetype)initWithWidth:(int)w height:(int)h sharedCtx:(XWindow*)s posX:(int)px posY:(int)py title:(NSString*)t fullscreen:(BOOL)f  screen:(int)s windowID:(void*)winID;

-(void)bringToFront;
-(void)makeFirstResponder;
-(void)goFullscreenOn:(int)screen savePrevFrame:(BOOL)yn;
	// scr: -1 => current screen
-(void)goWindowed;

@end
