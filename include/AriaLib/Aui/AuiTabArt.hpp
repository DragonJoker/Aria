/*
See LICENSE file in root folder
*/
#ifndef ___Aria_AuiTabArt_HPP___
#define ___Aria_AuiTabArt_HPP___

#include "AriaLib/Prerequisites.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/aui/tabart.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	class AuiTabArt
		: public wxAuiDefaultTabArt
	{
	public:
		AriaLib_API AuiTabArt();

		wxAuiTabArt * Clone()override;

		void DrawBorder( wxDC & dc
			, wxWindow * wnd
			, const wxRect & rect )override;

		void DrawBackground( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
		// DrawTab() draws an individual tab.
		//
		// dc       - output dc
		// inRect   - rectangle the tab should be confined to
		// caption  - tab's caption
		// active   - whether or not the tab is active
		// out_rect - actual output rectangle
		// x_extent - the advance x; where the next tab should start
		void DrawTab( wxDC & dc
			, wxWindow * wnd
			, const wxAuiNotebookPage & pane
			, const wxRect & inRect
			, int closeButtonState
			, wxRect * outTabRect
			, wxRect * outButtonRect
			, int * xExtent )override;
		// DrawButton() draws a button.
		//
		// dc          - output dc
		// inRect      - rectangle the tab should be confined to
		// caption     - tab's caption
		// bitmapId    - The bitmap to draw, from the bitmaps list
		// buttonState - The button state
		// orientation - The button orientation
		// outRect     - actual output rectangle
		void DrawButton( wxDC & dc
			, wxWindow * wnd
			, const wxRect & inRect
			, int bitmapId
			, int buttonState
			, int orientation
			, wxRect * outRect )override;

	private:
		wxColour m_disabledColour;
	};
}

#endif
