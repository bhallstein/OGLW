#ifndef WWindow_h
#define WWindow_h

#include <vector>
#include <algorithm>
#include "Multisampling.h"


namespace W {
	
	class Window {
	public:
		Window(int width,
					 int height,
					 const char *title,
					 Window *share,
					 bool fullscreen,
					 int screen,
					 Multisampling::T multisampling = Multisampling::None);
		~Window();
		static std::vector<Window*> windows() {
			return current_windows;
		}
		
		// Context
		void makeCurrentContext();
		void clearCurrentContext();
		void flushBuffer();
		
		// Window attributes
		void setTitle(const char *);
		
		// Events etc.
		void bringToFront();
		void makeFirstResp();
		
		// Size & position
		void getSize(int *w, int *h);
		void setSize(int w, int h);
		void getPos(int *x, int *y);
		void setPos(int x, int y);

		// Multiple screens
		int  getScreen();
		void setScreen(int screen);
		void goFullscreen();
		void goWindowed();
		bool isInFullscreenMode();
		
		// Mouse
		bool mouseIsOver();
		void getMousePosition(int *x, int *y);
		void setMousePosition(int x, int y);
		
		// Events
		void getEvents();	 // Only has any effect on Windows

		
	private:
		Window(const Window &) { }

		struct PlatformSpecificState;
		PlatformSpecificState *objs;

		struct Init;
		static Init *init;

		static std::vector<W::Window*> current_windows;
		void register_window(W::Window *win) {
			current_windows.push_back(win);
		}
		void deregister_window(W::Window *win) {
			std::remove(current_windows.begin(), current_windows.end(), win);
		}

	};
	
}

#endif
