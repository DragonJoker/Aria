#include "AriaLib/Editor/TestFileDialog.hpp"

#include "AriaLib/Plugin.hpp"
#include "AriaLib/Aui/AuiDockArt.hpp"
#include "AriaLib/Aui/AuiTabArt.hpp"
#include "AriaLib/Aui/AuiToolBarArt.hpp"
#include "AriaLib/Editor/StcTextEditor.hpp"
#include "AriaLib/Editor/TestFileEditor.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/dialog.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	typedef enum eID
	{
		eID_PAGES,
		eID_MENU_QUIT,
		eID_MENU_OPEN,
		eID_MENU_CLOSE,
		eID_MENU_SAVE,
		eID_MENU_RUN_SYNC,
		eID_MENU_RUN_ASYNC,
		eID_MENU_FIND,
		eID_MENU_FIND_NEXT,
		eID_MENU_REPLACE,
	}	eID;

	TestFileDialog::TestFileDialog( Plugin const & plugin
		, Test const & test
		, LanguageInfoPtr language
		, wxString const & filename
		, wxString const & title
		, wxWindow * parent
		, wxPoint const & position
		, const wxSize size )
		: wxFrame{ parent
			, wxID_ANY
			, _( "Test File" ) + wxT( " - " ) + title
			, position
			, size
			, wxDEFAULT_FRAME_STYLE | wxRESIZE_BORDER | wxMAXIMIZE_BOX | wxFRAME_FLOAT_ON_PARENT }
		, m_plugin{ plugin }
		, m_test{ test }
		, m_filename{ filename }
		, m_title{ title }
		, m_auiManager{ this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE }
		, m_stcContext{ .language = std::move( language ) }
	{
		doInitialiseLayout( filename );
		doPopulateMenu();

		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & event )
			{
				doCleanup();
				event.Skip();
			} );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doCleanup();
				Close();
				event.Skip();
			}
			, eID_MENU_QUIT );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doCloseFile();
				event.Skip();
			}
			, eID_MENU_CLOSE );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doOpenFile();
				event.Skip();
			}
			, eID_MENU_OPEN );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doSaveFile();
				event.Skip();
			}
			, eID_MENU_SAVE );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				( void )m_plugin.viewTest( m_test, false );
				event.Skip();
			}
			, eID_MENU_RUN_SYNC );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				( void )m_plugin.viewTest( m_test, true );
				event.Skip();
			}
			, eID_MENU_RUN_ASYNC );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doFind();
				event.Skip();
			}
			, eID_MENU_FIND );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doFindNext();
				event.Skip();
			}
			, eID_MENU_FIND_NEXT );
		Bind( wxEVT_MENU
			, [this]( wxCommandEvent & event )
			{
				doReplace();
				event.Skip();
			}
			, eID_MENU_REPLACE );
		Bind( wxEVT_FIND
			, [this]( wxFindDialogEvent & event )
			{
				onFindReplace( event );
			} );
		Bind( wxEVT_FIND_NEXT
			, [this]( wxFindDialogEvent & event )
			{
				onFindReplace( event );
			} );
		Bind( wxEVT_FIND_REPLACE
			, [this]( wxFindDialogEvent & event )
			{
				onFindReplace( event );
			} );
		Bind( wxEVT_FIND_REPLACE_ALL
			, [this]( wxFindDialogEvent & event )
			{
				onFindReplace( event );
			} );
		Bind( wxEVT_FIND_CLOSE
			, [this]( wxFindDialogEvent & event )
			{
				onFindReplace( event );
			} );
	}

	TestFileDialog::~TestFileDialog()
	{
		m_auiManager.UnInit();
	}

	void TestFileDialog::doInitialiseLayout( wxString const & filename )
	{
		m_editors = new wxAuiNotebook( this
			, eID_PAGES
			, wxDefaultPosition
			, wxDefaultSize
			, wxAUI_NB_TOP | wxAUI_NB_TAB_MOVE | wxAUI_NB_TAB_FIXED_WIDTH | wxAUI_NB_SCROLL_BUTTONS );
		m_editors->SetArtProvider( new AuiTabArt );

		wxSize size = GetClientSize();
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editors
			, wxAuiPaneInfo()
			.CaptionVisible( false )
			.Name( wxT( "Files" ) )
			.Caption( _( "Files" ) )
			.CenterPane()
			.Dock()
			.MinSize( size )
			.Layer( 1 )
			.PaneBorder( false ) );
		m_auiManager.Update();

		doLoadPage( filename );
	}

	void TestFileDialog::doPopulateMenu()
	{
		wxMenuBar * menuBar = new wxMenuBar;
		menuBar->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		menuBar->SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		wxMenu * fileMenu = new wxMenu;
		fileMenu->Append( eID_MENU_OPEN, _( "&Open File" ) + wxT( "\tCTRL+O" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( eID_MENU_CLOSE, _( "&Close File" ) + wxT( "\tCTRL+W" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( eID_MENU_SAVE, _( "&Save" ) + wxT( "\tCTRL+S" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( eID_MENU_RUN_SYNC, _( "&Run (Sync)" ) + wxT( "\tF5" ) );
		fileMenu->Append( eID_MENU_RUN_ASYNC, _( "Run (&Async)" ) + wxT( "\tCTRL+F5" ) );
		fileMenu->AppendSeparator();
		fileMenu->Append( eID_MENU_QUIT, _( "&Quit" ) + wxT( "\tCTRL+Q" ) );
		menuBar->Append( fileMenu, _T( "&File" ) );

		wxMenu * editMenu = new wxMenu;
		editMenu->Append( eID_MENU_FIND, _( "&Find Text..." ) + wxT( "\tCTRL+F" ) );
		editMenu->Append( eID_MENU_FIND_NEXT, _( "Find &Next" ) + wxT( "\tF3" ) );
		editMenu->Append( eID_MENU_REPLACE, _( "&Replace Text..." ) + wxT( "\tCTRL+R" ) );
		menuBar->Append( editMenu, _T( "&Edit" ) );

		SetMenuBar( menuBar );
	}

	void TestFileDialog::doCleanup()
	{
		m_auiManager.DetachPane( m_editors );
		m_editors->DeleteAllPages();
	}

	void TestFileDialog::doOpenFile()
	{
		wxFileName fileName{ m_filename };
		auto path = fileName.GetPath();
		wxString wildcard;
		wildcard << m_stcContext.language->getDescription();
		wildcard << m_stcContext.language->getWildcard();

		wxFileDialog fileDialog{ this, _( "Open a test file" )
			, path
			, wxEmptyString
			, wildcard
			, wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR };
		fileDialog.SetDirectory( path );

		if ( fileDialog.ShowModal() == wxID_OK )
		{
			doLoadPage( fileDialog.GetPath() );
		}
	}

	void TestFileDialog::doCloseFile()
	{
		m_editors->DeletePage( size_t( m_editors->GetSelection() ) );

		if ( m_editors->GetPageCount() == 0 )
		{
			Close();
		}
	}

	void TestFileDialog::doSaveFile()
	{
		if ( auto editor = static_cast< TestFileEditor * >( m_editors->GetPage( size_t( m_editors->GetSelection() ) ) ) )
		{
			editor->saveFile();
		}
	}

	void TestFileDialog::doFind()
	{
		if ( m_findDialog )
		{
			m_findDialog->Destroy();
			m_findDialog = nullptr;
		}
		else
		{
			m_findDialog = new wxFindReplaceDialog{ this
				, & m_findReplaceData
				, _( "Find Text" ) };
			m_findDialog->Show( true );
		}
	}

	void TestFileDialog::doReplace()
	{
		if ( m_replaceDialog )
		{
			m_replaceDialog->Destroy();
			m_replaceDialog = nullptr;
		}
		else
		{
			m_replaceDialog = new wxFindReplaceDialog{ this
				, &m_findReplaceData
				, _( "Replace Text" )
				, wxFR_REPLACEDIALOG };
			m_replaceDialog->Show( true );
		}
	}

	void TestFileDialog::doFindFirst()
	{
		auto editor = static_cast< TestFileEditor * >( m_editors->GetPage( size_t( m_editors->GetSelection() ) ) );

		if ( editor )
		{
			editor->findFirst( m_findReplaceData );
		}
	}

	void TestFileDialog::doFindNext()
	{
		auto editor = static_cast< TestFileEditor * >( m_editors->GetPage( size_t( m_editors->GetSelection() ) ) );

		if ( editor )
		{
			editor->findNext( m_findReplaceData );
		}
	}

	void TestFileDialog::doReplaceOne()
	{
		auto editor = static_cast< TestFileEditor * >( m_editors->GetPage( size_t( m_editors->GetSelection() ) ) );

		if ( editor )
		{
			editor->replace( m_findReplaceData );
		}
	}

	void TestFileDialog::doReplaceAll()
	{
		auto editor = static_cast< TestFileEditor * >( m_editors->GetPage( size_t( m_editors->GetSelection() ) ) );

		if ( editor )
		{
			editor->replaceAll( m_findReplaceData );
		}
	}

	void TestFileDialog::onFindReplace( wxFindDialogEvent & event )
	{
		wxEventType type = event.GetEventType();

		if ( type == wxEVT_FIND )
		{
			doFindFirst();
		}
		else if ( type == wxEVT_FIND_NEXT )
		{
			doFindNext();
		}
		else if ( type == wxEVT_FIND_REPLACE )
		{
			doReplaceOne();
		}
		else if ( type == wxEVT_FIND_REPLACE_ALL )
		{
			doReplaceAll();
		}
		else if ( type == wxEVT_FIND_CLOSE )
		{
			wxFindReplaceDialog * dlg = event.GetDialog();

			if ( dlg == m_findDialog )
			{
				m_findDialog = nullptr;
			}
			else if ( dlg == m_replaceDialog )
			{
				m_replaceDialog = nullptr;
			}

			dlg->Destroy();
		}
	}

	void TestFileDialog::doLoadPage( wxString const & filename )
	{
		wxFileName fileName{ filename };
		auto page = new TestFileEditor{ m_stcContext
			, filename
			, this
			, wxWindowID( m_editors->GetPageCount() + 10 ) };
		page->SetBackgroundColour( PANEL_BACKGROUND_COLOUR );
		page->SetForegroundColour( PANEL_FOREGROUND_COLOUR );
		m_editors->AddPage( page, fileName.GetName(), true );
		auto size = m_editors->GetClientSize();
		page->SetSize( 0, 22, size.x, size.y - 22 );
	}

	BEGIN_EVENT_TABLE( TestFileDialog, wxFrame )
		EVT_STC_MODIFIED( wxID_ANY, TestFileDialog::onModified )
	END_EVENT_TABLE()

	void TestFileDialog::onModified( wxStyledTextEvent & event )
	{
		auto pageId = event.GetId();
		auto pageIndex = size_t( pageId - 10 );

		if ( m_editors->GetPageCount() > pageIndex )
		{
			if ( auto editor = static_cast< TestFileEditor * >( m_editors->GetPage( pageIndex ) ) )
			{
				wxFileName fileName{ editor->getFileName() };

				if ( editor->isModified() )
				{
					m_editors->SetPageText( pageIndex, wxT( "* " ) + fileName.GetName() );
				}
				else
				{
					m_editors->SetPageText( pageIndex, fileName.GetName() );
				}
			}
		}
	}
}
