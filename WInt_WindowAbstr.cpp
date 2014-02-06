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
	#include "WInt_WindowWin.h"
#endif


// make_WInt_Window()

WInt_WindowAbstr* make_WInt_Window(
								   int w, int h,
								   const char *t,
								   WInt_WindowAbstr *share,
								   bool fullscreen,
								   int screen,
								   void *windowID
								   )
{
	WInt_WindowAbstr *win = 0;
	
	#ifdef WTARGET_MAC
		win = new WInt_WindowOSX(w, h, t, (WInt_WindowOSX*)share, fullscreen, screen, windowID);
	#elif defined WTARGET_WIN
		win = new WInt_WindowWin(w, h, t, posx, posy, (WInt_WindowWin*)share, fullscreen, screen, windowID);
	#endif
	
	return win;
}

