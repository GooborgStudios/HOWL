//
// HOWL - Music-synced animation library
// File: Selection.h
// ©2018 Gooborg Studios: Vinyl Darkscratch, Light Apacha.
// http://www.gooborg.com
//

#pragma once

#ifdef USE_WXWIDGETS
	#include <wx/wxprec.h>
	#ifndef WX_PRECOMP
		#include <wx/wx.h>
	#endif
#endif

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
			Selection();
			Selection(Selection *other);
			void clear();
			SingleSelection *add(KeyframeSet *set, long start, long end);
			void toggle(KeyframeSet *set, long start, long end);
			bool matches(Keyframe *kf);

			std::vector<SingleSelection *> sel;
	};

	#ifdef USE_WXWIDGETS
		class SelectionEvent : public wxEvent {
			public:
				SelectionEvent(wxEventType eventType, wxWindowID winid, SingleSelection selection);
				void forceRefresh();
				bool doRefresh();
				void SetSelection(SingleSelection selection);
				SingleSelection GetSelection() const;
				wxEvent* Clone() const;
			
			private:
				bool should_refresh = false;
				SingleSelection m_selection;
		};
		
		wxDECLARE_EVENT(SELECTION_ON, SelectionEvent);
		wxDECLARE_EVENT(SELECTION_OFF, SelectionEvent);
	#endif
};

#define SelectionEventHandler(func) (&func)
#define EVT_HOWL_SELECTION_ON(id, func) wx__DECLARE_EVT1(HOWL::SELECTION_ON, id, SelectionEventHandler(func))
#define EVT_HOWL_SELECTION_OFF(id, func) wx__DECLARE_EVT1(HOWL::SELECTION_OFF, id, SelectionEventHandler(func))
