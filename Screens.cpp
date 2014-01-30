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


#elif defined WTARGET_WIN
	
	#include "GLFW/glfw3.h"
	
	int W::numberOfScreens() {
		int count;
		glfwGetMonitors(&count);
		return count;
	}

#endif