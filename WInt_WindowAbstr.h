/*
 * W - simple cross-platform OpenGL windows
 *
 * ======================
 *  WInt_WindowAbstr.h
 * ======================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// WInt_WindowAbstr - abstract superclass defining behaviour
// of internal window.
// Has concrete subclasses implementing windows and mac: get with factory
// method: make_WInt_Window()

#ifndef __OGLMultiWindowTest__WInt_WindowAbstr__
#define __OGLMultiWindowTest__WInt_WindowAbstr__

class WInt_WindowAbstr {
public:
	virtual ~WInt_WindowAbstr() { }
	
	// Context
	virtual void makeCurrentContext() = 0;
	virtual void clearCurrentContext() = 0;
	virtual void flushBuffer() = 0;
	
	// Win attribs
	virtual void setTitle(const char *) = 0;
	
	// Respondery
	virtual void bringToFront() = 0;
	virtual void makeFirstResp() = 0;
	
	// Size & pos
	virtual void getSize(int *w, int *h) = 0;
	virtual void setSize(int w, int h) = 0;
	
	virtual void getPos(int *x, int *y) = 0;
	virtual void setPos(int x, int y) = 0;
	
	virtual int getScreen() = 0;
	virtual void setScreen(int screen) = 0;
	
	virtual void goFullscreen() = 0;
	virtual void goWindowed() = 0;
	virtual bool isInFullscreenMode() = 0;
	
	// Mouse
	virtual bool mouseIsOver() = 0;
	virtual void getMousePosition(int *x, int *y) = 0;
	virtual void setMousePosition(int x, int y) = 0;
	
	// Events
	virtual void getEvents() { }	// Def. impl does nothing; overridden on windows
};

WInt_WindowAbstr* make_WInt_Window(
								   int w,
								   int h,
								   const char *t,
								   WInt_WindowAbstr *share,
								   bool fullscreen,
								   int screen,
								   void *windowID	// Pointer to the W::Window, for OS X event passing, & ...?
								  );
// If position is -ve, window will be centred
// 'screen' currently ignored

#endif
