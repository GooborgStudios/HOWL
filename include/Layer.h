//
// HOWL - Music-synced animation library
// File: Layer.h
// ©2018 Gooborg Studios: Vinyl Darkscratch, Light Apacha.
// http://www.gooborg.com
//

#pragma once

#ifdef USE_WXWIDGETS
	#include <wx/wxprec.h>
	#ifndef WX_PRECOMP
		#include <wx/wx.h>
	#endif
#endif

#include <string>
#include <algorithm>
#include <vector>
#include <map>
#include <utility>

#include "GooCore/GooCore.h"

#include "tinysplinecpp.h"

#define PLAYBACK_DEBUG 0

namespace HOWL {
	class Interpolator {
	public:
		Interpolator();

	};

	class Layer;

	class EXPORT Keyframe {
		public:
			Keyframe();
			Keyframe(std::string name, long time);
			virtual ~Keyframe();
			virtual std::string serialize();
			void toBuffer(char *outbuf, int len);

			#ifdef USE_WXWIDGETS
				virtual void render(wxDC &canvas, wxRect bounding_box);
			#endif

			virtual bool operator==(Keyframe &a);

			std::string name;

			double smootherBefore[2];
			double smootherAfter[2];
			bool smootherHold;

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
		
			// virtual bool operator==(DoubleKeyframe &a); // XXX Implement me!

			double value;
	};

	class EXPORT StringKeyframe: public Keyframe {
		public:
			StringKeyframe(std::string name, long time, std::string *value);
			StringKeyframe(std::string name, long time, const char *value);
			~StringKeyframe();
			std::string serialize();

			// virtual bool operator==(StringKeyframe &a); // XXX Implement me!

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
