/*
 * W - simple cross-platform OpenGL windows
 *
 * ====================
 *  WInt_WindowOSX.h
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __OGLMultiWindowTest__WInt_WindowOSX__
#define __OGLMultiWindowTest__WInt_WindowOSX__

#include "WInt_WindowAbstr.h"

class WInt_WindowOSX : public WInt_WindowAbstr {
public:
	WInt_WindowOSX(
				   int w, int h,
				   const char *t,
				   int posx, int posy,
				   WInt_WindowOSX *share,
				   bool fullscreen,
				   int screen,
				   void *windowID
				   );
	~WInt_WindowOSX();
	
	void makeCurrentContext();
	void clearCurrentContext();
	
	void flushBuffer();
	
	void setTitle(const char *);
	void getSize(int *w, int *h);
	void bringToFront();
	void makeFirstResp();
	
	void goFullscreenOnCurScreen();
	void goFullscreenOn(int screen);
	void goWindowed();
	
private:
	struct NativeObjs;
	NativeObjs *objs;
	
	struct Init;
	static Init *init;
};

#endif
