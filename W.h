#ifndef __W__W
#define __W__W

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "mtrand.h"
#include "types.h"

#ifdef __APPLE__
	#include <pthread.h>
#elif defined WIN32 || WIN64
	#include "Windows.h"
#endif

namespace W {
	
	class GameState;
	class Returny;
	class Event;
	class Window;
	class Texture;
	
	/* Logging */
	extern std::ofstream log;
	void setLogFile(const char *);
	void setLogFile(const std::string &);
	
	/* Mersenne Twister */
	unsigned int randUpTo(int);
	extern MTRand_int32 twister;
	
	/* GameState management */
	void pushState(GameState *);
	void popState(Returny &);
	void _performPop();
	extern std::vector<GameState*> _gs;
	extern bool _gsShouldPop;
	extern Returny _returny;
	
	/* Events */
	extern std::vector<Event> _events;	// Outstanding events,
	void _addEvent(const Event &);		// not yet sent to game
	
	/* Texturing */
	extern std::vector<Texture*> _textures_to_upload;
	extern std::vector<unsigned int> _textures_to_unload;
		// Must lock the texture mutex before using these
	
	/* Window */
	void createWindow(const W::size &winSize, const char *winTitle);
	extern Window *_window;
	void _updateAllViewPositions();
	
	/* Drawing thread */
	#ifdef __APPLE__
		void* drawingThreadFn(void *);
		extern pthread_t _drawingThread;
	#elif defined __WIN32 || __WIN64
		DWORD WINAPI drawingThreadFn(LPVOID);
		extern DWORD _drawingThreadId;
		extern HANDLE _drawingThreadHandle;
	#endif
	
	/* Update */
	#ifdef __APPLE__
		extern void *_updateTimer;
	#elif defined __WIN32 || __WIN64
		extern UINT _updateTimer;
		void CALLBACK _mmtCallback(UINT, UINT, DWORD, DWORD, DWORD);
	#endif
	void _updateTimerStart();
	void _updateTimerStop();
	void _update();
	
	/* Start the game */
	void start();
	
	/* Quit */
	extern bool _quit;
	void quit();
	
	/* Mutexes */
	#ifdef __APPLE__
		extern pthread_mutex_t event_mutex, graphics_mutex, texture_mutex;
		inline void _lock_mutex(pthread_mutex_t *m) {
			if (int err = pthread_mutex_lock(m))
				throw W::Exception("Error locking mutex", err);
		}
		inline void _unlock_mutex(pthread_mutex_t *m) {
			if (int err = pthread_mutex_unlock(m))
				throw W::Exception("Error unlocking mutex", err);
		}
	#elif defined __WIN32 || __WIN64
		extern CRITICAL_SECTION event_mutex, graphics_mutex, texture_mutex;
		inline void _lock_mutex(CRITICAL_SECTION *m) {
			EnterCriticalSection(m);
		}
		inline void _unlock_mutex(CRITICAL_SECTION *m) {
			LeaveCriticalSection(m);
		}
	#endif
	
	/* Bollocks */
	extern int W_INFINITY;
	extern int W_MAXPATH;
	
	struct _init;
	extern _init *_initializer;

}

#include "types.h"
#include "Colour.h"
#include "Event.h"
#include "Window.h"
#include "View.h"
#include "UIView.h"
#include "Positioner.h"
#include "Messenger.h"
#include "GameState.h"
#include "Callback.h"
#include "MisterHeapy.h"
#include "NavMap.h"
#include "Texture.h"
#include "Timer.h"

#endif
