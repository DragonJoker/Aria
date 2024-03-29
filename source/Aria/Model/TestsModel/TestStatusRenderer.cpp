#include "Model/TestsModel/TestStatusRenderer.hpp"

#include "Model/TestsModel/TestTreeModel.hpp"

#include <AriaLib/TestsCounts.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/dc.h>
#include <wx/settings.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
#if defined( _WIN32 )
#	include "xpms/acceptable.xpm"
#	include "xpms/ignored.xpm"
#	include "xpms/negligible.xpm"
#	include "xpms/notrun.xpm"
#	include "xpms/outofdate.xpm"
#	include "xpms/outofdate2.xpm"
#	include "xpms/unacceptable.xpm"
#	include "xpms/unprocessed.xpm"
#	include "xpms/crashed.xpm"
#	include "xpms/pending.xpm"
#	include "xpms/progress_1.xpm"
#	include "xpms/progress_2.xpm"
#	include "xpms/progress_3.xpm"
#	include "xpms/progress_4.xpm"
#	include "xpms/progress_5.xpm"
#	include "xpms/progress_6.xpm"
#	include "xpms/progress_7.xpm"
#	include "xpms/progress_8.xpm"
#	include "xpms/progress_9.xpm"
#	include "xpms/progress_10.xpm"
#	include "xpms/progress_11.xpm"
#	include "xpms/progress_12.xpm"
#else
#	include "xpms/16x16/acceptable.xpm"
#	include "xpms/16x16/ignored.xpm"
#	include "xpms/16x16/negligible.xpm"
#	include "xpms/16x16/notrun.xpm"
#	include "xpms/16x16/outofdate.xpm"
#	include "xpms/16x16/outofdate2.xpm"
#	include "xpms/16x16/unacceptable.xpm"
#	include "xpms/16x16/unprocessed.xpm"
#	include "xpms/16x16/crashed.xpm"
#	include "xpms/16x16/pending.xpm"
#	include "xpms/16x16/progress_1.xpm"
#	include "xpms/16x16/progress_2.xpm"
#	include "xpms/16x16/progress_3.xpm"
#	include "xpms/16x16/progress_4.xpm"
#	include "xpms/16x16/progress_5.xpm"
#	include "xpms/16x16/progress_6.xpm"
#	include "xpms/16x16/progress_7.xpm"
#	include "xpms/16x16/progress_8.xpm"
#	include "xpms/16x16/progress_9.xpm"
#	include "xpms/16x16/progress_10.xpm"
#	include "xpms/16x16/progress_11.xpm"
#	include "xpms/16x16/progress_12.xpm"
#endif

	//*********************************************************************************************

	namespace statrend
	{
		static wxColourBase::ChannelType mix( float percent
			, wxColourBase::ChannelType const & lhs
			, wxColourBase::ChannelType const & rhs )
		{
			return wxColourBase::ChannelType( ( lhs * ( 1.0f - percent ) ) + ( rhs * percent ) );
		}

		static wxColour mix( float percent
			, wxColour const & lhs
			, wxColour const & rhs )
		{
			return wxColour{ mix( percent, lhs.Red(), rhs.Red() )
				, mix( percent, lhs.Green(), rhs.Green() )
				, mix( percent, lhs.Blue(), rhs.Blue() )
				, mix( percent, lhs.Alpha(), rhs.Alpha() ) };
		}

		static int getStatusSize( TestStatus status
			, int maxWidth
			, RendererTestsCounts const & counts )
		{
			return int( ( float( maxWidth ) * float( counts.getValue( getType( status ) ) ) / float( counts.getAllValue() ) ) );
		}

		static int getStatusSize( TestStatus status
			, int maxWidth
			, TestsCounts const & counts )
		{
			return int( ( float( maxWidth ) * float( counts.getValue( getType( status ) ) ) / float( counts.getAllValue() ) ) );
		}

		static wxColour getStatusColor( TestStatus status )
		{
			switch ( status )
			{
			case TestStatus::eNotRun:
				return wxColour{ 0xFF, 0xFF, 0xFF, 0x00 };
			case TestStatus::eNegligible:
				return wxColour{ 0x00, 0x8B, 0x19, 0xFF };
			case TestStatus::eAcceptable:
				return wxColour{ 0xDC, 0xAE, 0x00, 0xFF };
			case TestStatus::eUnacceptable:
				return wxColour{ 0xFF, 0x00, 0x00, 0xFF };
			case TestStatus::eUnprocessed:
				return wxColour{ 0x8A, 0x8A, 0x8A, 0xFF };
			case TestStatus::eCrashed:
				return wxColour{ 0x00, 0x00, 0x00, 0xFF };
			case TestStatus::ePending:
				return wxColour{ 0x00, 0x80, 0xC0, 0xFF };
			default:
				return wxColour{ 0x00, 0x00, 0xFF, 0xFF };
			}
		}
	}

	TestStatusRenderer::TestStatusRenderer( wxDataViewCtrl * parent
		, const wxString & varianttype
		, wxDataViewCellMode mode
		, int align )
		: wxDataViewCustomRenderer{ varianttype, mode, align }
		, m_parent{ parent }
#if defined( _WIN32 )
		, m_size{ 20, 20 }
#else
		, m_size{ 16, 16 }
#endif
		, m_outOfEngineDateBmp{ createImage( outofdate_xpm ) }
		, m_outOfSceneDateBmp{ createImage( outofdate2_xpm ) }
		, m_bitmaps{ createImage( ignored_xpm )
			, createImage( notrun_xpm )
			, createImage( negligible_xpm )
			, createImage( acceptable_xpm )
			, createImage( unacceptable_xpm )
			, createImage( unprocessed_xpm )
			, createImage( crashed_xpm )
			, createImage( pending_xpm )
			, createImage( progress_1_xpm )
			, createImage( progress_2_xpm )
			, createImage( progress_3_xpm )
			, createImage( progress_4_xpm )
			, createImage( progress_5_xpm )
			, createImage( progress_6_xpm )
			, createImage( progress_7_xpm )
			, createImage( progress_8_xpm )
			, createImage( progress_9_xpm )
			, createImage( progress_10_xpm )
			, createImage( progress_11_xpm )
			, createImage( progress_12_xpm ) }
	{
	}

	TestStatusRenderer::~TestStatusRenderer()
	{
	}

	bool TestStatusRenderer::SetValue( const wxVariant & value )
	{
		m_source = reinterpret_cast< StatusName * >( value.GetVoidPtr() );

		if ( m_source )
		{
			m_statusName = *m_source;
			m_index = uint32_t( ( m_statusName.ignored && !isRunning( m_statusName.status ) )
				? IgnoredIndex
				: size_t( m_statusName.status ) + AdditionalIndices );
			m_isTest = ( m_statusName.type == NodeType::eTestRun );
		}
		else
		{
			m_statusName = {};
			m_isTest = true;
		}

		return true;
	}

	bool TestStatusRenderer::GetValue( wxVariant & value ) const
	{
		value = reinterpret_cast< void * >( m_source );
		return true;
	}

	bool TestStatusRenderer::Render( wxRect cell, wxDC * dc, int state )
	{
		dc->DrawBitmap( m_bitmaps[m_index]
			, cell.x
			, cell.y
			, true );

		if ( !m_isTest )
		{
			renderCategory( cell, dc, state );
		}

		if ( !isRunning( m_statusName.status )
			&& ( m_statusName.status != TestStatus::eNotRun
				|| !m_isTest ) )
		{
			if ( m_statusName.outOfEngineDate )
			{
				dc->DrawBitmap( m_outOfEngineDateBmp
					, cell.x
					, cell.y
					, true );
			}

			if ( m_statusName.outOfSceneDate )
			{
				dc->DrawBitmap( m_outOfSceneDateBmp
					, cell.x
					, cell.y
					, true );
			}
		}

		auto size = dc->GetTextExtent( m_statusName.name );
		wxPoint offset{ cell.x + 10 + m_size.x, cell.y + ( cell.height - size.y ) / 2 };
		auto foreground = dc->GetTextForeground();
		dc->SetTextForeground( *wxBLACK );
		dc->DrawText( m_statusName.name, offset );
		dc->SetTextForeground( foreground );
		return false;
	}

	wxSize TestStatusRenderer::GetSize() const
	{
		return { m_parent->GetColumn( 0u )->GetWidth(), m_size.y };
	}

	wxImage TestStatusRenderer::createImage( char const * const * xpmData )
	{
		wxImage result{ xpmData };
		return result.Scale( m_size.x, m_size.y );
	}

	void TestStatusRenderer::renderCategory( wxRect cell, wxDC * dc, int state )
	{
		int xOffset = 10 + m_size.x;
		std::vector< TestStatus > valid;

		if ( m_statusName.rendererCounts )
		{
			for ( int i = 1; i <= int( TestStatus::eRunning_Begin ); ++i )
			{
				auto status = TestStatus( i );

				if ( m_statusName.rendererCounts->getValue( getType( status ) ) > 0 )
				{
					valid.push_back( status );
				}
			}
			{
				auto status = TestStatus::eNotRun;

				if ( m_statusName.rendererCounts->getValue( getType( status ) ) > 0 )
				{
					valid.push_back( status );
				}
			}
		}
		else if ( m_statusName.categoryCounts )
		{
			for ( int i = 1; i <= int( TestStatus::eRunning_Begin ); ++i )
			{
				auto status = TestStatus( i );

				if ( m_statusName.categoryCounts->getValue( getType( status ) ) > 0 )
				{
					valid.push_back( status );
				}
			}
			{
				auto status = TestStatus::eNotRun;

				if ( m_statusName.categoryCounts->getValue( getType( status ) ) > 0 )
				{
					valid.push_back( status );
				}
			}
		}

		if ( !valid.empty() )
		{
			auto nxt = valid.begin();
			auto cur = nxt++;
			auto curColor = state
				? wxSystemSettings::GetColour( wxSystemColour::wxSYS_COLOUR_GRADIENTACTIVECAPTION )
				: PANEL_FOREGROUND_COLOUR;
			auto nxtColor = statrend::getStatusColor( *cur );
			auto curWidth = cell.GetSize().x / 4;
			auto gradWidth = std::min( 50, curWidth / 4 );
			auto totalWidth = cell.width - xOffset;

			curWidth -= gradWidth;
			dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, curWidth, cell.height }, curColor, curColor );
			xOffset += curWidth;
			auto halfColour = statrend::mix( 0.5f, curColor, nxtColor );
			dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, curColor, halfColour );
			xOffset += gradWidth;
			totalWidth -= xOffset;

			while ( nxt != valid.end() )
			{
				curColor = statrend::getStatusColor( *cur );
				nxtColor = statrend::getStatusColor( *nxt );

				if ( m_statusName.rendererCounts )
				{
					curWidth = statrend::getStatusSize( *cur, totalWidth, *m_statusName.rendererCounts );
				}
				else
				{
					curWidth = statrend::getStatusSize( *cur, totalWidth, *m_statusName.categoryCounts );
				}

				gradWidth = curWidth / 4;
				curWidth -= gradWidth * 2;

				dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, halfColour, curColor );
				xOffset += gradWidth;
				dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, curWidth, cell.height }, curColor, curColor );
				xOffset += curWidth;
				halfColour = statrend::mix( 0.5f, curColor, nxtColor );
				dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, curColor, halfColour );
				xOffset += gradWidth;
				dc->SetPen( wxPen{ curColor, 2 } );
				dc->DrawLine( cell.x + xOffset - 2, cell.y, cell.x + xOffset - 2, cell.y + cell.height - 1 );

				cur = nxt++;
			}

			curColor = statrend::getStatusColor( *cur );
			curWidth = cell.width - xOffset;
			gradWidth = std::min( 50, curWidth / 4 );
			curWidth -= gradWidth;

			dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, gradWidth, cell.height }, halfColour, curColor );
			xOffset += gradWidth;
			dc->GradientFillLinear( wxRect{ cell.x + xOffset, cell.y, curWidth, cell.height }, curColor, curColor );
		}
	}

	//*********************************************************************************************
}
