//
// HOWL - Music-synced animation library
// File: Layer.cpp
// ©2018 Gooborg Studios: Vinyl Darkscratch, Light Apacha.
// http://www.gooborg.com
//

#include "Layer.h"

#ifdef USE_WXWIDGETS
	#include <wx/wxprec.h>
	#ifndef WX_PRECOMP
		#include <wx/wx.h>
	#endif
#endif

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
#include <iterator>

#include "GooCore/GooCore.h"
#include "Project.h"

using namespace HOWL;

Keyframe::Keyframe() {

}

Keyframe::Keyframe(std::string name, long time) {
	this->name = name;
	this->time = time;

	// set up smoother for linear by default;
	this->smootherBefore[0] = this->smootherAfter[0] = time;
	this->smootherBefore[1] = 1;
	this->smootherAfter[1] = 0;
	this->smootherHold = false;
}

Keyframe::~Keyframe() {

}

bool Keyframe::operator==(Keyframe &a) {
	return false;
}

std::string Keyframe::serialize() {
	return "";
}

void Keyframe::toBuffer(char *outbuf, int len) {
//	snprintf(outbuf, len, "keyframe %s %f %s %s %d\n", this->parent->description.c_str(), this->time, this->name.c_str(), this->serialize().c_str(), this->smoother);
	snprintf(outbuf, len, "");
}

#ifdef USE_WXWIDGETS
	void Keyframe::render(wxDC &canvas, wxRect bounding_box) {
		canvas.SetBrush(*wxBLUE_BRUSH);
		canvas.SetPen(*wxTRANSPARENT_PEN);
		canvas.DrawRectangle(bounding_box.GetLeft()-(bounding_box.GetHeight()/2), bounding_box.GetTop(), bounding_box.GetHeight(), bounding_box.GetHeight());
	}
#endif

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
	this->smootherHold = true;
};

StringKeyframe::StringKeyframe(std::string name, long time, const char *value) : Keyframe(name, time) {
	this->value = new std::string(value);
	this->smootherHold = true;
};

StringKeyframe::~StringKeyframe() {
	delete value;
}

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

void KeyframeSet::AddKeyframe(Keyframe *keyframe, bool do_replace) {
	// If do_replace is true, then replace any keyframes that are already at the specified time, otherwise ignore the new keyframe
	auto iters = getSurroundingKeyframes(keyframe->time);

	if (iters.first == keyframes.end()) {
		keyframes.push_back(keyframe);
	} else if (keyframe->time == (*iters.first)->time) {
		if (do_replace) {
			Keyframe *oldKF = *iters.first;
			*iters.first = keyframe;
			delete oldKF;
		}
	} else if (iters.second == keyframes.end()) {
		keyframes.push_back(keyframe);
	} else {
		keyframes.insert(std::next(iters.first), keyframe);
	}

	seek(keyframe->time);

	if (prevKF == keyframes.end() || (keyframe->time < currentTime && currentTime - keyframe->time < currentTime - (*prevKF)->time)) {
		prevKF = iters.first;
	} else if (nextKF == keyframes.end() || (keyframe->time > currentTime && keyframe->time - currentTime < (*nextKF)->time - currentTime)) {
		nextKF = iters.first;
	}
}

void KeyframeSet::removeKeyframes(KeyframePair keyframepair) {
	std::pair<KeyframeIterator, KeyframeIterator> pair1, pair2;
	pair1 = getSurroundingKeyframes(keyframepair.first->time);
	pair2 = getSurroundingKeyframes(keyframepair.second->time);

	while ((*pair1.first)->time < keyframepair.first->time) {
		pair1.first++;
	}

	keyframes.erase(pair1.first, pair2.second);
}
void KeyframeSet::removeKeyframes(Keyframe *first, Keyframe *second) {
	KeyframePair pair;
	pair.first = first;
	pair.second = second;
	removeKeyframes(pair);
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
	if (nextKF == keyframes.end()) return 0.0;
	
	assert(currentTime >= (*prevKF)->time);
	assert((*nextKF)->time >= currentTime);
	
	double dur = (*nextKF)->time - (*prevKF)->time;
	if (dur == 0) return 0.0; // Avoid divide-by-zero crash
	if ((*prevKF)->smootherHold) return 0.0;

	// Create a cubic spline with 4 control points in 2D using
	// a clamped knot vector. This call is equivalent to:
	// tinyspline::BSpline spline(4, 2, 3, TS_CLAMPED);
	tinyspline::BSpline spline(4);

	// Setup control points.
	std::vector<tinyspline::real> ctrlp = spline.ctrlp();

	// Stores our evaluation results.
	std::vector<tinyspline::real> result;

	ctrlp[0] = (*prevKF)->time; // x0
	ctrlp[1] = 0.0;  // y0
	ctrlp[2] = (*prevKF)->smootherAfter[0];  // x1
	ctrlp[3] = (*prevKF)->smootherAfter[1];  // y1
	ctrlp[4] = (*nextKF)->smootherBefore[0];  // x2
	ctrlp[5] = (*nextKF)->smootherBefore[1];  // y2
	ctrlp[6] = (*nextKF)->time; // x3
	ctrlp[7] = 1.0;  // y3
	spline.setCtrlp(ctrlp);

	result = spline.evaluate(((double)(currentTime-((*prevKF)->time))) / ((*nextKF)->time-(*prevKF)->time)).result();
	return result[1];
//	return (currentTime - (*prevKF)->time) / dur; // XXX Convert to handle Bezier!
}

std::pair<KeyframeIterator, KeyframeIterator> KeyframeSet::getSurroundingKeyframes(long time) {
	int before = 0, after = keyframes.size();
	int i;

	// we've already found them, or there are no keyframes
	if (currentTime == time || after == 0) return std::pair<KeyframeIterator, KeyframeIterator>(prevKF, nextKF);
	
	while (after - before > 1) {
		i = before + (after-before) / 2;
		if (keyframes[i]->time > time) after = i;
		else if (i+1 >= after) before = after - 1;
		else if (keyframes[i+1]->time <= time) before = i;
		else if (keyframes[i]->time <= time) {
			before = i;
			after = i+1;
		};
	}
	
	return std::pair<KeyframeIterator, KeyframeIterator>(keyframes.begin()+before, keyframes.begin()+after);

}

Layer::Layer() {
	description = "";
};

Layer::Layer(std::string description) {
	this->description = description;
}

std::vector<std::string> Layer::getSetNames() {
	std::vector<std::string> names;
	for (KeyframeSet *set : keyframes) names.push_back(set->name);
	return names;
}

KeyframeSet *Layer::findSet(std::string name) {
	for (auto kf: keyframes) {
		if (kf->name == name) return kf;
	}
	return NULL;
}

KeyframePair Layer::getSurroundingKeyframes(std::string name) {
	KeyframePair value;
	KeyframeSet *set = findSet(name);
	if (set == NULL) return KeyframePair(NULL, NULL);

	value.first = set->prevKF == set->keyframes.end() ? NULL : (*set->prevKF);
	value.second = set->nextKF == set->keyframes.end() ? NULL : (*set->nextKF);

	return value;
}

KeyframePair Layer::getSurroundingKeyframes(std::string name, long time) {
	KeyframePair value;

	KeyframeSet *set = findSet(name);
	if (set == NULL) return KeyframePair(NULL, NULL);

	std::pair<KeyframeIterator, KeyframeIterator> kf = set->getSurroundingKeyframes(time);
	value.first = kf.first == set->keyframes.end() ? NULL : (*kf.first);
	value.second = kf.second == set->keyframes.end() ? NULL : (*kf.second);

	return value;
}

void Layer::AddKeyframe(Keyframe *keyframe) {
	KeyframeSet *found = findSet(keyframe->name);
	if (!found) {
		found = new KeyframeSet(keyframe->name, this);
		keyframes.push_back(found);
	}
	found->AddKeyframe(keyframe);
}

void Layer::removeKeyframes(KeyframePair keyframepair) {
	KeyframeSet *set = findSet(keyframepair.first->name);
	assert(set);
	set->removeKeyframes(keyframepair);
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

double Layer::getDouble(std::string name) {
	KeyframeSet *set = findSet(name);
	KeyframePair value = getSurroundingKeyframes(name);

	if (!value.first) return 0.0;
	if (!value.second) return ((DoubleKeyframe *)(value.first))->value;
	double pos = set->smoother_fraction();

	return (((DoubleKeyframe *)(value.first))->value * (1 - pos))
		+ (((DoubleKeyframe *)(value.second))->value * pos);
}


std::string *Layer::getString(std::string name) {
	KeyframePair value = getSurroundingKeyframes(name);
	if (!value.first) return new std::string("");
	return ((StringKeyframe *)(value.first))->value;
}

bool is_sooner(Keyframe *a, Keyframe *b) {
	return a->time < b->time;
}
