/*
 * W - simple cross-platform OpenGL windows
 *
 * ===========
 *  XView.h
 * ===========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import <Cocoa/Cocoa.h>

@interface XView : NSView

@property BOOL clickThrough;

-(instancetype)initWithFrame:(NSRect)f clickThrough:(BOOL)c windowID:(void*)winID;
-(void)convertNSWindowToWCoords:(NSPoint*)p;

@end
