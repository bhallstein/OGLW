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

	// - capturing the mouse:
	//     glfwSetInputMode: GLFW_CURSOR, GLFW_CURSOR_NORMAL/HIDDEN/DISABLED (auto-sets invis and always centred)
}

WInt_WindowWin::~WInt_WindowWin()
{
	glfwDestroyWindow(objs->win);
}


// Context
void WInt_WindowWin::makeCurrentContext() {
	glfwMakeContextCurrent(objs->win);
}
void WInt_WindowWin::clearCurrentContext() {
	glfwMakeContextCurrent(NULL);
}
void WInt_WindowWin::flushBuffer() {
	glfwSwapBuffers(objs->win);
}

// Window attribs
void WInt_WindowWin::setTitle(const char *t) {
	objs->title = t;
	glfwSetWindowTitle(objs->win, t);
}

// Respondery
void WInt_WindowWin::bringToFront() {
	HWND winHandle = glfwGetWin32Window(objs->win);
	SetForegroundWindow(winHandle);
}
void WInt_WindowWin::makeFirstResp() {
	HWND winHandle = glfwGetWin32Window(objs->win);
	SetFocus(winHandle);
}

// Size & position
void WInt_WindowWin::getSize(int *w, int *h) {
	glfwGetWindowSize(objs->win, w, h);
}
void WInt_WindowWin::setSize(int w, int h) {
	glfwSetWindowSize(objs->win, w, h);
}

void WInt_WindowWin::getPos(int *x, int *y) {
	int winX, winY, monX, monY, nMons;
	int curScreen = getScreen();

	glfwGetWindowPos(objs->win, &winX, &winY);	
	GLFWmonitor **mons = glfwGetMonitors(&nMons);
	glfwGetMonitorPos(mons[curScreen], &monX, &monY);

	*x = winX - monX;
	*y = winY - monY;
}
void WInt_WindowWin::setPos(int x, int y) {
	int nMons, monX, monY;
	int curScreen = getScreen();

	GLFWmonitor **mons = glfwGetMonitors(&nMons);
	glfwGetMonitorPos(mons[curScreen], &monX, &monY);

	glfwSetWindowPos(objs->win, x + monX, y + monY);
}

int WInt_WindowWin::getScreen() {
	int i;
	// In fullscreen mode, we can use glfwGetWindowMonitor()
	if (objs->currentlyFullscreen) {
		int nMonitors;
		GLFWmonitor **mons = glfwGetMonitors(&nMonitors);
		GLFWmonitor *curMonitor = glfwGetWindowMonitor(objs->win);
		for (i=0; i < nMonitors; ++i)
			if (mons[i] == curMonitor)
				break;
	}
	// In windowed mode, find monitor whose frame encompasses the window's position
	else {
		int winX, winY, nMonitors;
		glfwGetWindowPos(objs->win, &winX, &winY);
		GLFWmonitor **mons = glfwGetMonitors(&nMonitors);
		for (i=0; i < nMonitors; ++i) {
			int monX, monY;
			glfwGetMonitorPos(mons[i], &monX, &monY);
			const GLFWvidmode *vidMode = glfwGetVideoMode(mons[i]);
			if (winX >= monX && winY >= monY && winX < monX + vidMode->width && winY < monY + vidMode->height)
				break;
		}
	}
	return i;
}
void WInt_WindowWin::setScreen(int newScreenInd) {
	int nMons;
	GLFWmonitor **mons = glfwGetMonitors(&nMons);
	if (newScreenInd >= nMons || newScreenInd < 0)
		newScreenInd = 0;

	// In fullscreen mode, have to create new window & destroy existing
	if (objs->currentlyFullscreen) {
		// Create new window in fullscreen on new screen
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
	}
	// In windowed mode, get current window position, correct to reflect desired monitor
	else {
		int curScreenInd = getScreen();
		int x1, y1, x2, y2;
		glfwGetMonitorPos(mons[curScreenInd], &x1, &y1);
		glfwGetMonitorPos(mons[newScreenInd], &x2, &y2);
		
		int winX, winY;
		glfwGetWindowPos(objs->win, &winX, &winY);
		glfwSetWindowPos(objs->win, winX - x1 + x2, winY - y1 + y2);
	}
}

void WInt_WindowWin::goFullscreen() {
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
void WInt_WindowWin::goWindowed() {
	if (!objs->currentlyFullscreen) return;

	// Create windowed at prev size
	GLFWwindow *newWindow = WInt_createWindow(
		objs->prevW,
		objs->prevH,
		objs->title.c_str(),
		objs->win,
		false,
		getScreen(),
		this
	);

	// Destroy previous
	glfwDestroyWindow(objs->win);

	objs->win = newWindow;
	objs->currentlyFullscreen = false;
}
bool WInt_WindowWin::isInFullscreenMode() {
	return objs->currentlyFullscreen;
}

// Mouse
bool WInt_WindowWin::mouseIsOver() {
	// Get size of window & its position on the virtual screen
	int winNativeX, winNativeY, winW, winH;
	glfwGetWindowPos(objs->win, &winNativeX, &winNativeY);
	getSize(&winW, &winH);

	// Get the position of the mouse on the virtual screen
	POINT nativeMPos;
	GetCursorPos(&nativeMPos);

	return (
		nativeMPos.x >= winNativeX && nativeMPos.y >= winNativeY &&
		nativeMPos.x < winNativeX + winW && nativeMPos.y < winNativeY + winH
	);
}
void WInt_WindowWin::getMousePosition(int *x, int *y) {
	double mouseX, mouseY;
	glfwGetCursorPos(objs->win, &mouseX, &mouseY);
	*x = (int) mouseX;
	*y = (int) mouseY;
}
void WInt_WindowWin::setMousePosition(int x, int y) {
	glfwSetCursorPos(objs->win, (double)x, (double)y);
}

// Events
void WInt_WindowWin::getEvents() {
	glfwPollEvents();
}


// Window creation helper impl.

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
		if (screenInd >= n || screenInd < 0)
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
		// Create window, centered on spec. monitor
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

	// Generate application fore/background events
	if (W::Event::on) {
		HWND foregroundWinHandle = GetForegroundWindow();
		// If the foreground window handle is null, consider us to be losing foreground
		if (foregroundWinHandle == NULL) {
			W::Event ev(W::EventType::AppBecameBackground);
			W::Event::newEvents.push_back(ev);
		}
		else {
			// If the pid of the foreground window matches ours, we are entering the foreground;
			// otherwise, the background
			DWORD pidForegroundWindow;
			GetWindowThreadProcessId(foregroundWinHandle, &pidForegroundWindow);
			if (pidForegroundWindow == GetCurrentProcessId()) {
				if (focused) {
					W::Event ev(W::EventType::AppBecameForeground);
					W::Event::newEvents.push_back(ev);
				}
			}
			else if (!focused) {
				W::Event ev(W::EventType::AppBecameBackground);
				W::Event::newEvents.push_back(ev);
			}
		}
	}
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
	ev.scrollEvent.dx = (float) x;
	ev.scrollEvent.dy = (float) y;

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

//	It seems the peekmessage-based glfwpollevents() system is used by glfw to retrieve messages from the windows queue.
//	These are then processed by glfw, triggering the appropriate callbacks.
//	Therefore on windows the user needs to synchronously trigger the retrieval of events in their update loop.
//	i.e. Win:
//		- W::getEvents();
//		- process events
//		- update
//		- draw
