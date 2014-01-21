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
	virtual void makeCurrentContext() = 0;
	virtual void clearCurrentContext() = 0;
	
	virtual void flushBuffer() = 0;
	
	virtual void setTitle(const char *) = 0;
	virtual void getSize(int *w, int *h) = 0;
	virtual void bringToFront() = 0;
	virtual void makeFirstResp() = 0;
	
	virtual void goFullscreenOnCurScreen() = 0;
	virtual void goFullscreenOn(int screen) = 0;
	virtual void goWindowed() = 0;
};

WInt_WindowAbstr* make_WInt_Window(
								   int w, int h,
								   const char *t,
								   int posx, int posy,
								   WInt_WindowAbstr *share,
								   bool fullscreen,
								   int screen,
								   void *windowID
								  );
// If position is -ve, window will be centred
// 'screen' currently ignored

#endif
