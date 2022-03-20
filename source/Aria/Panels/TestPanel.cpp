#include "TestPanel.hpp"

#include "DiffImage.hpp"
#include "Aui/AuiDockArt.hpp"
#include "Aui/AuiTabArt.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"
#include "Model/RunsModel/RunTreeModelNode.hpp"

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
			eID_RUNS,
			eID_DELETE_RUN,
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

		m_runsListMenu = new wxMenu{};
		m_runsListMenu->Append( eID_DELETE_RUN, _( "Delete Run" ) + wxT( "\tCTRL+D" ) );
		m_runsListMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( TestPanel::onMenuOption )
			, nullptr
			, this );

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

		m_runs = new TestRunsPanel{ this, eID_RUNS, size, m_database, m_runsListMenu };
		m_pages->AddPage( m_runs, _( "Runs" ) );

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
		m_runs->setTest( test );
		refresh();
	}

	void TestPanel::doDeleteRun()
	{
		auto selection = m_runs->getSelection();

		for ( auto item : selection )
		{
			auto node = reinterpret_cast< run::RunTreeModelNode const * >( ( void * )item );
			m_database.deleteRun( node->run.id );
			m_runs->deleteRun( node->run.id );
			m_stats->deleteRun( node->run.id );
		}
	}

	void TestPanel::onMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_DELETE_RUN:
			doDeleteRun();
			break;
		}
	}
}
