//
// HOWL - Music-synced animation library
// File: TimelinePanel.h
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#pragma once

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>
#include <wx/vscroll.h>

#include "Project.h"
#include "Layer.h"

namespace HOWL {
	// Timeline panel
	class TimelinePanel: public wxHVScrolledWindow {
		public:
			TimelinePanel(wxPanel *parent, wxWindowID window_id, Project *project, wxWindowID eventTarget);
			~TimelinePanel();
			void setProject(Project *project);
			void setEventTarget(wxWindowID eventTarget);
			void paintEvent(wxPaintEvent &event);
			void paintNow();
			void onLeftDown(wxMouseEvent &event);
			void onMouseMove(wxMouseEvent &event);
			void onLeftUp(wxMouseEvent &event);
			void render(wxDC &canvas);
			void render_row(wxDC &canvas, std::string rowname, KeyframeSet *keyframes, wxRect bounding_box);
			void render_header_segment(wxDC &canvas, int col, int xpos);
			void render_header(wxDC &canvas);
			void render_playhead(wxDC &canvas);
			void nextBeat();
			void nextQuarterBeat();
			void advanceCol(int cols);
			void movePlayhead(int time);
			void zoom(int percent);
			int getPlayhead();
			// void RefreshDisplay();
			// void ChangeNoteColor(wxCommandEvent &event);
			std::vector<wxPoint> getCurrentSelection();
			void setCurrentSelection(std::vector<wxPoint> selection);
		
		protected:
			wxCoord OnGetRowHeight(size_t row) const;
			wxCoord OnGetColumnWidth(size_t column) const;
			int playhead;
			int ticksPerCol;
			wxPoint active_button;
			std::vector<wxPoint> selected_cells;
		
		private:
			wxPanel *m_parent;
			wxBoxSizer *sizer;
			int rowsize;
			int colsize;
			int headersize;
			int labelsize;
			bool headerclicked = false;
			Project *activeProject = NULL;
			wxWindowID eventTarget = NULL;
		
			double colsPerBeat();
			int measureFromCol(int col);
			int beatFromCol(int col);
			int divFromCol(int col);
			int tickFromCol(int col);
			int playhead_in_pixels();
			wxPoint offset_in_pixels();
			wxPoint mousepos_to_buttons(wxPoint mousepos);
		
			wxDECLARE_EVENT_TABLE();
	};
	
	wxDECLARE_EVENT(DISPLAY_REFRESH, wxCommandEvent);
	wxDECLARE_EVENT(PLAYHEAD_MOVED, wxCommandEvent);
	wxDECLARE_EVENT(SELECTION_CHANGED, wxCommandEvent);
}
