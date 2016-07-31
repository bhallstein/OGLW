/*
 * W - simple cross-platform OpenGL windows
 *
 * ======================
 *  WInt_WindowOSX.cpp
 * ======================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "WInt_WindowOSX.h"
#import "XWindow.h"
#include "Event.h"

struct WInt_WindowOSX::NativeObjs {
	XWindow *xwindow;
};

int msToInt(W::Multisampling::T ms) {
	using namespace W::Multisampling;
	return
		ms == None ? 0 :
		ms == X2 ? 2 :
		ms == X4 ? 4 : 8;
}

WInt_WindowOSX::WInt_WindowOSX(int w, int h,
							   const char *t,
							   WInt_WindowOSX *share,
							   bool fullscreen,
							   int screen,
							   W::Multisampling::T multisampling,
							   void *winID)
{
	objs = new NativeObjs;
	objs->xwindow = [[XWindow alloc] initWithWidth:w
											height:h
										 sharedCtx:(share ? share->objs->xwindow : nil)
											 title:[NSString stringWithUTF8String:t]
										fullscreen:fullscreen
											screen:screen
										  windowID:winID
								multisamplingLevel:msToInt(multisampling)];
}

WInt_WindowOSX::~WInt_WindowOSX()
{
	delete objs;
}


#pragma mark - WInt_WindowAbstr methods

// OGL context
void WInt_WindowOSX::makeCurrentContext() {
	[objs->xwindow makeCurrentContext];
}
void WInt_WindowOSX::clearCurrentContext() {
	[objs->xwindow clearCurrentContext];
}
void WInt_WindowOSX::flushBuffer() {
	[objs->xwindow flushBuffer];
}

// Window attributes
void WInt_WindowOSX::setTitle(const char *t) {
	[objs->xwindow setTitle:t];
}

// Respondery things
void WInt_WindowOSX::bringToFront() {
	[objs->xwindow bringToFront];
}
void WInt_WindowOSX::makeFirstResp() {
	[objs->xwindow makeFirstResponder];
}

// Size & position
void WInt_WindowOSX::getSize(int *w, int *h) {
	[objs->xwindow getSizeW:w H:h];
}
void WInt_WindowOSX::setSize(int w, int h) {
	[objs->xwindow setSizeW:w H:h];
}
void WInt_WindowOSX::getPos(int *x, int *y) {
	[objs->xwindow getPosX:x Y:y];
}
void WInt_WindowOSX::setPos(int x, int y) {
	[objs->xwindow setPosX:x Y:y];
}
void WInt_WindowOSX::setScreen(int screenInd) {
	[objs->xwindow setScreen:screenInd];
}
int WInt_WindowOSX::getScreen() {
	return [objs->xwindow getScreen];
}
void WInt_WindowOSX::goFullscreen() {
	[objs->xwindow goFullscreen];
}
void WInt_WindowOSX::goWindowed() {
	[objs->xwindow goWindowed];
}
bool WInt_WindowOSX::isInFullscreenMode() {
	return [objs->xwindow isFullscreen];
}

// Mouse
bool WInt_WindowOSX::mouseIsOver() {
	return [objs->xwindow mouseIsOver];
}
void WInt_WindowOSX::getMousePosition(int *x, int *y) {
	[objs->xwindow getMousePositionX:x Y:y];
}
void WInt_WindowOSX::setMousePosition(int x, int y) {
	[objs->xwindow setMousePositionX:x Y:y];
}


#pragma mark - Init: subscr. to active application changes

struct WInt_WindowOSX::Init {
	Init() {
		void (^activateBlock)(NSNotification*) = ^void (NSNotification *note) {
			if (!W::Event::on) return;
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
WInt_WindowOSX::Init *WInt_WindowOSX::init = new WInt_WindowOSX::Init;

