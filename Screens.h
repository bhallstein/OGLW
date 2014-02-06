/*
 * W - simple cross-platform OpenGL windows
 *
 * =============
 *  Screens.h
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#ifndef __OGLMultiWindowTest__Screens__
#define __OGLMultiWindowTest__Screens__

namespace W {
	int numberOfScreens();
	void getScreenPosition(int screen, int *x, int *y);
	void getScreenSize(int screen, int *w, int *h);
}

#endif