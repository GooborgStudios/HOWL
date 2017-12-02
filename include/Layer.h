//
// HOWL - Music-synced animation library
// File: Layer.h
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include "NightwaveCore/NightwaveCore.h"

#define PLAYBACK_DEBUG 0

namespace HOWL {
	enum SmootherType {
		SMOOTH_HOLD = 0,
		SMOOTH_LINEAR = 1,
		SMOOTH_BEZIER = 2,
		SMOOTH_CONT_BEZIER = 3
	};

	class Layer;

	class EXPORT Keyframe {
		public:
			std::string name;
			SmootherType smoother;
			long time;

			Keyframe();
			Keyframe(std::string name, long time);
		
			virtual std::string serialize();
			void toBuffer(char *outbuf, int len);
		
			virtual void render(wxDC &canvas, wxRect bounding_box);
	};

	typedef std::vector<Keyframe *>::iterator KeyframeIterator;
	typedef std::pair<Keyframe *, Keyframe *> KeyframePair;

	class EXPORT DoubleKeyframe: public Keyframe {
		public:
			double value;
			DoubleKeyframe(std::string name, long time, double value);
			DoubleKeyframe(std::string name, long time, float value);
			DoubleKeyframe(std::string name, long time, int value);
			std::string serialize();
	};

	class EXPORT StringKeyframe: public Keyframe {
		public:
			std::string *value;
			StringKeyframe(std::string name, long time, std::string *value);
			StringKeyframe(std::string name, long time, const char *value);
			std::string serialize();
	};

	class EXPORT KeyframeSet {
		protected:
			KeyframeIterator prevKF;
			KeyframeIterator nextKF;
			Layer *parent;
			std::pair<KeyframeIterator, KeyframeIterator> getSurroundingKeyframes(long time);
	public:
			std::vector<Keyframe *> keyframes; // XXX make me protected
			std::string name;
			long currentTime;
		
			KeyframeSet(std::string name, Layer *parent);
		
			void AddKeyframe(Keyframe *f, bool do_replace = true);
			void seek(long newTime);
			bool advanceFrame(long increment);
			bool eof();
			double smoother_fraction();
		
			Keyframe *getFirst();
			Keyframe *getSecond();
	};

	class EXPORT Layer {
		public:
			Layer();
			Layer(std::string d);

			std::vector<KeyframeSet *> keyframes;
			std::string description;
			std::string type;
			KeyframeSet *findSet(std::string);
			void AddKeyframe(Keyframe *f);
			void seek(long newTime);
			bool advanceFrame(long increment);
			bool eof();
		
			double getDouble(std::string type);
			std::string *getString(std::string type);
	};

	typedef std::vector<Layer *>::iterator LayerIterator;

	EXPORT bool is_sooner(Keyframe *a, Keyframe *b);
}
