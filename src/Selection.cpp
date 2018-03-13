//
// HOWL - Music-synced animation library
// File: Selection.cpp
// ©2018 Gooborg Studios: Vinyl Darkscratch, Light Apacha.
// http://www.gooborg.com
//

#include "Selection.h"

#ifdef USE_WXWIDGETS
	#include <wx/wxprec.h>
	#ifndef WX_PRECOMP
		#include <wx/wx.h>
	#endif
#endif

#include <vector>

#include "Layer.h"

using namespace HOWL;

#ifdef USE_WXWIDGETS
	wxDEFINE_EVENT(HOWL::SELECTION_ON, HOWL::SelectionEvent);
	wxDEFINE_EVENT(HOWL::SELECTION_OFF, HOWL::SelectionEvent);
#endif

// XXX Rename me to SelectionSet!
Selection::Selection() {
	clear();
}

Selection::Selection(Selection *other) {
	clear();
	sel = std::vector<SingleSelection *>(other->sel);
}

void Selection::clear() {
	sel.clear();
}

SingleSelection *Selection::add(KeyframeSet *set, long start, long end) {
	SingleSelection *newsel = new SingleSelection;
	newsel->set = set;
	newsel->start = start;
	newsel->end = end;
	sel.push_back(newsel);

	return newsel;
}

void Selection::toggle(KeyframeSet *set, long start, long end) {
	// XXX Implement me!
}

bool Selection::matches(Keyframe *kf) {
	for (SingleSelection *s : sel) {
		if (s->set == NULL || !s->set) continue;
		if (s->set->name == kf->name
			&& s->start <= kf->time
			&& s->end > kf->time) return true;
	}
	
	return false;
}

#ifdef USE_WXWIDGETS
SelectionEvent::SelectionEvent(wxEventType eventType, wxWindowID winid, SingleSelection selection) : wxEvent(winid, eventType), m_selection(selection) {

};

void SelectionEvent::forceRefresh() {
	should_refresh = true;
}

bool SelectionEvent::doRefresh() {
	return should_refresh;
}

void SelectionEvent::SetSelection(SingleSelection selection) {
	m_selection = selection;
}

SingleSelection SelectionEvent::GetSelection() const {
	return m_selection;
}

wxEvent* SelectionEvent::Clone() const {
	return new SelectionEvent(*this);
}
#endif

