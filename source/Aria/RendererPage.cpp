#include "RendererPage.hpp"

#include "TestsMainPanel.hpp"
#include "Model/TestsModel/TestTreeModel.hpp"
#include "Model/TestsModel/TestTreeModelNode.hpp"
#include "Panels/CategoryPanel.hpp"
#include "Panels/LayeredPanel.hpp"
#include "Panels/TestPanel.hpp"

#include <AriaLib/TestsCounts.hpp>
#include <AriaLib/Aui/AuiDockArt.hpp>
#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/clipbrd.h>
#include <wx/progdlg.h>
#include <wx/stattext.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	//*********************************************************************************************

	namespace rendpage
	{
		enum ID
		{
			eID_GRID,
		};

		struct TestView
		{
			enum Type : size_t
			{
				eNone,
				eTest,
			};
		};

		struct GeneralView
		{
			enum Type : size_t
			{
				eNone,
				eCategory,
			};
		};
	}

	//*********************************************************************************************

	RendererPage::RendererPage( Plugin const & plugin
		, Renderer renderer
		, RendererTestRuns & runs
		, RendererTestsCounts & counts
		, wxWindow * parent
		, TestsMainPanel * frame
		, Menus const & menus )
		: wxPanel{ parent, wxID_ANY, wxDefaultPosition, wxDefaultSize }
		, m_mainFrame{ frame }
		, m_plugin{ plugin }
		, m_renderer{ renderer }
		, m_menus{ menus }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_runs{ runs }
		, m_counts{ counts }
		, m_selectionCounts{ m_plugin }
		, m_model{ new TestTreeModel{ renderer, counts } }
	{
		doInitLayout( frame );
	}

	RendererPage::~RendererPage()
	{
		m_auiManager.UnInit();
	}

	TestTreeModelNode * RendererPage::getTestNode( DatabaseTest const & test )const
	{
		return m_model->getTestNode( test );
	}

	void RendererPage::refreshView()const
	{
		m_view->Refresh();
	}

	void RendererPage::resizeModel( wxSize const & size )
	{
		auto genSize = m_generalViews->GetSize();
		m_model->resize( m_view
			, { size.GetWidth() - genSize.GetWidth(), size.GetHeight() } );
	}

	void RendererPage::listLatestRuns( TestDatabase & database
		, TestMap const & tests
		, AllTestsCounts & counts
		, wxProgressDialog & progress
		, int & index )
	{
		for ( auto & category : database.getCategories() )
		{
			auto testsIt = tests.find( category.second.get() );
			auto & catCounts = counts.addCategory( m_renderer
				, category.second.get()
				, testsIt->second );
			m_model->addCategory( category.second.get(), catCounts );
		}

		database.listLatestRuns( m_renderer
			, tests
			, m_runs
			, progress
			, index );

		for ( auto & run : m_runs )
		{
			auto category = run.getCategory();
			auto & catCounts = counts.getCategory( m_renderer
				, category );
			catCounts.addTest( run );
			( void )m_model->addTest( run );
#if defined( _WIN32 )
			progress.Update( index++
				, _( "Filling tests list" )
				+ wxT( "\n" ) + getProgressDetails( run ) );
			progress.Fit();
#else
			progress.Update( index++ );
#endif
		}

		auto & rendCounts = counts.getRenderer( m_renderer );
		m_categoryView->update( m_renderer->name
			, rendCounts );
		m_model->expandRoots( m_view );
		m_view->Update();
		m_view->Refresh();
		m_auiManager.Update();
	}

	void RendererPage::updateTest( TestTreeModelNode * node )
	{
		m_model->ItemChanged( wxDataViewItem{ node } );
		m_categoryView->refresh();
		m_view->Refresh();
	}

	std::vector< wxDataViewItem > RendererPage::listRendererTests( Renderer renderer
		, FilterFunc filter )const
	{
		std::vector< wxDataViewItem > result;
		doListRendererTests( renderer, filter, result );
		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listRenderersTests( FilterFunc filter )const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( isRendererNode( *node ) )
			{
				doListRendererTests( node->renderer, filter, result );
			}
		}

		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listCategoryTests( Category category
		, FilterFunc filter )const
	{
		std::vector< wxDataViewItem > result;
		doListCategoryTests( category, filter, result );
		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listCategoriesTests( FilterFunc filter )const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				doListCategoryTests( node->category, filter, result );
			}
		}

		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listSelectedTests()const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			TestTreeModelNode * node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				result.push_back( item );
			}
		}

		return result;
	}

	std::vector< wxDataViewItem > RendererPage::listSelectedCategories()const
	{
		std::vector< wxDataViewItem > result;

		for ( auto & item : m_selected.items )
		{
			TestTreeModelNode * node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( isCategoryNode( *node ) )
			{
				result.push_back( item );
			}
		}

		return result;
	}

	void RendererPage::copyTestFileName()const
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );
			auto clipboard = wxClipboard::Get();

			if ( isTestNode( *node ) && clipboard->Open() )
			{
				clipboard->SetData( new wxTextDataObject( m_plugin.getTestFileName( *node->test ).GetFullPath() ) );
				clipboard->Close();
			}
		}
	}

	void RendererPage::viewTestSceneFile()
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				m_plugin.editTest( this, *node->test );
			}
		}
	}

	void RendererPage::viewTest( wxProcess * process
		, wxStaticText * statusText
		, bool async )const
	{
		if ( m_selected.items.size() == 1 )
		{
			auto & item = *m_selected.items.begin();
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( isTestNode( *node ) )
			{
				auto result = m_plugin.viewTest( process
					, *node->test
					, node->test->getRenderer()->name
					, async );
				statusText->SetLabel( _( "Viewing: " ) + node->test->getName() );

				if ( result != 0 )
				{
					wxLogError( "doViewTest" );
				}
			}
		}
	}

	void RendererPage::setTestsReferences( AllTestsCounts & counts )
	{
		if ( !m_selected.items.empty() )
		{
			m_mainFrame->pushDbJob( "setTestsReferences"
				, [this, &counts]()
				{
					for ( auto & item : m_selected.items )
					{
						auto node = static_cast< TestTreeModelNode * >( item.GetID() );

						if ( isTestNode( *node ) )
						{
							auto & run = *node->test;
							doUpdateTestStatus( run, counts, TestStatus::eNegligible, true );
							m_model->ItemChanged( item );
						}
					}
				} );
		}
	}

	void RendererPage::ignoreTestsResult( bool ignore )
	{
		if ( !m_selected.items.empty() )
		{
			m_mainFrame->pushDbJob( "ignoreTestsResult"
				, [this, ignore]()
				{
					for ( auto & item : m_selected.items )
					{
						auto node = static_cast< TestTreeModelNode * >( item.GetID() );

						if ( isTestNode( *node ) )
						{
							auto & run = *node->test;
							run.updateIgnoreResult( ignore
								, m_plugin.getEngineRefDate()
								, true );
							m_model->ItemChanged( item );
						}
					}
				} );
		}
	}

	void RendererPage::updateTestsEngineDate()
	{
		if ( !m_selected.items.empty() )
		{
			m_mainFrame->pushDbJob( "updateTestsEngineDate"
				, [this]()
				{
					for ( auto & item : m_selected.items )
					{
						auto node = static_cast< TestTreeModelNode * >( item.GetID() );

						if ( isTestNode( *node ) )
						{
							auto & run = *node->test;
							run.updateEngineDate( m_plugin.getEngineRefDate() );
						}
					}
				} );
		}
	}

	void RendererPage::updateTestsSceneDate()
	{
		if ( !m_selected.items.empty() )
		{
			m_mainFrame->pushDbJob( "updateTestsSceneDate"
				, [this]()
				{
					for ( auto & item : m_selected.items )
					{
						auto node = static_cast< TestTreeModelNode * >( item.GetID() );

						if ( isTestNode( *node ) )
						{
							auto & run = *node->test;
							run.updateTestDate();
						}
					}
				} );
		}
	}

	void RendererPage::addCategory( Category category
		, TestsCounts & catCounts )
	{
		m_model->addCategory( category
			, catCounts
			, true );
	}

	void RendererPage::addTest( DatabaseTest & dbTest )
	{
		m_model->addTest( dbTest, true );
	}

	void RendererPage::removeTest( DatabaseTest const & dbTest )
	{
		m_model->removeTest( dbTest );
	}

	void RendererPage::updateTestView( DatabaseTest const & test
		, AllTestsCounts & counts )
	{
		wxDataViewItem testItem{ getTestNode( test ) };
		m_model->ItemChanged( testItem );
		wxDataViewItem categoryItem{ m_model->GetParent( testItem ) };
		m_model->ItemChanged( categoryItem );
		wxDataViewItem rendererItem{ m_model->GetParent( categoryItem ) };
		m_model->ItemChanged( rendererItem );

		if ( m_detailViews->isLayerShown( rendpage::TestView::eTest )
			&& m_testView->getTest() == &test )
		{
			m_testView->refresh();
		}
	}

	void RendererPage::preChangeTestName( Test const & test
		, wxString const & newName )
	{
		preChangeTestName( m_runs.getTest( test.id ), newName );
	}

	void RendererPage::postChangeTestName( Test const & test
		, wxString const & oldName )
	{
		postChangeTestName( m_runs.getTest( test.id ), oldName );
	}

	void RendererPage::preChangeTestName( DatabaseTest & test
		, wxString const & newName )
	{
		removeTest( test );
	}

	void RendererPage::postChangeTestName( DatabaseTest & test
		, wxString const & oldName )
	{
		addTest( test );
	}

	void RendererPage::removeCategory( Category category )
	{
		m_model->removeCategory( category );
	}

	void RendererPage::postChangeCategoryName( Category category
		, wxString const & oldName )
	{
		m_model->renameCategory( category, oldName );
	}

	void RendererPage::changeTestsCategory( ToMoveArray const & tests
		, Category newCategory )
	{
		for ( auto & test : tests )
		{
			auto & oldTest = m_runs.getTest( test.id );

			removeTest( oldTest );
			addTest( oldTest );

			auto & oldCounts = m_counts.getCounts( test.originalCategory );
			auto & newCounts = m_counts.getCounts( newCategory );
			oldCounts.removeTest( oldTest );
			newCounts.addTest( oldTest );

			m_runs.changeCategory( oldTest
				, test.originalCategory
				, newCategory );
		}
	}

	void RendererPage::doInitLayout( wxWindow * frame )
	{
		m_viewPanel = new wxPanel{ this
			, wxID_ANY
			, wxDefaultPosition
			, wxDefaultSize };
		m_viewPanel->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_viewPanel->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		wxBoxSizer * sizerList = new wxBoxSizer{ wxVERTICAL };
		m_view = new wxDataViewCtrl{ m_viewPanel
			, rendpage::eID_GRID
			, wxDefaultPosition
			, wxDefaultSize
			, wxDV_MULTIPLE };
		// Intently inverted back and fore.
		m_view->SetBackgroundColour( PANEL_FOREGROUND_COLOUR );
		m_view->SetForegroundColour( PANEL_BACKGROUND_COLOUR );
		sizerList->Add( m_view, wxSizerFlags( 1 ).Expand() );
		m_viewPanel->SetSizer( sizerList );
		sizerList->SetSizeHints( m_viewPanel );

		m_view->AssociateModel( m_model.get() );
		m_view->Connect( wxEVT_DATAVIEW_SELECTION_CHANGED
			, wxDataViewEventHandler( RendererPage::onSelectionChange )
			, nullptr
			, this );
		m_view->Connect( wxEVT_DATAVIEW_ITEM_CONTEXT_MENU
			, wxDataViewEventHandler( RendererPage::onItemContextMenu )
			, nullptr
			, this );
		m_model->instantiate( m_view );

		auto size = frame->GetClientSize();
		m_generalViews = new LayeredPanel{ this
			, wxDefaultPosition
			, wxDefaultSize };
		m_generalViews->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_generalViews->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		auto layer = new wxPanel{ m_generalViews, wxID_ANY, wxDefaultPosition, size };
		layer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		layer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_generalViews->addLayer( layer );
		m_categoryView = new CategoryPanel{ m_generalViews, wxDefaultPosition, size, true };
		m_generalViews->addLayer( m_categoryView );
		m_generalViews->showLayer( rendpage::GeneralView::eCategory );

		m_detailViews = new LayeredPanel{ this
			, wxDefaultPosition
			, wxDefaultSize };
		m_detailViews->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_detailViews->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		layer = new wxPanel{ m_detailViews, wxID_ANY, wxDefaultPosition, size };
		layer->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		layer->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_detailViews->addLayer( layer );
		m_testView = new TestPanel{ m_detailViews, m_plugin.config, m_mainFrame->getDatabase() };
		m_detailViews->addLayer( m_testView );
		m_detailViews->showLayer( rendpage::TestView::eNone );

		m_auiManager.AddPane( m_viewPanel
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( int( m_model->getMinWidth() )
				, std::max( 200, size.y - GridLineSize * int( TestsCountsType::eCount ) ) )
			.Caption( _( "Tests List" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Center()
			.Movable( false )
			.Dockable( false )
			.Floatable( false ) );
		m_auiManager.AddPane( m_detailViews
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( std::max( 500, size.x - int( m_model->getMinWidth() ) )
				, GridLineSize * int( TestsCountsType::eCount ) )
			.Caption( _( "Details View" ) )
			.CaptionVisible( true )
			.MaximizeButton( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Right()
			.Movable( true )
			.Dockable( true )
			.Floatable( true )
			.RightDockable( true )
			.LeftDockable( true )
			.BottomDockable( true )
			.TopDockable( true ) );
		m_auiManager.AddPane( m_generalViews
			, wxAuiPaneInfo()
			.Layer( 0 )
			.MinSize( 200
				, GridLineSize * int( TestsCountsType::eCount ) )
			.Caption( _( "General View" ) )
			.CaptionVisible( true )
			.CloseButton( false )
			.PaneBorder( false )
			.Bottom()
			.Movable( true )
			.Dockable( true )
			.Floatable( true )
			.BottomDockable( true )
			.TopDockable( true ) );
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.Update();
	}

	void RendererPage::doUpdateTestStatus( DatabaseTest & test
		, AllTestsCounts & counts
		, TestStatus newStatus
		, bool reference )
	{
		test.updateStatus( newStatus, reference );
		updateTestView( test, counts );
	}

	void RendererPage::doListCategoryTests( Category category
		, FilterFunc filter
		, std::vector< wxDataViewItem > & result )const
	{
		for ( auto & run : m_runs )
		{
			if ( run.getCategory() == category
				&& filter( run ) )
			{
				result.push_back( wxDataViewItem{ getTestNode( run ) } );
			}
		}
	}

	void RendererPage::doListRendererTests( Renderer renderer
		, FilterFunc filter
		, std::vector< wxDataViewItem > & result )const
	{
		for ( auto & run : m_runs )
		{
			if ( filter( run ) )
			{
				result.push_back( wxDataViewItem{ getTestNode( run ) } );
			}
		}
	}

	void RendererPage::onSelectionChange( wxDataViewEvent & evt )
	{
		m_selected.allTests = true;
		m_selected.allCategories = true;
		m_selected.allRenderers = true;
		m_view->GetSelections( m_selected.items );
		bool displayTest = false;
		bool displayCategory = false;
		wxString name;
		wxString genName;

		if ( m_selected.items.size() == 1 )
		{
			TestTreeModelNode * node = static_cast< TestTreeModelNode * >( m_selected.items[0].GetID() );

			if ( node )
			{
				Category category{};
				Renderer renderer{};
				DatabaseTest * test{};

				if ( node->test )
				{
					test = node->test;
				}
				else if ( node->isRootNode() )
				{
					renderer = node->renderer;
				}
				else if ( node->GetParent() )
				{
					renderer = node->renderer;
					category = node->category;
				}

				if ( test )
				{
					if ( !isPending( test->getStatus() )
						&& !isRunning( test->getStatus() ) )
					{
						m_testView->setTest( *test );
						m_detailViews->showLayer( rendpage::TestView::eTest );
						category = test->getCategory();
						auto & catCounts = m_counts.getCounts( category );
						m_categoryView->update( category->name
							, catCounts );
						m_generalViews->showLayer( rendpage::GeneralView::eCategory );
						displayTest = true;
						m_selected.allTests = true;
						name = test->getName();
						genName = category->name;
					}
				}
				else if ( category )
				{
					auto & catCounts = m_counts.getCounts( category );
					m_categoryView->update( category->name
						, catCounts );
					m_detailViews->showLayer( rendpage::TestView::eNone );
					m_generalViews->showLayer( rendpage::GeneralView::eCategory );
					m_view->SetFocus();
					displayCategory = true;
					name = category->name;
					genName = name;
				}
				else if ( renderer )
				{
					m_categoryView->update( renderer->name
						, m_counts );
					m_detailViews->showLayer( rendpage::TestView::eNone );
					m_generalViews->showLayer( rendpage::GeneralView::eCategory );
					displayCategory = true;
					name = renderer->name;
					genName = name;
				}

				m_view->SetFocus();
			}
		}
		else
		{
			m_selectionCounts.clear();

			for ( auto & item : m_selected.items )
			{
				TestTreeModelNode * node = static_cast< TestTreeModelNode * >( item.GetID() );

				if ( node )
				{
					if ( node->test )
					{
						m_selectionCounts.add( node->test->getStatus() );
					}
					else if ( node->GetParent() && node->category )
					{
						m_selectionCounts.add( m_counts.getCounts( node->category ) );
					}
				}
			}

			m_categoryView->update( _( "Selection" )
				, m_selectionCounts );
			m_detailViews->showLayer( rendpage::TestView::eNone );
			m_generalViews->showLayer( rendpage::GeneralView::eCategory );
			displayCategory = true;
			name = _( "Selection" );
			genName = _( "Selection" );
		}

		for ( auto & item : m_selected.items )
		{
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );
			m_selected.allRenderers = isRendererNode( *node )
				&& m_selected.allRenderers;
			m_selected.allCategories = isCategoryNode( *node )
				&& m_selected.allCategories;
			m_selected.allTests = node->test
				&& m_selected.allTests;
		}

		if ( !displayTest && !displayCategory )
		{
			m_detailViews->hideLayers();
			m_auiManager.GetPane( m_detailViews ).Caption( _( "Details View" ) );
		}
		else
		{
			m_auiManager.GetPane( m_detailViews ).Caption( _( "Details View" ) + wxT( ": " ) + name );
		}

		m_auiManager.GetPane( m_generalViews ).Caption( _( "General View" ) + wxT( ": " ) + genName );
		m_auiManager.Update();
	}

	void RendererPage::onItemContextMenu( wxDataViewEvent & evt )
	{
		if ( !m_selected.items.empty() )
		{
			if ( m_selected.allTests )
			{
				if ( m_selected.items.size() <= 1 )
				{
					m_menus.base.test->Enable( Menus::eID_TEST_IGNORE_RESULT, true );
					m_menus.base.test->Check( Menus::eID_TEST_IGNORE_RESULT, static_cast< TestTreeModelNode * >( m_selected.items.front().GetID() )->test->getIgnoreResult() );
				}
				else
				{
					m_menus.base.test->Enable( Menus::eID_TEST_IGNORE_RESULT, false );
					m_menus.base.test->Check( Menus::eID_TEST_IGNORE_RESULT, false );
				}

				if ( m_mainFrame->areTestsRunning() )
				{
					PopupMenu( m_menus.busy.test );
				}
				else
				{
					PopupMenu( m_menus.base.test );
				}
			}
			else if ( m_selected.allCategories )
			{
				if ( m_mainFrame->areTestsRunning() )
				{
					PopupMenu( m_menus.busy.category );
				}
				else
				{
					PopupMenu( m_menus.base.category );
				}
			}
			else if ( m_selected.allRenderers )
			{
				if ( m_mainFrame->areTestsRunning() )
				{
					PopupMenu( m_menus.busy.renderer );
				}
				else
				{
					PopupMenu( m_menus.base.renderer );
				}
			}
		}
		else
		{
			if ( m_mainFrame->areTestsRunning() )
			{
				PopupMenu( m_menus.busy.renderer );
			}
			else
			{
				PopupMenu( m_menus.base.renderer );
			}
		}
	}

	//*********************************************************************************************
}
