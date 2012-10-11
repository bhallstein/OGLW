#include "W.h"
#include <iostream>
#include <ctime>

#ifdef __APPLE__
	#include <Cocoa/Cocoa.h>
	#include "ObjC-Classes.h"
#elif defined WIN32 || WIN64
	#include "shlobj.h"
#endif

#include "Messenger.h"

namespace W {
	std::string   logFilePath, logFileName;
	std::ofstream log;

	MTRand_int32  twister;
	
	std::vector<GameState*> _gs;
	std::vector<Event>      _evqueue;
	std::vector<Window*>    _windows;
	W::Returny _returny(ReturnyType::EMPTY_RETURNY);
	bool  _gsShouldPop;
	bool  _quit = false;
	void* _demon;
	
	int W_INFINITY = 99999999;
	int W_MAXPATH = 300;
}

struct W::_init {
	_init() {
		// Default log: /dev/null
		std::string p;
		#ifdef __APPLE__
			p = "/dev/null";
		#elif defined WIN32 || WIN64
			p = "\\Device\\Null";
		#endif
		setLogFile(p.c_str());
		
		// Init MTRand
		time_t timey;
		time(&timey);
		twister.seed(timey);
		
		// Init the Demon (runs the gameloop timer)
		#ifdef __APPLE__
			_demon = (void*) [[W_Demon alloc] init];
		#elif defined WIN32 || WIN64
			// ...
		#endif

		W::log << "W app inited" << std::endl;
	}
};
struct W::_init *W::_initializer = new W::_init();

void W::setLogFile(const char *path) {
    if (log.is_open()) log.close();
    log.open(path);
}

unsigned int W::randUpTo(int x) {
	if (x == 0) return 0;
	return twister()%x;
}

void W::pushState(GameState *g) {
	for (std::vector<GameState*>::iterator it = _gs.begin(); it < _gs.end(); it++)
		if (*it == g) return;
//	if (!states.empty()) states.back()->pause();
	_gs.push_back(g);
	Messenger::_setActiveGamestate(g);
}
void W::popState(W::Returny &r) {
	_gsShouldPop = true;
	_returny = r;
}
void W::_performPop() {
	do {
		_gsShouldPop = false;
		GameState *last_gs = _gs.back();
		delete _gs.back();
		Messenger::_gamestateDestroyed(last_gs);
		_gs.pop_back();
		if (_gs.size()) {
			GameState *new_gs = _gs.back();
			Messenger::_setActiveGamestate(new_gs);
			new_gs->resume(&_returny);
		}
	} while (_gsShouldPop && _gs.size());
}

void W::_addEvent(const Event &ev) {
	_evqueue.push_back(ev);
}

#if defined WIN32 || WIN64
VOID CALLBACK W::TimerProc(HWND windowHandle, UINT msg, UINT_PTR idEvent, DWORD dwTime) {
	W::_runGameLoop();
}
#endif
void W::startGameLoop() {
	#ifdef __APPLE__
		[( W_Demon*)_demon gametimerStart];
	#elif defined WIN32 || WIN64
		SetTimer(NULL, 0, 33, (TIMERPROC) W::TimerProc);
	#endif
}
void W::stopGameLoop() {
	#ifdef __APPLE__
		[( W_Demon*)_demon gametimerStop];
	#elif defined WIN32 || WIN64
		KillTimer(NULL, 0);
	#endif
}
void W::_runGameLoop() {
	_windows[0]->_generateMouseMoveEvent();	// TODO: only generate if within window bounds (plus some border)
	GameState *g;
	_gsShouldPop = false;
	
	// Send events
	if (_gs.size()) {
		g = _gs.back();
		for (std::vector<Event>::iterator it = _evqueue.begin(); it < _evqueue.end(); it++) {
			Event &ev = *it;
			if (ev.type == EventType::CLOSED)
				g->handleCloseEvent();
			else
				Messenger::dispatchEvent(&ev);
			if (_gsShouldPop) break;
		}
		_evqueue.clear();
	}
	
	// Check for poppage due to event input
	if (_gsShouldPop) {
		_performPop();
		_gsShouldPop = false;
	}
	
	// Update if no poppage
	else if (_gs.size()) {
		g = _gs.back();
		g->update();
	}
	
	// Check for poppage due to update
	if (_gsShouldPop) {
		_performPop();
		_gsShouldPop = false;
	}
	
	// Draw
	if (_gs.size()) {
		g = _gs.back();
		std::map<Window*, std::list<View*> > *_wv = g->_getViews();
		for (std::map<Window*, std::list<View*> >::iterator it = _wv->begin(); it != _wv->end(); it++) {
			it->first->_startDrawing();
			std::list<View*> *l = &it->second;
			for (std::list<View*>::iterator itl = l->begin(); itl != l->end(); itl++)
				(*itl)->_draw();
			it->first->_finishDrawing();
		}
	}
	else {
		_quit = true;
		stopGameLoop();
		#ifdef __APPLE__
			[NSApp performSelector:@selector(terminate:) withObject:nil afterDelay:0];
		#elif defined WIN32 || WIN64
			// Need to monitor _quit in WinMain
		#endif
	}
}

void W::_updateAllViewPositions() {
	for (std::vector<GameState*>::iterator itgs = _gs.begin(); itgs < _gs.end(); itgs++) {
		std::map<Window*, std::list<View*> > *wv = (*itgs)->_getViews();
		for (std::map<Window*, std::list<View*> >::iterator itwv = wv->begin(); itwv != wv->end(); itwv++) {
			size s = itwv->first->_getDimensions();
			std::list<View*> *l = &itwv->second;
			for (std::list<View*>::iterator itl = l->begin(); itl != l->end(); itl++)
				(*itl)->_updatePosition(s);
		}
	}
}
