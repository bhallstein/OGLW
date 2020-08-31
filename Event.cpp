/*
 * W - simple cross-platform OpenGL windows
 *
 * =============
 *  Event.cpp
 * =============
 *
 * Copyright (C) 2012 - Ben Hallstein - http://ben.am
 * Published under the MIT license: http://opensource.org/licenses/MIT
 *
 */

#include "Event.h"
#include "WInt_PlatformIdentification.h"
#include "Cocoa/Cocoa.h"
#include "Window.h"

#pragma mark - Constructor

W::Event::Event(W::EventType::T _type) : type(_type) { }


#pragma mark - Statics

bool W::Event::on = false;
std::vector<W::Event> W::Event::newEvents;
unsigned int W::Event::typecounter = W::EventType::Count;

W::EventType::T W::Event::registerType() {
	return typecounter++;
}

W::KeyCode::T W::Event::charToKeycode(unsigned int c) {
	if (c == ' ') return KeyCode::SPACE;
	if (c >= 'a' && c <= 'z') return (KeyCode::T) ((int)KeyCode::_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (KeyCode::T) ((int)KeyCode::_A + c - 'A');
	if (c >= '0' && c <= '9') return (KeyCode::T) ((int)KeyCode::_0 + c - '0');
	if (c == 27)  return KeyCode::ESC;        // 
	if (c == 13)  return KeyCode::RETURN;     // Standard ASCII codes
	if (c == 8)   return KeyCode::BACKSPACE;  //
	if (c == 9)   return KeyCode::TAB;        //
	if (c == 127) return KeyCode::_DELETE;    //
	#ifdef WTARGET_MAC
		if (c == NSLeftArrowFunctionKey)  return KeyCode::LEFT_ARROW;
		if (c == NSRightArrowFunctionKey) return KeyCode::RIGHT_ARROW;
		if (c == NSUpArrowFunctionKey)    return KeyCode::UP_ARROW;
		if (c == NSDownArrowFunctionKey)  return KeyCode::DOWN_ARROW;
	#endif
	return KeyCode::UNKNOWN;
}

bool W::Event::is_positional() const {
	return (type >= EventType::LMouseUp && type <= EventType::TouchCancelled);
}

void W::Event::addNewEvent(const W::Event &ev) {
	newEvents.push_back(ev);
}

// Mousemove event tracking
int mx_prev = -10000;
int my_prev = -10000;
W::Window *win_prev = NULL;

std::vector<W::Event> W::Event::getNewEvents() {
	auto _newEvents = newEvents;
	newEvents.clear();

	// Generate mousemove event
	auto windows = Window::windows();
	if (windows.size() > 0) {
		if (windows[0] != win_prev) {
			mx_prev = mx_prev = -10000;
			win_prev = windows[0];
		}
		else {
			int mx;
			int my;
			Window *win = windows[0];
			win->getMousePosition(&mx, &my);
			
			int dx = mx - mx_prev;
			int dy = my - my_prev;
			if (!(dx == 0 && dy == 0)) {
				W::Event ev_mm(W::EventType::MouseMove);
				ev_mm.mouseEvent.x = mx;
				ev_mm.mouseEvent.y = my;
				
				_newEvents.push_back(ev_mm);
			}
			
			mx_prev = mx;
			my_prev = my;
			win_prev = win;
		}
	}
	
	return _newEvents;
}
