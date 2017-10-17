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
	keyframe = NULL;
	start = 0;
	end = 0;
}

Selection::Selection(Keyframe *kf, long start, long end) {
	keyframe = kf;
	this->start = start;
	this->end = end;
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

void Project::clearSelection() {
	if (selection) {
		delete selection;
	}
	selection = NULL;
}

void Project::addSelection(Keyframe *kf, long start, long end) {
	// XXX multiple selection will clobber and leak
	selection = new Selection(kf, start, end);
}

Selection *Project::getSelection() {
	return selection;
}
