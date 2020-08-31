#include "Window.h"
#include "Event.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WGL
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"
#include <string>


// Window creation method declaration
GLFWwindow* create_glfw_window(
	int w,
	int h,
	const char *t,
	GLFWwindow* share,
	bool fullscreen,
	int screenInd,
	Window *userPointer
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


// PlatformSpecificState

struct Window::PlatformSpecificState {
	PlatformSpecificState() : win(NULL), glfw_window(NULL) { }
	
	Window *window;
	GLFWwindow *gflw_window;

	bool currentlyFullscreen;
	int prevW;
	int prevH;
	std::string title;
	int currentScreenIndex;
};


// GLFW init

struct Window::Init {
	Init() { glfwInit(); }
};
Window::Init *Window::init = new Window::Init;


// Window

Window::Window(
	int width,
	int height,
	const char *title,
	Window *share,
	bool fullscreen,
	int screenInd
)
{
	objs = new PlatformSpecificState();
	objs->title = title;
	objs->prevW = width;
	objs->prevH = height;
	objs->currentlyFullscreen = fullscreen;
	objs->currentScreenIndex = screenInd;
	objs->window = this;
	objs->gflw_window = create_glfw_window(
		width,		// ignored when creating fullscreen window 
		height,		// 
		title,
		share ? share->objs->window : NULL,
		fullscreen,
		screenInd,
		this
	);

	// - capturing the mouse:
	//     glfwSetInputMode: GLFW_CURSOR, GLFW_CURSOR_NORMAL/HIDDEN/DISABLED (auto-sets invis and always centred)
}

Window::~Window()
{
	glfwDestroyWindow(objs->win);
}


// Context
void Window::makeCurrentContext() {
	glfwMakeContextCurrent(objs->glfw_window);
}
void Window::clearCurrentContext() {
	glfwMakeContextCurrent(NULL);
}
void Window::flushBuffer() {
	glfwSwapBuffers(objs->glfw_window);
}

// Window attribs
void Window::setTitle(const char *t) {
	objs->title = t;
	glfwSetWindowTitle(objs->glfw_window, t);
}

// Respondery
void Window::bringToFront() {
	HWND winHandle = glfwGetWin32Window(objs->glfw_window);
	SetForegroundWindow(winHandle);
}
void Window::makeFirstResp() {
	HWND winHandle = glfwGetWin32Window(objs->glfw_window);
	SetFocus(winHandle);
}

// Size & position
void Window::getSize(int *w, int *h) {
	glfwGetWindowSize(objs->glfw_window, w, h);
}
void Window::setSize(int w, int h) {
	glfwSetWindowSize(objs->glfw_window, w, h);
}

void Window::getPos(int *x, int *y) {
	int winX, winY, monX, monY, nMons;
	int curScreen = getScreen();

	glfwGetWindowPos(objs->glfw_window, &winX, &winY);	
	GLFWmonitor **mons = glfwGetMonitors(&nMons);
	glfwGetMonitorPos(mons[curScreen], &monX, &monY);

	*x = winX - monX;
	*y = winY - monY;
}
void Window::setPos(int x, int y) {
	int nMons;
	int monX;
	int monY;
	int curScreen = getScreen();

	GLFWmonitor **mons = glfwGetMonitors(&nMons);
	glfwGetMonitorPos(mons[curScreen], &monX, &monY);

	glfwSetWindowPos(objs->glfw_window, x + monX, y + monY);
}

int Window::getScreen() {
	int i;
	// In fullscreen mode, we can use glfwGetWindowMonitor()
	if (objs->currentlyFullscreen) {
		int nMonitors;
		GLFWmonitor **mons = glfwGetMonitors(&nMonitors);
		GLFWmonitor *curMonitor = glfwGetWindowMonitor(objs->glfw_window);
		for (i=0; i < nMonitors; ++i) {
			if (mons[i] == curMonitor) {
				break;
			}
		}
	}
	// In windowed mode, find monitor whose frame encompasses the window's position
	else {
		int winX;
		int winY;
		int nMonitors;
		glfwGetWindowPos(objs->glfw_window, &winX, &winY);
		GLFWmonitor **mons = glfwGetMonitors(&nMonitors);
		for (i=0; i < nMonitors; ++i) {
			int monX;
			int monY;
			glfwGetMonitorPos(mons[i], &monX, &monY);
			const GLFWvidmode *vidMode = glfwGetVideoMode(mons[i]);
			if (winX >= monX && winY >= monY && winX < monX + vidMode->width && winY < monY + vidMode->height) {
				break;
			}
		}
	}
	return i;
}
void Window::setScreen(int newScreenInd) {
	int nMons;
	GLFWmonitor **mons = glfwGetMonitors(&nMons);
	if (newScreenInd >= nMons || newScreenInd < 0) {
		newScreenInd = 0;
	}

	// In fullscreen mode, have to create new window & destroy existing
	if (objs->currentlyFullscreen) {
		// Create new window in fullscreen on new screen
		GLFWwindow *newWindow = create_glfw_window(
			0, 0,
			objs->title.c_str(),
			objs->glfw_window,
			true,
			newScreenInd,
			this
		);
		// Destroy previous
		glfwDestroyWindow(objs->glfw_window);
		objs->glfw_window = newWindow;
	}
	// In windowed mode, get current window position, correct to reflect desired monitor
	else {
		int curScreenInd = getScreen();
		int x1, y1, x2, y2;
		glfwGetMonitorPos(mons[curScreenInd], &x1, &y1);
		glfwGetMonitorPos(mons[newScreenInd], &x2, &y2);
		
		int winX, winY;
		glfwGetWindowPos(objs->glfw_window, &winX, &winY);
		glfwSetWindowPos(objs->glfw_window, winX - x1 + x2, winY - y1 + y2);
	}
}

void Window::goFullscreen() {
	if (objs->currentlyFullscreen) {
		return;
	}
	
	// Save previous window properties
	glfwGetWindowSize(objs->glfw_window, &objs->prevW, &objs->prevH);

	// Create new in fullscreen on cur screen
	GLFWwindow *newWindow = create_glfw_window(
		0, 0,		// w/h ignored in fullscre
		objs->title.c_str(),
		objs->glfw_window,
		true,
		objs->currentScreenIndex,
		this
	);

	// Destroy previous
	glfwDestroyWindow(objs->glfw_window);

	objs->glfw_window = newWindow;
	objs->currentlyFullscreen = true;
}
void Window::goWindowed() {
	if (!objs->currentlyFullscreen) {
		return;
	}

	// Create windowed at prev size
	GLFWwindow *newWindow = create_glfw_window(
		objs->prevW,
		objs->prevH,
		objs->title.c_str(),
		objs->glfw_window,
		false,
		getScreen(),
		this
	);

	// Destroy previous
	glfwDestroyWindow(objs->glfw_window);

	objs->glfw_window = newWindow;
	objs->currentlyFullscreen = false;
}
bool Window::isInFullscreenMode() {
	return objs->currentlyFullscreen;
}

// Mouse
bool Window::mouseIsOver() {
	// Get size of window & its position on the virtual screen
	int winNativeX, winNativeY, winW, winH;
	glfwGetWindowPos(objs->glfw_window, &winNativeX, &winNativeY);
	getSize(&winW, &winH);

	// Get the position of the mouse on the virtual screen
	POINT nativeMPos;
	GetCursorPos(&nativeMPos);

	return (
		nativeMPos.x >= winNativeX && nativeMPos.y >= winNativeY &&
		nativeMPos.x < winNativeX + winW && nativeMPos.y < winNativeY + winH
	);
}
void Window::getMousePosition(int *x, int *y) {
	double mouseX, mouseY;
	glfwGetCursorPos(objs->glfw_window, &mouseX, &mouseY);
	*x = (int) mouseX;
	*y = (int) mouseY;
}
void Window::setMousePosition(int x, int y) {
	glfwSetCursorPos(objs->glfw_window, (double)x, (double)y);
}

// Events
void Window::getEvents() {
	glfwPollEvents();
}


// Window creation helper impl.

GLFWwindow* create_glfw_window(
	int w,
	int h,
	const char *t,
	GLFWwindow* share,
	bool fullscreen,
	int screenInd,
	Window *userPointer
) {
	GLFWwindow *glfw_window;

	// Get screen
	GLFWmonitor *mon;
	if (screenInd == 0) {
		mon = glfwGetPrimaryMonitor();
	}
	else {
		int n;
		GLFWmonitor **monitors = glfwGetMonitors(&n);
		if (screenInd >= n || screenInd < 0) {
			screenInd = 0;
		}
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

		glfw_window = glfwCreateWindow(monVidMode->width, monVidMode->height, t, mon, share);
	}
	else {
		// Create window, centered on spec. monitor
		int monX;
		int monY;
		const GLFWvidmode *vidmode;
		glfwGetMonitorPos(mon, &monX, &monY);
		vidmode = glfwGetVideoMode(mon);

		glfw_window = glfwCreateWindow(w, h, t, NULL, share);
		glfwSetWindowPos(glfw_window, monX + (vidmode->width - w)/2, monY + (vidmode->height - h)/2);
	}

	// Set user pointer
	glfwSetWindowUserPointer(glfw_window, userPointer);

	// Set window callback functions
	glfwSetWindowCloseCallback(glfw_window, cb_WindowClose);
	glfwSetWindowFocusCallback(glfw_window, cb_WindowFocus);
	glfwSetWindowIconifyCallback(glfw_window, cb_WindowMinimise);
	glfwSetWindowSizeCallback(glfw_window, cb_WindowResize);

	// Set mouse callback functions
	glfwSetMouseButtonCallback(glfw_window, cb_MouseButton);
	glfwSetScrollCallback(glfw_window, cb_ScrollWheel);
	
	return glfw_window;
}


// Window event callback function impls

void _addWinEvent(W::EventType::T type, W::Window *window) {
	if (W::Event::on) {
		W::Event ev(type);
		ev.winEvent.window = window;
		W::Event::newEvents.push_back(ev);
	}
}

void cb_WindowFocus(GLFWwindow *w, int focused) {
	Window *win = (Window*) glfwGetWindowUserPointer(w);

	_addWinEvent(
		focused ? W::EventType::WinBecameKey : W::EventType::WinStoppedBeingKey,
		win
	);

	// Generate application foreground & background events
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
	Window *win = (Window*) glfwGetWindowUserPointer(w);
	_addWinEvent(
		minimised ? W::EventType::WinMinimized : W::EventType::WinDeminimized,
		win
	);
}
void cb_WindowResize(GLFWwindow *w, int width, int height) {
	Window *win = (Window*) glfwGetWindowUserPointer(w);
	_addWinEvent(W::EventType::WinResized, win);
}
void cb_WindowClose(GLFWwindow *w) {
	Window *win = (Window*) glfwGetWindowUserPointer(w);
	_addWinEvent(W::EventType::WinClosed, win);
}


// Mouse event callback implementations:

void cb_MouseButton(GLFWwindow *w, int button, int action, int modifierKeys) {
	if (!W::Event::on) {
		return;
	}

	Window *win = (Window*) glfwGetWindowUserPointer(w);

	W::EventType::T type;
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		type = (action == GLFW_PRESS ? W::EventType::LMouseDown : W::EventType::LMouseUp);
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
		type = (action == GLFW_PRESS ? W::EventType::RMouseDown : W::EventType::RMouseUp);
	}
	else {
		return;
	}

	W::Event ev(type);
	ev.mouseEvent.window = win;
	double mouseX;
	double mouseY;
	glfwGetCursorPos(w, &mouseX, &mouseY);
	ev.mouseEvent.x = (int) mouseX;
	ev.mouseEvent.y = (int) mouseY;

	W::Event::newEvents.push_back(ev);
}
void cb_ScrollWheel(GLFWwindow *w, double x, double y) {
	if (!W::Event::on) {
		return;
	}

	Window *win = (Window*) glfwGetWindowUserPointer(w);

	W::Event ev(W::EventType::ScrollWheel);
	ev.scrollEvent.window = win;
	ev.scrollEvent.dx = (float) x;
	ev.scrollEvent.dy = (float) y;

	W::Event::newEvents.push_back(ev);
}

// Key callback function impl.:
extern W::KeyCode::T _translGLFWToKeyCode(int glfwKey);
void cb_KeyCallback(GLFWwindow *w, int key, int scancode, int action, int modifierKeys) {
	if (!W::Event::on) {
		return;
	}

	W::EventType::T type;
	if (action == GLFW_PRESS) {
		type = W::EventType::KeyDown;
	}
	else if (action == GLFW_RELEASE) {
		type = W::EventType::KeyUp;
	}
	else {
		return;
	}

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
 