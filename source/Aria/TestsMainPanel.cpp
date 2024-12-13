#include "TestsMainPanel.hpp"

#include "DiffImage.hpp"
#include "ConfigurationDialog.hpp"
#include "MainFrame.hpp"
#include "RendererPage.hpp"
#include "FileSystem/GitFileSystemPlugin.hpp"
#include "Model/TestsModel/TestTreeModel.hpp"
#include "Model/TestsModel/TestTreeModelNode.hpp"
#include "Panels/CategoryPanel.hpp"
#include "Panels/LayeredPanel.hpp"
#include "Panels/TestPanel.hpp"

#include <AriaLib/Options.hpp>
#include <AriaLib/TestsCounts.hpp>
#include <AriaLib/Aui/AuiDockArt.hpp>
#include <AriaLib/Aui/AuiTabArt.hpp>
#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/DbResult.hpp>
#include <AriaLib/Database/DbStatement.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/choicdlg.h>
#include <wx/dc.h>
#include <wx/filedlg.h>
#include <wx/gauge.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/progdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textdlg.h>

#include <fstream>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	//*********************************************************************************************

	namespace tests
	{
		// Wait maximum 10 mins for a test run.
		static int constexpr timerKillTimeout = 1000 * 60 * 10;

		static Category selectCategory( wxWindow * parent
			, TestDatabase const & database )
		{
			wxArrayString categories;

			for ( auto & category : database.getCategories() )
			{
				categories.push_back( category.first );
			}

			wxSingleChoiceDialog categoryDlg{ parent
				, _( "Select the test's category" )
				, _( "Test creation" )
				, categories };
			Category result{};

			if ( categoryDlg.ShowModal() == wxID_OK )
			{
				wxString categoryName = categoryDlg.GetStringSelection();
				auto catIt = database.getCategories().find( makeStdString( categoryName ) );

				if ( catIt == database.getCategories().end() )
				{
					wxMessageBox( wxString{} << _( "Invalid category name: " ) << categoryName
						, wxT( "Error" )
						, wxICON_ERROR );
				}
				else
				{
					result = catIt->second.get();
				}
			}

			return result;
		}

		static FileSystemPtr createFileSystem( wxFrame * parent
			, wxWindowID handlerID
			, wxFileName const & curDir )
		{
			FileSystemPtr result = std::make_unique< FileSystem >();
			result->registerThreadedPlugin< Git >( parent, handlerID, result.get(), curDir );
			return result;
		}

		static TestTimes processTestOutputTimes( TestDatabase & database
			, wxFileName const & timesFilePath )
		{
			TestTimes result{};

			if ( timesFilePath.FileExists() )
			{
				{
					std::ifstream file{ timesFilePath.GetFullPath().ToStdString() };

					if ( file.is_open() )
					{
						std::string line;
						auto lineIndex = 0u;
						uint32_t t, a, l;
						std::string platform, cpu, gpu;

						while ( std::getline( file, line ) && lineIndex < 4u )
						{
							switch ( lineIndex )
							{
							case 0u:
								platform = line;
								break;
							case 1u:
								cpu = line;
								break;
							case 2u:
								gpu = line;
								break;
							case 3u:
								{
									std::stringstream stream{ line };
									stream >> t >> a >> l;
								}
								break;
							default:
								break;
							}

							++lineIndex;
						}

						result.host = database.getHost( platform, cpu, gpu );
						result.total = Microseconds{ t };
						result.avg = Microseconds{ a };
						result.last = Microseconds{ l };
					}
				}
				wxRemoveFile( timesFilePath.GetFullPath() );
			}
			else
			{
				result.host = database.getHost( "Unknown", "Unknown", "Unknown" );
			}

			return result;
		}
	}

	//*********************************************************************************************

	TestsMainPanel::TestProcess::TestProcess( wxEvtHandler * mainframe
		, int flags )
		: wxProcess{ flags }
		, m_mainframe{ mainframe }
	{
	}

	void TestsMainPanel::TestProcess::OnTerminate( int pid, int status )
	{
		auto event = new wxProcessEvent{ wxID_ANY, pid, status };
		m_mainframe->QueueEvent( event );
	}

	//*********************************************************************************************

	TestNode TestsMainPanel::RunningTest::current()
	{
		return running;
	}

	void TestsMainPanel::RunningTest::push( TestNode node )
	{
		pending.emplace_back( std::move( node ) );
	}

	TestNode TestsMainPanel::RunningTest::next()
	{
		if ( !pending.empty() )
		{
			running = *pending.begin();
			running.status = TestStatus::eRunning_0;
			pending.erase( pending.begin() );
		}
		else
		{
			running = {};
		}

		return running;
	}

	void TestsMainPanel::RunningTest::end()
	{
		running = {};
	}

	void TestsMainPanel::RunningTest::clear()
	{
		for ( auto & it : pending )
		{
			it.test->updateStatusNW( it.status );
		}

		pending.clear();

		if ( running.test )
		{
			running.test->updateStatusNW( running.status );
		}

		running = {};
	}

	bool TestsMainPanel::RunningTest::empty()const
	{
		return size() == 0u;
	}

	size_t TestsMainPanel::RunningTest::size()const
	{
		return pending.size()
			+ ( running.test ? 1u : 0u );
	}

	bool TestsMainPanel::RunningTest::isRunning()const
	{
		return running.test != nullptr;
	}

	//*********************************************************************************************

	TestsMainPanel::TestsMainPanel( wxFrame * parent
		, Plugin * plugin
		, Menus const & menus )
		: wxPanel{ parent }
		, m_plugin{ plugin }
		, m_config{ m_plugin->config }
		, m_menus{ menus }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_fileSystem{ tests::createFileSystem( parent, eID_GIT, m_config.test ) }
		, m_database{ *m_plugin, *m_fileSystem }
		, m_timerKillRun{ new wxTimer{ this, eID_TIMER_KILL_RUN } }
		, m_testUpdater{ new wxTimer{ this, eID_TIMER_TEST_UPDATER } }
		, m_categoriesUpdater{ new wxTimer{ this, eID_TIMER_CATEGORY_UPDATER } }
	{
		m_tests.runs = std::make_shared< AllTestRuns >( m_database );
		SetMinClientSize( { 900, 600 } );
	}

	TestsMainPanel::~TestsMainPanel()
	{
		if ( m_thread.joinable() )
		{
			m_thread.join();
		}

		m_fileSystem->cleanup();
		m_categoriesUpdater->Stop();
		m_testUpdater->Stop();
		m_timerKillRun->Stop();

		if ( m_runningTest.disProcess )
		{
			m_runningTest.disProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.disProcess = nullptr;
		}

		if ( m_runningTest.genProcess )
		{
			m_runningTest.genProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.genProcess = nullptr;
		}

		m_testsPages.clear();
		m_auiManager.UnInit();
	}

	void TestsMainPanel::initialise()
	{
		{
			wxProgressDialog progress{ _( "Initialising" )
				, _( "Initialising..." )
				, 1
				, this };
			int index = 0;
			m_database.initialise( progress, index );
			m_tests.tests = m_database.listTests( progress, index );
			doInitGui();
		}
		auto onTimer = [this]( wxTimerEvent & evt )
		{
			if ( evt.GetId() == eID_TIMER_TEST_UPDATER )
			{
				onTestUpdateTimer( evt );
			}
			else if ( evt.GetId() == eID_TIMER_CATEGORY_UPDATER )
			{
				onCategoryUpdateTimer( evt );
			}
			else if ( evt.GetId() == eID_TIMER_KILL_RUN )
			{
				if ( wxProcess::Exists( int( m_runningTest.genProcess->GetPid() ) ) )
				{
					auto res = wxProcess::Kill( int( m_runningTest.genProcess->GetPid() ), wxSIGKILL );

					switch ( res )
					{
					case wxKILL_OK:
						break;
					case wxKILL_BAD_SIGNAL:
						wxLogError( "Couldn't kill process: bad signal." );
						break;
					case wxKILL_ACCESS_DENIED:
						wxLogError( "Couldn't kill process: access denied." );
						break;
					case wxKILL_NO_PROCESS:
						wxLogError( "Couldn't kill process: no process." );
						break;
					case wxKILL_ERROR:
						wxLogError( "Couldn't kill process: error." );
						break;
					default:
						wxLogError( wxString{ wxT( "Couldn't kill process: unknown error: " ) } << res );
						break;
					}
				}
			}
			else
			{
				evt.Skip();
			}
		};
		Bind( wxEVT_TIMER
			, onTimer
			, eID_TIMER_KILL_RUN );
		Bind( wxEVT_TIMER
			, onTimer
			, eID_TIMER_TEST_UPDATER );
		m_testUpdater->Start( 100 );
		Bind( wxEVT_TIMER
			, onTimer
			, eID_TIMER_CATEGORY_UPDATER );
		m_testUpdater->Start( 100 );
		{
			wxProgressDialog progress{ _( "Initialising" )
				, _( "Initialising..." )
				, 1
				, this };
			int index = 0;
			doFillLists( progress, index );
		}

		m_runningTest.genProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
		m_runningTest.disProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );

		Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( TestsMainPanel::onProcessEnd )
			, nullptr
			, this );
		m_statusText->SetLabel( _( "Idle" ) );

		m_fileSystem->initialise();
		auto statusBar = m_menus.statusBar;
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	TestTreeModelNode * TestsMainPanel::getTestNode( DatabaseTest const & test )
	{
		auto rendIt = m_testsPages.find( test->renderer );
		assert( rendIt != m_testsPages.end() );
		return rendIt->second->getTestNode( test );
	}

	wxDataViewItem TestsMainPanel::getTestItem( DatabaseTest const & test )
	{
		return wxDataViewItem{ getTestNode( test ) };
	}

	void TestsMainPanel::pushDbJob( std::string name
		, std::function< void() > job )
	{
		if ( m_thread.joinable() )
		{
			m_thread.join();
		}

		m_thread = std::thread{ [this, name, job]()
			{
				if ( auto transaction = m_database.beginTransaction( name ) )
				{
					try
					{
						job();
						transaction.commit();
					}
					catch ( std::exception & exc )
					{
						wxLogError( "Failure: %s.", exc.what() );
						transaction.rollback();
					}
					catch ( ... )
					{
						wxLogError( "Failure: Unknown exception." );
						transaction.rollback();
					}
				}
			} };
	}

	void TestsMainPanel::editConfig()
	{
		ConfigurationDialog dialog{ this, *m_plugin };
		dialog.ShowModal();
	}

	void TestsMainPanel::onRendererMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case Menus::eID_RENDERER_RUN_TESTS_ALL:
			doRunAllRendererTests( 1u );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_ALL_5:
			doRunAllRendererTests( 5u );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_NOTRUN:
			doRunRendererTests( TestStatus::eNotRun );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_ACCEPTABLE:
			doRunRendererTests( TestStatus::eAcceptable );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_UNACCEPTABLE:
			doRunRendererTests( TestStatus::eUnacceptable );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_CRASHED:
			doRunRendererTests( TestStatus::eCrashed );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllRendererTestsBut( TestStatus::eNegligible );
			break;
		case Menus::eID_RENDERER_RUN_TESTS_OUTDATED:
			doRunAllRendererOutdatedTests();
			break;
		case Menus::eID_RENDERER_UPDATE_ENGINE:
			doUpdateRendererEngineDate();
			break;
		case Menus::eID_RENDERER_UPDATE_SCENE:
			doUpdateRendererSceneDate();
			break;
		case Menus::eID_RENDERER_CREATE_CATEGORY:
			doNewCategory();
			break;
		}
	}

	void TestsMainPanel::onCategoryMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case Menus::eID_CATEGORY_RUN_TESTS_ALL:
			doRunAllCategoryTests( 1u );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_ALL_5:
			doRunAllCategoryTests( 5u );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_NOTRUN:
			doRunCategoryTests( TestStatus::eNotRun );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_ACCEPTABLE:
			doRunCategoryTests( TestStatus::eAcceptable );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_UNACCEPTABLE:
			doRunCategoryTests( TestStatus::eUnacceptable );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_CRASHED:
			doRunCategoryTests( TestStatus::eCrashed );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllCategoryTestsBut( TestStatus::eNegligible );
			break;
		case Menus::eID_CATEGORY_RUN_TESTS_OUTDATED:
			doRunAllCategoryOutdatedTests();
			break;
		case Menus::eID_CATEGORY_UPDATE_ENGINE:
			doUpdateCategoryEngineDate();
			break;
		case Menus::eID_CATEGORY_UPDATE_SCENE:
			doUpdateCategorySceneDate();
			break;
		case Menus::eID_CATEGORY_ADD_NUMPREFIX:
			doAddCategoryNumPrefix();
			break;
		case Menus::eID_CATEGORY_REMOVE_NUMPREFIX:
			doRemoveCategoryNumPrefix();
			break;
		case Menus::eID_CATEGORY_CHANGE_NAME:
			doChangeCategoryName();
			break;
		case Menus::eID_CATEGORY_CREATE_TEST:
			{
				auto items = m_selectedPage->listSelectedCategories();

				for ( auto item : items )
				{
					auto node = static_cast< TestTreeModelNode * >( item.GetID() );
					doNewTest( node->category );
				}
			}
			break;
		case Menus::eID_CATEGORY_DELETE:
			doDeleteCategory();
			break;
		}
	}

	void TestsMainPanel::onTestMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case Menus::eID_TEST_RUN:
			doRunTest( 1u );
			break;
		case Menus::eID_TEST_RUN_5:
			doRunTest( 5u );
			break;
		case Menus::eID_TEST_COPY_FILE_NAME:
			doCopyTestFileName();
			break;
		case Menus::eID_TEST_VIEW_FILE:
			doViewTestSceneFile();
			break;
		case Menus::eID_TEST_VIEW_SYNC:
			doViewTest( false );
			break;
		case Menus::eID_TEST_VIEW_ASYNC:
			doViewTest( true );
			break;
		case Menus::eID_TEST_SET_REF:
			doSetRef();
			break;
		case Menus::eID_TEST_IGNORE_RESULT:
			doIgnoreTestResult();
			break;
		case Menus::eID_TEST_UPDATE_ENGINE:
			doUpdateEngineDate();
			break;
		case Menus::eID_TEST_UPDATE_SCENE:
			doUpdateSceneDate();
			break;
		case Menus::eID_TEST_CHANGE_CATEGORY:
			doChangeTestCategory();
			break;
		case Menus::eID_TEST_CHANGE_NAME:
			doChangeTestName();
			break;
		case Menus::eID_TEST_DELETE:
			doDeleteTest();
			break;
		case Menus::eID_TEST_DELETE_REFERENCE:
			doDeleteReference();
			break;
		case Menus::eID_CANCEL_RUNS:
			doCancel();
			break;
		}
	}

	void TestsMainPanel::onDatabaseMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_DB_NEW_RENDERER:
			doNewRenderer();
			break;
		case eID_DB_NEW_CATEGORY:
			doNewCategory();
			break;
		case eID_DB_NEW_TEST:
			doNewTest();
			break;
		case eID_DB_EXPORT_LATEST_TIMES:
			doExportLatestTimes();
			break;
		}
	}

	wxWindow * TestsMainPanel::doInitTestsLists()
	{
		m_testsBook = new wxAuiNotebook{ this
			, eID_TESTS_BOOK
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH };
		m_testsBook->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		m_testsBook->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_testsBook->SetArtProvider( new AuiTabArt );
		m_testsBook->Connect( wxEVT_AUINOTEBOOK_PAGE_CHANGED
			, wxAuiNotebookEventHandler( TestsMainPanel::onTestsPageChange )
			, nullptr
			, this );
		m_tests.counts = std::make_shared< AllTestsCounts >( *m_plugin );

		for ( auto & renderer : m_database.getRenderers() )
		{
			doInitTestsList( renderer.second.get() );
		}

		m_testsBook->SetSelection( 0u );
		return m_testsBook;
	}

	void TestsMainPanel::doInitTestsList( Renderer renderer )
	{
		auto & rendererRuns = m_tests.runs->addRenderer( renderer );
		auto & rendererCounts = m_tests.counts->addRenderer( renderer );
		auto it = m_testsPages.find( renderer );

		if ( it == m_testsPages.end() )
		{
			auto page = new RendererPage{ *m_plugin
				, renderer
				, rendererRuns
				, rendererCounts
				, m_testsBook
				, this
				, m_menus };
			m_testsPages.emplace( renderer, page );
			it = m_testsPages.find( renderer );
		}

		auto testsPage = it->second;
		m_testsBook->AddPage( testsPage, renderer->name );
	}

	void TestsMainPanel::doInitGui()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		doInitTestsLists();

		auto statusBar = m_menus.statusBar;
		auto sizer = new wxBoxSizer{ wxHORIZONTAL };
		statusBar->SetSizer( sizer );
		statusBar->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		statusBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		auto size = wxSize( 100, statusBar->GetSize().GetHeight() - 6 );
		m_testProgress = new wxGauge{ statusBar, wxID_ANY, 100, wxPoint( 10, 3 ), size, wxGA_SMOOTH, wxDefaultValidator };
		m_testProgress->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_testProgress->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_testProgress->SetValue( 0 );
		m_testProgress->SetMinSize( size );
		m_testProgress->Hide();
		sizer->Add( m_testProgress, wxSizerFlags{}.Border( wxLEFT, 10 ).FixedMinSize().ReserveSpaceEvenIfHidden() );

		m_statusText = new wxStaticText{ statusBar, wxID_ANY, _( "Idle" ), wxPoint( 120, 5 ), wxDefaultSize, 0 };
		m_statusText->SetBackgroundColour( INACTIVE_TAB_COLOUR );
		m_statusText->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		sizer->Add( m_statusText, wxSizerFlags{}.Border( wxLEFT, 5 ) );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();

		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_testsBook
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

	void TestsMainPanel::doFillLists( wxProgressDialog & progress
		, int & index )
	{
		uint32_t testCount = 0u;

		for ( auto & test : m_tests.tests )
		{
			testCount += uint32_t( test.second.size() );
		}

		wxLogMessage( "Filling Data View" );
		progress.SetTitle( _( "Filling Data View" ) );
		progress.SetRange( progress.GetRange() + int( testCount * m_database.getRenderers().size() ) );
		progress.Update( index, _( "Filling Data View..." ) );

		for ( auto & renderer : *m_tests.runs )
		{
			doFillList( renderer.first, progress, index );
		}
	}

	void TestsMainPanel::doFillList( Renderer renderer
		, wxProgressDialog & progress
		, int & index )
	{
		auto rendIt = m_testsPages.find( renderer );
		assert( rendIt != m_testsPages.end() );
		rendIt->second->listLatestRuns( m_database
			, m_tests.tests
			, *m_tests.counts
			, progress
			, index );
	}

	RendererPage * TestsMainPanel::doGetPage( wxDataViewItem const & item )
	{
		auto node = static_cast< TestTreeModelNode * >( item.GetID() );
		auto rendIt = m_testsPages.find( node->test
			? node->test->getRenderer()
			: node->renderer );

		if ( rendIt != m_testsPages.end() )
		{
			return rendIt->second;
		}

		return nullptr;
	}

	uint32_t TestsMainPanel::doGetAllTestsRange()const
	{
		uint32_t range = 0u;

		for ( auto & category : m_tests.tests )
		{
			range += uint32_t( category.second.size() );
		}

		return range;
	}

	void TestsMainPanel::doProcessTest()
	{
		auto testNode = m_runningTest.next();

		if ( !m_cancelled
			&& testNode.test )
		{
			auto & test = *testNode.test;
			test.updateStatusNW( TestStatus::eRunning_Begin );
			auto page = doGetPage( wxDataViewItem{ testNode.node } );

			if ( page )
			{
				page->updateTest( testNode.node );
				m_statusText->SetLabel( _( "Running test: " ) + test.getName() );
				m_timerKillRun->StartOnce( tests::timerKillTimeout );
				auto result = m_plugin->runTest( m_runningTest.genProcess.get()
					, test
					, testNode.test->getRenderer()->name );
#if Aria_UseAsync

				if ( result == 0 )
				{
					wxLogError( "doProcessTest failed to launch the test" );
				}
				else
				{
					m_runningTest.currentProcess = m_runningTest.genProcess.get();
				}

#else
				onTestRunEnd( wxProcessEvent{} );
#endif
				m_testProgress->SetValue( m_testProgress->GetValue() + 1 );
			}
		}
		else
		{
			m_statusText->SetLabel( _( "Idle" ) );
			m_testProgress->Hide();
		}

		auto statusBar = m_menus.statusBar;
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void TestsMainPanel::doStartTests()
	{
		m_testProgress->SetRange( int( m_runningTest.size() ) );

		if ( !m_runningTest.isRunning() )
		{
			m_testProgress->SetValue( 0 );
			m_testProgress->Show();
			doProcessTest();
		}

		if ( m_selectedPage )
		{
			m_selectedPage->refreshView();
		}
	}

	void TestsMainPanel::doPushTest( wxDataViewItem & item
		, uint32_t count )
	{
		auto node = static_cast< TestTreeModelNode * >( item.GetID() );
		auto & run = *node->test;

		for ( uint32_t i = 0u; i < count; ++i )
		{
			m_runningTest.push( { &run, run.getStatus(), node } );
		}

		run.updateStatusNW( TestStatus::ePending );
		m_selectedPage->updateTest( node );
	}

	void TestsMainPanel::doClearRunning()
	{
		m_runningTest.clear();
	}

	void TestsMainPanel::doRunTest( uint32_t count )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto tests = m_selectedPage->listSelectedTests();

			if ( !tests.empty() )
			{
				for ( auto & item : tests )
				{
					doPushTest( item, count );
				}

				doStartTests();
			}
		}
	}

	void TestsMainPanel::doCopyTestFileName()
	{
		if ( m_selectedPage )
		{
			m_selectedPage->copyTestFileName();
		}
	}

	void TestsMainPanel::doViewTestSceneFile()
	{
		if ( m_selectedPage )
		{
			m_selectedPage->viewTestSceneFile();
		}
	}

	void TestsMainPanel::doViewTest( bool async )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->viewTest( m_runningTest.disProcess.get()
				, m_statusText
				, async );
		}

		if ( m_runningTest.empty() )
		{
			m_statusText->SetLabel( _( "Idle" ) );
		}
		else
		{
			m_statusText->SetLabel( _( "Running Test" ) );
		}

		auto statusBar = m_menus.statusBar;
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void TestsMainPanel::doSetRef()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->setTestsReferences( *m_tests.counts );
			m_fileSystem->commit( "Updated reference images" );
		}
	}

	void TestsMainPanel::doIgnoreTestResult()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			m_selectedPage->ignoreTestsResult( m_menus.base.test->IsChecked( Menus::eID_TEST_IGNORE_RESULT ) );
		}
	}

	void TestsMainPanel::doUpdateEngineDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			m_selectedPage->updateTestsEngineDate();
		}
	}

	void TestsMainPanel::doUpdateSceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->updateTestsSceneDate();
		}
	}

	void TestsMainPanel::doChangeTestCategory()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			if ( auto category = tests::selectCategory( this, m_database ) )
			{
				auto items = m_selectedPage->listSelectedTests();
				pushDbJob( "changeTestCategory"
					, [this, category, items]()
					{
						using wxAsyncUpdateTestCategoryCallback = std::function< void() >;
						using wxAsyncUpdateTestCategory = wxAsyncMethodCallEventFunctor< wxAsyncUpdateTestCategoryCallback >;

						RendererPage::ToMoveArray toMove;
						struct CatChange
						{
							Test * test;
							Category category;
						};
						std::vector< CatChange > changes;

						for ( auto & item : items )
						{
							auto node = static_cast< TestTreeModelNode * >( item.GetID() );

							if ( node->category != category )
							{
								toMove.push_back( { node->category, node->test->getTestId() } );

								auto catIt = m_tests.tests.find( node->category );
								auto testIt = std::find_if( catIt->second.begin()
									, catIt->second.end()
									, [&node]( TestPtr const & lookup )
									{
										return lookup->id == node->test->getTestId();
									} );
								auto test = std::move( *testIt );
								catIt->second.erase( testIt );

								changes.push_back( { test.get(), test->category } );
								test->category = category;
								catIt = m_tests.tests.find( category );
								catIt->second.emplace_back( std::move( test ) );
							}
						}

						for ( auto & page : m_testsPages )
						{
							auto evtHandler = page.second;
							QueueEvent( new wxAsyncUpdateTestCategory{ evtHandler
								, [evtHandler, &toMove, &category]()
								{
									evtHandler->changeTestsCategory( toMove, category );
								} } );
						}

						for ( auto & change : changes )
						{
							m_plugin->changeTestCategory( *change.test
								, change.category
								, category
								, *m_fileSystem );
							m_database.updateTestCategory( *change.test, category );
						}
					} );
			}
		}
	}

	void TestsMainPanel::doRenameTest( DatabaseTest & dbTest
		, std::string const & newName
		, std::string const & commitText
		, bool commit )
	{
		auto & test = *dbTest->test;
		auto oldName = test.name;

		for ( auto & page : m_testsPages )
		{
			page.second->preChangeTestName( *dbTest->test, newName );
		}

		m_database.updateTestName( test, newName );
		test.name = newName;

		for ( auto & page : m_testsPages )
		{
			page.second->postChangeTestName( *dbTest->test, oldName );
		}

		if ( commit )
		{
			if ( commitText.empty() )
			{
				m_fileSystem->commit( wxString{} << "Renamed test [" << oldName << "] to [" << newName << "]." );
			}
			else
			{
				m_fileSystem->commit( makeWxString( commitText ) );
			}
		}
	}

	void TestsMainPanel::doChangeTestName()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listSelectedTests();
			size_t index = 0u;
			std::string commitText = items.size() == 1u
				? std::string{}
			: std::string{ "Bulk rename." };

			for ( auto & item : items )
			{
				++index;
				auto node = static_cast< TestTreeModelNode * >( item.GetID() );
				wxTextEntryDialog dialog{ this
					, _( "Enter a new name for " ) + makeWxString( node->test->getName() )
					, _( "Renaming test" )
					, node->test->getName() };

				if ( dialog.ShowModal() == wxID_OK )
				{
					doRenameTest( *node->test
						, makeStdString( dialog.GetValue() )
						, commitText
						, index == items.size() );
				}
			}
		}
	}

	void TestsMainPanel::doRemoveTest( DatabaseTest & dbTest
		, std::string const & commitText
		, bool commit )
	{
		auto & test = *dbTest->test;
		auto name = test.name;

		for ( auto & page : m_testsPages )
		{
			page.second->removeTest( dbTest );
		}

		m_database.deleteTest( uint32_t( test.id ) );
		m_plugin->deleteTest( test, *m_fileSystem );
		m_fileSystem->removeFile( test.name
			, m_config.test / getReferenceFolder( test ) / getReferenceName( test )
			, true );
		m_fileSystem->removeFile( test.name
			, m_config.work / getResultFolder( test ) / getResultName( *dbTest )
			, false );

		if ( commit )
		{
			if ( commitText.empty() )
			{
				m_fileSystem->commit( wxString{} << "Removed test [" << name << "]." );
			}
			else
			{
				m_fileSystem->commit( makeWxString( commitText ) );
			}
		}
	}

	void TestsMainPanel::doRemoveReference( DatabaseTest & dbTest
		, std::string const & commitText
		, bool commit )
	{
		auto & test = *dbTest->test;
		auto name = test.name;

		m_fileSystem->removeFile( test.name
			, m_config.test / getReferenceFolder( test ) / getReferenceName( test )
			, true );

		if ( commit )
		{
			if ( commitText.empty() )
			{
				m_fileSystem->commit( wxString{} << "Removed reference for test [" << name << "]." );
			}
			else
			{
				m_fileSystem->commit( makeWxString( commitText ) );
			}
		}
	}

	void TestsMainPanel::doDeleteTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			wxMessageDialog dialog{ this
				, _( "Do you want to delete this test ?" )
				, _( "Confirm suppression" )
				, wxICON_QUESTION | wxOK | wxCANCEL };

			if ( dialog.ShowModal() == wxID_OK )
			{
				auto items = m_selectedPage->listSelectedTests();
				size_t index = 0u;
				std::string commitText = items.size() == 1u
					? std::string{}
				: std::string{ "Bulk delete." };

				for ( auto & item : items )
				{
					++index;
					auto node = static_cast< TestTreeModelNode * >( item.GetID() );
					doRemoveTest( *node->test
						, commitText
						, index == items.size() );
				}
			}
		}
	}

	void TestsMainPanel::doDeleteReference()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			wxMessageDialog dialog{ this
				, _( "Do you want to delete those tests' reference images ?" )
				, _( "Confirm suppression" )
				, wxICON_QUESTION | wxOK | wxCANCEL };

			if ( dialog.ShowModal() == wxID_OK )
			{
				auto items = m_selectedPage->listSelectedTests();
				size_t index = 0u;
				std::string commitText = items.size() == 1u
					? std::string{}
					: std::string{ "Bulk reference delete." };

				for ( auto & item : items )
				{
					++index;
					auto node = static_cast< TestTreeModelNode * >( item.GetID() );
					doRemoveReference( *node->test
						, commitText
						, index == items.size() );
				}
			}
		}
	}

	void TestsMainPanel::doRenameCategory( Category category
		, std::string const & newName
		, std::string const & commitText
		, bool commit )
	{
		auto oldName = category->name;
		m_database.updateCategoryName( category, newName );

		for ( auto & page : m_testsPages )
		{
			page.second->postChangeCategoryName( category, oldName );
		}

		m_fileSystem->moveFolder( m_config.test
			, oldName
			, newName
			, true );

		if ( commit )
		{
			if ( commitText.empty() )
			{
				m_fileSystem->commit( wxString{} << "Renamed category [" << oldName << "] to [" << newName << "]." );
			}
			else
			{
				m_fileSystem->commit( makeWxString( commitText ) );
			}
		}
	}

	void TestsMainPanel::doChangeCategoryName()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listSelectedCategories();
			size_t index = 0u;
			std::string commitText = items.size() == 1u
				? std::string{}
			: std::string{ "Bulk rename." };

			for ( auto & item : items )
			{
				++index;
				auto node = static_cast< TestTreeModelNode * >( item.GetID() );
				wxTextEntryDialog dialog{ this
					, _( "Enter a new name for " ) + makeWxString( node->category->name )
					, _( "Renaming category" )
					, node->category->name };

				if ( dialog.ShowModal() == wxID_OK )
				{
					doRenameCategory( node->category
						, makeStdString( dialog.GetValue() )
						, commitText
						, index == items.size() );
				}
			}
		}
	}

	void TestsMainPanel::doRemoveCategory( Category category
		, std::string const & commitText
		, bool commit )
	{
		auto name = category->name;

		for ( auto & page : m_testsPages )
		{
			page.second->removeCategory( category );
		}

		m_fileSystem->removeFolder( m_config.test
			, category->name
			, true );
		m_database.deleteCategory( category );

		if ( commit )
		{
			if ( commitText.empty() )
			{
				m_fileSystem->commit( wxString{} << "Removed category [" << name << "]." );
			}
			else
			{
				m_fileSystem->commit( makeWxString( commitText ) );
			}
		}
	}

	void TestsMainPanel::doDeleteCategory()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listSelectedCategories();
			size_t index = 0u;
			std::string commitText = items.size() == 1u
				? std::string{}
			: std::string{ "Bulk remove." };

			for ( auto & item : items )
			{
				++index;
				auto node = static_cast< TestTreeModelNode * >( item.GetID() );
				doRemoveCategory( node->category
					, commitText
					, index == items.size() );
			}
		}
	}

	void TestsMainPanel::doRunAllCategoryTests( uint32_t count )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( []( DatabaseTest const & lookup )
				{
					return true;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, count );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doRunCategoryTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status == filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, 1u );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doRunAllCategoryTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status != filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, 1u );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doRunAllCategoryOutdatedTests()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( [this]( DatabaseTest const & lookup )
				{
					return m_plugin->isOutOfDate( *lookup )
						|| lookup->status == TestStatus::eNotRun;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, 1u );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doUpdateCategoryEngineDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfEngineDate();
				} );
			pushDbJob( "updateCategoryEngineDate"
				, [this, items]()
				{
					for ( auto & item : items )
					{
						auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
						auto & run = *node->test;
						run.updateEngineDate( m_plugin->getEngineRefDate() );
					}

					using wxAsyncRefreshViewCallback = std::function< void() >;
					using wxAsyncRefreshView = wxAsyncMethodCallEventFunctor< wxAsyncRefreshViewCallback >;
					auto evtHandler = m_selectedPage;
					QueueEvent( new wxAsyncRefreshView{ evtHandler
						, [evtHandler]()
						{
							evtHandler->refreshView();
						} } );
				} );
		}
	}

	void TestsMainPanel::doUpdateCategorySceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfTestDate();
				} );
			pushDbJob( "updateCategorySceneDate"
				, [this, items]()
				{
					for ( auto & item : items )
					{
						auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
						auto & run = *node->test;
						auto testDate = m_plugin->getTestDate( *run );
						run.updateTestDate( testDate );

						using wxAsyncRefreshViewCallback = std::function< void() >;
						using wxAsyncRefreshView = wxAsyncMethodCallEventFunctor< wxAsyncRefreshViewCallback >;
						auto evtHandler = m_selectedPage;
						QueueEvent( new wxAsyncRefreshView{ evtHandler
							, [evtHandler]()
							{
								evtHandler->refreshView();
							} } );
					}
				} );
		}
	}

	void TestsMainPanel::doAddCategoryNumPrefix()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( []( DatabaseTest const & lookup )
				{
					return true;
				} );
			pushDbJob( "addCategoryNumPrefix"
				, [this, items]()
				{
					uint32_t index{};
					std::string commitText = items.size() == 1u
						? std::string{}
					: std::string{ "Bulk rename." };

					for ( auto & item : items )
					{
						auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
						auto & run = *node->test;
						++index;
						doRenameTest( run
							, run.getPrefixedName( uint32_t( index ) )
							, commitText
							, index == items.size() );
					}

					using wxAsyncRefreshViewCallback = std::function< void() >;
					using wxAsyncRefreshView = wxAsyncMethodCallEventFunctor< wxAsyncRefreshViewCallback >;
					auto evtHandler = m_selectedPage;
					QueueEvent( new wxAsyncRefreshView{ evtHandler
						, [evtHandler]()
						{
							evtHandler->refreshView();
						} } );
				} );
		}
	}

	void TestsMainPanel::doRemoveCategoryNumPrefix()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoriesTests( []( DatabaseTest const & lookup )
				{
					return lookup.hasNumPrefix();
				} );
			pushDbJob( "removeCategoryNumPrefix"
				, [this, items]()
				{
					uint32_t index{};
					std::string commitText = items.size() == 1u
						? std::string{}
					: std::string{ "Bulk rename." };

					for ( auto & item : items )
					{
						auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
						auto & run = *node->test;
						++index;
						doRenameTest( run
							, run.getUnprefixedName()
							, commitText
							, index == items.size() );
					}

					using wxAsyncRefreshViewCallback = std::function< void() >;
					using wxAsyncRefreshView = wxAsyncMethodCallEventFunctor< wxAsyncRefreshViewCallback >;
					auto evtHandler = m_selectedPage;
					QueueEvent( new wxAsyncRefreshView{ evtHandler
						, [evtHandler]()
						{
							evtHandler->refreshView();
						} } );
				} );
		}
	}

	void TestsMainPanel::doRunAllRendererTests( uint32_t count )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRenderersTests( []( DatabaseTest const & lookup )
				{
					return true;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, count );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doRunRendererTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRenderersTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status == filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, 1u );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doRunAllRendererTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRenderersTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status != filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, 1u );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doRunAllRendererOutdatedTests()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRenderersTests( [this]( DatabaseTest const & lookup )
				{
					return m_plugin->isOutOfDate( *lookup )
						|| lookup->status == TestStatus::eNotRun;
				} );

			for ( auto & item : items )
			{
				doPushTest( item, 1u );
			}

			doStartTests();
		}
	}

	void TestsMainPanel::doUpdateRendererEngineDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRenderersTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfEngineDate();
				} );
			pushDbJob( "updateRendererEngineDate"
				, [this, items]()
				{
					for ( auto & item : items )
					{
						auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
						auto & run = *node->test;
						run.updateEngineDate( m_plugin->getEngineRefDate() );
					}

					using wxAsyncRefreshViewCallback = std::function< void() >;
					using wxAsyncRefreshView = wxAsyncMethodCallEventFunctor< wxAsyncRefreshViewCallback >;
					auto evtHandler = m_selectedPage;
					QueueEvent( new wxAsyncRefreshView{ evtHandler
						, [evtHandler]()
						{
							evtHandler->refreshView();
						} } );
				} );
		}
	}

	void TestsMainPanel::doUpdateRendererSceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRenderersTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfTestDate();
				} );
			pushDbJob( "updateRendererEngineDate"
				, [this, items]()
				{
					for ( auto & item : items )
					{
						auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
						auto & run = *node->test;
						auto testDate = m_plugin->getTestDate( *run );
						run.updateTestDate( testDate );
					}

					using wxAsyncRefreshViewCallback = std::function< void() >;
					using wxAsyncRefreshView = wxAsyncMethodCallEventFunctor< wxAsyncRefreshViewCallback >;
					auto evtHandler = m_selectedPage;
					QueueEvent( new wxAsyncRefreshView{ evtHandler
						, [evtHandler]()
						{
							evtHandler->refreshView();
						} } );
				} );
		}
	}

	std::vector< wxDataViewItem > TestsMainPanel::doListAllTests( FilterFunc filter )
	{
		DatabaseTestArray runs;
		m_tests.runs->listTests( filter, runs );
		std::vector< wxDataViewItem > result;

		for ( auto & run : runs )
		{
			result.push_back( getTestItem( *run ) );
		}

		return result;
	}

	void TestsMainPanel::doCancelTest( DatabaseTest & test
		, TestStatus status )
	{
		test.updateStatusNW( status );
		auto node = getTestNode( test );
		wxDataViewItem item = wxDataViewItem{ node };
		auto page = doGetPage( item );

		if ( page )
		{
			m_selectedPage->updateTest( node );
		}

		doClearRunning();
		m_statusText->SetLabel( _( "Idle" ) );
		m_testProgress->Hide();
		auto statusBar = m_menus.statusBar;
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void TestsMainPanel::doCancel()
	{
		m_cancelled.exchange( true );
	}

	void TestsMainPanel::doNewRenderer()
	{
		wxTextEntryDialog dialog{ this, _( "Enter the new renderer name" ) };

		if ( dialog.ShowModal() == wxID_OK )
		{
			auto renderer = m_database.createRenderer( makeStdString( dialog.GetValue() ) );
			m_tests.runs->addRenderer( renderer );
			auto range = doGetAllTestsRange();
			wxProgressDialog progress{ _( "Creating renderer entries" )
				, _( "Creating renderer entries..." )
				, int( range )
				, this };
			int index = 0;
			doInitTestsList( renderer );
			doFillList( renderer, progress, index );
		}
	}

	void TestsMainPanel::doNewCategory()
	{
		wxTextEntryDialog dialog{ this
			, _( "Enter the new category name" )
			, _( "Category creation" ) };

		if ( dialog.ShowModal() == wxID_OK )
		{
			auto categoryName = makeStdString( dialog.GetValue() );
			auto catIt = m_database.getCategories().find( makeStdString( categoryName ) );

			if ( catIt != m_database.getCategories().end() )
			{
				wxMessageBox( wxString{} << _( "Invalid category name: " ) << categoryName
					, _( "Error" )
					, wxICON_ERROR );
				return;
			}

			auto category = m_database.createCategory( makeStdString( categoryName ) );
			m_tests.tests.emplace( category, TestArray{} );

			if ( !( m_config.test / category->name ).DirExists() )
			{
				wxMkdir( ( m_config.test / category->name ).GetFullName() );
			}

			for ( auto & rendererIt : m_database.getRenderers() )
			{
				auto renderer = rendererIt.second.get();

				auto & rendCounts = m_tests.counts->getRenderer( renderer );
				auto & catCounts = rendCounts.addCategory( category, {} );

				auto rendPageIt = m_testsPages.find( renderer );
				rendPageIt->second->addCategory( category, catCounts );
			}
		}
	}

	void TestsMainPanel::doNewTest( Category category )
	{
		if ( !category )
		{
			category = tests::selectCategory( this, m_database );
		}

		if ( category )
		{
			wxTextEntryDialog dialog{ this
				, _( "Enter the new test name" )
				, _( "Test creation" ) };

			if ( dialog.ShowModal() == wxID_OK )
			{
				auto testName = makeStdString( dialog.GetValue() );
				auto catTestIt = m_tests.tests.find( category );
				catTestIt->second.emplace_back( std::make_unique< Test >( 0
					, testName
					, category ) );
				auto & test = *catTestIt->second.back();
				m_database.insertTest( test, false );

				for ( auto & rendererIt : m_database.getRenderers() )
				{
					auto renderer = rendererIt.second.get();
					auto & rendRuns = m_tests.runs->getRenderer( renderer );
					auto & rendCounts = m_tests.counts->getRenderer( renderer );
					auto & catCounts = rendCounts.getCounts( category );
					auto & dbTest = rendRuns.addTest( TestRun{ &test
						, renderer
						, db::DateTime{}
						, TestStatus::eNotRun
						, db::DateTime{}
						, db::DateTime{}
					, TestTimes{} } );

					auto rendPageIt = m_testsPages.find( renderer );
					rendPageIt->second->addTest( dbTest );
					catCounts.addTest( dbTest );
				}

				m_plugin->createTest( test, *m_fileSystem );
				m_plugin->editTest( this, test );
			}
		}
	}

	void TestsMainPanel::doExportLatestTimes()
	{
		std::stringstream stream;
		stream.imbue( std::locale{ "C" } );

		for ( auto & runs : *m_tests.runs )
		{
			for ( auto & run : runs.second )
			{
				if ( run->times.total != Microseconds{ 0ull } )
				{
					stream << run.getRenderer()->name
						<< " - " << run.getCategory()->name
						<< " - " << run.getName()
						<< " - " << run->times.total.count()
						<< " - " << run->times.avg.count()
						<< " - " << run->times.last.count()
						<< "\n";
				}
			}
		}

		auto fileName = wxSaveFileSelector( _( "Renderer tests latest times" )
			, "TXT files (*.txt)|*.txt" );
		std::ofstream file{ fileName.ToStdString() };

		if ( file.is_open() )
		{
			file << stream.str();
		}
	}

	void TestsMainPanel::onTestRunEnd( int status )
	{
		auto testNode = m_runningTest.current();
		auto & run = *testNode.test;
		m_timerKillRun->Stop();

		if ( status < 0 && status != std::numeric_limits< int >::max() )
		{
			wxLogError( wxString() << "Test run failed (" << status << ")" );
		}

		if ( !m_cancelled )
		{
			DiffOptions options;
			auto file = ( m_config.test / run.getCategory()->name / m_plugin->getTestName( *run ) );
			options.input = file.GetPath() / ( file.GetName() + wxT( "_ref.png" ) );
			options.outputs.emplace_back( file.GetPath() / wxT( "Compare" ) / ( file.GetName() + wxT( "_" ) + run.getRenderer()->name + wxT( ".png" ) ) );
			auto times = tests::processTestOutputTimes( m_database
				, file.GetPath() / wxT( "Compare" ) / ( file.GetName() + wxT( "_" ) + run.getRenderer()->name + wxT( ".times" ) ) );

			try
			{
				DiffConfig config{ options };

				for ( auto output : options.outputs )
				{
					compareImages( options, config, output );
				}

				onTestDiffEnd( times );
			}
			catch ( std::exception & exc )
			{
				wxLogWarning( wxString() << "Test result comparison not possible: " << exc.what() );
				TestNode curTestNode = m_runningTest.current();
				auto & test = *curTestNode.test;
				test.createNewRun( TestStatus::eUnprocessed
					, wxDateTime::Now()
					, times );

				auto page = doGetPage( wxDataViewItem{ curTestNode.node } );

				if ( page )
				{
					page->updateTest( curTestNode.node );
					page->updateTestView( test, *m_tests.counts );
				}

				doProcessTest();
			}
		}
		else
		{
			doCancelTest( run, testNode.node->statusName.status );
		}
	}

	void TestsMainPanel::onTestDisplayEnd( int status )
	{
		wxLogMessage( wxString() << "Test display ended (" << status << ")" );
	}

	void TestsMainPanel::onTestDiffEnd( TestTimes const & times )
	{
		wxLogMessage( wxString() << "Test run ended" );
		TestNode testNode = m_runningTest.current();
		auto & test = *testNode.test;

		if ( !m_cancelled )
		{
			auto resultName = getResultName( *test ).GetFullName();
			auto matches = filterDirectoryFiles( m_config.test / getCompareFolder( *test->test )
				, [&resultName]( wxString const & folder, wxString name )
				{
					return name.find( resultName ) == 0u;
				}
			, true );
			assert( matches.size() < 2 );

			if ( !matches.empty() )
			{
				test.createNewRun( matches[0], times );
			}
			else
			{
				test.createNewRun( TestStatus::eCrashed
					, wxDateTime::Now()
					, times );
			}

			auto page = doGetPage( wxDataViewItem{ testNode.node } );

			if ( page )
			{
				page->updateTest( testNode.node );
				page->updateTestView( test, *m_tests.counts );
			}

			doProcessTest();
		}
		else
		{
			doCancelTest( *testNode.test, testNode.node->statusName.status );
		}
	}

	bool TestsMainPanel::onTestProcessEnd( int pid, int status )
	{
		auto currentProcess = m_runningTest.currentProcess;
		m_runningTest.currentProcess = nullptr;

		if ( currentProcess )
		{
			if ( currentProcess == m_runningTest.disProcess.get() )
			{
				onTestDisplayEnd( status );
				return true;
			}

			if ( currentProcess == m_runningTest.genProcess.get() )
			{
				onTestRunEnd( status );
				return true;
			}
		}

		return false;
	}

	void TestsMainPanel::onTestsPageChange( wxAuiNotebookEvent & evt )
	{
		if ( m_testsBook->GetPageCount() > 0 )
		{
			m_selectedPage = static_cast< RendererPage * >( m_testsBook->GetPage( size_t( m_testsBook->GetSelection() ) ) );
		}
	}

	void TestsMainPanel::onProcessEnd( wxProcessEvent & evt )
	{
		if ( !onTestProcessEnd( evt.GetPid(), evt.GetExitCode() ) )
		{
			evt.Skip();
		}
	}

	void TestsMainPanel::onTestUpdateTimer( wxTimerEvent & evt )
	{
		auto testNode = m_runningTest.current();

		if ( !testNode.test )
		{
			return;
		}

		TestTreeModelNode * node{ testNode.node };
		auto updatePage = [this]( TestTreeModelNode * treeNode )
		{
			auto page = doGetPage( wxDataViewItem{ treeNode } );

			if ( page )
			{
				page->updateTest( treeNode );
			}

			return treeNode->GetParent();
		};

		if ( node && isTestNode( *node ) )
		{
			node->test->updateStatusNW( ( node->test->getStatus() == TestStatus::eRunning_End )
				? TestStatus::eRunning_Begin
				: TestStatus( uint32_t( node->test->getStatus() ) + 1 ) );
			node = updatePage( node );
		}

		if ( node && isCategoryNode( *node ) )
		{
			node->statusName.status = ( node->statusName.status == TestStatus::eRunning_End )
				? TestStatus::eRunning_Begin
				: TestStatus( uint32_t( node->statusName.status ) + 1 );
			node = updatePage( node );
		}

		if ( node && isRendererNode( *node ) )
		{
			node->statusName.status = ( node->statusName.status == TestStatus::eRunning_End )
				? TestStatus::eRunning_Begin
				: TestStatus( uint32_t( node->statusName.status ) + 1 );
			updatePage( node );
		}

		evt.Skip();
	}

	void TestsMainPanel::onCategoryUpdateTimer( wxTimerEvent & evt )
	{
		evt.Skip();
	}

	void TestsMainPanel::onSize( wxSizeEvent & evt )
	{
		for ( auto & testPageIt : m_testsPages )
		{
			auto page = testPageIt.second;

			if ( page )
			{
				auto size = GetClientSize();
				page->resizeModel( size );
			}
		}

		evt.Skip();
	}

	//*********************************************************************************************
}
