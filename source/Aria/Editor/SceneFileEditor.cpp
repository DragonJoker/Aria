#include "Editor/SceneFileEditor.hpp"

#include "Aui/AuiDockArt.hpp"
#include "Editor/StcTextEditor.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/fdrepdlg.h>
#pragma warning( pop )

namespace aria
{
	namespace
	{
		int convertFlags( wxUint32 flags )
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
		, wxPoint const & position
		, const wxSize size )
		: wxPanel( parent, wxID_ANY, position, size )
		, m_auiManager( this, wxAUI_MGR_ALLOW_FLOATING | wxAUI_MGR_TRANSPARENT_HINT | wxAUI_MGR_HINT_FADE | wxAUI_MGR_VENETIAN_BLINDS_HINT | wxAUI_MGR_LIVE_RESIZE )
		, m_stcContext( stcContext )
	{
		doInitialiseLayout( filename );
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

	bool SceneFileEditor::saveFile()
	{
		return m_editor->saveFile();
	}

	void SceneFileEditor::findFirst( wxFindReplaceData const & data )
	{
		m_currentIter = m_editor->FindText( 0
			, int( m_editor->GetLastPosition() )
			, data.GetFindString()
			, convertFlags( wxUint32( data.GetFlags() ) ) );

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
			, convertFlags( wxUint32( data.GetFlags() ) ) );

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

	void SceneFileEditor::doInitialiseLayout( wxString const & filename )
	{
		wxSize size = GetClientSize();
		// The editor
		m_editor = new StcTextEditor( m_stcContext, this, wxID_ANY, wxDefaultPosition, size );
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
		m_auiManager.DetachPane( m_editor );
	}
}
