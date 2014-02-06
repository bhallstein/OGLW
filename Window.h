/*
 * W - simple cross-platform OpenGL windows
 *
 * ============
 *  Window.h
 * ============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __OGLMultiWindowTest__Window__
#define __OGLMultiWindowTest__Window__

class WInt_WindowAbstr;

namespace W {

	class Window {
	public:
		Window(
			   int width,
			   int height,
			   const char *title,
			   Window *share,
			   bool fullscreen,
			   int screen
			   );
		~Window();
		
		// Passthrough methods: OGL context
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
		void setScreen(int screen);
		
		void goFullscreen();
		void goWindowed();
		bool isInFullscreenMode();
		
		// Mouse
		bool mouseIsOver();
		void getMousePosition(int *x, int *y);
		void setMousePosition(int x, int y);
		
		// Events
		void getEvents();	// Only has any effect on windows
		
	private:
		Window(const Window &) { }
		WInt_WindowAbstr *windowAbstr;
		
	};
	
}

#endif
