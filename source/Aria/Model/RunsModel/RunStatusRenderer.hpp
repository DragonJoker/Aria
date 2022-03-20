/*
See LICENSE file in root folder
*/
#ifndef ___CTP_RunStatusRenderer_HPP___
#define ___CTP_RunStatusRenderer_HPP___

#include "RunsModelPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/bitmap.h>
#include <wx/dataview.h>
#pragma warning( pop )

#include <array>
#include <chrono>

namespace aria::run
{
	class RunStatusRenderer
		: public wxDataViewCustomRenderer
	{
	private:
		using Clock = std::chrono::high_resolution_clock;

	public:
		static wxString GetDefaultType()
		{
			return wxT( "long" );
		}

		RunStatusRenderer( wxDataViewCtrl * parent
			, const wxString & varianttype
			, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT
			, int align = wxDVR_DEFAULT_ALIGNMENT );
		~RunStatusRenderer()override;

		bool SetValue( const wxVariant & value ) override;
		bool GetValue( wxVariant & value ) const override;
		bool Render( wxRect cell, wxDC * dc, int state ) override;
		wxSize GetSize() const override;

	private:
		wxImage createImage( char const * const * xpmData );

	private:
		wxDataViewCtrl * m_parent;
		wxSize m_size;
		RunStatus m_status;
		std::array< wxBitmap, size_t( RunStatus::eCount ) > m_bitmaps;
		uint32_t m_index{};
	};
}

#endif
