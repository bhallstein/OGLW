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
			   int posx, int posy,
			   Window *share,
			   bool fullscreen,
			   int screen
			   );
		~Window();
		
		// Passthru methods
		void makeCurrentContext();
		void clearCurrentContext();
		
		void flushBuffer();
		
		void setTitle(const char *);
		void getSize(int *w, int *h);
		
		void bringToFront();
		void makeFirstResp();
		
		void goFullscreen();
		void goFullscreenOn(int screen);
		void goWindowed();

		void getEvents();	// Only has any effect on windows
		
		int numberOfScreens();
		
	private:
		Window(const Window &) { }
		WInt_WindowAbstr *windowAbstr;
		
	};
	
}

#endif
