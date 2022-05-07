#import "XAppDelegate.h"
#import <OpenGL/gl.h>
#include "W.h"

// What to do when events are disabled and window is closed?

@interface XAppDelegate () {
	W::Window *window1;
	W::Window *window2;
}
@property (strong, nonatomic) NSTimer *timer;
@end


@implementation XAppDelegate

@synthesize timer;

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
//	v1NeedsViewportUpdate = YES;
//	v2NeedsViewportUpdate = YES;
	
	// Create windows
	window1 = new W::Window(400, 300, "A window", 0, false, 0);
	window2 = new W::Window(400, 300, "Another window", window1, false, 1);
	
	printf("window1 is on screen: %d\n", window1->getScreen());
	printf("window2 is on screen: %d\n", window2->getScreen());
	
	// Install timer
	self.timer = [NSTimer scheduledTimerWithTimeInterval:1./40.
												  target:self
												selector:@selector(timerCB:)
												userInfo:nil
												 repeats:YES];

//	int nScreens = W::numberOfScreens();
//	printf("Screens: %d\n", W::numberOfScreens());
//	for (int i=0; i < nScreens; ++i) {
//		int sX, sY, sW, sH;
//		W::getScreenPosition(i, &sX, &sY);
//		W::getScreenSize(i, &sW, &sH);
//		printf("  screen %d: %d,%d %dx%d\n", i, sX, sY, sW, sH);
//	}
	
	// Set up initial opengl state
	window1->makeCurrentContext();
	window1->clearCurrentContext();
	
	W::Event::on = true;
}


#pragma mark - Timer callback

- (void)timerCB:(id)n {
	auto new_events = W::Event::getNewEvents();

	for (auto ev : new_events) {
		if (ev.type == W::EventType::LMouseDown) {
			if (ev.mouseEvent.window == window1) {
				printf("left mouse down in window 1\n");
				if (window1->isInFullscreenMode()) {
          window1->goWindowed();
          printf("  switching to windowed\n");
				}
				else {
          window1->goFullscreen();
          printf("  switching to fullscreen\n");
				}
			}
			else if (ev.mouseEvent.window == window2) {
				printf("left mouse down in window 2: ");
				if (window2->isInFullscreenMode()) {
          window2->goWindowed();
          printf("  switching to windowed\n");
				}
				else {
          window2->goFullscreen();
          printf("  switching to fullscreen\n");
				}
			}
		}
		else if (ev.type == W::EventType::WinResized) {
			printf("window resized: %p\n", ev.winEvent.window);
		}
		else if (ev.type == W::EventType::WinMinimized) {
			printf("window minimized: %p\n", ev.winEvent.window);
		}
	}
	
	int wW, wH;
	{
		window1->makeCurrentContext();
		window1->getSize(&wW, &wH);
		glViewport(0, 0, wW, wH);
		glClearColor(1.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		window1->flushBuffer();
		window1->clearCurrentContext();
	}
	{
		window2->makeCurrentContext();
		window2->getSize(&wW, &wH);
		glViewport(0, 0, wW, wH);
		glClearColor(0.0, 1.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		window2->flushBuffer();
		window2->clearCurrentContext();
	}
	
}

@end

