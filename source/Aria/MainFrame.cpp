#include "MainFrame.hpp"

#include "DiffImage.hpp"
#include "ConfigurationDialog.hpp"
#include "RendererPage.hpp"
#include "TestsMainPanel.hpp"
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

	namespace main
	{
		static wxString const & getVersion()
		{
			static wxString const result{ wxString{ wxT( "v" ) } << Aria_VERSION_MAJOR << wxT( "." ) << Aria_VERSION_MINOR << wxT( "." ) << Aria_VERSION_BUILD };
			return result;
		}
	}

	//*********************************************************************************************

	MainFrame::MainFrame( OptionsPtr options )
		: wxFrame{ nullptr, wxID_ANY, wxT( "Aria " ) + main::getVersion(), wxDefaultPosition, wxSize( 1280, 720 ) }
		, m_options{ std::move( options ) }
		, m_configs{ m_options->listConfigs() }
	{
		SetMinClientSize( { 1280, 720 } );
		doInitMenus();
		doInitMenuBar();
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & evt )
			{
				onClose( evt );
			} );

		if ( m_options->hasPlugin() )
		{
			loadConfiguration( m_options->getPlugin() );
		}

		Maximize( true );
	}

	TestTreeModelNode * MainFrame::getTestNode( DatabaseTest const & test )
	{
		return m_configurationPanel
			? m_configurationPanel->getTestNode( test )
			: nullptr;
	}

	wxDataViewItem MainFrame::getTestItem( DatabaseTest const & test )
	{
		return m_configurationPanel
			? m_configurationPanel->getTestItem( test )
			: wxDataViewItem{};
	}

	void MainFrame::loadConfiguration( Plugin * plugin )
	{
		wxASSERT( m_configurationPanel == nullptr );
		m_configurationPanel = new TestsMainPanel{ this, plugin, m_menus };
		m_configurationPanel->Hide();
		m_configurationPanel->SetSize( GetClientSize() );
		m_configurationPanel->initialise();

		auto sizer = new wxBoxSizer{ wxVERTICAL };
		sizer->Add( m_configurationPanel, wxSizerFlags{ 1 }.Expand().ReserveSpaceEvenIfHidden() );
		SetSizer( sizer );
		sizer->SetSizeHints( this );
		m_configurationPanel->Show();

		Maximize( true );
	}

	void MainFrame::unloadConfiguration()
	{
		if ( m_configurationPanel )
		{
			m_configurationPanel->Hide();
			RemoveChild( m_configurationPanel );
			delete m_configurationPanel;
			m_configurationPanel = nullptr;
		}
	}

	bool MainFrame::areTestsRunning()const
	{
		return m_configurationPanel
			&& m_configurationPanel->areTestsRunning();
	}

	TestDatabase & MainFrame::getDatabase()
	{
		wxASSERT( m_configurationPanel != nullptr );
		return m_configurationPanel->getDatabase();
	}

	void MainFrame::doInitMenus()
	{
		auto addTestBaseMenus = []( wxMenu & menu )
		{
			uint32_t i = 2;
			menu.Append( Menus::eID_TEST_RUN, _( "Run Test" ) + wxT( "\tCTRL+R" ) );
			menu.Append( Menus::eID_TEST_RUN_5, _( "Run 5x Test" ) + wxT( "\tCTRL+SHIFT+R" ) );
			menu.Append( Menus::eID_TEST_COPY_FILE_NAME, _( "Copy test file path" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_VIEW_FILE, _( "View test scene file" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_SET_REF, _( "Set Reference" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_VIEW_SYNC, _( "View Test (sync)" ) + wxT( "\tF" ) << ( i ) );
			menu.Append( Menus::eID_TEST_VIEW_ASYNC, _( "View Test (async)" ) + wxT( "\tCtrl+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( Menus::eID_TEST_IGNORE_RESULT, _( "Ignore result" ) + wxT( "\tF" ) << ( i++ ), wxEmptyString, true );
			menu.Append( Menus::eID_TEST_UPDATE_ENGINE, _( "Update Engine's date" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_UPDATE_SCENE, _( "Update Scene's date" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_CHANGE_CATEGORY, _( "Change test category" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_CHANGE_NAME, _( "Change test name" ) + wxT( "\tF" ) << ( i++ ) );
			menu.Append( Menus::eID_TEST_DELETE, _( "Delete test" ) + wxT( "\tCTRL+D" ) );
		};
		auto addRendererMenus = []( wxMenu & menu )
		{
			uint32_t i = 1;
			wxString modKey = "CTRL";
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_ALL, _( "Run all renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_ALL_5, _( "Run 5x all renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+SHIFT+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_NOTRUN, _( "Run all <not run> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_UNACCEPTABLE, _( "Run all <unacceptable> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_CRASHED, _( "Run all <crashed> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_RUN_TESTS_OUTDATED, _( "Run all outdated renderer's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( Menus::eID_RENDERER_UPDATE_ENGINE, _( "Update renderer's tests Engine's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_UPDATE_SCENE, _( "Update renderer's tests Scene's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_RENDERER_CREATE_CATEGORY, _( "Create category" ) + wxT( "\t" ) + modKey + wxT( "+CTRL+N" ) << ( i++ ) );
		};
		auto addCategoryMenus = []( wxMenu & menu )
		{
			uint32_t i = 1;
			wxString modKey = "SHIFT";
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_ALL, _( "Run all category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_ALL_5, _( "Run 5x all category's tests" ) + wxT( "\t" ) + modKey + wxT( "+SHIFT+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_NOTRUN, _( "Run all <not run> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_ACCEPTABLE, _( "Run all <acceptable> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_UNACCEPTABLE, _( "Run all <unacceptable> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_CRASHED, _( "Run all <crashed> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE, _( "Run all but <negligible> category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_RUN_TESTS_OUTDATED, _( "Run all outdated category's tests" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( Menus::eID_CATEGORY_ADD_NUMPREFIX, _( "Add category's tests numeric prefix" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_REMOVE_NUMPREFIX, _( "Remove category's tests numeric prefix (if any)" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.AppendSeparator();
			menu.Append( Menus::eID_CATEGORY_UPDATE_ENGINE, _( "Update category's tests Engine's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_UPDATE_SCENE, _( "Update category's tests Scene's date" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_CHANGE_NAME, _( "Change category name" ) + wxT( "\t" ) + modKey + wxT( "+F" ) << ( i++ ) );
			menu.Append( Menus::eID_CATEGORY_CREATE_TEST, _( "Create test" ) + wxT( "\t" ) + modKey + wxT( "+CTRL+N" ) );
			menu.Append( Menus::eID_CATEGORY_DELETE, _( "Delete category" ) + wxT( "\t" ) + modKey + wxT( "+CTRL+D" ) );
		};

		m_testMenu = std::make_unique< wxMenu >();
		addTestBaseMenus( *m_testMenu );
		m_barTestMenu = new wxMenu;
		addTestBaseMenus( *m_barTestMenu );
		m_busyTestMenu = std::make_unique< wxMenu >();
		addTestBaseMenus( *m_busyTestMenu );
		m_busyTestMenu->Append( Menus::eID_CANCEL_RUNS, _( "Cancel runs" ) + wxT( "\tCTRL+SHIFT+R" ) );

		m_categoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_categoryMenu );
		m_barCategoryMenu = new wxMenu;
		addCategoryMenus( *m_barCategoryMenu );
		m_busyCategoryMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_busyCategoryMenu );
		m_busyCategoryMenu->Append( Menus::eID_CANCEL_RUNS, _( "Cancel runs" ) + wxT( "\tCTRL+SHIFT+R" ) );

		m_rendererMenu = std::make_unique< wxMenu >();
		addRendererMenus( *m_rendererMenu );
		m_barRendererMenu = new wxMenu;
		addRendererMenus( *m_barRendererMenu );
		m_busyRendererMenu = std::make_unique< wxMenu >();
		addCategoryMenus( *m_busyRendererMenu );
		m_busyRendererMenu->Append( Menus::eID_CANCEL_RUNS, _( "Cancel runs" ) + wxT( "\tCTRL+SHIFT+R" ) );

		m_menus.base.test = m_testMenu.get();
		m_menus.base.category= m_categoryMenu.get();
		m_menus.base.renderer= m_rendererMenu.get();
		m_menus.busy.test = m_busyTestMenu.get();
		m_menus.busy.category = m_busyCategoryMenu.get();
		m_menus.busy.renderer = m_busyRendererMenu.get();
		m_menus.bar.test = m_barTestMenu;
		m_menus.bar.category = m_barCategoryMenu;
		m_menus.bar.renderer = m_barRendererMenu;
		m_menus.statusBar = CreateStatusBar();

		m_menus.bind( wxCommandEventHandler( MainFrame::onTestMenuOption )
			, wxCommandEventHandler( MainFrame::onCategoryMenuOption )
			, wxCommandEventHandler( MainFrame::onRendererMenuOption )
			, this );
	}

	void MainFrame::doInitMenuBar()
	{
		wxMenuBar * menuBar{ new wxMenuBar };
		wxMenu * configsMenu{ new wxMenu };
		configsMenu->Connect( wxEVT_COMMAND_MENU_SELECTED
			, wxCommandEventHandler( MainFrame::onConfigMenuOption )
			, nullptr
			, this );
		int index = 0;

		for ( auto config : m_configs )
		{
			configsMenu->Append( eID_CONFIG_SELECT_ + index++, config.GetFullPath() );
		}


		wxMenu * configMenu{ new wxMenu };
		configMenu->AppendSubMenu( configsMenu, _( "Load configuration" ) );
		configMenu->Append( eID_CONFIG_IMPORT, _( "Import configuration" ) );
		configMenu->Append( eID_CONFIG_EDIT, _( "Edit configuration" ) );
		configMenu->Append( eID_CONFIG_NEW, _( "New configuration" ) );
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

		wxMenu * testsBarMenu{ new wxMenu };
		testsBarMenu->AppendSubMenu( m_barTestMenu, _( "Single" ) );
		testsBarMenu->AppendSubMenu( m_barCategoryMenu, _( "Category" ) );
		testsBarMenu->AppendSubMenu( m_barRendererMenu, _( "Renderer" ) );
		menuBar->Append( testsBarMenu, _( "Tests" ) );

		SetMenuBar( menuBar );
	}

	void MainFrame::onClose( wxCloseEvent & evt )
	{
		unloadConfiguration();
		evt.Skip();
	}

	void MainFrame::onRendererMenuOption( wxCommandEvent & evt )
	{
		if ( m_configurationPanel )
		{
			m_configurationPanel->onRendererMenuOption( evt );
		}
	}

	void MainFrame::onCategoryMenuOption( wxCommandEvent & evt )
	{
		if ( m_configurationPanel )
		{
			m_configurationPanel->onCategoryMenuOption( evt );
		}
	}

	void MainFrame::onTestMenuOption( wxCommandEvent & evt )
	{
		if ( m_configurationPanel )
		{
			m_configurationPanel->onTestMenuOption( evt );
		}
	}

	void MainFrame::onDatabaseMenuOption( wxCommandEvent & evt )
	{
		if ( m_configurationPanel )
		{
			m_configurationPanel->onDatabaseMenuOption( evt );
		}
	}

	void MainFrame::onConfigMenuOption( wxCommandEvent & evt )
	{
		if ( evt.GetId() == eID_CONFIG_EDIT )
		{
			if ( m_configurationPanel )
			{
				m_configurationPanel->editConfig();
			}
		}
		else if ( evt.GetId() == eID_CONFIG_NEW )
		{
			auto & factory = m_options->getFactory();
			wxString pluginName;

			if ( pluginName.empty() )
			{
				pluginName = option::selectPlugin( factory );
			}

			if ( !pluginName.empty() )
			{
				auto plugin = factory.create( makeStdString( pluginName ) );
				ConfigurationDialog dlg{ this, *plugin };

				if ( dlg.ShowModal() == wxID_OK )
				{
					auto fileName = plugin->config.work / "aria.ini";
					plugin->config.initFromFolder = true;
					m_options->load( std::move( plugin ), fileName );
					m_options->select( fileName );

					if ( m_options->hasPlugin() )
					{
						m_options->write();
						unloadConfiguration();
						loadConfiguration( m_options->getPlugin() );
					}
				}
			}
		}
		else if ( evt.GetId() == eID_CONFIG_IMPORT )
		{
			auto file = wxFileSelector( _( "Choose the file to import" )
				, wxEmptyString
				, wxEmptyString
				, wxEmptyString
				, wxFileSelectorDefaultWildcardStr
				, wxFD_OPEN | wxFD_FILE_MUST_EXIST );

			if ( !file.empty() && wxFileName::Exists( file ) )
			{
				m_options->load( wxFileName{ file } );
				m_options->select( wxFileName{ file } );

				if ( m_options->hasPlugin() )
				{
					m_options->write();
					unloadConfiguration();
					loadConfiguration( m_options->getPlugin() );
				}
			}
		}
		else if ( evt.GetId() >= eID_CONFIG_SELECT_ + 0
			&& evt.GetId() < eID_CONFIG_SELECT_ + int( m_configs.size() ) )
		{
			auto old = m_options->getPlugin();
			m_options->select( m_configs[size_t( evt.GetId() - eID_CONFIG_SELECT_ )] );

			if ( m_options->hasPlugin()
				&& old != m_options->getPlugin() )
			{
				unloadConfiguration();
				loadConfiguration( m_options->getPlugin() );
			}
		}
	}

	//*********************************************************************************************
}
