#include "Window.h"
#import "XWindow.h"
#include "Event.h"

struct W::Window::PlatformSpecificState {
	XWindow *xwindow;
};

std::vector<W::Window*> W::Window::current_windows;

int msToInt(W::Multisampling::T ms) {
	using namespace W::Multisampling;
	return
		ms == None ? 0 :
		ms == X2 ? 2 :
		ms == X4 ? 4 : 8;
}


W::Window::Window(int width,
									int height,
									const char *title,
									W::Window *share,
									bool fullscreen,
									int screen,
									W::Multisampling::T multisampling)
{
	objs = new PlatformSpecificState;
	objs->xwindow = [[XWindow alloc] initWithWidth:width
																					height:height
																			 sharedCtx:(share ? share->objs->xwindow : nil)
																					 title:[NSString stringWithUTF8String:title]
																			fullscreen:fullscreen
																					screen:screen
																				windowID:this
															multisamplingLevel:msToInt(multisampling)];

	register_window(this);
}

W::Window::~Window()
{
	deregister_window(this);
	[objs->xwindow close];
	delete objs;
}


#pragma mark - W::Window methods

// OGL context
void W::Window::makeCurrentContext() {
	[objs->xwindow makeCurrentContext];
}
void W::Window::clearCurrentContext() {
	[objs->xwindow clearCurrentContext];
}
void W::Window::flushBuffer() {
	[objs->xwindow flushBuffer];
}

// W::Window attributes
void W::Window::setTitle(const char *t) {
	[objs->xwindow setTitle:t];
}

// Respondery things
void W::Window::bringToFront() {
	[objs->xwindow bringToFront];
}
void W::Window::makeFirstResp() {
	[objs->xwindow makeFirstResponder];
}

// Size & position
void W::Window::getSize(int *w, int *h) {
	[objs->xwindow getSizeW:w H:h];
}
void W::Window::setSize(int w, int h) {
	[objs->xwindow setSizeW:w H:h];
}
void W::Window::getPos(int *x, int *y) {
	[objs->xwindow getPosX:x Y:y];
}
void W::Window::setPos(int x, int y) {
	[objs->xwindow setPosX:x Y:y];
}
void W::Window::setScreen(int screenInd) {
	[objs->xwindow setScreen:screenInd];
}
int W::Window::getScreen() {
	return [objs->xwindow getScreen];
}
void W::Window::goFullscreen() {
	[objs->xwindow goFullscreen];
}
void W::Window::goWindowed() {
	[objs->xwindow goWindowed];
}
bool W::Window::isInFullscreenMode() {
	return [objs->xwindow isFullscreen];
}

// Mouse
bool W::Window::mouseIsOver() {
	return [objs->xwindow mouseIsOver];
}
void W::Window::getMousePosition(int *x, int *y) {
	[objs->xwindow getMousePositionX:x Y:y];
}
void W::Window::setMousePosition(int x, int y) {
	[objs->xwindow setMousePositionX:x Y:y];
}


#pragma mark - Init: subscribe to active application changes

struct W::Window::Init {
	Init() {
		void (^activateBlock)(NSNotification*) = ^void (NSNotification *note) {
			if (!W::Event::on) {
				return;
			}
			NSRunningApplication *app = [note.userInfo objectForKey:NSWorkspaceApplicationKey];
			if (app.processIdentifier == [NSRunningApplication currentApplication].processIdentifier) {
				W::Event ev(W::EventType::AppBecameForeground);
				W::Event::addNewEvent(ev);
			}
		};

		void (^deactivateBlock)(NSNotification*) = ^void (NSNotification *note) {
			if (!W::Event::on) return;
			NSRunningApplication *app = [note.userInfo objectForKey:NSWorkspaceApplicationKey];
			if (app.processIdentifier == [NSRunningApplication currentApplication].processIdentifier) {
				W::Event ev(W::EventType::AppBecameBackground);
				W::Event::addNewEvent(ev);
			}
		};

		[[[NSWorkspace sharedWorkspace] notificationCenter] addObserverForName:NSWorkspaceDidActivateApplicationNotification
																																		object:nil
																																		 queue:nil
																																usingBlock:activateBlock];

		[[[NSWorkspace sharedWorkspace] notificationCenter] addObserverForName:NSWorkspaceDidDeactivateApplicationNotification
																																		object:nil
																																		 queue:nil
																																usingBlock:deactivateBlock];
		
	}
};
W::Window::Init *W::Window::init = new W::Window::Init;

