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
#include "Multisampling.h"

@interface XWindow : NSObject

-(instancetype)initWithWidth:(int)w height:(int)h sharedCtx:(XWindow*)s title:(NSString*)t fullscreen:(BOOL)f screen:(int)scrInd windowID:(void*)winID multisamplingLevel:(int)level;

// Context things
-(void)makeCurrentContext;
-(void)clearCurrentContext;
-(void)flushBuffer;

// Windowy things
-(void)bringToFront;
-(void)makeFirstResponder;
-(void)setTitle:(const char *)t;

// Size & position
-(void)getSizeW:(int*)w H:(int*)h;
-(void)setSizeW:(int)w H:(int)h;

-(void)getPosX:(int*)x Y:(int*)y;
-(void)setPosX:(int)x Y:(int)y;

-(void)setScreen:(int)screenInd;
-(int)getScreen;

-(void)goFullscreen;
-(void)goWindowed;
-(BOOL)isFullscreen;

// Mouse
-(BOOL)mouseIsOver;
-(void)getMousePositionX:(int*)x Y:(int*)y;
-(void)setMousePositionX:(int)x Y:(int)y;

@end
