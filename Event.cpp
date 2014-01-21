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

#pragma mark - Constructor

W::Event::Event(W::EventType::T _type) : type(_type) { }


#pragma mark - Statics

bool W::Event::on = false;
std::vector<W::Event> W::Event::newEvents;
unsigned int W::Event::typecounter = 100;

W::EventType::T W::Event::registerType() {
	return typecounter++;
}

W::KeyCode::T W::Event::charToKeycode(unsigned int c) {
	if (c == ' ') return KeyCode::SPACE;
	if (c >= 'a' && c <= 'z') return (KeyCode::T) ((int)KeyCode::_A + c - 'a');
	if (c >= 'A' && c <= 'Z') return (KeyCode::T) ((int)KeyCode::_A + c - 'A');
	if (c >= '0' && c <= '9') return (KeyCode::T) ((int)KeyCode::_0 + c - '0');
	if (c == 27) return KeyCode::ESC;		// Standard ASCII codes
	if (c == 13) return KeyCode::RETURN;		//
	if (c == 8)  return KeyCode::BACKSPACE;	//
	if (c == 9)  return KeyCode::TAB;		//
	#ifdef WTARGET_MAC
		if (c == NSLeftArrowFunctionKey)  return KeyCode::LEFT_ARROW;
		if (c == NSRightArrowFunctionKey) return KeyCode::RIGHT_ARROW;
		if (c == NSUpArrowFunctionKey)    return KeyCode::UP_ARROW;
		if (c == NSDownArrowFunctionKey)  return KeyCode::DOWN_ARROW;
	#endif
	return KeyCode::K_OTHER;
}
