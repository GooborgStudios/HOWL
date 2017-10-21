//
// HOWL - Music-synced animation library
// File: Layer.cpp
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#include "Layer.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <sys/time.h>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <map>
#include <cmath>
#include <cassert>
#include <utility>

#include "NightwaveCore/Helpers.h"
#include "Project.h"

using namespace HOWL;

Keyframe::Keyframe() {

}

Keyframe::Keyframe(std::string name, long time) {
	this->name = name;
	this->time = time;
	this->smoother = SMOOTH_LINEAR;
}

std::string Keyframe::serialize() {
	return "";
}

void Keyframe::toBuffer(char *outbuf, int len) {
//	snprintf(outbuf, len, "keyframe %s %f %s %s %d\n", this->parent->description.c_str(), this->time, this->name.c_str(), this->serialize().c_str(), this->smoother);
	snprintf(outbuf, len, "");
}

void Keyframe::render(wxDC &canvas, wxRect bounding_box, bool selected) {
	
}

DoubleKeyframe::DoubleKeyframe(std::string name, long time, double value) : Keyframe(name, time) {
	this->value = value;
};

DoubleKeyframe::DoubleKeyframe(std::string name, long time, float value) : Keyframe(name, time) {
	this->value = (double)value;
};

DoubleKeyframe::DoubleKeyframe(std::string name, long time, int value) : Keyframe(name, time) {
	this->value = (double)value;
};

std::string DoubleKeyframe::serialize() {
	std::stringstream stream;
	stream << value;
	return stream.str();
}

StringKeyframe::StringKeyframe(std::string name, long time, std::string *value) : Keyframe(name, time) {
	this->value = value;
	this->smoother = SMOOTH_HOLD;
};

StringKeyframe::StringKeyframe(std::string name, long time, const char *value) : Keyframe(name, time) {
	this->value = new std::string(value);
	this->smoother = SMOOTH_HOLD;
};

std::string StringKeyframe::serialize() {
	return *value;
}

KeyframeSet::KeyframeSet(std::string name, Layer *parent) {
	this->name = name;
	this->parent = parent;
	this->currentTime = -1;
	this->prevKF = keyframes.end();
	this->nextKF = keyframes.end();
}

void KeyframeSet::AddKeyframe(Keyframe *keyframe) {
	auto iters = getSurroundingKeyframes(keyframe->time);

	if (iters.second == keyframes.end()) {
		keyframes.push_back(keyframe);
	} else {
		if (keyframe->time == (*iters.first)->time) {
			Keyframe *oldKF = *iters.first;
			*iters.first = keyframe;
			delete oldKF;
		} else {
			int kftime = (*nextKF)->time;
			int oldcapacity = keyframes.capacity();
			keyframes.insert(iters.first, keyframe);
			if (kftime >= keyframe->time || oldcapacity != keyframes.capacity()) seek(keyframe->time);
		}
	}
	
	seek(keyframe->time);
	
	if (prevKF == keyframes.end() || (keyframe->time < currentTime && currentTime - keyframe->time < currentTime - (*prevKF)->time)) {
		prevKF = iters.first;
	} else if (nextKF == keyframes.end() || (keyframe->time > currentTime && keyframe->time - currentTime < (*nextKF)->time - currentTime)) {
		nextKF = iters.first;
	}
}

std::pair<KeyframeIterator, KeyframeIterator> KeyframeSet::getSurroundingKeyframes(long time) {
	int before = 0, after = keyframes.size();
	int i;
	
	if (currentTime == time || after == 0) return std::pair<KeyframeIterator, KeyframeIterator>(prevKF, nextKF);
	
	while (after - before > 1) {
		i = before + (after-before) / 2;
		if (keyframes[i]->time > time) after = i+1;
		else if (i+1 >= after) break;
		else if (keyframes[i+1]->time <= time) before = i;
		else if (keyframes[i]->time <= time) break;
	}
	
	return std::pair<KeyframeIterator, KeyframeIterator>(keyframes.begin()+before, keyframes.begin()+after);

}

#define USE_STUPID_SEEK

#ifdef USE_STUPID_SEEK
void KeyframeSet::seek(long newTime) {
	currentTime = 0;
	prevKF = keyframes.begin();
	if (prevKF != keyframes.end()) {
		nextKF = std::next(prevKF, 1);
	} else {
		nextKF = keyframes.end();
	}
	advanceFrame(newTime);
}
#else
void KeyframeSet::seek(long newTime) {
	std::pair<KeyframeIterator,KeyframeIterator> iters = getSurroundingKeyframes(newTime);

	prevKF = iters.first;
	nextKF = iters.second;
	currentTime = newTime;
}
#endif

bool KeyframeSet::advanceFrame(long increment) {
	currentTime += increment;
	while (nextKF != keyframes.end() && currentTime >= (*nextKF)->time) {
		++prevKF;
		++nextKF;
	}
	return eof();
}

bool KeyframeSet::eof() {
	if (nextKF == keyframes.end()) return true;
	return false;
}

double KeyframeSet::smoother_fraction() {
	// XXX Only does Linear smoothing, add other smoother types
	if (nextKF == keyframes.end()) return 0.0;
	
	assert(currentTime >= (*prevKF)->time);
	assert((*nextKF)->time >= currentTime);
	
	double dur = (*nextKF)->time - (*prevKF)->time;
	if (dur == 0) return 0.0; // Avoid divide-by-zero crash
	return (currentTime - (*prevKF)->time) / dur;
}

Keyframe *KeyframeSet::getFirst() {
	if (prevKF == keyframes.end()) return NULL;
	return *prevKF;
}

Keyframe *KeyframeSet::getSecond() {
	if (nextKF == keyframes.end()) return NULL;
	return *nextKF;
}

Layer::Layer() {
	description = "";
};

Layer::Layer(std::string description) {
	this->description = description;
}

KeyframeSet *Layer::findSet(std::string name) {
	for (auto kf: keyframes) {
		if (kf->name == name) return kf;
	}
	return NULL;
}

void Layer::AddKeyframe(Keyframe *keyframe) {
	KeyframeSet *found = findSet(keyframe->name);
	if (!found) {
		found = new KeyframeSet(keyframe->name, this);
		keyframes.push_back(found);
	}
//	if (keyframes.find(type) == keyframes.end()) keyframes[type] = new KeyframeSet(this);
	found->AddKeyframe(keyframe);
}

void Layer::seek(long newTime) {
	for (auto iter : keyframes) iter->seek(newTime);
}

bool Layer::advanceFrame(long increment) {
	bool eof = true;
	for (auto iter : keyframes) {
		if (iter->advanceFrame(increment) == false) eof = false;
	}
	return eof;
}

bool Layer::eof() {
	bool eof = true;
	for (auto iter : keyframes) {
		if (iter->eof() == false) eof = false;
	}
	return eof;
}

double Layer::getDouble(std::string type) {
	KeyframeSet *set = findSet(type);
	if (set == NULL) return 0.0;
	
	DoubleKeyframe *KF1 = (DoubleKeyframe *)(set->getFirst());
	DoubleKeyframe *KF2 = (DoubleKeyframe *)(set->getSecond());
	
	if (KF1 == NULL) return 0.0;
	if (KF2 == NULL) return KF1->value;
	double pos = set->smoother_fraction();
	return KF1->value - (KF1->value * pos) + (KF2->value * pos); // XXX Simplify me (to get a good grade)!
}

std::string *Layer::getString(std::string type) {
	KeyframeSet *set = findSet(type);
	if (set == NULL || set->getFirst() == NULL) return new std::string("");
	return ((StringKeyframe *)(set->getFirst()))->value;
}

bool is_sooner(Keyframe *a, Keyframe *b) {
	return a->time < b->time;
}
