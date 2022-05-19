/*
See LICENSE file in root folder
*/
#ifndef ___CTP_AuiToolBarArt_H___
#define ___CTP_AuiToolBarArt_H___

#include "Aria/Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#include <wx/aui/auibar.h>
#pragma warning( pop )

namespace aria
{
	class AuiToolBarArt
		: public wxAuiDefaultToolBarArt
	{
	public:
		Aria_API AuiToolBarArt();

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
