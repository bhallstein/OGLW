/*
 * W - simple cross-platform OpenGL windows
 *
 * ===========
 *  XView.m
 * ===========
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import "XView.h"
#include "Event.h"

@interface XView () {
	void *windowID;
}
@end


@implementation XView

@synthesize clickThrough;

-(instancetype)initWithFrame:(NSRect)f clickThrough:(BOOL)c windowID:(void *)winID {
	if (self = [super initWithFrame:f]) {
		clickThrough = c;
		windowID = winID;
	}
	return self;
}

-(BOOL) acceptsFirstResponder {
	return YES;
}

-(void)convertNSWindowToWCoords:(NSPoint *)p {
	*p = [self convertPoint:*p toView:self];
	p->y = self.bounds.size.height - p->y;
	p->y -= 1;
		// y-axis is 1-based and inverted
}


#pragma mark - Event handling

void addMouseEventIfOn(W::EventType::T type, W::Event::MouseEvent info) {
	if (!W::Event::on) return;
	
	W::Event ev(type);
	ev.mouseEvent = info;
	W::Event::newEvents.push_back(ev);
}
void addKeyEventIfOn(W::EventType::T type, W::Event::KeyEvent info) {
	if (!W::Event::on) return;
	
	W::Event ev(type);
	ev.keyEvent = info;
	W::Event::newEvents.push_back(ev);
}

-(void)mouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self convertNSWindowToWCoords:&p];
	
	addMouseEventIfOn(
					  W::EventType::LMouseDown,
					  (W::Event::MouseEvent) { (int)p.x, (int)p.y, (W::Window*)windowID }
					  );
}
-(void)mouseUp:(NSEvent*)nsev {
	NSPoint p = [nsev locationInWindow];
	[self convertNSWindowToWCoords:&p];

	addMouseEventIfOn(
					  W::EventType::LMouseUp,
					  (W::Event::MouseEvent) { (int)p.x, (int)p.y, (W::Window*)windowID }
					  );
}
-(void)rightMouseDown:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self convertNSWindowToWCoords:&p];
	addMouseEventIfOn(
					  W::EventType::RMouseDown,
					  (W::Event::MouseEvent) { (int)p.x, (int)p.y, (W::Window*)windowID }
					  );
}
-(void)rightMouseUp:(NSEvent *)nsev {
	NSPoint p = [nsev locationInWindow];
	[self convertNSWindowToWCoords:&p];
	addMouseEventIfOn(
					  W::EventType::RMouseUp,
					  (W::Event::MouseEvent) { (int)p.x, (int)p.y }
					  );
}
-(void)keyDown:(NSEvent *)nsev {
	addKeyEventIfOn(
					W::EventType::KeyDown,
					{ W::Event::charToKeycode([[nsev characters] characterAtIndex:0]) }
					);
}
-(void)keyUp:(NSEvent *)nsev {
	addKeyEventIfOn(
					W::EventType::KeyUp,
					{ W::Event::charToKeycode([[nsev characters] characterAtIndex:0]) }
					);
}
-(void)scrollWheel:(NSEvent *)nsev {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::ScrollWheel);
	ev.scrollEvent = { (float)nsev.deltaX, (float)nsev.deltaY, (W::Window*)windowID };
	W::Event::newEvents.push_back(ev);
}

-(BOOL)acceptsFirstMouse:(NSEvent *)ev {
	return clickThrough;
}

@end
