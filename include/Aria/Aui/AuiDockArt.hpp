/*
See LICENSE file in root folder
*/
#ifndef ___CTP_AuiDockArt_HPP___
#define ___CTP_AuiDockArt_HPP___

#include "Aria/Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/aui/aui.h>
#include <wx/aui/dockart.h>
#pragma warning( pop )

namespace aria
{
	class AuiDockArt
		: public wxAuiDefaultDockArt
	{
	public:
		Aria_API AuiDockArt();

		void DrawBackground( wxDC & dc
			, wxWindow * window
			, int orientation
			, wxRect const & rect )override;
	};
}

#endif
