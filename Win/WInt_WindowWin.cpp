/*
 * W - simple cross-platform OpenGL windows
 *
 * ======================
 *  WInt_WindowWin.cpp
 * ======================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

// Tasks remaining:
// - application fore/background events

// Both windows and os x:
// - getting mouse position
//     glfwGetCursorPos
// - also, setting mouse position
//     glfwSetCursorPos (nb window must be focused)
// - capturing the mouse:
//     glfwSetInputMode: GLFW_CURSOR, GLFW_CURSOR_NORMAL/HIDDEN/DISABLED (auto-sets invis and always centred)


#include "WInt_WindowWin.h"
#include "Event.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <string>


// Window creation method declaration
GLFWwindow* WInt_createWindow(
	int w,
	int h,
	const char *t,
	GLFWwindow* share,
	bool fullscreen,
	int screenInd,
	WInt_WindowWin *userPointer
);


// Window event callback function declarations
void cb_WindowFocus(GLFWwindow*, int focused);
void cb_WindowMinimise(GLFWwindow*, int minimised);
void cb_WindowResize(GLFWwindow*, int w, int h);
void cb_WindowClose(GLFWwindow*);

// Mouse callback function declarations:
void cb_MouseButton(GLFWwindow*, int button, int action, int modifierKeys);
void cb_ScrollWheel(GLFWwindow*, double x, double y);

// Key callback functions:
void cb_KeyCallback(GLFWwindow*, int key, int scancode, int action, int modifierKeys);
void cb_CharCallback(GLFWwindow*, int unicode_code_point);


// NativeObjs

struct WInt_WindowWin::NativeObjs {
	NativeObjs() : win(NULL) { }
	
	GLFWwindow *win;

	bool currentlyFullscreen;
	int prevW, prevH;
	std::string title;
	int currentScreenIndex;

	void *winID;
};


// GLFW init

struct WInt_WindowWin::Init {
	Init() { glfwInit(); }
};
WInt_WindowWin::Init *WInt_WindowWin::init = new WInt_WindowWin::Init;

// WInt_WindowWin

WInt_WindowWin::WInt_WindowWin(
	int width,
	int height,
	const char *title,
	int posx,
	int posy,
	WInt_WindowWin *share,
	bool fullscreen,
	int screenInd,
	void *windowID
)
{
	objs = new NativeObjs();
	objs->title = title;
	objs->prevW = width;
	objs->prevH = height;
	objs->currentlyFullscreen = fullscreen;
	objs->currentScreenIndex = screenInd;
	objs->winID = windowID;

	if (fullscreen) {
		objs->win = WInt_createWindow(
			width,		// ignored when creating fullscreen window 
			height,		// 
			title,
			share ? share->objs->win : NULL,
			true,
			screenInd,
			this
		);
	}
	else {
		objs->win = WInt_createWindow(
			width,
			height,
			title,
			share ? share->objs->win : NULL,
			false,
			screenInd,
			this
		);
	}
	
/*
	glfwGetWindowMonitor

	glfwDestroyWindow

	wm_setfocus, wm_killfocus  - sent to window when gains/loses keybd focus
	wm_activate                - sent to window when de-/activated 

	getforegroundwindow fn - gets the hwnd w/ focus? compare to application windows

	setwineventhook - event_system_foreground events 
	setwindowshookex?
*/

}

WInt_WindowWin::~WInt_WindowWin()
{
	glfwDestroyWindow(objs->win);
}


void WInt_WindowWin::makeCurrentContext() {
	glfwMakeContextCurrent(objs->win);
}

void WInt_WindowWin::clearCurrentContext() {
	glfwMakeContextCurrent(NULL);
}

void WInt_WindowWin::flushBuffer() {
	glfwSwapBuffers(objs->win);
}

void WInt_WindowWin::setTitle(const char *t) {
	objs->title = t;
	glfwSetWindowTitle(objs->win, t);
}

void WInt_WindowWin::getSize(int *w, int *h) {
	glfwGetWindowSize(objs->win, w, h);
}

void WInt_WindowWin::bringToFront() {
	HWND winHandle = glfwGetWin32Window(objs->win);
	SetForegroundWindow(winHandle);
}
void WInt_WindowWin::makeFirstResp() {
	HWND winHandle = glfwGetWin32Window(objs->win);
	SetFocus(winHandle);
}

void WInt_WindowWin::goFullscreenOnCurScreen() {
	if (objs->currentlyFullscreen) return;
	
	// Save previous window properties
	glfwGetWindowSize(objs->win, &objs->prevW, &objs->prevH);

	// Create new in fullscreen on cur screen
	GLFWwindow *newWindow = WInt_createWindow(
		0, 0,		// w/h ignored in fullscre
		objs->title.c_str(),
		objs->win,
		true,
		objs->currentScreenIndex,
		this
	);

	// Destroy previous
	glfwDestroyWindow(objs->win);

	objs->win = newWindow;
	objs->currentlyFullscreen = true;
}

void WInt_WindowWin::goFullscreenOn(int newScreenInd) {
	// If already fullscreen on spec. screen, return
	if (objs->currentlyFullscreen && objs->currentScreenIndex == newScreenInd)
		return;
	
	// If currently windowed, save previous window properties
	if (!objs->currentlyFullscreen)
		glfwGetWindowSize(objs->win, &objs->prevW, &objs->prevH);

	// Create new fullscreen window
	GLFWwindow *newWindow = WInt_createWindow(
		0, 0,
		objs->title.c_str(),
		objs->win,
		true,
		newScreenInd,
		this
	);

	// Destroy previous
	glfwDestroyWindow(objs->win);

	objs->win = newWindow;
	objs->currentlyFullscreen = true;
	objs->currentScreenIndex = newScreenInd;

}
void WInt_WindowWin::goWindowed() {
	if (!objs->currentlyFullscreen) return;

	// Create windowed at prev size
	GLFWwindow *newWindow = WInt_createWindow(
		objs->prevW,
		objs->prevH,
		objs->title.c_str(),
		objs->win,
		false,
		objs->currentScreenIndex,
		this
	);

	// Destroy previous
	glfwDestroyWindow(objs->win);

	objs->win = newWindow;
	objs->currentlyFullscreen = false;
}

void WInt_WindowWin::getEvents() {
	glfwPollEvents();
}


// Window creation helper impl.

GLFWwindow* WInt_createWindow(int w, int h, const char *t, GLFWwindow* share, bool fullscreen, int screenInd);

GLFWwindow* WInt_createWindow(
	int w,
	int h,
	const char *t,
	GLFWwindow* share,
	bool fullscreen,
	int screenInd,
	WInt_WindowWin *userPointer		// user pointer of GLFW window is our WindowWin object, so we can set the required
									// properties on W window/mouse events
) {
	GLFWwindow *win;

	// Get screen
	GLFWmonitor *mon;
	if (screenInd == 0)
		mon = glfwGetPrimaryMonitor();
	else {
		int n;
		GLFWmonitor **monitors = glfwGetMonitors(&n);
		if (screenInd >= n)
			screenInd = 0;
		mon = monitors[screenInd];
	}

	// Set hints
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (fullscreen) {
		// Create fullscreen on spec. monitor
		const GLFWvidmode *monVidMode;
		monVidMode = glfwGetVideoMode(mon);

		win = glfwCreateWindow(monVidMode->width, monVidMode->height, t, mon, share);
	}
	else {
		// Create windowed on spec. monitor
		int monX, monY;
		const GLFWvidmode *vidmode;
		glfwGetMonitorPos(mon, &monX, &monY);
		vidmode = glfwGetVideoMode(mon);

		win = glfwCreateWindow(w, h, t, NULL, share);
		glfwSetWindowPos(win, monX + (vidmode->width - w)/2, monY + (vidmode->height - h)/2);
	}

	// Set user pointer
	glfwSetWindowUserPointer(win, userPointer);

	// Set window callback functions
	glfwSetWindowCloseCallback(win, cb_WindowClose);
	glfwSetWindowFocusCallback(win, cb_WindowFocus);
	glfwSetWindowIconifyCallback(win, cb_WindowMinimise);
	glfwSetWindowSizeCallback(win, cb_WindowResize);

	// Set mouse callback functions
	glfwSetMouseButtonCallback(win, cb_MouseButton);
	glfwSetScrollCallback(win, cb_ScrollWheel);
	
	return win;
}


// Window event callback function impls

void _addWinEvent(W::EventType::T type, void *winID) {
	if (W::Event::on) {
		W::Event ev(type);
		ev.winEvent.window = (W::Window*) winID;
		W::Event::newEvents.push_back(ev);
	}
}

void cb_WindowFocus(GLFWwindow *w, int focused) {
	WInt_WindowWin *win = (WInt_WindowWin*) glfwGetWindowUserPointer(w);
	void *winID = win->objs->winID;

	_addWinEvent(
		focused ? W::EventType::WinBecameKey : W::EventType::WinStoppedBeingKey,
		winID
	);
}
void cb_WindowMinimise(GLFWwindow *w, int minimised) {
	WInt_WindowWin *win = (WInt_WindowWin*) glfwGetWindowUserPointer(w);
	void *winID = win->objs->winID;

	_addWinEvent(
		minimised ? W::EventType::WinMinimized : W::EventType::WinDeminimized,
		winID
	);
}
void cb_WindowResize(GLFWwindow *w, int width, int height) {
	WInt_WindowWin *win = (WInt_WindowWin*) glfwGetWindowUserPointer(w);
	void *winID = win->objs->winID;

	_addWinEvent(W::EventType::WinResized, winID);
}
void cb_WindowClose(GLFWwindow *w) {
	WInt_WindowWin *win = (WInt_WindowWin*) glfwGetWindowUserPointer(w);
	void *winID = win->objs->winID;

	_addWinEvent(W::EventType::WinClosed, winID);
}


// Mouse event callback implementations:

void cb_MouseButton(GLFWwindow *w, int button, int action, int modifierKeys) {
	if (!W::Event::on) return;

	WInt_WindowWin *win = (WInt_WindowWin*) glfwGetWindowUserPointer(w);
	void *winID = win->objs->winID;

	W::EventType::T evType = -1;
	if (button == GLFW_MOUSE_BUTTON_LEFT)
		evType = (action == GLFW_PRESS ? W::EventType::LMouseDown : W::EventType::LMouseUp);
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		evType = (action == GLFW_PRESS ? W::EventType::RMouseDown : W::EventType::RMouseUp);

	if (evType == -1) return;

	W::Event ev(evType);
	ev.mouseEvent.window = (W::Window*) winID;
	double mouseX, mouseY;
	glfwGetCursorPos(w, &mouseX, &mouseY);
	ev.mouseEvent.x = (int) mouseX;
	ev.mouseEvent.y = (int) mouseY;

	W::Event::newEvents.push_back(ev);
}
void cb_ScrollWheel(GLFWwindow *w, double x, double y) {
	if (!W::Event::on) return;

	WInt_WindowWin *win = (WInt_WindowWin*) glfwGetWindowUserPointer(w);
	void *winID = win->objs->winID;

	W::Event ev(W::EventType::ScrollWheel);
	ev.scrollEvent.window = (W::Window*) winID;
	ev.scrollEvent.dx = (int) x;
	ev.scrollEvent.dy = (int) y;

	W::Event::newEvents.push_back(ev);
}

// Key callback function impl.:
extern W::KeyCode::T _translGLFWToKeyCode(int glfwKey);
void cb_KeyCallback(GLFWwindow *w, int key, int scancode, int action, int modifierKeys) {
	if (!W::Event::on) return;

	W::EventType::T type = -1;
	if (action == GLFW_PRESS)        type = W::EventType::KeyDown;
	else if (action == GLFW_RELEASE) type = W::EventType::KeyUp;
	if (type == -1) return;

	W::Event ev(type);
	ev.keyEvent.keyCode = _translGLFWToKeyCode(key);

	W::Event::newEvents.push_back(ev);
}
//void cb_CharCallback(GLFWwindow*, int unicode_code_point);

//	So, it seems w/ glfw the peekmessage-based glfwpollevents() system is used to retrieve messages from the windows queue.
//	These are then processed by glfw, triggering the appropriate callbacks.
//	Therefore on windows the user needs to synchronously trigger the retrieval of events in their update loop.
//	i.e. Win:
//		- W::getEvents();
//		- process events
//		- update
//		- draw
