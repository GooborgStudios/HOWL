//
// HOWL - Music-synced animation library
// File: TimelinePanel.cpp
// ©2017 Nightwave Studios: Vinyl Darkscratch, Light Apacha.
// https://www.nightwave.co
//

#include "TimelinePanel.h"

#include <wx/wxprec.h>
#ifndef WX_PRECOMP
	#include <wx/wx.h>
#endif

#include <wx/dcbuffer.h>
#include <wx/vscroll.h>
#include <wx/clrpicker.h>

#include "Layer.h"
#include "Selection.h"

using namespace HOWL;

wxDEFINE_EVENT(HOWL::DISPLAY_REFRESH, wxCommandEvent);
wxDEFINE_EVENT(HOWL::PLAYHEAD_MOVED, wxCommandEvent);
wxDEFINE_EVENT(HOWL::SELECTION_CHANGED, wxCommandEvent);

TimelinePanel::TimelinePanel(wxPanel *parent, wxWindowID window_id, Project *project, wxWindowID eventTarget): wxHVScrolledWindow(parent, window_id, wxPoint(-1, -1), wxSize(-1, 250), wxBORDER_SUNKEN) {
	m_parent = parent;
	sizer = new wxBoxSizer(wxHORIZONTAL);
	
	rowsize = 20;
	colsize = 40;
	headersize = 30;
	labelsize = 40;
	
	setEventTarget(eventTarget);
	setProject(project);

	Bind(HOWL::DISPLAY_REFRESH, &TimelinePanel::refreshNow, this);
}

TimelinePanel::~TimelinePanel() {
	
}

void TimelinePanel::setProject(Project *project) {
	activeProject = project;
	ticksPerCol = activeProject->ticksPerBeat / 4;
	colsize = 40;
	
	
	int rows = 0;
	for (auto l: activeProject->layers) {
		if (true /* l->isExpanded */) {
			for (auto k: l->keyframes) {
				visible_layers[rows] = k;
				rows++;
			}
		}
	}
	SetRowColumnCount(rows+1, 1);
	
	Update();
	movePlayhead(0);
}

void TimelinePanel::setEventTarget(wxWindowID eventTarget) {
	this->eventTarget = eventTarget;
}

void TimelinePanel::paintEvent(wxPaintEvent &WXUNUSED(event)) {
	// depending on your system you may need to look at double-buffered dcs
	wxPaintDC canvas(this);
	render(canvas);
}

void TimelinePanel::paintNow() {
	// depending on your system you may need to look at double-buffered dcs
	wxClientDC canvas(this);
	render(canvas);
}

void TimelinePanel::refreshNow(wxCommandEvent &WXUNUSED(event)) {
	paintNow();
}

void TimelinePanel::onLeftDown(wxMouseEvent &event) {
	wxPoint mousepos = event.GetLogicalPosition(wxClientDC(this));
	wxPoint btn = mousepos_to_buttons(mousepos);
	if (mousepos.y < headersize) {
		headerclicked = true;
		movePlayhead(btn.x * ticksPerCol);
		paintNow();
		return;
	}
	
	if (!event.ControlDown()) {
		for (auto s: activeProject->selection.sel) {
			if (eventTarget) {
				SelectionEvent fin_evt(SELECTION_OFF, eventTarget, *s);
				fin_evt.SetEventObject(this);
				wxPostEvent(wxWindow::FindWindowById(eventTarget), fin_evt);
			}
		}
		activeProject->selection.clear();
	}
	SingleSelection *sel = activeProject->selection.add(visible_layers[btn.y], btn.x * ticksPerCol, (btn.x + 1) * ticksPerCol);
	
	// if controli s down check status of current clicked box
	// send on/off event
	
	// otherwise, if this isnt a selected box already,
	// send a deslect event for current selection,
	// send event for new selection
	
	if (eventTarget) {
		SelectionEvent fin_evt(SELECTION_ON, eventTarget, *sel);
		fin_evt.forceRefresh();
		fin_evt.SetEventObject(this);
		wxPostEvent(wxWindow::FindWindowById(eventTarget), fin_evt);
	}
	
	paintNow();
}

void TimelinePanel::onMouseMove(wxMouseEvent &event) {
	if (!event.LeftIsDown()) return;
	
	wxPoint mousepos = event.GetLogicalPosition(wxClientDC(this));
	wxPoint btn = mousepos_to_buttons(mousepos);
	if (headerclicked) movePlayhead(btn.x * ticksPerCol);
}

void TimelinePanel::onLeftUp(wxMouseEvent &WXUNUSED(event)) {
	headerclicked = false;
}

void TimelinePanel::render(wxDC &canvas) {
	int width = canvas.GetSize().GetX();
	int xpos = labelsize-(GetVisibleBegin().GetCol()*colsize);
	int ypos = headersize-(GetVisibleBegin().GetRow()*rowsize);
	int cypos = ypos;
	for (auto kf: visible_layers) {
		if (cypos >= headersize-rowsize) render_row(canvas, kf.second->name, kf.second, wxRect(0, cypos, width, rowsize));
		cypos += rowsize;
		if (cypos >= canvas.GetSize().GetY()) break;
	}
	render_selection(canvas, activeProject->selection);
	render_header(canvas);
	render_playhead(canvas);
	
	canvas.SetBrush(*wxTRANSPARENT_BRUSH);
	
}

void TimelinePanel::render_selection(wxDC &canvas, Selection sel) {
	for (SingleSelection *s : sel.sel) {
		if (s->set == NULL) return;
		
		canvas.SetBrush(*wxTRANSPARENT_BRUSH);
		canvas.SetPen(wxPen(*wxWHITE, 2));
		
		int i=0;
		for (; i<visible_layers.size(); i++) {
			if (visible_layers[i]->name == s->set->name) break;
		}
		
		wxRect bounding_box = wxRect(index_to_screenpos(wxPoint(s->start/ticksPerCol, i)), index_to_screenpos(wxPoint(s->end/ticksPerCol, i+1)));
		canvas.DrawRectangle(bounding_box);
	}
}

void TimelinePanel::render_row(wxDC &canvas, std::string rowname, KeyframeSet *keyframes, wxRect bounding_box) {
	int col1time = GetVisibleBegin().GetCol() * ticksPerCol;
	int colNtime = GetVisibleEnd().GetCol() * ticksPerCol;
	int lastCol = 0;
	
    Selection sel = activeProject->selection;

	for (auto iter: keyframes->keyframes) {
		auto time = iter->time;
		double col = (time - col1time) / (ticksPerCol * 1.0);
		if (col < 0.0) col = 0.0;
		if (col > lastCol) lastCol = (int)(col);
		int left = bounding_box.GetLeft()+labelsize+(col*colsize);

		wxRect kfbox(bounding_box);
		kfbox.SetLeft(left);
		iter->render(canvas, kfbox);
	}
	
	if (lastCol > GetColumnCount()) SetColumnCount(lastCol);
	
	canvas.SetPen(*wxBLACK_PEN);
	canvas.SetBrush(*wxTRANSPARENT_BRUSH);
	
	canvas.DrawText(rowname, bounding_box.GetTopLeft());
	canvas.DrawLine(bounding_box.GetBottomLeft(), bounding_box.GetBottomRight());
}

void TimelinePanel::render_header_segment(wxDC &canvas, int col, int xpos) {
	char buf[16];
	int divsPerBeat = 4;
	int measure = col / (colsPerBeat() * activeProject->beatsPerMeasure) + 1;
	int beat = (int)(col / colsPerBeat()) % activeProject->beatsPerMeasure + 1;
	int div = (int)(col * divsPerBeat / colsPerBeat()) % activeProject->beatsPerMeasure + 1;
	int tick = (col * ticksPerCol) % (activeProject->ticksPerBeat / divsPerBeat) + 1;

	if (beat == 1 && div == 1 && tick == 1) snprintf(buf, sizeof(buf), "%d", measure);
	else if (div == 1 && tick == 1) snprintf(buf, sizeof(buf), "%d.%d", measure, beat);
	else if (tick == 1) snprintf(buf, sizeof(buf), "%d.%d.%d", measure, beat, div);
	else snprintf(buf, sizeof(buf), "%d.%d.%d.%d", measure, beat, div, tick);
	
	canvas.DrawText(buf, xpos+4, 0);

	if (beat == 1 && div == 1 && tick == 1) canvas.DrawLine(xpos, 0, xpos, headersize-2);
	else if (div == 1 && tick == 1) canvas.DrawLine(xpos, headersize/2, xpos, headersize-2);
	else if (tick == 1) canvas.DrawLine(xpos, headersize/4*3, xpos, headersize-2);
	else canvas.DrawLine(xpos, headersize-6, xpos, headersize-2);
}

void TimelinePanel::render_header(wxDC &canvas) {
	int width = canvas.GetSize().GetX();
	int col = GetVisibleBegin().GetCol();
	
	canvas.SetPen(*wxTRANSPARENT_PEN);
	canvas.SetBrush(*wxWHITE_BRUSH);
	canvas.DrawRectangle(0, 0, width, headersize);
	
	canvas.SetPen(*wxBLACK_PEN);
	canvas.SetBrush(*wxTRANSPARENT_BRUSH);
	
	for (int x = labelsize; x < width; x += colsize) {
		render_header_segment(canvas, col, x);
		col++;
	}
	
	canvas.SetPen(wxPen(*wxBLACK, 3));
	canvas.DrawLine(0, headersize-2, width, headersize-2);
}

void TimelinePanel::render_playhead(wxDC &canvas) {
	int xpos = playhead_in_pixels() - offset_in_pixels().x + labelsize;
	
	if (xpos >= labelsize) {
		canvas.SetPen(wxPen(*wxBLACK, 6));
		canvas.DrawLine(xpos, headersize, xpos, canvas.GetSize().GetHeight());
		canvas.SetPen(wxPen(*wxWHITE, 3));
		canvas.DrawLine(xpos, headersize, xpos, canvas.GetSize().GetHeight());
		canvas.DrawCircle(xpos, headersize, 5.0);
	}
}

void TimelinePanel::nextBeat() {
	movePlayhead(playhead+activeProject->ticksPerBeat);
}

void TimelinePanel::nextQuarterBeat() {
	movePlayhead(playhead+(activeProject->ticksPerBeat/4));
}

void TimelinePanel::advanceCol(int cols) {
	movePlayhead(playhead+(ticksPerCol*cols));
}

void TimelinePanel::movePlayhead(int time) {
	if (time < 0) time = 0;
	int phCol = time / ticksPerCol;
	int advance = time - playhead;
	if (advance < ticksPerCol*2 && advance > 0) {
		activeProject->advanceFrame(advance);
	} else {
		activeProject->seek(time);
	}
	playhead = time;
	
	if (phCol >= GetVisibleEnd().GetCol() || phCol < GetVisibleBegin().GetCol()) {
		ScrollToColumn(phCol == 0 ? 0 : phCol - 1);
	}
	
	if (eventTarget) {
		wxCommandEvent fin_evt(PLAYHEAD_MOVED, eventTarget);
		fin_evt.SetEventObject(this);
		wxPostEvent(wxWindow::FindWindowById(eventTarget), fin_evt);
	}

	Refresh();
}

void TimelinePanel::zoom(int percent) {
	if ((ticksPerCol <= 1 && percent > 100) || (ticksPerCol >= pow(2, 27) && percent < 100)) return;
	colsize = colsize * percent / 100;
	if (colsize <= 10) {
		ticksPerCol *= 4;
		if (ticksPerCol > pow(2, 27)) ticksPerCol = pow(2, 27);
		else colsize = 40;
	}
	if (colsize >= 160) {
		ticksPerCol /= 4;
		if (ticksPerCol < 1) ticksPerCol = 1;
		else colsize = 40;
	}
	Refresh();
}

int TimelinePanel::getPlayhead() {
	return playhead;
}

wxCoord TimelinePanel::OnGetRowHeight(size_t row) const {
	return rowsize;
}

wxCoord TimelinePanel::OnGetColumnWidth(size_t column) const {
	return colsize;
	//	return wxClientDC(this).GetSize().GetX();
}

double TimelinePanel::colsPerBeat() {
	return activeProject->ticksPerBeat / (ticksPerCol * 1.0);
}

int TimelinePanel::measureFromCol(int col) {
	return col / (colsPerBeat() * activeProject->beatsPerMeasure) + 1;
}

int TimelinePanel::beatFromCol(int col) {
	return (int)(col / colsPerBeat()) % activeProject->beatsPerMeasure + 1;
}

int TimelinePanel::divFromCol(int col) {
	return (int)(col * 4 / colsPerBeat()) % activeProject->beatsPerMeasure + 1;
}

int TimelinePanel::tickFromCol(int col) {
	return (col * ticksPerCol) % activeProject->beatsPerMeasure + 1;
}

int TimelinePanel::playhead_in_pixels() {
	return colsize * activeProject->currentTime / activeProject->ticksPerBeat * 4;
}

wxPoint TimelinePanel::offset_in_pixels() {
	return wxPoint(colsize * GetVisibleBegin().GetCol(), rowsize * GetVisibleBegin().GetRow());
}

wxPoint TimelinePanel::mousepos_to_buttons(wxPoint mousepos) {
	return wxPoint(
		(mousepos.x + offset_in_pixels().x - labelsize)  / colsize,
		(mousepos.y + offset_in_pixels().y - headersize) / rowsize
	);
}

wxPoint TimelinePanel::index_to_screenpos(wxPoint index) {
	return wxPoint(
	   (index.x * colsize) - offset_in_pixels().x + labelsize,
	   (index.y * rowsize) - offset_in_pixels().y + headersize
	);
}

wxBEGIN_EVENT_TABLE(TimelinePanel, wxPanel)
	EVT_PAINT(TimelinePanel::paintEvent)
	EVT_LEFT_DOWN(TimelinePanel::onLeftDown)
	EVT_MOTION(TimelinePanel::onMouseMove)
	EVT_LEFT_UP(TimelinePanel::onLeftUp)
wxEND_EVENT_TABLE()

//click to create/move/recolor notes
//snap to grid
//zooming
