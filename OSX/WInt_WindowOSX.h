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

#ifndef WInt_WindowOSX_h
#define WInt_WindowOSX_h

#include "WInt_WindowAbstr.h"

class WInt_WindowOSX : public WInt_WindowAbstr {
public:
	WInt_WindowOSX(
				   int w, int h,
				   const char *t,
				   WInt_WindowOSX *share,
				   bool fullscreen,
				   int screen,
				   void *windowID
				   );
	~WInt_WindowOSX();
	
	// OGL Context
	void makeCurrentContext();
	void clearCurrentContext();
	void flushBuffer();
	
	// Window attributes
	void setTitle(const char *);
	
	// Respondery things
	void bringToFront();
	void makeFirstResp();
	
	// Size & position
	void getSize(int *w, int *h);
	void setSize(int w, int h);
	
	void getPos(int *x, int *y);
	void setPos(int x, int y);
	
	int getScreen();
	void setScreen(int screenInd);
	
	void goFullscreen();
	void goWindowed();
	bool isInFullscreenMode();
	
	// Mouse
	bool mouseIsOver();
	void getMousePosition(int *x, int *y);
	void setMousePosition(int x, int y);
	
private:
	struct NativeObjs;
	NativeObjs *objs;
	
	struct Init;
	static Init *init;
};

#endif
