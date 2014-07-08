# W


*Under consideration:*

- adding a timer
- screen config changed event
- hiding the mouse


*Known issues:*

- Windows:
	- Fullscreen windows automatically iconify when defocused due to GLFW bug #143
	- `Window::getPos()` & `setPos()` specify the position of the client area, rather than of the window itself, due to GLFW bug #105
	- When the window is switched from fullscreen mode to windowed mode or vice versa, it must be destroyed and recreated. The newly created window's context is shared with the previous one, but some OpenGL state will still need setting back up. This is also the case when calling `setScreen()` in fullscreen mode.
	- When a window is given focus, it may generate an `AppBecameForeground` event, even when the app wasn't previously in the background.
- Mac:
	- Double-clicks on a window that is in the background result in two `LMouseDown/Up` events appearing in the `newEvents` vector simultaneously, as it is brought to the foreground.


## About

W is a very simple set of C++ classes to take care of setting up an OpenGL-backed window. The goal being to just get started working with OpenGL as quickly as possible.

It supports multiple windows (optionally using OGL resource sharing*), multiple screens, and switching between windowed and fullscreen modes. It also harvests native mouse, key, touch, and window events.

W works on OS X, Windows and iOS [iOS is not actually yet supported]. On Windows, it requires GLFW 3.

*Note that OpenGL contexts can share the following types of resource:

- Textures
- Buffer objects (VBOs, etc)
- Shader & program objects

OpenGL does not share VAOs or Framebuffer objects among contexts.


## The Window class

The class W::Window represents a window. They are very simple to create & manipulate.

Constructor:
    Window(
        int width, int height,
        const char *title,
        Window *share,
        bool fullscreen,
        int screen
    )
        
- Parameters:
	- width, height: the desired width and height of the window (only affects windowed mode)
	- title: the title of the window (only affects windowed mode)
	- share: another window with whose OpenGL context this window will use resource sharing, or NULL
	- fullscreen: create in fullscreen or windowed mode?
	- screen: the screen on which to create the window


### Window methods:

OpenGL context methods

- `void makeCurrentContext()` – make the window's OpenGL context current.
- `void clearCurrentContext()` – make the OGL context current no longer.
- `void flushBuffer()` – flush drawing calls made previously through the graphics pipeline.

Window attributes

- `void setTitle(const char *)` – set the window title.

Size & position

- `void getSize(int *w, int *h)` – get the current size of the OpenGL view contained by the window.
- `void setSize(int w, int h)` – set the OpenGL view size of the window. In windowed mode, this will result in a slightly larger overall window size, because of window chrome. If the window is in fullscreen mode, this has no effect.
- `void getPos(int *x, int *y)` – get the position of the window’s top left corner, relative to the same corner of the screen.
- `void setPos(int x, int y)` – set the position of the window’s top left corner.
- `int getScreen()` – get the window’s current screen.
- `void setScreen()` – set the window’s screen. The window will be repositioned onto that screen. If the requested screen doesn’t exist, screen zero will be used. If the window is in fullscreen mode, it will be resized to fill the new screen. On windows, if the window is in fullscreen mode, it will be destroyed and recreated..
- `void goFullscreen()` – if in windowed mode, switch to fullscreen mode.
- `void goWindowed()` – if in fullscreen mode, switch to windowed mode. If the window has ever been in windowed mode before, its position will be restored. Otherwise, it will be centered on the screen.


Mouse

- `bool mouseIsOver()` – returns true if the mouse is over the window.
- `void getMousePosition(int *x, int *y)` – gets the mouse position, relative to the top left pixel of the OpenGL view contained by the window.
`void setMousePosition(int x, int y)` – similarly, sets the mouse position.

Events

- `void getEvents()` – on Windows, retrieve system events and convert them to W events. NB: you only need to call this on one window per update cycle; this will retrieve events for all current windows.


## Events

W converts native events to the `W::Event` type.

Events have a `type` property, for example, `LMouseDown`. The property is of `W::EventType::T` type. See Event.h for the full list of event types.

You can register custom types of event like so:

    MyClass.h:   extern W::EventType::T MyEventType;
    MyClass.cpp: W::EventType::T MyEventType = W::Event::registerType();

Event defines several struct types encapsulating the information pertaining to an event:

- MouseEvent:
	- int x, y
	- Window *window
- KeyEvent:
	- KeyCode::T keyCode
- ScrollEvent:
	- int dx, dy
	- Window *window
- WindowEvent:
	- Window *window
- TouchEvent:
	- int x, y
	- int touchID

These are stored in the following properties, respectively. Check the type of the event and use the appropriate property:

- mouseEvent
- keyEvent
- scrollEvent
- winEvent
- touchEvent

Events are stored in the vector `Event::newEvents`. Iterate over them in your update cycle, then clear them.

KeyCodes are of the type W::KeyCode::T and are defined in Event.h.

On Windows, GLFW’s peekmessage-based glfwPollEvents() system is used to retrieve messages from the windows queue.

These are then processed by glfw, triggering the appropriate callbacks.

Therefore on Windows you need to synchronously trigger the retrieval of events in your update loop with `window->getEvents()`. NB: this can be called on any open window, but triggers the processing of events from all windows.

Mouse events have a window property. Key events do not – for now, they’re considered global.

The event system is switched off initially. To turn it on, set `W::Event::on = true`.


## Screens

Screens are referred to by their integer index, starting at zero.

- Retrieve the total number of screens using `W::numberOfScreens()`
- Find a screen’s position: `W::getScreenPosition(int screen, int *x, int *y)`. The top left of screen zero is considered the origin.
- Find a screen’s size: `W::getScreenSize(int screen, int *w, int *h)`.

If the requested screen doesn't exist, `getScreenSize` &`getScreenPosition` will set their parameters to -1.


## License

W is published under the open source MIT license, and comes with no warranty whatsoever. If you use it, drop me a message to let me know :)

Ben Hallstein, 2014