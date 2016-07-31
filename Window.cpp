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

// Window

W::Window::Window(
				  int width,
				  int height,
				  const char *title,
				  Window *share,
				  bool fullscreen,
				  int screen,
				  Multisampling::T multisampling
				  )
{
	windowAbstr = make_WInt_Window(width,
								   height,
								   title,
								   share ? share->windowAbstr : 0,
								   fullscreen,
								   screen,
								   multisampling,
								   (void*)this);
	winabstr_registry.push_back(windowAbstr);
}

W::Window::~Window()
{
	for (auto it = winabstr_registry.begin(); it != winabstr_registry.end(); ) {
		if (*it == windowAbstr)
			it = winabstr_registry.erase(it);
		else
			++it;
	}
	delete windowAbstr;
}


#pragma mark - Passthru methods

// OGL context
void W::Window::makeCurrentContext()  { windowAbstr->makeCurrentContext(); }
void W::Window::clearCurrentContext() { windowAbstr->clearCurrentContext(); }
void W::Window::flushBuffer()         { windowAbstr->flushBuffer(); }

// Win attribs
void W::Window::setTitle(const char *t) { windowAbstr->setTitle(t); }

// Respondery
void W::Window::bringToFront()  { windowAbstr->bringToFront(); }
void W::Window::makeFirstResp() { windowAbstr->makeFirstResp(); }

// Size & pos
void W::Window::getSize(int *w, int *h) { windowAbstr->getSize(w, h); }
void W::Window::setSize(int w, int h)   { windowAbstr->setSize(w, h); }
int W::Window::w() { int w, h; windowAbstr->getSize(&w, &h); return w; }
int W::Window::h() { int w, h; windowAbstr->getSize(&w, &h); return h; }

void W::Window::getPos(int *x, int *y) { windowAbstr->getPos(x, y); }
void W::Window::setPos(int x, int y)   { windowAbstr->setPos(x, y); }

int W::Window::getScreen()            { return windowAbstr->getScreen(); }
void W::Window::setScreen(int screen) { windowAbstr->setScreen(screen); }

void W::Window::goFullscreen() { windowAbstr->goFullscreen(); }
void W::Window::goWindowed()   { windowAbstr->goWindowed(); }
bool W::Window::isInFullscreenMode() { return windowAbstr->isInFullscreenMode(); }

// Mouse
bool W::Window::mouseIsOver() { return windowAbstr->mouseIsOver(); }
void W::Window::getMousePosition(int *x, int *y) { windowAbstr->getMousePosition(x, y); }
void W::Window::setMousePosition(int x, int y) { windowAbstr->setMousePosition(x, y); }

// Events
void W::Window::getEvents() { windowAbstr->getEvents(); }
