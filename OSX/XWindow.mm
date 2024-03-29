#import "XWindow.h"
#import "XView.h"
#import "XWindowDelegate.h"

#include "Event.h"
#include <vector>

#define STYLEMASK_NORMAL     NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask
#define STYLEMASK_BORDERLESS NSBorderlessWindowMask

#pragma mark - XWindow privates

@interface XWindow () {
	void *windowID;
	bool currentlyFullscreen;
	bool windowPositionConsideredSpecified;
		// When the window has been in windowed mode, it may have been
		// positioned explicitly by the user.
		// Therefore a window's position is considered "specified" when
		// the window has been in windowed mode.
	NSRect savedFrame;
		// The frame rect last used (or not yet used) in windowed mode.
	int screenIndex;
}
@property (strong, nonatomic) NSWindow *nswin;
@property (strong, nonatomic) XView *view;
@property (strong, nonatomic) XWindowDelegate *xWinDelegate;
@property (strong, nonatomic) NSOpenGLContext *context;
@property (strong, nonatomic) NSString *cachedTitle;
@end


#pragma mark - Helpers

void getWOriginForNativeWindowFrameRectOnScreen(int *x, int *y, NSRect nativeFrameRct, NSScreen *scr) {
	*x = nativeFrameRct.origin.x - scr.frame.origin.x;
	*y = scr.frame.size.height + scr.frame.origin.y - nativeFrameRct.size.height - nativeFrameRct.origin.y;
}
void getNativeWindowOriginForWOriginOnScreen(int *x, int *y, int wx, int wy, int winHeight, NSScreen *scr) {
	*x = scr.frame.origin.x + wx;
	*y = scr.frame.size.height + scr.frame.origin.y - wy - winHeight;
}
// NSWindow -center occasionally fails to work as expected. Workaround:
NSRect centredRectForFrameRectOnScreen(NSRect rct, NSScreen *nsscreen) {
	NSRect scrFrame = nsscreen.frame;
	
	int px = scrFrame.origin.x + 0.5*(scrFrame.size.width - rct.size.width);
	int py = scrFrame.origin.y + 0.5*(scrFrame.size.height - rct.size.height);
	
	return NSMakeRect(px, py, rct.size.width, rct.size.height);
}


#pragma mark - XWindow implementation

@implementation XWindow

@synthesize nswin = _nswin;
@synthesize view = _view;
@synthesize context = _context;
@synthesize xWinDelegate = _xWinDelegate;
@synthesize cachedTitle = _cachedTitle;

-(instancetype)initWithWidth:(int)w height:(int)h
				   sharedCtx:(XWindow*)_sharedCtx
					   title:(NSString*)_title
				  fullscreen:(BOOL)_fullscr
					  screen:(int)_scrInd
					windowID:(void*)_winID
		  multisamplingLevel:(int)msLevel {
	if (self = [super init]) {
		// Get screen, ensuring screen index is in bounds
		NSArray *screens = [NSScreen screens];
		if (_scrInd >= screens.count || _scrInd < 0) _scrInd = 0;
		screenIndex = _scrInd;
		NSScreen *nsscreen = screens[screenIndex];
		
		// Get style mask for window creation
		NSUInteger styleMask = _fullscr ? STYLEMASK_BORDERLESS : STYLEMASK_NORMAL;
		
		// Get frame
		NSRect frame;
		if (_fullscr) frame = nsscreen.frame;
		else          frame = [NSWindow frameRectForContentRect:NSMakeRect(0, 0, w, h)
													  styleMask:styleMask];
		
		// Save param properties
		windowID = _winID;
		currentlyFullscreen = _fullscr;
		self.cachedTitle = _title;
		
		// Create NSWindow
		if (currentlyFullscreen) {
			// In fullscreen mode, we save the w/h params to be used if we ever enter
			// windowed mode
			savedFrame = NSMakeRect(0, 0, w, h);
			
			// We also consider the window's position unspecified
			windowPositionConsideredSpecified = NO;
			
			// Init with a content rect of the supplied screen
			self.nswin = [[NSWindow alloc] initWithContentRect:nsscreen.frame
												styleMask:styleMask
												  backing:NSBackingStoreBuffered
													defer:NO];
			[self setToFullscreenStyle];
		}
		else {
			// In windowed mode, we center the window on the supplied screen, and consider
			// its position to be specified.
			windowPositionConsideredSpecified = YES;
			NSRect contentRect = NSMakeRect(0, 0, w, h);
			NSRect frameRect = [NSWindow frameRectForContentRect:contentRect
													   styleMask:styleMask];
			frameRect = centredRectForFrameRectOnScreen(frameRect, nsscreen);
			contentRect = [NSWindow contentRectForFrameRect:frameRect
												  styleMask:styleMask];
			self.nswin = [[NSWindow alloc] initWithContentRect:NSMakeRect(100, 100, 400, 200)
												styleMask:styleMask
												  backing:NSBackingStoreBuffered
													defer:NO];
			[self.nswin setReleasedWhenClosed:NO];
			[self.nswin setFrame:frameRect display:YES];
		}
		
		// Set up OGL context, view, window delegate, etc.
		{
			// Create PF
			NSOpenGLPixelFormat *pf;
			std::vector<NSOpenGLPixelFormatAttribute> attrs;
			if (msLevel > 0)
				attrs = {
					NSOpenGLPFADoubleBuffer,
					NSOpenGLPFADepthSize, 24,
					/*** enable multisampling ***/
					NSOpenGLPFAMultisample,
					NSOpenGLPFASampleBuffers, (NSOpenGLPixelFormatAttribute)1,
					NSOpenGLPFASamples, (NSOpenGLPixelFormatAttribute)msLevel,
					/****************************/
					NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
					0
				};
			else
				attrs = {
					NSOpenGLPFADoubleBuffer,
					NSOpenGLPFADepthSize, 24,
					NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
					0
				};
			
			if ((pf = [[NSOpenGLPixelFormat alloc] initWithAttributes:&attrs[0]]) == nil) {
				NSLog(@"Window: Error creating NSOpenGLPixelFormat");
				return nil;
			}
			
			// Create OGL context (shared if applicable)
			self.context = [[NSOpenGLContext alloc] initWithFormat:pf shareContext:_sharedCtx.context];
			if (!self.context) {
				NSLog(@"Window: Error creating NSOpenGLContext");
				return nil;
			}
			
			// Create view, add to NSWindow, set as context’s destination
			self.view = [[XView alloc] initWithFrame:frame clickThrough:NO windowID:windowID];
			[self.nswin setContentView:self.view];
			[self.context setView:self.view];
			
//			GLint swapInt = 1;
//			[self.openGLContext setValues:&swapInt forParameter:NSOpenGLCPSwapInterval];
// to enable vsync
			
			// Set title
			[self.nswin setTitle:_title];
			
			// Create & set window delegate
			//  - responds to windowShouldClose, miniaturization, becoming key notifications
			//    & sends W events based on those
			self.xWinDelegate = [[XWindowDelegate alloc] initWithWindowID:windowID];
			[self.nswin setDelegate:self.xWinDelegate];
			
			// Subscribe to view frame changes
			[[NSNotificationCenter defaultCenter] addObserver:self
													 selector:@selector(frameChanged:)
														 name:NSViewGlobalFrameDidChangeNotification
													   object:self.view];
			// Make active
			[self.nswin makeKeyAndOrderFront:self];
		}
	}
	
	return self;
}

#pragma mark - Public methods

// Context things
-(void)makeCurrentContext {
	[self.context makeCurrentContext];
}
-(void)clearCurrentContext {
	[NSOpenGLContext clearCurrentContext];
}
-(void)flushBuffer {
	[self.context flushBuffer];
}

// Windowy things
-(void)bringToFront {
	[self.nswin makeKeyAndOrderFront:NSApp];
}
-(void)makeFirstResponder {
	[self.nswin makeFirstResponder:self.view];
}
-(void)setTitle:(const char *)t {
	[self.nswin setTitle:[NSString stringWithUTF8String:t]];
}
-(void)close {
	[self.nswin close];
}

// Size & position
-(void)getSizeW:(int *)w H:(int *)h {
	NSRect rct = [self.nswin contentRectForFrameRect:self.nswin.frame];
	*w = rct.size.width;
	*h = rct.size.height;
}
-(void)setSizeW:(int)w H:(int)h {
	if (currentlyFullscreen) return;

	// Get current W-style position
	int wX, wY;
	getWOriginForNativeWindowFrameRectOnScreen(&wX, &wY, self.nswin.frame, self.nswin.screen);
	
	// Make new frame, preserving W-style position
	NSRect newContentRect = NSMakeRect(0, 0, w, h);
	NSRect newFrameRect = [self.nswin frameRectForContentRect:newContentRect];
	int nX, nY;
	getNativeWindowOriginForWOriginOnScreen(&nX, &nY, wX, wY, newFrameRect.size.height, self.nswin.screen);
	newFrameRect.origin.x = nX;
	newFrameRect.origin.y = nY;
	
	[self.nswin setFrame:newFrameRect display:YES];
  // TODO: Limit the size of the window to the size of the screen?"
  // Could also automatically readjust position?
}

-(void)getPosX:(int*)x Y:(int*)y {
	getWOriginForNativeWindowFrameRectOnScreen(x, y, self.nswin.frame, self.nswin.screen);
}
-(void)setPosX:(int)x Y:(int)y {
	int nativeX, nativeY;
	getNativeWindowOriginForWOriginOnScreen(&nativeX, &nativeY, x, y, self.nswin.frame.size.height, self.nswin.screen);
	NSRect newFrameRect = NSMakeRect(nativeX, nativeY, self.nswin.frame.size.width, self.nswin.frame.size.height);
	[self.nswin setFrame:newFrameRect display:YES];
  // TODO: Could auto-position to fit entirely on the screen?
}

-(int)getScreen {
	NSArray *screens = [NSScreen screens];
	int i = 0;
	for (i=0; i < screens.count; ++i)
		if (screens[i] == [self.nswin screen])
			break;
	return i;
}
-(void)setScreen:(int)_screenInd {
	NSArray *screens = [NSScreen screens];
	if (_screenInd >= screens.count || _screenInd < 0)
		_screenInd = 0;
	screenIndex = _screenInd;

	NSRect newFrame;
	
	if (currentlyFullscreen) {
		// In fullscreen mode, adjust the window to cover the new screen
		NSScreen *screen = screens[screenIndex];
		newFrame = screen.frame;
	}
	else {
		// In windowed mode, get native position of window on the new screen, preserving position from top left
		int wx, wy;
		getWOriginForNativeWindowFrameRectOnScreen(&wx, &wy, self.nswin.frame, self.nswin.screen);
		int nx, ny;
		getNativeWindowOriginForWOriginOnScreen(&nx, &ny, wx, wy, self.nswin.frame.size.height, screens[screenIndex]);
		newFrame = NSMakeRect(nx, ny, self.nswin.frame.size.width, self.nswin.frame.size.height);
	}
	
	[self.nswin setFrame:newFrame display:YES];
}

-(void)goFullscreen {
	if (currentlyFullscreen) return;
	
	savedFrame = self.nswin.frame;
	NSScreen *nsscreen = self.nswin.screen;
	[self setToFullscreenStyle];
	[self.nswin setFrame:nsscreen.frame display:YES];

	currentlyFullscreen = YES;
}
-(void)goWindowed {
	if (!currentlyFullscreen) return;
	
	[self setToWindowedStyle];
	NSRect newFrame;
	// If position considered specified, simply restore it.
	// If not, center it on the screen.
	if (windowPositionConsideredSpecified)
		newFrame = savedFrame;
	else
		newFrame = centredRectForFrameRectOnScreen(savedFrame, self.nswin.screen);
	[self.nswin setFrame:newFrame display:YES];
	
	currentlyFullscreen = NO;
}
-(BOOL)isFullscreen {
	return currentlyFullscreen;
}

-(BOOL)mouseIsOver {
	int mX, mY;
	[self getMousePositionX:&mX Y:&mY];
	NSRect contentRect = [self.nswin contentRectForFrameRect:self.nswin.frame];
	return mX >= 0 && mY >= 0 && mX < contentRect.size.width && mY < contentRect.size.height;
}
-(void)getMousePositionX:(int *)x Y:(int *)y {
	NSPoint p = self.nswin.mouseLocationOutsideOfEventStream;
	[self.view convertNSWindowToWCoords:&p];
	*x = p.x;
	*y = p.y;
}
-(void)setMousePositionX:(int)x Y:(int)y {
	// Convert from W to NSView coords (origin at btm left)
	int yN = self.view.bounds.size.height - y;
	
	// Get window origin in screen coordinates
	NSRect winOrigin = [self.nswin convertRectToScreen:NSMakeRect(0, 0, 0, 0)];
	int xN = x + winOrigin.origin.x;
	yN = [NSScreen mainScreen].frame.size.height - winOrigin.origin.y - yN;

	CGWarpMouseCursorPosition(CGPointMake(xN, yN));
}


#pragma mark - Private methods

-(void)frameChanged:(id)ntfn {
	[self.context update];
	
	// Resize the backing buffer
	GLint _size[] = { (GLint)self.view.bounds.size.width, (GLint)self.view.bounds.size.height };
	CGLContextObj ctx = (CGLContextObj) [self.context CGLContextObj];
	CGLSetParameter(ctx, kCGLCPSurfaceBackingSize, _size);
	CGLEnable(ctx, kCGLCESurfaceBackingSize);
	
	// Send resize event
	if (W::Event::on) {
		W::Event ev(W::EventType::WinResized);
		ev.winEvent.window = (W::Window*)windowID;
		W::Event::addNewEvent(ev);
	}
}

-(void)setToFullscreenStyle {
	[self.nswin setStyleMask:STYLEMASK_BORDERLESS];
	[self.nswin setLevel:(NSMainMenuWindowLevel + 1)];
	[self.nswin setOpaque:YES];
	[self.nswin setHidesOnDeactivate:YES];
}
-(void)setToWindowedStyle {
	[self.nswin setStyleMask:STYLEMASK_NORMAL];
	[self.nswin setOpaque:NO];
	[self.nswin setTitle:self.cachedTitle];
	[self.nswin setHidesOnDeactivate:NO];
}

@end
