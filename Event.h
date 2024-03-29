#ifndef WEvent_h
#define WEvent_h

#include <vector>

namespace W {
	
	class Window;
	
	namespace EventType {
		typedef unsigned int T;
		enum {
			// Key
			KeyDown, KeyUp,
			
			// Positional
			LMouseUp, LMouseDown, RMouseUp, RMouseDown, MouseMove, // Mouse
			TouchDown, TouchMoved, TouchUp, TouchCancelled,	       // Touch
			
			// Scroll wheel
			ScrollWheel,
			
			// Windowing
			WinClosed,
			WinResized,
			WinMinimized, WinDeminimized,
			WinBecameKey, WinStoppedBeingKey,
			
			// Application activeness
			AppBecameForeground, AppBecameBackground,
			
			// Other
			Unknown,
			
			Count
		};
	}
	
	namespace KeyCode {
		typedef int T;
		enum {
			_A,	_B,	_C,	_D,
			_E,	_F,	_G,	_H,
			_I,	_J,	_K,	_L,
			_M,	_N,	_O,	_P,
			_Q,	_R,	_S,	_T,
			_U,	_V,	_W,	_X,
			_Y,	_Z,
			
			_0,	_1,	_2,
			_3,	_4,	_5,
			_6,	_7,	_8,	_9,
			
			SPACE,
			
			LEFT_ARROW,	RIGHT_ARROW,
			UP_ARROW,	DOWN_ARROW,
			
			ESC,   RETURN,  BACKSPACE,  _DELETE,  TAB,
			HOME,  END,
			
			UNKNOWN
		};
	}
	
	class Event {
	public:
		
		// Information for different event types is saved in a struct type.
		// To limit object size, the structs are stored in a union - use whichever
		// member of the union is appropriate.
		// (This is based on SFML's event class.)
		
		Event(EventType::T);
		
		struct MouseEvent {
			int x, y;
			Window *window;
		};
		struct KeyEvent {
			KeyCode::T keyCode;
		};
		struct ScrollEvent {
			float dx, dy;
			Window *window;
		};
		struct WindowEvent {
			Window *window;
		};
		struct TouchEvent {
			int x, y;
			int touchID;
		};
		struct UIEvent {
			std::string *element;
		};
		
		EventType::T type;
		
		union {
			KeyEvent    keyEvent;
			MouseEvent  mouseEvent;
			ScrollEvent scrollEvent;
			TouchEvent  touchEvent;
			WindowEvent winEvent;
			UIEvent     uiEvent;
		};
		
		bool is_positional() const;
		
		// Turn event harvesting on/off by setting this property.
		static bool on;
		
		// When event harvesting is on, events are stored in newEvents.
		// Iterate over them in your update cycle, then clear them.
		static std::vector<Event> getNewEvents();
		static void addNewEvent(const W::Event &);
		
		// Register custom event types using registerType():
		// .h:   extern W::EventType::T MyEventType;
		// .cpp: W::EventType::T MyEventType = Event::registerType();
		static EventType::T registerType();
		
		static KeyCode::T charToKeycode(unsigned int c);
		
	private:
		static std::vector<Event> newEvents;
		static unsigned int typecounter;
		
	};
}




#endif
