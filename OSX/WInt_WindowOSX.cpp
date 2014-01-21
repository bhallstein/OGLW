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

WInt_WindowOSX::WInt_WindowOSX(
							   int w, int h,
							   const char *t,
							   int posx, int posy,
							   WInt_WindowOSX *share,
							   bool fullscreen,
							   int screen,
							   void *winID
							   )
{
	objs = new NativeObjs;
	objs->xwindow = [[XWindow alloc] initWithWidth:w
											height:h
										 sharedCtx:(share ? share->objs->xwindow : nil)
											  posX:posx
											  posY:posy
											 title:[NSString stringWithUTF8String:t]
										fullscreen:fullscreen
											screen:screen
										  windowID:winID];
}

WInt_WindowOSX::~WInt_WindowOSX()
{
	delete objs;
}


#pragma mark - WInt_WindowAbstr methods

void WInt_WindowOSX::makeCurrentContext() {
	[objs->xwindow.context makeCurrentContext];
}

void WInt_WindowOSX::clearCurrentContext() {
	[NSOpenGLContext clearCurrentContext];
}

void WInt_WindowOSX::flushBuffer() {
	[objs->xwindow.context flushBuffer];
}

void WInt_WindowOSX::setTitle(const char *t) {
	[objs->xwindow setTitle:[NSString stringWithUTF8String:t]];
}

void WInt_WindowOSX::getSize(int *w, int *h) {
	*w = objs->xwindow.size.width;
	*h = objs->xwindow.size.height;
}

void WInt_WindowOSX::bringToFront() {
	[objs->xwindow bringToFront];
}

void WInt_WindowOSX::makeFirstResp() {
	[objs->xwindow makeFirstResponder];
}

void WInt_WindowOSX::goFullscreenOnCurScreen() {
	[objs->xwindow goFullscreenOn:-1 savePrevFrame:YES];
}
void WInt_WindowOSX::goFullscreenOn(int screen) {
	[objs->xwindow goFullscreenOn:screen savePrevFrame:YES];
}
void WInt_WindowOSX::goWindowed() {
	[objs->xwindow goWindowed];
}


#pragma mark - Subscr to active application changes

struct WInt_WindowOSX::Init {
	Init() {
		void (^activateBlock)(NSNotification*) = ^void (NSNotification *note) {
			if (!W::Event::on) return;
			NSRunningApplication *app = [note.userInfo objectForKey:NSWorkspaceApplicationKey];
			if (app.processIdentifier == [NSRunningApplication currentApplication].processIdentifier) {
				W::Event ev(W::EventType::AppBecameForeground);
				W::Event::newEvents.push_back(ev);
			}
		};
		void (^deactivateBlock)(NSNotification*) = ^void (NSNotification *note) {
			if (!W::Event::on) return;
			NSRunningApplication *app = [note.userInfo objectForKey:NSWorkspaceApplicationKey];
			if (app.processIdentifier == [NSRunningApplication currentApplication].processIdentifier) {
				W::Event ev(W::EventType::AppBecameBackground);
				W::Event::newEvents.push_back(ev);
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

