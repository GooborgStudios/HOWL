//
// HOWL - Music-synced animation library
// File: Project.cpp
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#include "Project.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>
#include <string>

#include "Layer.h"

using namespace HOWL;

Selection::Selection() {
	clear();
}

void Selection::clear() {
	sel.clear();
}

void Selection::add(KeyframeSet *set, long start, long end) {
	SingleSelection *newsel = new SingleSelection;
	newsel->set = set;
	newsel->start = start;
	newsel->end = end;
	sel.push_back(newsel);
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
