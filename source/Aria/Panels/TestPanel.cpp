#include "TestPanel.hpp"

#include "DiffImage.hpp"
#include "Aui/AuiDockArt.hpp"
#include "Aui/AuiTabArt.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"

#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/filename.h>

namespace aria
{
	namespace
	{
		enum ID
		{
			eID_PAGES,
			eID_RESULTS,
			eID_STATS,
		};
	}

	TestPanel::TestPanel( wxWindow * parent
		, Config const & config
		, TestDatabase & database )
		: wxPanel{ parent }
		, m_config{ config }
		, m_database{ database }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_pages = new wxAuiNotebook{ this
			, eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH };
		m_pages->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pages->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_pages->SetArtProvider( new AuiTabArt );

		auto size = GetClientSize();
		m_results = new TestResultsPanel{ this, eID_RESULTS, size, m_config };
		m_pages->AddPage( m_results, _( "Results" ) );

		m_stats = new TestStatsPanel{ this, eID_STATS, size, m_database };
		m_pages->AddPage( m_stats, _( "Statistics" ) );

		m_pages->SetSelection( 0u );

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_pages
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( 0, 200 )
			.CaptionVisible( false )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( false )
			.Dockable( false ) );
		m_auiManager.Update();
	}

	TestPanel::~TestPanel()
	{
		m_auiManager.UnInit();
	}

	void TestPanel::refresh()
	{
		m_results->refresh();
		m_stats->refresh();
		m_auiManager.Update();
	}

	void TestPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		m_results->setTest( test );
		m_stats->setTest( test );
		refresh();
	}
}
