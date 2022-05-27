#include "MainFrame.hpp"

#include "ConfigurationDialog.hpp"
#include "DiffImage.hpp"
#include "RendererPage.hpp"
#include "TestsCounts.hpp"
#include "Aui/AuiDockArt.hpp"
#include "Aui/AuiTabArt.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/DbResult.hpp"
#include "Database/DbStatement.hpp"
#include "FileSystem/GitFileSystemPlugin.hpp"
#include "Model/TestsModel/TestTreeModel.hpp"
#include "Model/TestsModel/TestTreeModelNode.hpp"
#include "Panels/CategoryPanel.hpp"
#include "Panels/LayeredPanel.hpp"
#include "Panels/TestPanel.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
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
#pragma warning( pop )

#include <fstream>

#define Aria_DebugTimerKill 0


namespace aria
{
	//*********************************************************************************************

	namespace main
	{
#if Aria_DebugTimerKill
		static int constexpr timerKillTimeout = 1000;
#else
		// Wait maximum 10 mins for a test run.
		static int constexpr timerKillTimeout = 1000 * 60 * 10;
#endif
		static wxString const & getVersion()
		{
			static wxString const result{ wxString{ wxT( "v" ) } << Aria_VERSION_MAJOR << wxT( "." ) << Aria_VERSION_MINOR << wxT( "." ) << Aria_VERSION_BUILD };
			return result;
		}

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
					wxMessageBox( wxString{} << wxT( "Invalid category name: " ) << categoryName
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

		static TestTimes doProcessTestOutputTimes( TestDatabase & database
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

						while ( std::getline( file, line ) )
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

		static wxFileName getOldResultName( TestRun const & test )
		{
			return wxFileName{ test.test->name + "_" + test.renderer->name + ".png" };
		}

		static wxFileName getOldReferenceName( Test const & test )
		{
			return wxFileName{ test.name + "_ref.png" };
		}
	}

	//*********************************************************************************************

	MainFrame::TestProcess::TestProcess( MainFrame * mainframe
		, int flags )
		: wxProcess{ flags }
		, m_mainframe{ mainframe }
	{
	}

	void MainFrame::TestProcess::OnTerminate( int pid, int status )
	{
		auto event = new wxProcessEvent{ wxID_ANY, pid, status };
		m_mainframe->QueueEvent( event );
	}

	//*********************************************************************************************

	TestNode MainFrame::RunningTest::current()
	{
		return running;
	}

	void MainFrame::RunningTest::push( TestNode node )
	{
		pending.emplace_back( std::move( node ) );
	}

	TestNode MainFrame::RunningTest::next()
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

	void MainFrame::RunningTest::end()
	{
		running = {};
	}

	void MainFrame::RunningTest::clear()
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

	bool MainFrame::RunningTest::empty()const
	{
		return size() == 0u;
	}

	size_t MainFrame::RunningTest::size()const
	{
		return pending.size()
			+ ( running.test ? 1u : 0u );
	}

	bool MainFrame::RunningTest::isRunning()const
	{
		return running.test != nullptr;
	}

	//*********************************************************************************************

	MainFrame::MainFrame( PluginPtr plugin )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Aria " ) + main::getVersion(), wxDefaultPosition, wxSize( 800, 700 ) }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_plugin{ std::move( plugin ) }
		, m_config{ m_plugin->config }
		, m_fileSystem{ main::createFileSystem( this, eID_GIT, m_config.test ) }
		, m_database{ *m_plugin, *m_fileSystem }
		, m_timerKillRun{ new wxTimer{ this, eID_TIMER_KILL_RUN } }
		, m_testUpdater{ new wxTimer{ this, eID_TIMER_TEST_UPDATER } }
		, m_categoriesUpdater{ new wxTimer{ this, eID_TIMER_CATEGORY_UPDATER } }
	{
		m_tests.runs = std::make_shared< AllTestRuns >( m_database );
		SetClientSize( 900, 600 );
		doInitMenus();
		doInitMenuBar();
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, 1
				, this };
			int index = 0;
			m_database.initialise( progress, index );
			m_tests.tests = m_database.listTests( progress, index );
			doInitGui();
		}
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & evt )
			{
				onClose( evt );
			} );
		Bind( wxEVT_SIZE
			, [this]( wxSizeEvent & evt )
			{
				onSize( evt );
			} );
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
	}

	MainFrame::~MainFrame()
	{
		m_testsPages.clear();
		m_auiManager.UnInit();
	}

	void MainFrame::initialise()
	{
		{
			wxProgressDialog progress{ wxT( "Initialising" )
				, wxT( "Initialising..." )
				, 1
				, this };
			int index = 0;
			doFillLists( progress, index );
		}

		m_runningTest.genProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );
		m_runningTest.disProcess = std::make_unique< TestProcess >( this, wxPROCESS_DEFAULT );

		Connect( wxEVT_END_PROCESS
			, wxProcessEventHandler( MainFrame::onProcessEnd )
			, nullptr
			, this );
		m_statusText->SetLabel( _( "Idle" ) );

		m_fileSystem->initialise();
		auto statusBar = GetStatusBar();
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void MainFrame::doInitTestsList( Renderer renderer )
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
				, m_testMenu.get()
				, m_categoryMenu.get()
				, m_rendererMenu.get()
				, m_allMenu.get()
				, m_busyTestMenu.get()
				, m_busyCategoryMenu.get()
				, m_busyRendererMenu.get()
				, m_busyAllMenu.get() };
			m_testsPages.emplace( renderer, page );
			it = m_testsPages.find( renderer );
		}

		auto testsPage = it->second;
		m_testsBook->AddPage( testsPage, renderer->name );
	}

	wxWindow * MainFrame::doInitTestsLists()
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
			, wxAuiNotebookEventHandler( MainFrame::onTestsPageChange )
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

	void MainFrame::doInitGui()
	{
		SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		doInitTestsLists();

		auto statusBar = CreateStatusBar();
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

	void MainFrame::doInitMenus( )
	{
		auto addTestBaseMenus = []( wxMenu & menu )
		{
			uint32_t i = 2;
			menu.Append( eID_TEST_COPY_FILE_NAME, _( "Copy test file path" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_VIEW_FILE, _( "View test scene file" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_SET_REF, _( "Set Reference" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_VIEW_SYNC, _( "View Test (sync)" ) + wxT( "\tF" ) << ( i ) );
			menu.Append( eID_TEST_VIEW_ASYNC, _( "View Test (async)" ) + wxT( "\tCtrl+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( eID_TEST_IGNORE_RESULT, _( "Ignore result" ) + wxT( "\tF" ) << ( i++ ), wxEmptyString, true );
			menu.Append( eID_TEST_UPDATE_CASTOR, _( "Update Engine's date" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_UPDATE_SCENE, _( "Update Scene's date" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_CHANGE_CATEGORY, _( "Change test category" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_CHANGE_NAME, _( "Change test name" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( eID_TEST_DELETE, _( "Delete test" ) + wxT( "\tCTRL+D" ) );
		};
		auto addRendererMenus = []( wxMenu & menu )
		{
			uint32_t i = 1;
			wxString modKey = "CTRL";
			menu.Append( eID_RENDERER_RUN_TESTS_ALL, _( "Run all renderer's tests" ) + wxT( "\t" ) + modKey  + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_RENDERER_RUN_TESTS_NOTRUN, _( "Run all <not run> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_RENDERER_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_RENDERER_RUN_TESTS_CRASHED, _( "Run all <crashed> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_RENDERER_RUN_TESTS_OUTDATED, _( "Run all outdated renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( eID_RENDERER_UPDATE_CASTOR, _( "Update renderer's tests Engine's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_RENDERER_UPDATE_SCENE, _( "Update renderer's tests Scene's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
		};
		auto addCategoryMenus = []( wxMenu & menu )
		{
			uint32_t i = 1;
			wxString modKey = "ALT";
			menu.Append( eID_CATEGORY_RUN_TESTS_ALL, _( "Run all category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_NOTRUN, _( "Run all <not run> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_CRASHED, _( "Run all <crashed> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_RUN_TESTS_OUTDATED, _( "Run all outdated category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( eID_CATEGORY_ADD_NUMPREFIX, _( "Add category's tests numeric prefix" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_REMOVE_NUMPREFIX, _( "Remove category's tests numeric prefix (if any)" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( eID_CATEGORY_UPDATE_CASTOR, _( "Update category's tests Engine's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_UPDATE_SCENE, _( "Update category's tests Scene's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_CHANGE_NAME, _( "Change category name" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_CATEGORY_DELETE, _( "Delete category" ) + wxT( "\t" ) + modKey + wxT( "+CTRL+D" ) );
		};
		auto addAllMenus = []( wxMenu & menu )
		{
			uint32_t i = 1;
			wxString modKey = "CTRL+ALT";
			menu.Append( eID_ALL_RUN_TESTS_ALL, _( "Run all tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_ALL_RUN_TESTS_NOTRUN, _( "Run all <not run> tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_ALL_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_ALL_RUN_TESTS_CRASHED, _( "Run all <crashed> tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_ALL_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_ALL_RUN_TESTS_OUTDATED, _( "Run all outdated tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( eID_ALL_UPDATE_CASTOR, _( "Update tests Engine's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( eID_ALL_UPDATE_SCENE, _( "Update tests Scene's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
		};
		m_testMenu = std::make_unique< wxMenu >();
		m_testMenu->Append( eID_TEST_RUN, _( "Run Test" ) + wxT( "\tF1" ) );
		addTestBaseMenus( *m_testMenu );
		m_testMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barTestMenu = new wxMenu;
		m_barTestMenu->Append( eID_TEST_RUN, _( "Run Test" ) + wxT( "\tF1" ) );
		addTestBaseMenus( *m_barTestMenu );
		m_barTestMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_busyTestMenu = std::make_unique< wxMenu >();
		m_busyTestMenu->Append( eID_TEST_RUN, _( "Run Test" ) + wxT( "\tF1" ) );
		addTestBaseMenus( *m_busyTestMenu );
		m_busyTestMenu->Append( eID_CANCEL, _( "Cancel runs" ) + wxT( "\tSHIFT+F1" ) );
		m_busyTestMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_categoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_categoryMenu );
		m_categoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barCategoryMenu = new wxMenu;
		addCategoryMenus( *m_barCategoryMenu );
		m_barCategoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_busyCategoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_busyCategoryMenu );
		m_busyCategoryMenu->Append( eID_CANCEL, _( "Cancel runs" ) + wxT( "\tSHIFT+F1" ) );
		m_busyCategoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		
		m_rendererMenu = std::make_unique< wxMenu >();
		addRendererMenus( *m_rendererMenu );
		m_rendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barRendererMenu = new wxMenu;
		addRendererMenus( *m_barRendererMenu );
		m_barRendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_busyRendererMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_busyRendererMenu );
		m_busyRendererMenu->Append( eID_CANCEL, _( "Cancel runs" ) + wxT( "\tSHIFT+F1" ) );
		m_busyRendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_allMenu = std::make_unique< wxMenu >();
		addAllMenus( *m_allMenu );
		m_allMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
		m_barAllMenu = new wxMenu;
		addAllMenus( *m_barAllMenu );
		m_barAllMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );

		m_busyAllMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_busyAllMenu );
		m_busyAllMenu->Append( eID_CANCEL, _( "Cancel runs" ) + wxT( "\tSHIFT+F1" ) );
		m_busyAllMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onTestsMenuOption )
			, nullptr
			, this );
	}

	void MainFrame::doInitMenuBar()
	{
		wxMenuBar * menuBar{ new wxMenuBar };

		wxMenu * configMenu{ new wxMenu };
		configMenu->Append( eID_EDIT_CONFIG, _( "Edit configuration" ) );
		configMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onConfigMenuOption )
			, nullptr
			, this );
		menuBar->Append( configMenu, _( "Configuration" ) );

		wxMenu * rendererMenu{ new wxMenu };
		rendererMenu->Append( eID_DB_NEW_RENDERER, _( "New Renderer" ) );
		rendererMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );

		wxMenu * categoryMenu{ new wxMenu };
		categoryMenu->Append( eID_DB_NEW_CATEGORY, _( "New Category" ) );
		categoryMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );

		wxMenu * testMenu{ new wxMenu };
		testMenu->Append( eID_DB_NEW_TEST, _( "New Test" ) );
		testMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );

		wxMenu * databaseMenu{ new wxMenu };
		databaseMenu->AppendSubMenu( rendererMenu, _( "Renderer" ) );
		databaseMenu->AppendSubMenu( categoryMenu, _( "Category" ) );
		databaseMenu->AppendSubMenu( testMenu, _( "Test" ) );
		databaseMenu->Append( eID_DB_EXPORT_LATEST_TIMES, _( "Export latest times" ) );
		databaseMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onDatabaseMenuOption )
			, nullptr
			, this );
		menuBar->Append( databaseMenu, _( "Database" ) );

		wxMenu * testsMenu{ new wxMenu };
		testsMenu->AppendSubMenu( m_barTestMenu, _( "Single" ) );
		testsMenu->AppendSubMenu( m_barCategoryMenu, _( "Category" ) );
		testsMenu->AppendSubMenu( m_barRendererMenu, _( "Renderer" ) );
		testsMenu->AppendSubMenu( m_barAllMenu, _( "All" ) );
		menuBar->Append( testsMenu, _( "Tests" ) );

		SetMenuBar( menuBar );
	}

	void MainFrame::doFillLists( wxProgressDialog & progress
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

	void MainFrame::doFillList( Renderer renderer
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

	RendererPage * MainFrame::doGetPage( wxDataViewItem const & item )
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

	uint32_t MainFrame::doGetAllTestsRange()const
	{
		uint32_t range = 0u;

		for ( auto & category : m_tests.tests )
		{
			range += uint32_t( category.second.size() );
		}

		return range;
	}

	void MainFrame::doProcessTest()
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
				m_timerKillRun->StartOnce( main::timerKillTimeout );
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

		auto statusBar = GetStatusBar();
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void MainFrame::doStartTests()
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

	void MainFrame::doPushTest( wxDataViewItem & item )
	{
		auto node = static_cast< TestTreeModelNode * >( item.GetID() );
		auto & run = *node->test;
		m_runningTest.push( { &run, run.getStatus(), node } );
		run.updateStatusNW( TestStatus::ePending );
		m_selectedPage->updateTest( node );
	}

	void MainFrame::doClearRunning()
	{
		m_runningTest.clear();
	}

	void MainFrame::doRunTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto tests = m_selectedPage->listSelectedTests();

			if ( !tests.empty() )
			{
				for ( auto & item : tests )
				{
					doPushTest( item );
				}

				doStartTests();
			}
		}
	}

	void MainFrame::doCopyTestFileName()
	{
		if ( m_selectedPage )
		{
			m_selectedPage->copyTestFileName();
		}
	}

	void MainFrame::doViewTestSceneFile()
	{
		if ( m_selectedPage )
		{
			m_selectedPage->viewTestSceneFile();
		}
	}

	void MainFrame::doViewTest( bool async )
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

		auto statusBar = GetStatusBar();
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void MainFrame::doSetRef()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->setTestsReferences( *m_tests.counts );
			m_fileSystem->commit( "Updated reference images" );
		}
	}

	void MainFrame::doIgnoreTestResult()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			m_selectedPage->ignoreTestsResult( m_testMenu->IsChecked( eID_TEST_IGNORE_RESULT ) );
		}
	}

	void MainFrame::doUpdateCastorDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			m_selectedPage->updateTestsCastorDate();
		}
	}

	void MainFrame::doUpdateSceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			m_selectedPage->updateTestsSceneDate();
		}
	}

	void MainFrame::doChangeTestCategory()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto category = main::selectCategory( this, m_database );

			if ( category )
			{
				auto items = m_selectedPage->listSelectedTests();
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
					page.second->changeTestsCategory( toMove, category );
				}

				wxProgressDialog progress{ wxT( "Changing tests category" )
					, wxT( "Changing tests category..." )
					, int( changes.size() )
					, this };
				int index = 0;

				for ( auto & change : changes )
				{
					progress.Update( index++
						, _( "Changing test category" )
						+ wxT( "\n" ) + getProgressDetails( *change.test ) );
					progress.Fit();
					m_plugin->changeTestCategory( *change.test
						, change.category
						, category
						, *m_fileSystem );
					m_database.updateTestCategory( *change.test, category );
				}
			}
		}
	}

	void MainFrame::doRenameTest( DatabaseTest & dbTest
		, std::string const & newName
		, std::string const & commitText
		, bool commit )
	{
		auto & test = *dbTest->test;
		auto oldName = test.name;

		for ( auto & page : m_testsPages )
		{
			page.second->preChangeTestName( *dbTest->test, newName);
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

	void MainFrame::doChangeTestName()
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

	void MainFrame::doRemoveTest( DatabaseTest & dbTest
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
			, false );
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

	void MainFrame::doDeleteTest()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
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

	void MainFrame::doRenameCategory( Category category
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

	void MainFrame::doChangeCategoryName()
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

	void MainFrame::doRemoveCategory( Category category
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

	void MainFrame::doDeleteCategory()
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

	void MainFrame::doEditConfig()
	{
		ConfigurationDialog dialog{ this, *m_plugin };
		dialog.ShowModal();
	}

	TestTreeModelNode * MainFrame::getTestNode( DatabaseTest const & test )
	{
		auto rendIt = m_testsPages.find( test->renderer );
		assert( rendIt != m_testsPages.end() );
		return rendIt->second->getTestNode( test );
	}

	wxDataViewItem MainFrame::getTestItem( DatabaseTest const & test )
	{
		return wxDataViewItem{ getTestNode ( test ) };
	}

	void MainFrame::doRunAllCategoryTests()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( []( DatabaseTest const & lookup )
				{
					return true;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunCategoryTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status == filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllCategoryTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status != filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllCategoryOutdatedTests()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( [this]( DatabaseTest const & lookup )
				{
					return m_plugin->isOutOfDate( *lookup )
						|| lookup->status == TestStatus::eNotRun;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doUpdateCategoryCastorDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfCastorDate();
				} );
			wxProgressDialog progress{ wxT( "Updating tests Engine date" )
				, wxT( "Updating tests..." )
				, int( items.size() )
				, this };
			int index = 0;

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				progress.Update( index++
					, _( "Updating tests Engine date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateEngineDate( m_plugin->getEngineRefDate() );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doUpdateCategorySceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfTestDate();
				} );
			wxProgressDialog progress{ wxT( "Updating tests date" )
				, wxT( "Updating tests..." )
				, int( items.size() )
				, this };
			int index = 0;

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				auto testDate = m_plugin->getTestDate( *run );
				progress.Update( index++
					, _( "Updating tests date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateTestDate( testDate );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doAddCategoryNumPrefix()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( []( DatabaseTest const & lookup )
				{
					return true;
				} );
			wxProgressDialog progress{ wxT( "Removing tests numeric prefix" )
				, wxT( "Updating tests..." )
				, int( items.size() )
				, this };
			size_t index = 0u;
			std::string commitText = items.size() == 1u
				? std::string{}
				: std::string{ "Bulk rename." };

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				progress.Update( int( index )
					, _( "Updating tests Scene date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				++index;
				doRenameTest( run
					, run.getPrefixedName( uint32_t( index ) )
					, commitText
					, index == items.size() );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doRemoveCategoryNumPrefix()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listCategoryTests( []( DatabaseTest const & lookup )
				{
					return lookup.hasNumPrefix();
				} );
			wxProgressDialog progress{ wxT( "Removing tests numeric prefix" )
				, wxT( "Updating tests..." )
				, int( items.size() )
				, this };
			size_t index = 0u;
			std::string commitText = items.size() == 1u
				? std::string{}
				: std::string{ "Bulk rename." };

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				progress.Update( int( index )
					, _( "Updating tests Scene date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				++index;
				doRenameTest( run
					, run.getUnprefixedName()
					, commitText
					, index == items.size() );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doRunAllRendererTests()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( []( DatabaseTest const & lookup )
				{
					return true;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunRendererTests( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status == filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllRendererTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( [filter]( DatabaseTest const & lookup )
				{
					return lookup->status != filter;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doRunAllRendererOutdatedTests()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( [this]( DatabaseTest const & lookup )
				{
					return m_plugin->isOutOfDate( *lookup )
						|| lookup->status == TestStatus::eNotRun;
				} );

			for ( auto & item : items )
			{
				doPushTest( item );
			}

			doStartTests();
		}
	}

	void MainFrame::doUpdateRendererCastorDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfCastorDate();
				} );
			wxProgressDialog progress{ wxT( "Updating tests Engine date" )
				, wxT( "Updating tests..." )
				, int( items.size() )
				, this };
			int index = 0;

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				progress.Update( index++
					, _( "Updating tests Engine date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateEngineDate( m_plugin->getEngineRefDate() );
			}

			m_selectedPage->refreshView();
		}
	}

	void MainFrame::doUpdateRendererSceneDate()
	{
		m_cancelled.exchange( false );

		if ( m_selectedPage )
		{
			auto items = m_selectedPage->listRendererTests( []( DatabaseTest const & lookup )
				{
					return lookup.checkOutOfTestDate();
				} );
			wxProgressDialog progress{ wxT( "Updating tests date" )
				, wxT( "Updating tests..." )
				, int( items.size() )
				, this };
			int index = 0;

			for ( auto & item : items )
			{
				auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
				auto & run = *node->test;
				auto testDate = m_plugin->getTestDate( *run );
				progress.Update( index++
					, _( "Updating tests date" )
					+ wxT( "\n" ) + getProgressDetails( run ) );
				progress.Fit();
				run.updateTestDate( testDate );
			}

			m_selectedPage->refreshView();
		}
	}

	std::vector< wxDataViewItem > MainFrame::doListAllTests( FilterFunc filter )
	{
		std::vector< DatabaseTest * > runs;
		m_tests.runs->listTests( filter, runs );
		std::vector< wxDataViewItem > result;

		for ( auto & run : runs )
		{
			result.push_back( getTestItem( *run ) );
		}

		return result;
	}

	void MainFrame::doRunAllTests()
	{
		m_cancelled.exchange( false );
		auto items = doListAllTests( []( DatabaseTest const & lookup )
			{
				return true;
			} );

		for ( auto & item : items )
		{
			doPushTest( item );
		}

		doStartTests();
	}

	void MainFrame::doRunTests( TestStatus filter )
	{
		m_cancelled.exchange( false );
		auto items = doListAllTests( [filter]( DatabaseTest const & lookup )
			{
				return lookup->status == filter;
			} );

		for ( auto & item : items )
		{
			doPushTest( item );
		}

		doStartTests();
	}

	void MainFrame::doRunAllTestsBut( TestStatus filter )
	{
		m_cancelled.exchange( false );
		auto items = doListAllTests( [filter]( DatabaseTest const & lookup )
			{
				return lookup->status != filter;
			} );

		for ( auto & item : items )
		{
			doPushTest( item );
		}

		doStartTests();
	}

	void MainFrame::doRunAllOutdatedTests()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();
		auto items = doListAllTests( [this]( DatabaseTest const & lookup )
			{
				return m_plugin->isOutOfDate( *lookup );
			} );

		for ( auto & item : items )
		{
			doPushTest( item );
		}

		doStartTests();
	}

	void MainFrame::doUpdateAllCastorDate()
	{
		m_cancelled.exchange( false );
		m_plugin->updateEngineRefDate();
		m_database.updateRunsEngineDate( m_plugin->getEngineRefDate() );
		auto items = doListAllTests( []( DatabaseTest const & lookup )
			{
				return lookup.checkOutOfCastorDate();
			} );
		wxProgressDialog progress{ wxT( "Updating tests Engine date" )
			, wxT( "Updating tests..." )
			, int( items.size() )
			, this };
		int index = 0;

		for ( auto & item : items )
		{
			auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
			auto & run = *node->test;
			progress.Update( index++
				, _( "Updating tests Engine date" )
				+ wxT( "\n" ) + getProgressDetails( run ) );
			progress.Fit();
			run.updateCastorDateNW( m_plugin->getEngineRefDate() );
		}

		m_selectedPage->refreshView();
	}

	void MainFrame::doUpdateAllSceneDate()
	{
		m_cancelled.exchange( false );
		auto items = doListAllTests( []( DatabaseTest const & lookup )
			{
				return lookup.checkOutOfTestDate();
			} );
		wxProgressDialog progress{ wxT( "Updating tests date" )
			, wxT( "Updating tests..." )
			, int( items.size() )
			, this };
		int index = 0;

		for ( auto & item : items )
		{
			auto node = reinterpret_cast< TestTreeModelNode * >( item.GetID() );
			auto & run = *node->test;
			auto testDate = m_plugin->getTestDate( *run );
			progress.Update( index++
				, _( "Updating tests Scene date" )
				+ wxT( "\n" ) + getProgressDetails( run ) );
			progress.Fit();

			if ( run.checkOutOfTestDate() )
			{
				run.updateTestDate( testDate );
			}
		}

		m_selectedPage->refreshView();
	}

	void MainFrame::doCancelTest( DatabaseTest & test
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
		auto statusBar = GetStatusBar();
		auto sizer = statusBar->GetSizer();
		assert( sizer != nullptr );
		sizer->SetSizeHints( statusBar );
		sizer->Layout();
	}

	void MainFrame::doCancel()
	{
		m_cancelled.exchange( true );
	}

	void MainFrame::doNewRenderer()
	{
		wxTextEntryDialog dialog{ this, _( "Enter the new renderer name" ) };

		if ( dialog.ShowModal() == wxID_OK )
		{
			auto renderer = m_database.createRenderer( makeStdString( dialog.GetValue() ) );
			m_tests.runs->addRenderer( renderer );
			auto range = doGetAllTestsRange();
			wxProgressDialog progress{ wxT( "Creating renderer entries" )
				, wxT( "Creating renderer entries..." )
				, int( range )
				, this };
			int index = 0;
			doInitTestsList( renderer );
			doFillList( renderer, progress, index );
		}
	}

	void MainFrame::doNewCategory()
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
				wxMessageBox( wxString{} << wxT( "Invalid category name: " ) << categoryName
					, wxT( "Error" )
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

	void MainFrame::doNewTest()
	{
		auto category = main::selectCategory( this, m_database );

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

	void MainFrame::doExportLatestTimes()
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

	void MainFrame::onTestRunEnd( int status )
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
			auto times = main::doProcessTestOutputTimes( m_database
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

	void MainFrame::onTestDisplayEnd( int status )
	{
		wxLogMessage( wxString() << "Test display ended (" << status << ")" );
	}

	void MainFrame::onTestDiffEnd( TestTimes const & times )
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

	bool MainFrame::onTestProcessEnd( int pid, int status )
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

	void MainFrame::onClose( wxCloseEvent & evt )
	{
		m_fileSystem->cleanup();
		m_categoriesUpdater->Stop();
		m_testUpdater->Stop();

		if ( m_runningTest.disProcess )
		{
			if ( wxProcess::Exists( int( m_runningTest.disProcess->GetPid() ) ) )
			{
				wxProcess::Kill( int( m_runningTest.disProcess->GetPid() ) );
			}

			m_runningTest.disProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.disProcess = nullptr;
		}

		if ( m_runningTest.genProcess )
		{
			if ( wxProcess::Exists( int( m_runningTest.genProcess->GetPid() ) ) )
			{
				wxProcess::Kill( int( m_runningTest.genProcess->GetPid() ) );
			}

			m_runningTest.genProcess->Disconnect( wxEVT_END_PROCESS );
			m_runningTest.genProcess = nullptr;
		}

		evt.Skip();
	}

	void MainFrame::onTestsPageChange( wxAuiNotebookEvent & evt )
	{
		if ( m_testsBook->GetPageCount() > 0 )
		{
			m_selectedPage = static_cast< RendererPage * >( m_testsBook->GetPage( size_t( m_testsBook->GetSelection() ) ) );
		}
	}

	void MainFrame::onTestsMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_TEST_RUN:
			doRunTest();
			break;
		case eID_TEST_COPY_FILE_NAME:
			doCopyTestFileName();
			break;
		case eID_TEST_VIEW_FILE:
			doViewTestSceneFile();
			break;
		case eID_TEST_VIEW_SYNC:
			doViewTest( false );
			break;
		case eID_TEST_VIEW_ASYNC:
			doViewTest( true);
			break;
		case eID_TEST_SET_REF:
			doSetRef();
			break;
		case eID_TEST_IGNORE_RESULT:
			doIgnoreTestResult();
			break;
		case eID_TEST_UPDATE_CASTOR:
			doUpdateCastorDate();
			break;
		case eID_TEST_UPDATE_SCENE:
			doUpdateSceneDate();
			break;
		case eID_TEST_CHANGE_CATEGORY:
			doChangeTestCategory();
			break;
		case eID_TEST_CHANGE_NAME:
			doChangeTestName();
			break;
		case eID_TEST_DELETE:
			doDeleteTest();
			break;
		case eID_CATEGORY_RUN_TESTS_ALL:
			doRunAllCategoryTests();
			break;
		case eID_CATEGORY_RUN_TESTS_NOTRUN:
			doRunCategoryTests( TestStatus::eNotRun );
			break;
		case eID_CATEGORY_RUN_TESTS_ACCEPTABLE:
			doRunCategoryTests( TestStatus::eAcceptable );
			break;
		case eID_CATEGORY_RUN_TESTS_CRASHED:
			doRunCategoryTests( TestStatus::eCrashed );
			break;
		case eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllCategoryTestsBut( TestStatus::eNegligible );
			break;
		case eID_CATEGORY_RUN_TESTS_OUTDATED:
			doRunAllCategoryOutdatedTests();
			break;
		case eID_CATEGORY_UPDATE_CASTOR:
			doUpdateCategoryCastorDate();
			break;
		case eID_CATEGORY_UPDATE_SCENE:
			doUpdateCategorySceneDate();
			break;
		case eID_CATEGORY_ADD_NUMPREFIX:
			doAddCategoryNumPrefix();
			break;
		case eID_CATEGORY_REMOVE_NUMPREFIX:
			doRemoveCategoryNumPrefix();
			break;
		case eID_CATEGORY_CHANGE_NAME:
			doChangeCategoryName();
			break;
		case eID_CATEGORY_DELETE:
			doDeleteCategory();
			break;
		case eID_RENDERER_RUN_TESTS_ALL:
			doRunAllRendererTests();
			break;
		case eID_RENDERER_RUN_TESTS_NOTRUN:
			doRunRendererTests( TestStatus::eNotRun );
			break;
		case eID_RENDERER_RUN_TESTS_ACCEPTABLE:
			doRunRendererTests( TestStatus::eAcceptable );
			break;
		case eID_RENDERER_RUN_TESTS_CRASHED:
			doRunRendererTests( TestStatus::eCrashed );
			break;
		case eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllRendererTestsBut( TestStatus::eNegligible );
			break;
		case eID_RENDERER_RUN_TESTS_OUTDATED:
			doRunAllRendererOutdatedTests();
			break;
		case eID_RENDERER_UPDATE_CASTOR:
			doUpdateRendererCastorDate();
			break;
		case eID_RENDERER_UPDATE_SCENE:
			doUpdateRendererSceneDate();
			break;
		case eID_ALL_RUN_TESTS_ALL:
			doRunAllTests();
			break;
		case eID_ALL_RUN_TESTS_NOTRUN:
			doRunTests( TestStatus::eNotRun );
			break;
		case eID_ALL_RUN_TESTS_ACCEPTABLE:
			doRunTests( TestStatus::eAcceptable );
			break;
		case eID_ALL_RUN_TESTS_CRASHED:
			doRunTests( TestStatus::eCrashed );
			break;
		case eID_ALL_RUN_TESTS_ALL_BUT_NEGLIGIBLE:
			doRunAllTestsBut( TestStatus::eNegligible );
			break;
		case eID_ALL_RUN_TESTS_OUTDATED:
			doRunAllOutdatedTests();
			break;
		case eID_ALL_UPDATE_CASTOR:
			doUpdateAllCastorDate();
			break;
		case eID_ALL_UPDATE_SCENE:
			doUpdateAllSceneDate();
			break;
		case eID_CANCEL:
			doCancel();
			break;
		}
	}

	void MainFrame::onConfigMenuOption( wxCommandEvent & evt )
	{
		switch ( evt.GetId() )
		{
		case eID_EDIT_CONFIG:
			doEditConfig();
			break;
		}
	}

	void MainFrame::onDatabaseMenuOption( wxCommandEvent & evt )
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

	void MainFrame::onProcessEnd( wxProcessEvent & evt )
	{
		if ( !onTestProcessEnd( evt.GetPid(), evt.GetExitCode() ) )
		{
			evt.Skip();
		}
	}

	void MainFrame::onTestUpdateTimer( wxTimerEvent & evt )
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

	void MainFrame::onCategoryUpdateTimer( wxTimerEvent & evt )
	{
		evt.Skip();
	}

	void MainFrame::onSize( wxSizeEvent & evt )
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
