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


WInt_WindowAbstr* make_WInt_Window(int w, int h,
								   const char *t,
								   WInt_WindowAbstr *share,
								   bool fullscreen,
								   int screen,
								   W::Multisampling::T multisampling,
								   void *windowID)
{
	WInt_WindowAbstr *win = 0;
	
	#ifdef WTARGET_MAC
		win = new WInt_WindowOSX(w, h, t, (WInt_WindowOSX*)share, fullscreen, screen, multisampling, windowID);
	#elif defined WTARGET_WIN
		win = new WInt_WindowWin(w, h, t, (WInt_WindowWin*)share, fullscreen, screen, windowID);
	#endif
	
	return win;
}


std::vector<WInt_WindowAbstr*> winabstr_registry;
