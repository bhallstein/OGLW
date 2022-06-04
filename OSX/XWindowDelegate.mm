#import "XWindowDelegate.h"
#include "Event.h"

@interface XWindowDelegate () {
	void *windowID;
}
@end


@implementation XWindowDelegate

-(instancetype) initWithWindowID:(void *)winID {
  if (self = [super init]) {
		windowID = winID;
  }
	return self;
}

-(BOOL)windowShouldClose:(id)sender {
	if (W::Event::on) {
		W::Event ev(W::EventType::WinClosed);
		ev.winEvent.window = (W::Window*)windowID;
		W::Event::addNewEvent(ev);
	}
	return NO;
}

-(void)windowDidMiniaturize:(NSNotification*)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinMinimized);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::addNewEvent(ev);
}
-(void)windowDidDeminiaturize:(NSNotification *)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinDeminimized);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::addNewEvent(ev);
}

-(void)windowDidBecomeKey:(NSNotification *)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinBecameKey);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::addNewEvent(ev);
}
-(void)windowDidResignKey:(NSNotification *)n {
	if (!W::Event::on) return;
	
	W::Event ev(W::EventType::WinStoppedBeingKey);
	ev.winEvent.window = (W::Window*)windowID;
	W::Event::addNewEvent(ev);
}

@end
