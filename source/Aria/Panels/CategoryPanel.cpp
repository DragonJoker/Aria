#include "CategoryPanel.hpp"

#include <AriaLib/TestsCounts.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/grid.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <algorithm>
#include <iomanip>
#include <numeric>
#include <sstream>
#include <AriaLib/EndExternHeaderGuard.hpp>

#if defined( _WIN32 )
#	include "xpms/acceptable.xpm"
#	include "xpms/ignored.xpm"
#	include "xpms/negligible.xpm"
#	include "xpms/notrun.xpm"
#	include "xpms/outofdate2.xpm"
#	include "xpms/unacceptable.xpm"
#	include "xpms/unprocessed.xpm"
#	include "xpms/crashed.xpm"
#	include "xpms/pending.xpm"
#	include "xpms/progress_1.xpm"
#else
#	include "xpms/16x16/acceptable.xpm"
#	include "xpms/16x16/ignored.xpm"
#	include "xpms/16x16/negligible.xpm"
#	include "xpms/16x16/notrun.xpm"
#	include "xpms/16x16/outofdate2.xpm"
#	include "xpms/16x16/unacceptable.xpm"
#	include "xpms/16x16/unprocessed.xpm"
#	include "xpms/16x16/crashed.xpm"
#	include "xpms/16x16/pending.xpm"
#	include "xpms/16x16/progress_1.xpm"
#endif

namespace aria
{
	//*********************************************************************************************

	namespace cat
	{
		static std::string displayPercent( float percent )
		{
			std::stringstream stream;
			stream.imbue( std::locale{ "C" } );
			stream << std::setprecision( 2 ) << std::fixed << percent << "%";
			return stream.str();
		}

		static std::string getName( TestsCountsType type )
		{
			switch ( type )
			{
			case aria::TestsCountsType::eNotRun:
				return "Not run";
			case aria::TestsCountsType::eNegligible:
				return "Negligible";
			case aria::TestsCountsType::eAcceptable:
				return "Acceptable";
			case aria::TestsCountsType::eUnacceptable:
				return "Unacceptable";
			case aria::TestsCountsType::eUnprocessed:
				return "Unprocessed";
			case aria::TestsCountsType::ePending:
				return "Pending";
			case aria::TestsCountsType::eCrashed:
				return "Crashed";
			case aria::TestsCountsType::eRunning:
				return "Running";
			case aria::TestsCountsType::eIgnored:
				return "Ignored";
			case aria::TestsCountsType::eOutdated:
				return "Out of date";
			case aria::TestsCountsType::eAll:
				return "All";
			default:
				return "Unknown";
			}
		}

		class StatusGridCellRenderer
			: public wxGridCellRenderer
		{
		public:
			StatusGridCellRenderer()
				: wxGridCellRenderer{}
#if defined( _WIN32 )
				, m_size{ 20, 20 }
#else
				, m_size{ 16, 16 }
#endif
				, m_bitmaps{ createImage( notrun_xpm )
					, createImage( negligible_xpm )
					, createImage( acceptable_xpm )
					, createImage( unacceptable_xpm )
					, createImage( unprocessed_xpm )
					, createImage( crashed_xpm )
					, createImage( pending_xpm )
					, createImage( progress_1_xpm )
					, createImage( ignored_xpm )
					, createImage( outofdate2_xpm )
					, createImage( notrun_xpm ) }
			{
			}

			void Draw( wxGrid & grid
				, wxGridCellAttr & attr
				, wxDC & dc
				, const wxRect & rect
				, int row
				, int col
				, bool isSelected )override
			{
				dc.SetBrush( PANEL_BACKGROUND_COLOUR );
				dc.SetPen( PANEL_BACKGROUND_COLOUR );
				dc.DrawRectangle( rect );

				long index;
				auto value = grid.GetCellValue( row, col );
				value.ToLong( &index );
				dc.DrawBitmap( m_bitmaps[size_t( std::max( 0l, index ) )]
					, rect.x + ( rect.width - m_size.GetWidth() ) / 2
					, rect.y + ( rect.height - m_size.GetHeight() ) / 2
					, true );
			}

			wxSize GetBestSize( wxGrid & grid,
				wxGridCellAttr & attr,
				wxDC & dc,
				int row, int col )override
			{
				return m_size;
			}

			wxGridCellRenderer * Clone()const override
			{
				return new StatusGridCellRenderer{};
			}

		private:
			wxImage createImage( char const * const * xpmData )
			{
				wxImage result{ xpmData };
				return result.Scale( m_size.x, m_size.y );
			}

		private:
			wxSize m_size;
			std::array< wxBitmap, size_t( TestsCountsType::eCount ) > m_bitmaps;
		};

		static wxColour getTypeColor( TestsCountsType status )
		{
			switch ( status )
			{
			case TestsCountsType::eNotRun:
				return wxColour{ 0xFF, 0xFF, 0xFF, 0x00 };
			case TestsCountsType::eNegligible:
				return wxColour{ 0x00, 0x8B, 0x19, 0xFF };
			case TestsCountsType::eAcceptable:
				return wxColour{ 0xDC, 0xAE, 0x00, 0xFF };
			case TestsCountsType::eUnacceptable:
				return wxColour{ 0xFF, 0x00, 0x00, 0xFF };
			case TestsCountsType::eUnprocessed:
				return wxColour{ 0x8A, 0x8A, 0x8A, 0xFF };
			case TestsCountsType::eCrashed:
				return wxColour{ 0x00, 0x00, 0x00, 0xFF };
			case TestsCountsType::ePending:
				return wxColour{ 0x00, 0x80, 0xC0, 0xFF };
			case TestsCountsType::eRunning:
				return wxColour{ 0x00, 0x00, 0xFF, 0xFF };
			default:
				return PANEL_FOREGROUND_COLOUR;
			}
		}

		class PercentGridCellRenderer
			: public wxGridCellRenderer
		{
		public:
			PercentGridCellRenderer( wxColour colour )
				: wxGridCellRenderer{}
#if defined( _WIN32 )
				, m_size{ 200, 20 }
#else
				, m_size{ 200, 16 }
#endif
				, m_colour{ std::move( colour ) }
			{
			}

			void Draw( wxGrid & grid
				, wxGridCellAttr & attr
				, wxDC & dc
				, const wxRect & rect
				, int row
				, int col
				, bool isSelected )override
			{
				dc.SetBrush( PANEL_BACKGROUND_COLOUR );
				dc.SetPen( PANEL_BACKGROUND_COLOUR );
				dc.DrawRectangle( rect );

				double percent;
				auto value = grid.GetCellValue( row, col );
				value.ToDouble( &percent );
				auto width = int( std::max( 0.0, percent / 100.0 ) * rect.GetWidth() );

				dc.SetBrush( m_colour );
				dc.SetPen( m_colour );
				dc.DrawRectangle( wxRect{ rect.x, rect.y + 2, width, rect.GetHeight() - 4 } );

				dc.SetBrush( wxColour( 90, 90, 90 ) );
				dc.SetPen( wxColour( 90, 90, 90 ) );
				dc.DrawRectangle( wxRect{ rect.x + width, rect.y + 2, rect.GetWidth() - width, rect.GetHeight() - 4 } );
			}

			wxSize GetBestSize( wxGrid & grid,
				wxGridCellAttr & attr,
				wxDC & dc,
				int row, int col )override
			{
				return m_size;
			}

			wxGridCellRenderer * Clone()const override
			{
				return new PercentGridCellRenderer{ m_colour };
			}

		private:
			wxImage createImage( char const * const * xpmData )
			{
				wxImage result{ xpmData };
				return result.Scale( m_size.x, m_size.y );
			}

		private:
			wxSize m_size;
			wxColour m_colour;
		};
	}

	//*********************************************************************************************

	CategoryPanel::CategoryPanel( wxWindow * parent
		, wxPoint const & position
		, wxSize const & size
		, bool useGrid )
		: wxPanel{ parent, wxID_ANY, position, size }
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxVERTICAL } };
		sizer->AddStretchSpacer();

		if ( useGrid )
		{
			m_grid = new wxGrid{ this, wxID_ANY };
			m_grid->CreateGrid( TestsCountsType::eCount, 5 );
			m_grid->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_grid->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			m_grid->SetSelectionBackground( PANEL_BACKGROUND_COLOUR );
			m_grid->SetSelectionForeground( PANEL_FOREGROUND_COLOUR );
			m_grid->SetDefaultCellBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_grid->SetDefaultCellTextColour( PANEL_FOREGROUND_COLOUR );
			m_grid->SetCellHighlightColour( PANEL_BACKGROUND_COLOUR );
			m_grid->SetCellHighlightPenWidth( 0 );
			m_grid->SetCellHighlightROPenWidth( 0 );
			m_grid->DisableCellEditControl();
			m_grid->HideColLabels();
			m_grid->HideRowLabels();
			m_grid->EnableDragCell( false );
			m_grid->EnableEditing( false );
			m_grid->EnableGridLines( false );
			m_grid->DisableDragColMove();
			m_grid->DisableDragGridSize();
			m_grid->DisableDragCell();
			m_grid->DisableColResize( 0 );
			m_grid->DisableColResize( 1 );
			m_grid->DisableColResize( 2 );
			m_grid->DisableColResize( 3 );
			m_grid->DisableColResize( 4 );
			m_grid->SetColSize( 0, 20 );
			m_grid->SetColSize( 1, 100 );
			m_grid->SetColSize( 2, 50 );
			m_grid->SetColSize( 3, 50 );
			m_grid->SetColSize( 4, 200 );
#if defined( _WIN32 ) && wxCHECK_VERSION( 3, 2, 0 )
			m_grid->DisableDragRowMove();
			m_grid->DisableHidingColumns();
#endif

			for ( int i = 0; i < int( TestsCountsType::eCount ); ++i )
			{
				m_grid->SetReadOnly( i, 0 );
				m_grid->SetReadOnly( i, 1 );
				m_grid->SetReadOnly( i, 2 );
				m_grid->SetReadOnly( i, 3 );
				m_grid->SetReadOnly( i, 4 );
				m_grid->DisableRowResize( i );
				m_grid->SetRowSize( i, GridLineSize );

				m_grid->SetCellBackgroundColour( i, 1, PANEL_BACKGROUND_COLOUR );
				m_grid->SetCellBackgroundColour( i, 2, PANEL_BACKGROUND_COLOUR );
				m_grid->SetCellBackgroundColour( i, 3, PANEL_BACKGROUND_COLOUR );
				m_grid->SetCellTextColour( i, 1, PANEL_FOREGROUND_COLOUR );
				m_grid->SetCellTextColour( i, 2, PANEL_FOREGROUND_COLOUR );
				m_grid->SetCellTextColour( i, 3, PANEL_FOREGROUND_COLOUR );

				m_grid->SetCellRenderer( i, 0, new cat::StatusGridCellRenderer{} );
				m_grid->SetCellAlignment( i, 1, wxALIGN_LEFT, wxALIGN_CENTER );
				m_grid->SetCellAlignment( i, 2, wxALIGN_LEFT, wxALIGN_CENTER );
				m_grid->SetCellAlignment( i, 3, wxALIGN_LEFT, wxALIGN_CENTER );
				m_grid->SetCellRenderer( i, 4, new cat::PercentGridCellRenderer{ cat::getTypeColor( TestsCountsType( i ) ) } );

				m_grid->SetCellValue( i, 0, wxString{} << i );
				m_grid->SetCellValue( i, 1, cat::getName( TestsCountsType( i ) ) );
				m_grid->SetCellValue( i, 2, wxT( "0" ) );
				m_grid->SetCellValue( i, 3, wxT( "100.00%" ) );
				m_grid->SetCellValue( i, 4, wxT( "0.0" ) );
			}

			sizer->Add( m_grid, wxSizerFlags{}.Left() );
		}
		else
		{
			m_values[TestsCountsType::eAll] = new wxStaticText{ this, wxID_ANY, wxEmptyString };
			m_values[TestsCountsType::eAll]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_values[TestsCountsType::eAll]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			sizer->Add( m_values[TestsCountsType::eAll], wxSizerFlags{}.Left() );

			for ( uint32_t i = 1; i < TestsCountsType::eAll; ++i )
			{
				m_values[i] = new wxStaticText{ this, wxID_ANY, wxEmptyString };
				m_values[i]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
				m_values[i]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
				sizer->Add( m_values[i], wxSizerFlags{}.Left() );
			}

			m_values[TestsCountsType::eNotRun] = new wxStaticText{ this, wxID_ANY, wxEmptyString };
			m_values[TestsCountsType::eNotRun]->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
			m_values[TestsCountsType::eNotRun]->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
			sizer->Add( m_values[TestsCountsType::eNotRun], wxSizerFlags{}.Left() );
		}

		sizer->AddStretchSpacer();
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void CategoryPanel::update( wxString const & name
		, AllTestsCounts & counts )
	{
		m_name = name;
		m_allCounts = &counts;
		m_rendererCounts = nullptr;
		m_categoryCounts = nullptr;
		refresh();
	}

	void CategoryPanel::update( wxString const & name
		, RendererTestsCounts & counts )
	{
		m_name = name;
		m_allCounts = nullptr;
		m_rendererCounts = &counts;
		m_categoryCounts = nullptr;
		refresh();
	}

	void CategoryPanel::update( wxString const & name
		, TestsCounts & counts )
	{
		m_name = name;
		m_allCounts = nullptr;
		m_rendererCounts = nullptr;
		m_categoryCounts = &counts;
		refresh();
	}

	void CategoryPanel::refresh()
	{
		auto fillCounts = [this]( auto & testCounts )
		{
			if ( m_grid )
			{
				m_grid->ClearSelection();

				for ( int i = 0; i < int( TestsCountsType::eCount ); ++i )
				{
					auto type = TestsCountsType( i );

					if ( type == TestsCountsType::eAll )
					{
						m_grid->SetCellValue( i, 2, wxString{} << testCounts.getValue( type ) );
						m_grid->SetCellValue( i, 3, wxT( "" ) );
						m_grid->SetCellValue( i, 4, wxT( "0" ) );
					}
					else
					{
						m_grid->SetCellValue( i, 2, wxString{} << testCounts.getValue( type ) );
						m_grid->SetCellValue( i, 3, wxString{} << cat::displayPercent( testCounts.getPercent( type ) ) );
						m_grid->SetCellValue( i, 4, wxString{} << testCounts.getPercent( type ) );
					}
				}
			}
			else
			{
				for ( uint32_t i = 0; i < int( TestsCountsType::eCount ); ++i )
				{
					auto type = TestsCountsType( i );

					if ( type == TestsCountsType::eAll )
					{
						m_values[type]->SetLabel( wxString{ m_name } << ": "
							<< testCounts.getValue( type ) << " tests" );
					}
					else
					{
						m_values[type]->SetLabel( wxString{ "- " }
							<< cat::getName( type ) << ": "
							<< testCounts.getValue( type ) << " ("
							<< cat::displayPercent( testCounts.getPercent( type ) ) << ")." );
					}
				}
			}
		};

		if ( m_allCounts )
		{
			fillCounts( *m_allCounts );
		}
		else if ( m_rendererCounts )
		{
			fillCounts( *m_rendererCounts );
		}
		else if ( m_categoryCounts )
		{
			fillCounts( *m_categoryCounts );
		}
	}
}
