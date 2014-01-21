/*
 * W - simple cross-platform OpenGL windows
 *
 * =====================
 *  XWindowDelegate.m
 * =====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import "XWindowDelegate.h"
#include "Event.h"

@interface XWindowDelegate () {
	void *windowID;
}
@end


@implementation XWindowDelegate

-(instancetype) initWithWindowID:(void *)winID {
	if (self = [super init])
		windowID = winID;
	return self;
}

-(BOOL)windowShouldClose:(id)sender {
	if (W::Event::on) {
		W::Event ev(W::EventType::WinClosed);
		ev.winEvent.window = (W::Window*)windowID;
		W::Event::newEvents.push_back(ev);
	}
	return NO;
}

-(void)windowDidMiniaturize:(NSNotification*)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinMinimized);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::newEvents.push_back(ev);
}
-(void)windowDidDeminiaturize:(NSNotification *)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinDeminimized);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::newEvents.push_back(ev);
}

-(void)windowDidBecomeKey:(NSNotification *)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinBecameKey);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::newEvents.push_back(ev);
}
-(void)windowDidResignKey:(NSNotification *)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinStoppedBeingKey);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::newEvents.push_back(ev);
}

@end
