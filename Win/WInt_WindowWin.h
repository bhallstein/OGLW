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
		WInt_WindowWin *share,
		bool fullscreen,
		int screen,
		void *windowID
	);
	~WInt_WindowWin();
	
	// OGL context
	void makeCurrentContext();
	void clearCurrentContext();
	void flushBuffer();

	// Win attribs
	void setTitle(const char *);

	// Respondery
	void bringToFront();
	void makeFirstResp();

	// Size & pos
	void getSize(int *w, int *h);
	void setSize(int w, int h);

	void getPos(int *x, int *y);
	void setPos(int x, int y);

	int getScreen();
	void setScreen(int screen);
	
	void goFullscreen();
	void goWindowed();
	bool isInFullscreenMode();
	
	// Mouse
	bool mouseIsOver();
	void getMousePosition(int *x, int *y);
	void setMousePosition(int x, int y);

	// Events
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
