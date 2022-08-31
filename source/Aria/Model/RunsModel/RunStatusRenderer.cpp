#include "Model/RunsModel/RunStatusRenderer.hpp"

#include "Model/RunsModel/RunTreeModel.hpp"

#include <AriaLib/TestsCounts.hpp>

#include <wx/dc.h>
#include <wx/settings.h>

namespace aria::run
{
#	include "xpms/acceptable.xpm"
#	include "xpms/negligible.xpm"
#	include "xpms/notrun.xpm"
#	include "xpms/unacceptable.xpm"
#	include "xpms/unprocessed.xpm"
#	include "xpms/crashed.xpm"

	//*********************************************************************************************

	RunStatusRenderer::RunStatusRenderer( wxDataViewCtrl * parent
		, const wxString & varianttype
		, wxDataViewCellMode mode
		, int align )
		: wxDataViewCustomRenderer{ varianttype, mode, align }
		, m_parent{ parent }
		, m_size{ 20, 20 }
		, m_bitmaps{ createImage( notrun_xpm )
			, createImage( negligible_xpm )
			, createImage( acceptable_xpm )
			, createImage( unacceptable_xpm )
			, createImage( unprocessed_xpm )
			, createImage( crashed_xpm ) }
	{
	}

	RunStatusRenderer::~RunStatusRenderer()
	{
	}

	bool RunStatusRenderer::SetValue( const wxVariant & value )
	{
		m_status = RunStatus( value.GetLong() );
		m_index = uint32_t( value.GetLong() );
		return true;
	}

	bool RunStatusRenderer::GetValue( wxVariant & value ) const
	{
		value = long( m_status );
		return true;
	}

	bool RunStatusRenderer::Render( wxRect cell, wxDC * dc, int state )
	{
		dc->DrawBitmap( m_bitmaps[m_index]
			, cell.x
			, cell.y
			, true );
		return false;
	}

	wxSize RunStatusRenderer::GetSize() const
	{
		return { m_parent->GetColumn( 0u )->GetWidth(), m_size.y };
	}

	wxImage RunStatusRenderer::createImage( char const * const * xpmData )
	{
		wxImage result{ xpmData };
		return result.Scale( m_size.x, m_size.y );
	}

	//*********************************************************************************************
}
