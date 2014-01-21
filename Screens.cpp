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
#include <Cocoa/Cocoa.h>

int W::numberOfScreens() {
	return (int) [[NSScreen screens] count];
}
