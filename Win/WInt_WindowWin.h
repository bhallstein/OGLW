/*
 * W - simple cross-platform OpenGL windows
 *
 * ====================
 *  WInt_WindowWin.h
 * ====================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "WInt_WindowAbstr.h"

struct GLFWwindow;

class WInt_WindowWin : public WInt_WindowAbstr {
public:
	WInt_WindowWin(
		int width,
		int height,
		const char *title,
		int posx,
		int posy,
		WInt_WindowWin *share,
		bool fullscreen,
		int screen,
		void *windowID
	);
	~WInt_WindowWin();
	
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
	
	void getEvents();

private:
	struct NativeObjs;
	NativeObjs *objs;

	struct Init;
	static Init *init;

	friend void cb_WindowFocus(GLFWwindow*, int);
	friend void cb_WindowMinimise(GLFWwindow*, int);
	friend void cb_WindowResize(GLFWwindow*, int, int);
	friend void cb_WindowClose(GLFWwindow*);

	friend void cb_MouseButton(GLFWwindow*, int, int, int);
	friend void cb_ScrollWheel(GLFWwindow*, double, double);

};
