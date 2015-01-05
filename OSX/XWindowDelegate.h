/*
 * W - simple cross-platform OpenGL windows
 *
 * =====================
 *  XWindowDelegate.h
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import <Cocoa/Cocoa.h>

@interface XWindowDelegate : NSObject <NSWindowDelegate>

-(instancetype)initWithWindowID:(void*)winID;

-(BOOL)windowShouldClose:(id)sender;
-(void)windowDidMiniaturize:(NSNotification*)n;
-(void)windowDidDeminiaturize:(NSNotification *)n;
-(void)windowDidBecomeKey:(NSNotification *)n;
-(void)windowDidResignKey:(NSNotification *)n;

@end
