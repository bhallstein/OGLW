/*
 * W - simple cross-platform OpenGL windows
 *
 * ========================
 *  WInt_WindowAbstr.cpp
 * ========================
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "WInt_WindowAbstr.h"


#include "WInt_PlatformIdentification.h"

#ifdef WTARGET_MAC
	#include "WInt_WindowOSX.h"
#elif defined WTARGET_WIN
	#inlcude "WInt_WindowWin.h"
#endif

WInt_WindowAbstr* make_WInt_Window(
								   int w, int h,
								   const char *t,
								   int posx, int posy,
								   WInt_WindowAbstr *share,
								   bool fullscreen,
								   int screen,
								   void *windowID
								   )
{
	WInt_WindowAbstr *win = 0;
	
	#ifdef WTARGET_MAC
		win = new WInt_WindowOSX(w, h, t, posx, posy, (WInt_WindowOSX*)share, fullscreen, screen, windowID);
	#elif defined WTARGET_WIN
		dkjghkdjgh
	#endif
	
	return win;
}

