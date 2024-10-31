#include "TestRunsList.hpp"

#include "Model/RunsModel/RunTreeModel.hpp"
#include "Model/RunsModel/RunTreeModelNode.hpp"

#include <AriaLib/Aui/AuiDockArt.hpp>
#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/sizer.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	//*********************************************************************************************

	namespace testruns
	{
		enum ID
		{
			eID_GRID,
			eID_DELETE_RUN,
		};
	}

	//*********************************************************************************************

	TestRunsPanel::TestRunsPanel( wxWindow * parent
		, wxWindowID id
		, wxSize const & size
		, TestDatabase & database
		, wxMenu * contextMenu )
		: wxPanel{ parent, id, {}, size }
		, m_database{ database }
		, m_contextMenu{ contextMenu }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_model{ new run::RunTreeModel{} }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxPanel * listPanel = new wxPanel{ this
			, wxID_ANY
			, wxDefaultPosition
			, wxDefaultSize };
		listPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		listPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizerList = new wxBoxSizer{ wxVERTICAL };
		m_view = new wxDataViewCtrl{ listPanel
			, testruns::eID_GRID
			, wxDefaultPosition
			, wxDefaultSize
			, wxDV_MULTIPLE };
		// Intently inverted back and fore.
		m_view->SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		m_view->SetForegroundColour( PANEL_BACKGROUND_COLOUR );
		sizerList->Add( m_view, wxSizerFlags( 1 ).Expand() );
		listPanel->SetSizer( sizerList );
		sizerList->SetSizeHints( listPanel );

		m_view->AssociateModel( m_model.get() );
		m_view->Connect( wxEVT_DATAVIEW_SELECTION_CHANGED
			, wxDataViewEventHandler( TestRunsPanel::onSelectionChange )
			, nullptr
			, this );
		m_view->Connect( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU
			, wxDataViewEventHandler( TestRunsPanel::onItemContextMenu )
			, nullptr
			, this );
		m_model->instantiate( m_view );

		m_auiManager.AddPane( listPanel
			, wxAuiPaneInfo()
			.Layer( 0 )
			.Caption( _( "Runs List" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( false )
			.Dockable( false )
			.Floatable( false ) );
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.Update();
	}

	TestRunsPanel::~TestRunsPanel()
	{
		m_auiManager.UnInit();
	}

	void TestRunsPanel::setTest( DatabaseTest const & test )
	{
		m_test = &test;
		m_model->clear();
		auto runs = m_database.listRuns( test.getTestId() );

		for ( auto & run : runs )
		{
			m_model->addRun( std::move( run.second ) );
		}

		m_model->expandRoots( m_view );
		m_view->Refresh();
	}

	void TestRunsPanel::deleteRun( uint32_t runId )
	{
		m_model->removeRun( runId );
	}

	void TestRunsPanel::updateRunHost( uint32_t runId )
	{
		m_model->updateRunHost( runId );
	}

	void TestRunsPanel::updateRunStatus( uint32_t runId )
	{
		m_model->updateRunStatus( runId );
	}

	wxDataViewItemArray TestRunsPanel::getSelection()const
	{
		wxDataViewItemArray result;
		m_view->GetSelections( result );
		return result;
	}

	void TestRunsPanel::onSelectionChange( wxDataViewEvent & evt )
	{
	}

	void TestRunsPanel::onItemContextMenu( wxDataViewEvent & evt )
	{
		PopupMenu( m_contextMenu );
	}
}
