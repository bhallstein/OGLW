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

#ifndef W_Screens_h
#define W_Screens_h

namespace W {
	int numberOfScreens();
	void getScreenPosition(int screen, int *x, int *y);
	void getScreenSize(int screen, int *w, int *h);
}

#endif
