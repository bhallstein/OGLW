/*
 * W - simple cross-platform OpenGL windows
 *
 * ===============
 *  Screens.cpp
 * ===============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Screens.h"
#include "WInt_PlatformIdentification.h"

#ifdef WTARGET_MAC

	#include <Cocoa/Cocoa.h>
	
	int W::numberOfScreens() {
		return (int) [[NSScreen screens] count];
	}

	void W::getScreenPosition(int screenInd, int *x, int *y) {
		NSArray *screens = [NSScreen screens];
		if (screenInd >= screens.count || screenInd < 0)
			*x = *y = -1;
		else {
			NSScreen *scr = screens[screenInd];
			*x = scr.frame.origin.x;
			*y = -scr.frame.origin.y;
		}
	}

	void W::getScreenSize(int screenInd, int *w, int *h) {
		NSArray *screens = [NSScreen screens];
		if (screenInd >= screens.count || screenInd < 0)
			*w = *h = -1;
		else {
			NSScreen *scr = screens[screenInd];
			*w = scr.frame.size.width;
			*h = scr.frame.size.height;
		}
	}

#elif defined WTARGET_WIN
	
	#include "GLFW/glfw3.h"
	
	int W::numberOfScreens() {
		int count;
		glfwGetMonitors(&count);
		return count;
	}

#endif