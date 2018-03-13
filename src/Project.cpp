//
// HOWL - Music-synced animation library
// File: Project.cpp
// ©2018 Gooborg Studios: Vinyl Darkscratch, Light Apacha.
// http://www.gooborg.com
//

#include "Project.h"

#ifdef USE_WXWIDGETS
	#include <wx/wxprec.h>
	#ifndef WX_PRECOMP
		#include <wx/wx.h>
	#endif
#endif

#include <vector>
#include <string>

#include "Layer.h"
#include "Selection.h"

using namespace HOWL;

#ifdef USE_WXWIDGETS
	wxDEFINE_EVENT(HOWL::PROJECT_SAVED, wxCommandEvent);
	wxDEFINE_EVENT(HOWL::PROJECT_LOADED, wxCommandEvent);
#endif

void Project::seek(long newTime) {
	currentTime = newTime;
	for (auto layer : layers) layer->seek(newTime);
}

bool Project::advanceFrame(long increment) {
	bool eof = true;
	currentTime += increment;
	for (auto layer : layers) {
		if (layer->advanceFrame(increment) == false) eof = false;
	}
	return eof;
}

bool Project::eof() {
	bool eof = true;
	for (auto layer : layers) {
		if (layer->eof() == false) eof = false;
	}
	return eof;
}
