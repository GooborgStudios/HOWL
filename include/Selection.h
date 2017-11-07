//
// HOWL - Music-synced animation library
// File: Selection.h
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#pragma once

#include <vector>

#include "Layer.h"

namespace HOWL {
	struct SingleSelection {
		KeyframeSet *set;
		long start;
		long end;
	};
	
	class Selection {
		public:
			std::vector<SingleSelection *> sel;
		
			Selection();
			Selection(Selection *other);

			void clear();
			SingleSelection *add(KeyframeSet *set, long start, long end);
			void toggle(KeyframeSet *set, long start, long end);
			bool matches(Keyframe *kf);
	};
	
	class SelectionEvent : public wxEvent {
		public:
			SelectionEvent(wxEventType eventType, wxWindowID winid, SingleSelection selection);
		
			void SetSelection(SingleSelection selection);
			SingleSelection GetSelection() const;
			wxEvent* Clone() const;
		
		private:
			SingleSelection m_selection;
	};
	
	wxDECLARE_EVENT(SELECTION_ON, SelectionEvent);
	wxDECLARE_EVENT(SELECTION_OFF, SelectionEvent);
};

#define SelectionEventHandler(func) (&func)
#define EVT_HOWL_SELECTION_ON(id, func) wx__DECLARE_EVT1(HOWL::SELECTION_ON, id, SelectionEventHandler(func))
#define EVT_HOWL_SELECTION_OFF(id, func) wx__DECLARE_EVT1(HOWL::SELECTION_OFF, id, SelectionEventHandler(func))
