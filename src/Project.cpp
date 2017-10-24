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
	set = NULL;
	start = 0;
	end = 0;
}

void Selection::add(KeyframeSet *set, long start, long end) {
	this->set = set;
	this->start = start;
	this->end = end;
}

void Selection::toggle(KeyframeSet *set, long start, long end) {
	// XXX Implement me!
}

bool Selection::matches(Keyframe *kf) {
    if (set == NULL || !set) return false;
    return (set->name == kf->name
        && start <= kf->time
        && end > kf->time);
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
