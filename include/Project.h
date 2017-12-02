//
// HOWL - Music-synced animation library
// File: Project.h
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

/**
 * @file   Project.h
 * @brief  This file holds the abstract Project class that holds all of the project's data.
 */

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <vector>
#include <string>

#include "NightwaveCore/NightwaveCore.h"
#include "Layer.h"
#include "Selection.h"

namespace HOWL {
	/**
	 * @brief   The abstract Project class that holds all project data.
	 */
	class EXPORT Project {
		protected:
			std::string filePath = "";
		
		public:
			int BPM = 120;
			int ticksPerBeat = 32;
			int beatsPerMeasure = 4;
			long currentTime = 0;
		
			std::vector<Layer *> layers;
			Selection selection;
			
			/**
			 * @brief   Saves all project data to the specified file path.
			 */ 
			virtual int save(std::string filePath)=0;
			
			/// \overload virtual int save(std::string filePath)=0 
			virtual int save()=0;
			
			
			void seek(long newTime); ///< Seeks to the specified tick in the animation.

			bool advanceFrame(long increment); ///< Advances the time by the specified increment of ticks in the animation.
			
			/**
			 * @brief   Check if we're at the end of the animation.
			 *
			 * @retval TRUE   We're at the end of the animation.
			 * @retval FALSE  We are not at the end of the animation yet.
			 */ 
			bool eof();

//	void applySelection(void (*selectionCB)(void *), void *data);
	};

	wxDECLARE_EVENT(PROJECT_SAVED, wxCommandEvent);
	wxDECLARE_EVENT(PROJECT_LOADED, wxCommandEvent);
}
