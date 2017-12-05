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
			Keyframe();
			Keyframe(std::string name, long time);
			virtual ~Keyframe();
			virtual std::string serialize();
			void toBuffer(char *outbuf, int len);
			virtual void render(wxDC &canvas, wxRect bounding_box);

			std::string name;
			SmootherType smoother;
			long time;
	};

	typedef std::vector<Keyframe *>::iterator KeyframeIterator;
	typedef std::pair<Keyframe *, Keyframe *> KeyframePair;

	class EXPORT DoubleKeyframe: public Keyframe {
		public:
			DoubleKeyframe(std::string name, long time, double value);
			DoubleKeyframe(std::string name, long time, float value);
			DoubleKeyframe(std::string name, long time, int value);
			std::string serialize();

			double value;
	};

	class EXPORT StringKeyframe: public Keyframe {
		public:
			StringKeyframe(std::string name, long time, std::string *value);
			StringKeyframe(std::string name, long time, const char *value);
			~StringKeyframe();
			std::string serialize();

			std::string *value;
	};

	class EXPORT KeyframeSet {
		public:
			KeyframeSet(std::string name, Layer *parent);
			void AddKeyframe(Keyframe *f, bool do_replace = true);
			void removeKeyframes(KeyframePair keyframepair);
			void removeKeyframes(Keyframe *first, Keyframe *second);
			void seek(long newTime);
			bool advanceFrame(long increment);
			bool eof();
			double smoother_fraction();

			std::vector<Keyframe *> keyframes; // XXX make me protected
			std::string name;
			long currentTime;

			friend class Layer;

		protected:
			std::pair<KeyframeIterator, KeyframeIterator> getSurroundingKeyframes(long time);

			KeyframeIterator prevKF;
			KeyframeIterator nextKF;
			Layer *parent;
	};

	class EXPORT Layer {
		public:
			Layer();
			Layer(std::string d);
			std::vector<std::string> getSetNames();
			KeyframeSet *findSet(std::string type);
			KeyframePair getSurroundingKeyframes(std::string name);
			KeyframePair getSurroundingKeyframes(std::string name, long time);
			void AddKeyframe(Keyframe *f);
			void removeKeyframes(KeyframePair keyframepair);
			void seek(long newTime);
			bool advanceFrame(long increment);
			bool eof();
			double getDouble(std::string name);
			std::string *getString(std::string name);

			std::string description;
			std::string type;

		protected:
			std::vector<KeyframeSet *> keyframes;
	};

	typedef std::vector<Layer *>::iterator LayerIterator;

	EXPORT bool is_sooner(Keyframe *a, Keyframe *b);
}
