/*
See LICENSE file in root folder
*/
#ifndef ___Aria_AuiToolBarArt_H___
#define ___Aria_AuiToolBarArt_H___

#include "AriaLib/Prerequisites.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/aui/auibar.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	class AuiToolBarArt
		: public wxAuiDefaultToolBarArt
	{
	public:
		AriaLib_API AuiToolBarArt();

		wxAuiToolBarArt * Clone()override;

		void DrawBackground( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
		void DrawPlainBackground( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
		void DrawSeparator( wxDC & dc
			, wxWindow * window
			, wxRect const & rect )override;
	};
}

#endif
