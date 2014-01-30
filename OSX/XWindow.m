/*
 * W - simple cross-platform OpenGL windows
 *
 * =============
 *  XWindow.m
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#import "XWindow.h"
#import "XView.h"
#import "XWindowDelegate.h"
#include "Event.h"


#define STYLEMASK_NORMAL     NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
#define STYLEMASK_BORDERLESS NSBorderlessWindowMask

@interface XWindow () {
	bool currentlyFullscreen;
	void *windowID;
	int indexOfScreenForFullscreen;
}
@property (strong, nonatomic) XView *view;
@property (strong, nonatomic) XWindowDelegate *xWinDelegate;
@property (strong, nonatomic) NSString *cachedTitle;
@property NSRect prevFrame;

-(void)restoreWindowPositionFromPrev;

@end

#pragma message("Check if window is off bottom right of screen when positioning?")


@implementation XWindow

@synthesize context;
@synthesize view;
@synthesize cachedTitle;
@synthesize pos, size;
@synthesize xWinDelegate;
@synthesize prevFrame;

void getWindowPositionForScreen(int screenInd, int *px, int *py, int winH) {
	NSArray *screens = [NSScreen screens];
	
	if (screenInd >= screens.count)
		screenInd = 0;
	
	NSScreen *nsscreen = [screens objectAtIndex:screenInd];
	*px += nsscreen.frame.origin.x;
	
	// The window position is calculated relative to the bottom left of
	// all screens
	*py = nsscreen.frame.origin.y + nsscreen.frame.size.height - *py - winH;
}

-(instancetype)initWithWidth:(int)w height:(int)h sharedCtx:(XWindow *)shareWin posX:(int)px posY:(int)py title:(NSString*)t fullscreen:(BOOL)f screen:(int)screenInd windowID:(void *)winID {
	NSRect frame;
	if (f)
		frame = [[NSScreen mainScreen] frame];
	else
		frame = NSMakeRect(0, 0, w, h);
	if (self = [super initWithContentRect:frame
								styleMask:STYLEMASK_NORMAL
								  backing:NSBackingStoreBuffered
									defer:NO]) {
		
		// Set up window
		windowID = winID;
		self.cachedTitle = t;
		
		currentlyFullscreen = !f;
		if (f) {
			prevFrame.origin = NSMakePoint(-1, -1);
			prevFrame.size   = NSMakeSize(w, h);
			[self goFullscreenOn:screenInd savePrevFrame:NO];
		}
		else {
			prevFrame.size = NSMakeSize(w, h);

			// Correct window origin for screen
			NSArray *screens = [NSScreen screens];
			if (screenInd >= screens.count) screenInd = 0;
//			NSScreen *nsscr = [screens objectAtIndex:screenInd];
			
			if (px > 0 && py > 0)
				getWindowPositionForScreen(screenInd, &px, &py, h);
			
			prevFrame.origin = NSMakePoint(px, py);
			[self goWindowed];
		}
		currentlyFullscreen = f;
		
		self.size = self.frame.size;
		
		// Create PF
		NSOpenGLPixelFormat *pf;
		NSOpenGLPixelFormatAttribute attrs[] = {
			NSOpenGLPFADoubleBuffer,
			NSOpenGLPFADepthSize, 24,
			NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
			0
		};
		if ((pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs]) == nil) {
			NSLog(@"Window: Error creating NSOpenGLPixelFormat");
			return nil;
		}
		
		// Create OGL context (shared if applicable)
		self.context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:shareWin.context];
		if (!self.context) {
			NSLog(@"Window: Error creating NSOpenGLContext");
			return nil;
		}
		
		// Create view
		self.view = [[XView alloc] initWithFrame:frame clickThrough:NO windowID:winID];
		[self setContentView:self.view];
		
		// Set context destination view
		[self.context setView:self.view];
		
		// Set title
		[self setTitle:t];
		
		// Create & set window delegate
		//  - responds to windowShouldClose, miniaturization, becoming key notifications
		//    & sends W events based on those
		self.xWinDelegate = [[XWindowDelegate alloc] initWithWindowID:winID];
		[self setDelegate:self.xWinDelegate];
		
		// Subscribe to view frame changes
		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(frameChanged:)
													 name:NSViewGlobalFrameDidChangeNotification
												   object:self.view];
		
		[self makeKeyAndOrderFront:self];
	}
	
	return self;
}

-(void)bringToFront {
	[self makeKeyAndOrderFront:NSApp];
}
-(void)makeFirstResponder {
	[self makeFirstResponder:self.view];
}


#pragma mark - Fullscreen/windowed switching

-(void)goFullscreenOn:(int)screenInd savePrevFrame:(BOOL)saveFrame {
	if (screenInd == -1) {
		// Go fullscreen on current screen
		if (currentlyFullscreen)
			return;
		NSScreen *nsscr = [self screen];
		if (saveFrame)
			self.prevFrame = self.frame;
		
		[self setFullscreenStyle];
		
		NSRect scrFrame = nsscr.frame;
		[self setFrame:scrFrame display:YES];
	}
	else {
		// Go fullscreen on specified screen
		NSArray *screens = [NSScreen screens];
		if (screenInd >= screens.count)
			screenInd = 0;
		int curScreenInd;
		for (curScreenInd = 0; curScreenInd < screens.count; ++curScreenInd)
			if (screens[curScreenInd] == [self screen])
				break;
		
		if (currentlyFullscreen && curScreenInd == screenInd)
			return;
		
		if (saveFrame)
			self.prevFrame = self.frame;

		[self setFullscreenStyle];
		NSScreen *destScreen = screens[screenInd];
		[self setFrame:destScreen.frame display:YES];
	}
	currentlyFullscreen = true;
}
-(void)goWindowed {
	if (!currentlyFullscreen)
		return;
	[self setWindowedStyle];
	[self restoreWindowPositionFromPrev];
	currentlyFullscreen = false;
}

// Set window style
-(void)setFullscreenStyle {
	[self setStyleMask:STYLEMASK_BORDERLESS];
	[self setLevel:(NSMainMenuWindowLevel + 1)];
	[self setOpaque:YES];
	[self setHidesOnDeactivate:YES];
}
-(void)setWindowedStyle {
	[self setStyleMask:STYLEMASK_NORMAL];
	[self setOpaque:NO];
	[self setTitle:cachedTitle];
	[self setHidesOnDeactivate:NO];
}

// Restore position & size
-(void)restoreWindowPositionFromPrev {
	if (self.prevFrame.origin.x < 0 || self.prevFrame.origin.y < 0) {
		[self centerOnCurScreen];
	}
	else {
		[self setFrame:prevFrame display:YES];
	}
}

-(void)centerOnCurScreen {
	NSRect scrFrame = [self screen].frame;
	NSSize winSize = self.prevFrame.size;
	
	int px = 0.5 * (scrFrame.size.width - winSize.width) + scrFrame.origin.x;
	int py = 0.5 * (scrFrame.size.height - winSize.height) + scrFrame.origin.y;
	
	NSRect newFrame = NSMakeRect(px, py, winSize.width, winSize.height);
	[self setFrame:newFrame display:YES];
}


#pragma mark - Frame change

-(void)frameChanged:(id)ntfn {
	[self.context update];
	
	// Resize the backing buffer
	GLint _size[] = { (GLint)view.bounds.size.width, (GLint)view.bounds.size.height };
	CGLContextObj ctx = (CGLContextObj) [context CGLContextObj];
	CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, _size);
	CGLEnable(ctx, kCGLCESurfaceBackingSize);
	
	// Update size property
	self.size = self.frame.size;
	
	// Send resize event
	if (W::Event::on) {
		W::Event ev(W::EventType::WinResized);
		ev.winEvent.window = (W::Window*)windowID;
		W::Event::newEvents.push_back(ev);
	}
}


@end
