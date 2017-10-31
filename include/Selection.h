//
// HOWL - Music-synced animation library
// File: Selection.h
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#pragma once

#include <vector>

#include "Layer.h"

namespace HOWL {
	struct SingleSelection {
		KeyframeSet *set;
		long start;
		long end;
	};
	
	class Selection {
		public:
			std::vector<SingleSelection *> sel;
		
			Selection();
			Selection(Selection *other);
		
			void clear();
			void add(KeyframeSet *set, long start, long end);
			void toggle(KeyframeSet *set, long start, long end);
			bool matches(Keyframe *kf);
	};
};
