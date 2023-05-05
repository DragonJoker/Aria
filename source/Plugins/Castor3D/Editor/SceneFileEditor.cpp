#include "Editor/SceneFileEditor.hpp"

#include "Editor/StcTextEditor.hpp"

#include <AriaLib/Aui/AuiDockArt.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/fdrepdlg.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	namespace sfed
	{
		static int convertFlags( wxUint32 flags )
		{
			int result = 0;

			if ( flags & wxFR_WHOLEWORD )
			{
				result |= wxSTC_FIND_WHOLEWORD;
			}

			if ( flags & wxFR_MATCHCASE )
			{
				result |= wxSTC_FIND_MATCHCASE;
			}

			return result;
		}
	}

	SceneFileEditor::SceneFileEditor( StcContext & stcContext
		, wxString const & filename
		, wxWindow * parent
		, wxWindowID editId
		, wxPoint const & position
		, const wxSize size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_stcContext( stcContext )
	{
		doInitialiseLayout( filename, editId );
		Bind( wxEVT_CLOSE_WINDOW
			, [this]( wxCloseEvent & event )
			{
				doCleanup();
				event.Skip();
			} );
	}

	SceneFileEditor::~SceneFileEditor()
	{
		doCleanup();
		m_auiManager.UnInit();
	}

	bool SceneFileEditor::isModified()const
	{
		return m_editor->IsModified();
	}

	wxString SceneFileEditor::getFileName()const
	{
		return m_editor->getFileName();
	}

	bool SceneFileEditor::saveFile()
	{
		return m_editor->saveFile();
	}

	void SceneFileEditor::findFirst( wxFindReplaceData const & data )
	{
		m_currentIter = m_editor->FindText( 0
			, int( m_editor->GetLastPosition() )
			, data.GetFindString()
			, sfed::convertFlags( wxUint32( data.GetFlags() ) ) );

		if ( m_currentIter != -1 )
		{
			m_editor->GotoPos( m_currentIter );
			m_editor->SetSelection( m_currentIter
				, m_currentIter + long( data.GetFindString().size() ) );
		}
	}

	void SceneFileEditor::findNext( wxFindReplaceData const & data )
	{
		m_currentIter = m_editor->FindText( int( m_editor->GetInsertionPoint() )
			, int( m_editor->GetLastPosition() )
			, data.GetFindString()
			, sfed::convertFlags( wxUint32( data.GetFlags() ) ) );

		if ( m_currentIter != -1 )
		{
			m_editor->GotoPos( m_currentIter );
			m_editor->SetSelection( m_currentIter
				, m_currentIter + long( data.GetFindString().size() ) );
		}
	}

	void SceneFileEditor::replace( wxFindReplaceData const & data )
	{
	}

	void SceneFileEditor::replaceAll( wxFindReplaceData const & data )
	{
	}

	void SceneFileEditor::doInitialiseLayout( wxString const & filename
		, wxWindowID editId )
	{
		wxSize size = GetClientSize();
		// The editor
		m_editor = new StcTextEditor( m_stcContext, this, editId, wxDefaultPosition, size );
		m_editor->Show();
#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		m_editor->AlwaysShowScrollbars( true, true );
#endif
		m_editor->loadFile( filename );
		m_editor->SetReadOnly( false );

		// Put all that in the AUI manager
		m_auiManager.SetArtProvider( new AuiDockArt );
		m_auiManager.AddPane( m_editor
			, wxAuiPaneInfo()
				.CaptionVisible( false )
				.CloseButton( false )
				.Name( wxT( "Scene File" ) )
				.Caption( _( "Scene File" ) )
				.CaptionVisible( false )
				.Center()
				.Layer( 0 )
				.Movable( false )
				.PaneBorder( false )
				.Dockable( false ) );
		m_auiManager.Update();
	}

	void SceneFileEditor::doCleanup()
	{
		if ( m_editor->IsModified() )
		{
			auto name = getFileName();
			auto idx = name.find_last_of( "\\/" );

			if ( idx != wxString::npos )
			{
				name = name.substr( idx + 1u );
			}

			if ( wxID_YES == wxMessageBox( wxString{} << wxT( "File " ) << name << wxT( "\nis modified, do you want to save it ?" )
				, wxT( "Pending modifications" )
				, wxYES_NO
				, this ) )
			{
				m_editor->saveFile();
			}
		}

		m_auiManager.DetachPane( m_editor );
	}
}
