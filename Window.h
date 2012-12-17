#ifndef __W_Window
#define __W_Window

#include "types.h"
#include "Colour.h"
#include "Event.h"

#if defined _WIN32 || _WIN64
	#include "Windows.h"
	#include <map>
#endif

namespace W {

	class View;
	
	class Window {
	public:
		Window(const W::size &, const char *_title);
		~Window();
		
		W::size getDimensions();
		
		void setTitle(const std::string &);
		void setTitle(const char *);
		void generateMouseMoveEvent();
		
		void setOpenGLThreadAffinity();
		void clearOpenGLThreadAffinity();
		
#if defined _WIN32 || _WIN64
		LRESULT CALLBACK _WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
		struct NativeObjs;
		void swapBuffers();
		
	protected:
		void createWindow(const W::size &, const char *_title);
		void closeWindow();
		void setUpOpenGL();
		
		struct NativeObjs *_objs;
		
#if defined _WIN32 || _WIN64
		struct _initializer;
		static _initializer *_init;
		static HINSTANCE _appInstance;
		static std::map<UINT, W::EventType::T> _win_event_type_map;
#endif
	};

#if defined _WIN32 || _WIN64
	LRESULT CALLBACK Window_WndProc(HWND, UINT, WPARAM, LPARAM);
#endif
	
}

#endif
