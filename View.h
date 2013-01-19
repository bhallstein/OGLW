#ifndef __W__View
#define __W__View

#include "Colour.h"
#include "Callback.h"

#include <map>

#define MR_CURRENCY '$'

namespace W {
	
	class Window;
	class Event;
	class Positioner;
	class GameState;
	class DrawnObj;
	
	class View {
	public:
		View(Positioner *);
		~View();
			// If NULL is supplied for positioner, subclass must
			// set one up and call _updatePosition.
		
		void _updatePosition(const size &winsize);
		
		W::EventPropagation::T receiveEvent(Event *);					// Converts event to view’s coordinates, and calls processMouseEvent()
		virtual void processMouseEvent(Event *) { }	// Override to do something with mouse events
		
		void _subscribeToMouseEvents();		// Called by GS in addView()
		void _unsubscribeFromMouseEvents();	// Called by GS in removeView()
		
		// Drawn Object adding/removing/updating
		void addDO(DrawnObj *, int layer = 1);
		void removeDO(DrawnObj *);
		void _markDOAsDirty(DrawnObj *);
		void _updateDOs();
			// Update status of all drawn objects.
			// Graphics mutex must first be locked before this fn is called.
		
		void _draw(const W::size &winSz);
	protected:
		Positioner *_positioner;
		rect rct;
		
		// Drawn Object vectors
		typedef std::vector<DrawnObj*> DO_list;
		std::map<int, DO_list> scene;
		DO_list
			deletedDOs,
			dirtyDOs;
		struct DOAndLayer {
			DOAndLayer(DrawnObj *_DO, int _layer) : DO(_DO), layer(_layer) { }
			DrawnObj *DO;
			int layer;
		};
		std::vector<DOAndLayer> newDOs;
		
		void _removeDO(DrawnObj *);
		
		position _offset;
		
		virtual void updatePosition(const W::size &winsize) { }	// Let subsclasses perform own position update behaviours
		virtual void performOpenGLBackgroundDrawing() { }
	};
	
}

#endif
