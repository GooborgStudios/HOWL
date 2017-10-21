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

#define PLAYBACK_DEBUG 0

namespace HOWL {
	enum SmootherType {
		SMOOTH_HOLD = 0,
		SMOOTH_LINEAR = 1,
		SMOOTH_BEZIER = 2,
		SMOOTH_CONT_BEZIER = 3
	};

	class Layer;

	class Keyframe {
		public:
			std::string name;
			SmootherType smoother;
			long time;

			Keyframe();
			Keyframe(std::string name, long time);
		
			virtual std::string serialize();
			void toBuffer(char *outbuf, int len);
		
			virtual void render(wxDC &canvas, wxRect bounding_box, bool selected = false);
	};

	typedef std::vector<Keyframe *>::iterator KeyframeIterator;

	class DoubleKeyframe: public Keyframe {
		public:
			double value;
			DoubleKeyframe(std::string name, long time, double value);
			DoubleKeyframe(std::string name, long time, float value);
			DoubleKeyframe(std::string name, long time, int value);
			std::string serialize();
	};

	class StringKeyframe: public Keyframe {
		public:
			std::string *value;
			StringKeyframe(std::string name, long time, std::string *value);
			StringKeyframe(std::string name, long time, const char *value);
			std::string serialize();
	};

	class KeyframeSet {
		protected:
			KeyframeIterator prevKF;
			KeyframeIterator nextKF;
			Layer *parent;
		public:
			std::string name;
			std::vector<Keyframe *> keyframes;
			long currentTime;
		
			KeyframeSet(std::string name, Layer *parent);
		
			void AddKeyframe(Keyframe *f);
			std::pair<KeyframeIterator, KeyframeIterator> getSurroundingKeyframes(long time);
			void seek(long newTime);
			bool advanceFrame(long increment);
			bool eof();
			double smoother_fraction();
		
			Keyframe *getFirst();
			Keyframe *getSecond();
	};

	class Layer {
	private:
			KeyframeSet *findSet(std::string);
		public:
			Layer();
			Layer(std::string d);

			std::vector<KeyframeSet *> keyframes;
			std::string description;
			std::string type;
			void AddKeyframe(Keyframe *f);
			void seek(long newTime);
			bool advanceFrame(long increment);
			bool eof();
		
			double getDouble(std::string type);
			std::string *getString(std::string type);
	};

	typedef std::vector<Layer *>::iterator LayerIterator;

	bool is_sooner(Keyframe *a, Keyframe *b);
}
