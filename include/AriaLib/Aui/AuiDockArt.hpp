/*
See LICENSE file in root folder
*/
#ifndef ___Aria_AuiDockArt_HPP___
#define ___Aria_AuiDockArt_HPP___

#include "AriaLib/Prerequisites.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	class AuiDockArt
		: public wxAuiDefaultDockArt
	{
	public:
		AriaLib_API AuiDockArt();

		void DrawBackground( wxDC & dc
			, wxWindow * window
			, int orientation
			, wxRect const & rect )override;
	};
}

#endif
