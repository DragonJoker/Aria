/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestStatusRenderer_HPP___
#define ___CTP_TestStatusRenderer_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/bitmap.h>
#include <wx/dataview.h>

#include <array>
#include <chrono>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	class TestStatusRenderer
		: public wxDataViewCustomRenderer
	{
	private:
		using Clock = std::chrono::high_resolution_clock;

	public:
		static wxString GetDefaultType()
		{
			return wxT( "void*" );
		}

		TestStatusRenderer( wxDataViewCtrl * parent
			, const wxString & varianttype
			, wxDataViewCellMode mode = wxDATAVIEW_CELL_INERT
			, int align = wxDVR_DEFAULT_ALIGNMENT );
		~TestStatusRenderer()override;

		bool SetValue( const wxVariant & value ) override;
		bool GetValue( wxVariant & value ) const override;
		bool Render( wxRect cell, wxDC * dc, int state ) override;
		wxSize GetSize() const override;

	private:
		wxImage createImage( char const * const * xpmData );
		void renderCategory( wxRect cell, wxDC * dc, int state );

	private:
		wxDataViewCtrl * m_parent;
		wxSize m_size;
		wxBitmap m_outOfEngineDateBmp;
		wxBitmap m_outOfSceneDateBmp;
		std::array< wxBitmap, size_t( TestStatus::eCount ) + AdditionalIndices > m_bitmaps;
		StatusName * m_source{};
		StatusName m_statusName{};
		bool m_isTest{ true };
		uint32_t m_index{};
	};
}

#endif
