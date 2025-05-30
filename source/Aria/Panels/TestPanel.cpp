#include "TestPanel.hpp"

#include "DiffImage.hpp"
#include "Model/RunsModel/RunTreeModelNode.hpp"

#include <AriaLib/Aui/AuiDockArt.hpp>
#include <AriaLib/Aui/AuiTabArt.hpp>
#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>
#include <wx/filename.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	namespace test
	{
		enum ID
		{
			eID_PAGES,
			eID_RESULTS,
			eID_STATS,
			eID_RUNS,
			eID_DELETE_RUN,
			eID_CHANGE_CPU,
			eID_CHANGE_GPU,
			eID_CHANGE_PLAT,
			eID_CHANGE_STATUS,
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
		m_runsListMenu->Append( test::eID_DELETE_RUN, _( "Delete Run" ) + wxT( "\tCTRL+D" ) );
		m_runsListMenu->Append( test::eID_CHANGE_CPU, _( "Change CPU" ) + wxT( "\tCTRL+C" ) );
		m_runsListMenu->Append( test::eID_CHANGE_GPU, _( "Change GPU" ) + wxT( "\tCTRL+G" ) );
		m_runsListMenu->Append( test::eID_CHANGE_PLAT, _( "Change Platform" ) + wxT( "\tCTRL+P" ) );
		m_runsListMenu->Append( test::eID_CHANGE_STATUS, _( "Change Status" ) + wxT( "\tCTRL+S" ) );
		m_runsListMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( TestPanel::onMenuOption )
			, nullptr
			, this );

		m_pages = new wxAuiNotebook{ this
			, test::eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH };
		m_pages->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_pages->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_pages->SetArtProvider( new AuiTabArt );

		auto size = GetClientSize();
		m_results = new TestResultsPanel{ this, test::eID_RESULTS, size, m_config };
		m_pages->AddPage( m_results, _( "Results" ) );

		m_stats = new TestStatsPanel{ this, test::eID_STATS, size, m_database };
		m_pages->AddPage( m_stats, _( "Statistics" ) );

		m_runs = new TestRunsPanel{ this, test::eID_RUNS, size, m_database, m_runsListMenu };
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
		Freeze();
		auto sel = m_pages->GetSelection();
		m_results->refresh();
		m_stats->refresh();
		m_pages->SetSelection( size_t( sel ) );
		Thaw();
		m_auiManager.Update();
	}

	void TestPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		Freeze();
		auto sel = m_pages->GetSelection();
		m_results->setTest( test );
		m_results->refresh();
		m_stats->setTest( test );
		m_runs->setTest( test );
		m_pages->SetSelection( size_t( sel ) );
		Thaw();
		m_auiManager.Update();
	}

	void TestPanel::doDeleteRun()
	{
		auto selection = m_runs->getSelection();

		for ( auto item : selection )
		{
			auto node = reinterpret_cast< run::RunTreeModelNode const * >( static_cast< void * >( item ) );
			m_database.deleteRun( node->run.id );
			m_runs->deleteRun( node->run.id );
			m_stats->deleteRun( uint32_t( node->run.host->id )
				, node->run.id );
		}
	}

	void TestPanel::doChangeCpu()
	{
		wxArrayString choices;

		for ( auto & it : m_database.getCpus() )
		{
			choices.push_back( it.first );
		}

		wxSingleChoiceDialog choice{ this
			, _( "Select the wanted CPU" )
			, _( "CPU Change" )
			, choices };

		if ( choice.ShowModal() == wxID_OK )
		{
			wxString selected = choice.GetStringSelection();
			auto it = m_database.getCpus().find( makeStdString( selected ) );

			if ( it == m_database.getCpus().end() )
			{
				wxMessageBox( wxString{} << _( "Invalid CPU name: " ) << selected
					, _( "Error" )
					, wxICON_ERROR );
			}
			else
			{
				auto selection = m_runs->getSelection();

				for ( auto item : selection )
				{
					auto node = reinterpret_cast< run::RunTreeModelNode * >( static_cast< void * >( item ) );
					node->run.host = m_database.getHost( node->run.host->platform->name
						, it->first
						, node->run.host->gpu->name );
					m_database.updateRunHost( node->run.id, node->run.host->id );
					m_runs->updateRunHost( node->run.id );
				}

				m_stats->refresh();
			}
		}
	}

	void TestPanel::doChangeGpu()
	{
		wxArrayString choices;

		for ( auto & it : m_database.getGpus() )
		{
			choices.push_back( it.first );
		}

		wxSingleChoiceDialog choice{ this
			, _( "Select the wanted GPU" )
			, _( "GPU Change" )
			, choices };

		if ( choice.ShowModal() == wxID_OK )
		{
			wxString selected = choice.GetStringSelection();
			auto it = m_database.getGpus().find( makeStdString( selected ) );

			if ( it == m_database.getGpus().end() )
			{
				wxMessageBox( wxString{} << _( "Invalid GPU name: " ) << selected
					, _( "Error" )
					, wxICON_ERROR );
			}
			else
			{
				auto selection = m_runs->getSelection();

				for ( auto item : selection )
				{
					auto node = reinterpret_cast< run::RunTreeModelNode * >( static_cast< void * >( item ) );
					node->run.host = m_database.getHost( node->run.host->platform->name
						, node->run.host->cpu->name
						, it->first );
					m_database.updateRunHost( node->run.id, node->run.host->id );
					m_runs->updateRunHost( node->run.id );
				}

				m_stats->refresh();
			}
		}
	}

	void TestPanel::doChangePlatform()
	{
		wxArrayString choices;

		for ( auto & it : m_database.getPlatforms() )
		{
			choices.push_back( it.first );
		}

		wxSingleChoiceDialog choice{ this
			, _( "Select the wanted platform" )
			, _( "Platform Change" )
			, choices };

		if ( choice.ShowModal() == wxID_OK )
		{
			wxString selected = choice.GetStringSelection();
			auto it = m_database.getPlatforms().find( makeStdString( selected ) );

			if ( it == m_database.getPlatforms().end() )
			{
				wxMessageBox( wxString{} << _( "Invalid platform name: " ) << selected
					, _( "Error" )
					, wxICON_ERROR );
			}
			else
			{
				auto selection = m_runs->getSelection();

				for ( auto item : selection )
				{
					auto node = reinterpret_cast< run::RunTreeModelNode * >( static_cast< void * >( item ) );
					node->run.host = m_database.getHost( it->first
						, node->run.host->cpu->name
						, node->run.host->gpu->name );
					m_database.updateRunHost( node->run.id, node->run.host->id );
					m_runs->updateRunHost( node->run.id );
				}

				m_stats->refresh();
			}
		}
	}

	void TestPanel::doChangeStatus()
	{
		wxArrayString choices;
		choices.push_back( _( "Not Run" ) );
		choices.push_back( _( "Negligible" ) );
		choices.push_back( _( "Acceptable" ) );
		choices.push_back( _( "Unacceptable" ) );
		choices.push_back( _( "Unprocessed" ) );
		choices.push_back( _( "Crashed" ) );

		wxSingleChoiceDialog choice{ this
			, _( "Select the wanted run status" )
			, _( "Run Status Change" )
			, choices };

		if ( choice.ShowModal() == wxID_OK )
		{
			auto status = RunStatus( choice.GetSelection() );
			auto selection = m_runs->getSelection();

			for ( auto item : selection )
			{
				auto node = reinterpret_cast< run::RunTreeModelNode * >( static_cast< void * >( item ) );
				node->run.status = status;
				m_database.updateRunStatus( node->run.id, status );
				m_runs->updateRunStatus( node->run.id );
			}

			m_stats->refresh();
		}
	}

	void TestPanel::onMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case test::eID_DELETE_RUN:
			doDeleteRun();
			break;
		case test::eID_CHANGE_CPU:
			doChangeCpu();
			break;
		case test::eID_CHANGE_GPU:
			doChangeGpu();
			break;
		case test::eID_CHANGE_PLAT:
			doChangePlatform();
			break;
		case test::eID_CHANGE_STATUS:
			doChangeStatus();
			break;
		}
	}
}
