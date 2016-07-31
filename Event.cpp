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
