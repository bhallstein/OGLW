/*
 * W - simple cross-platform OpenGL windows
 *
 * ==============
 *  Window.cpp
 * ==============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Window.h"
#include "WInt_WindowAbstr.h"
#include <iostream>

W::Window::Window(
				 int width,
				 int height,
				 const char *title,
				 int posx, int posy,
				 Window *share,
				 bool fullscreen,
				 int screen
				 )
{
	windowAbstr = make_WInt_Window(width,
								   height,
								   title,
								   posx,
								   posy,
								   share ? share->windowAbstr : 0,
								   fullscreen,
								   screen,
								   (void*)this);
}

W::Window::~Window()
{
	delete windowAbstr;
}


#pragma mark - Passthru methods

void W::Window::makeCurrentContext() { windowAbstr->makeCurrentContext(); }
void W::Window::clearCurrentContext() { windowAbstr->clearCurrentContext(); }

void W::Window::flushBuffer() { windowAbstr->flushBuffer(); }

void W::Window::setTitle(const char *t) { windowAbstr->setTitle(t); }
void W::Window::getSize(int *w, int *h) { windowAbstr->getSize(w, h); }

void W::Window::bringToFront() { windowAbstr->bringToFront(); }
void W::Window::makeFirstResp() { windowAbstr->makeFirstResp(); }

void W::Window::goFullscreen() { windowAbstr->goFullscreenOnCurScreen(); }
void W::Window::goFullscreenOn(int s) { windowAbstr->goFullscreenOn(s); }
void W::Window::goWindowed() { windowAbstr->goWindowed(); }
