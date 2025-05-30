#include "AriaLib/Editor/StcTextEditor.hpp"

#include "AriaLib/StringUtils.hpp"
#include "AriaLib/Editor/LanguageInfo.hpp"
#include "AriaLib/Editor/StyleInfo.hpp"

#include <wx/filename.h>

#include <algorithm>

#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"

namespace aria
{
	namespace stced
	{
		enum gcID
		{
			gcID_FINDNEXT,
			gcID_REPLACE,
			gcID_REPLACENEXT,
			gcID_BRACEMATCH,
			gcID_GOTO,
			gcID_PAGEACTIVE,
			gcID_DISPLAYEOL,
			gcID_INDENTGUIDE,
			gcID_LINENUMBER,
			gcID_LONGLINEON,
			gcID_WHITESPACE,
			gcID_FOLDTOGGLE,
			gcID_OVERTYPE,
			gcID_READONLY,
			gcID_WRAPMODEON,
			gcID_CONVERTCR,
			gcID_CONVERTCRLF,
			gcID_CONVERTLF,
			gcID_CHARSETANSI,
			gcID_CHARSETMAC,
			gcID_HILIGHTFIRST,
			gcID_HILIGHTLAST = gcID_HILIGHTFIRST + 99,
		};
	}

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
	StcTextEditor::TextAutoCompleter::TextAutoCompleter( wxArrayString const & keywords )
	{
		for ( auto & keyword: keywords )
		{
			m_keywords.insert( keyword );
		}
	}

	bool StcTextEditor::TextAutoCompleter::Start( wxString const & prefix )
	{
		m_prefix = prefix;

		if ( m_prefix.size() >= 3 )
		{
			m_current = std::find_if( m_keywords.begin(), m_keywords.end(), [&]( wxString const & keyword )
				{
					return keyword.find( m_prefix ) == 0;
				} );
		}
		else
		{
			m_current = m_keywords.end();
		}

		return m_current != m_keywords.end();
	}

	wxString StcTextEditor::TextAutoCompleter::GetNext()
	{
		wxString result;

		if ( m_current != m_keywords.end() )
		{
			++m_current;

			if ( m_current != m_keywords.end() )
			{
				if ( m_current->find( m_prefix ) == 0 )
				{
					result = *m_current;
				}
			}
		}

		return result;
	}
#endif

	//*************************************************************************************************

	StcTextEditor::StcTextEditor( StcContext & context
		, wxWindow * parent
		, wxWindowID id
		, wxPoint const & pos
		, wxSize const & size
		, long style )
		: wxStyledTextCtrl{ parent, id, pos, size, style }
		, m_filename( wxEmptyString )
		, m_context( context )
		, m_lineNrID( 0 )
		, m_foldingID( 2 )
		, m_dividerID( 1 )
		, m_tabSpaces( 4 )
		, m_useTabs( true )
		, m_tabIndents( true )
		, m_backspaceUnindents( true )
	{
		SetModEventMask( wxSTC_MOD_INSERTTEXT
			| wxSTC_MOD_DELETETEXT
			| wxSTC_PERFORMED_UNDO
			| wxSTC_PERFORMED_REDO );
		SetTabWidth( m_tabSpaces );
		SetViewEOL( m_context.displayEOLEnable );
		SetIndentationGuides( m_context.indentGuideEnable );
		SetEdgeMode( m_context.longLineOnEnable ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
		static const wxColour WHITESPACE_COLOUR = wxColour( 25, 76, 127, 255 );
		SetWhitespaceForeground( true, WHITESPACE_COLOUR );
		SetViewWhiteSpace( wxSTC_WS_VISIBLEALWAYS );
		SetOvertype( m_context.overTypeInitial );
		SetReadOnly( m_context.readOnlyInitial );
		SetWrapMode( m_context.wrapModeInitial ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
		// set visibility
		SetVisiblePolicy( wxSTC_VISIBLE_STRICT | wxSTC_VISIBLE_SLOP, 1 );
		SetXCaretPolicy( wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1 );
		SetYCaretPolicy( wxSTC_CARET_EVEN | wxSTC_VISIBLE_STRICT | wxSTC_CARET_SLOP, 1 );
		// miscellaneous
		m_lineNrMargin = TextWidth( wxSTC_STYLE_LINENUMBER, wxT( "_999999" ) );
		m_foldingMargin = 16;
		SetLayoutCache( wxSTC_CACHE_PAGE );

		doInitializePrefs();
	}

	StcTextEditor::~StcTextEditor()
	{
	}

	bool StcTextEditor::loadFile( wxString const & filename )
	{
		if ( !filename.empty() )
		{
			m_filename = filename;
		}

		wxStyledTextCtrl::ClearAll();
		wxStyledTextCtrl::EmptyUndoBuffer();
		wxStyledTextCtrl::SetEOLMode( wxSTC_EOL_LF );
#ifdef __WXMSW__
		wxStyledTextCtrl::SetTechnology( wxSTC_TECHNOLOGY_DIRECTWRITE );
		wxStyledTextCtrl::SetFontQuality( wxSTC_EFF_QUALITY_ANTIALIASED );
#endif
		auto result = wxStyledTextCtrl::LoadFile( m_filename );

		if ( result )
		{
			wxStyledTextCtrl::ConvertEOLs( wxSTC_EOL_LF );
		}

		wxStyledTextCtrl::SetCodePage( wxSTC_CP_UTF8 );
		SetModified( false );
		return result;
	}

	bool StcTextEditor::saveFile()
	{
		bool result = true;

		if ( isModifiable() )
		{
			result = wxStyledTextCtrl::SaveFile( m_filename );
			SetModified( false );
			wxPostEvent( this, wxStyledTextEvent{ wxEVT_STC_MODIFIED, GetId() } );
		}

		return result;
	}

	bool StcTextEditor::isModifiable()const
	{
		return ( GetModify() && !GetReadOnly() );
	}

	void StcTextEditor::doInitializePrefs()
	{
		// Set margin for line numbers
		SetMarginType( m_lineNrID, wxSTC_MARGIN_NUMBER );
		SetMarginWidth( m_lineNrID, 50 );

		LanguageInfo const & language = *m_context.language;
		SetLexer( language.getLexer() );
		auto defaultFgColour = language.getDefaultStyle().foreground;
		auto defaultBgColour = language.getDefaultStyle().background;
		wxFont font( language.getFontSize()
			, wxFONTFAMILY_MODERN
			, wxFONTSTYLE_NORMAL
			, wxFONTWEIGHT_NORMAL
			, false
			, language.getFontName() );
		StyleSetFont( wxSTC_STYLE_DEFAULT, font );
		doInitialiseBaseColours( defaultBgColour, defaultFgColour );

		wxArrayString keywords;

		// initialize settings
		if ( m_context.syntaxEnable )
		{
			for ( auto & styleIt : language.getStyles() )
			{
				StyleInfo const & styleInfo = styleIt.second;
				int style = styleIt.first;

				if ( styleInfo.foreground.IsOk() )
				{
					StyleSetForeground( style, styleInfo.foreground );
				}

				if ( styleInfo.background.IsOk() )
				{
					StyleSetBackground( style, styleInfo.background );
				}

				StyleSetBold( style, ( styleInfo.fontStyle & eSTC_STYLE_BOLD ) > 0 );
				StyleSetItalic( style, ( styleInfo.fontStyle & eSTC_STYLE_ITALIC ) > 0 );
				StyleSetUnderline( style, ( styleInfo.fontStyle & eSTC_STYLE_UNDERL ) > 0 );
				StyleSetVisible( style, ( styleInfo.fontStyle & eSTC_STYLE_HIDDEN ) == 0 );
				StyleSetCase( style, ( styleInfo.letterCase ) );
			}

			int index = 0;

			for ( auto & words : language.getKeywords() )
			{
				if ( !words.empty() )
				{
					SetKeyWords( index, words.c_str() );
					StringArray array = split( makeStdString( words ), " \t\n\r", uint32_t( -1 ), false);

					for ( auto keyword : array )
					{
						keywords.push_back( keyword );
					}
				}

				++index;
			}
		}

#if wxMAJOR_VERSION >= 3 || ( wxMAJOR_VERSION == 2 && wxMINOR_VERSION >= 9 )
		AutoComplete( new TextAutoCompleter( keywords ) );
#endif
		// Set margin as unused
		SetMarginType( m_dividerID, wxSTC_MARGIN_SYMBOL );
		SetMarginWidth( m_dividerID, 0 );
		SetMarginSensitive( m_dividerID, false );
		// folding
		SetMarginType( m_foldingID, wxSTC_MARGIN_SYMBOL );
		SetMarginMask( m_foldingID, wxSTC_MASK_FOLDERS );
		StyleSetBackground( m_foldingID, defaultBgColour );
		SetMarginWidth( m_foldingID, 0 );
		SetMarginSensitive( m_foldingID, false );

		if ( m_context.foldEnable )
		{
			SetMarginWidth( m_foldingID, ( ( language.getFoldFlags() != 0 ) ? m_foldingMargin : 0 ) );
			SetMarginSensitive( m_foldingID, ( ( language.getFoldFlags() != 0 ) ) );
			SetProperty( wxT( "fold" ), ( ( language.getFoldFlags() != 0 ) ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment" ), ( ( language.getFoldFlags() & eSTC_FOLD_COMMENT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.compact" ), ( ( language.getFoldFlags() & eSTC_FOLD_COMPACT ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.preprocessor" ), ( ( language.getFoldFlags() & eSTC_FOLD_PREPROC ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html" ), ( ( language.getFoldFlags() & eSTC_FOLD_HTML ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.html.preprocessor" ), ( ( language.getFoldFlags() & eSTC_FOLD_HTMLPREP ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.comment.python" ), ( ( language.getFoldFlags() & eSTC_FOLD_COMMENTPY ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
			SetProperty( wxT( "fold.quotes.python" ), ( ( language.getFoldFlags() & eSTC_FOLD_QUOTESPY ) > 0 ? wxT( "1" ) : wxT( "0" ) ) );
		}

		SetFoldFlags( wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED );
		// set spaces and indention
		SetTabWidth( m_tabSpaces );
		SetUseTabs( m_useTabs );
		SetTabIndents( m_tabIndents );
		SetBackSpaceUnIndents( m_backspaceUnindents );
		SetIndent( m_context.indentEnable ? m_tabSpaces : 0 );
		// others
		SetViewEOL( m_context.displayEOLEnable );
		SetIndentationGuides( m_context.indentGuideEnable );
		SetEdgeColumn( 80 );
		SetEdgeMode( m_context.longLineOnEnable ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
		SetViewWhiteSpace( m_context.whiteSpaceEnable ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
		SetOvertype( m_context.overTypeInitial );
		SetReadOnly( m_context.readOnlyInitial );
		SetWrapMode( m_context.wrapModeInitial ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	}

	void StcTextEditor::doInitialiseBaseColours( wxColour const & bgColour
		, wxColour const & fgColour )
	{
		SetBackgroundColour( bgColour );
		SetForegroundColour( fgColour );
		SetSelAlpha( 127 );
		SetSelBackground( true, wxColour( 51, 153, 255, 127 ) );
		SetFoldMarginColour( true, bgColour );
		SetFoldMarginHiColour( true, bgColour );
		StyleSetBackground( wxSTC_STYLE_DEFAULT, bgColour );
		StyleSetForeground( wxSTC_STYLE_DEFAULT, fgColour );
		StyleClearAll();

		SetCaretForeground( fgColour );

		MarkerDefine( wxSTC_MARKNUM_FOLDER, wxSTC_MARK_DOTDOTDOT, fgColour, bgColour );
		MarkerDefine( wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN, fgColour, bgColour );
		MarkerDefine( wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_EMPTY, fgColour, bgColour );
		MarkerDefine( wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_DOTDOTDOT, fgColour, bgColour );
		MarkerDefine( wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN, fgColour, bgColour );
		MarkerDefine( wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_EMPTY, fgColour, bgColour );
		MarkerDefine( wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_EMPTY, fgColour, bgColour );

		MarkerSetBackground( wxSTC_MARKNUM_FOLDER, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDER, fgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPEN, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPEN, fgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDERSUB, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDERSUB, fgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDEREND, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDEREND, fgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDEROPENMID, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDEROPENMID, fgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDERMIDTAIL, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDERMIDTAIL, fgColour );
		MarkerSetBackground( wxSTC_MARKNUM_FOLDERTAIL, bgColour );
		MarkerSetForeground( wxSTC_MARKNUM_FOLDERTAIL, fgColour );

		// set common styles
		StyleSetBackground( wxSTC_STYLE_LINENUMBER, bgColour );
		StyleSetForeground( wxSTC_STYLE_LINENUMBER, fgColour );
		StyleSetBackground( wxSTC_STYLE_CONTROLCHAR, bgColour );
		StyleSetForeground( wxSTC_STYLE_CONTROLCHAR, fgColour );
		StyleSetBackground( wxSTC_STYLE_BRACELIGHT, bgColour );
		StyleSetForeground( wxSTC_STYLE_BRACELIGHT, wxColour( wxT( "DARK GREEN" ) ) );
		StyleSetBackground( wxSTC_STYLE_BRACEBAD, bgColour );
		StyleSetForeground( wxSTC_STYLE_BRACEBAD, wxColour( wxT( "DARK RED" ) ) );
		StyleSetBackground( wxSTC_STYLE_INDENTGUIDE, bgColour );
		StyleSetBackground( wxSTC_STYLE_CALLTIP, bgColour );
		StyleSetBackground( wxSTC_STYLE_LASTPREDEFINED, bgColour );
	}

	BEGIN_EVENT_TABLE( StcTextEditor, wxStyledTextCtrl )
		EVT_SIZE( StcTextEditor::onSize )
		EVT_MENU( wxID_FIND, StcTextEditor::onFind )
		EVT_MENU( stced::gcID_FINDNEXT, StcTextEditor::onFindNext )
		EVT_MENU( stced::gcID_REPLACE, StcTextEditor::onReplace )
		EVT_MENU( stced::gcID_REPLACENEXT, StcTextEditor::onReplaceNext )
		EVT_MENU( stced::gcID_BRACEMATCH, StcTextEditor::onBraceMatch )
		EVT_MENU( stced::gcID_GOTO, StcTextEditor::onGoto )
		EVT_MENU( stced::gcID_DISPLAYEOL, StcTextEditor::onDisplayEOL )
		EVT_MENU( stced::gcID_INDENTGUIDE, StcTextEditor::onIndentGuide )
		EVT_MENU( stced::gcID_LINENUMBER, StcTextEditor::onLineNumber )
		EVT_MENU( stced::gcID_LONGLINEON, StcTextEditor::onLongLineOn )
		EVT_MENU( stced::gcID_WHITESPACE, StcTextEditor::onWhiteSpace )
		EVT_MENU( stced::gcID_FOLDTOGGLE, StcTextEditor::onFoldToggle )
		EVT_MENU( stced::gcID_OVERTYPE, StcTextEditor::onSetOverType )
		EVT_MENU( stced::gcID_READONLY, StcTextEditor::onSetReadOnly )
		EVT_MENU( stced::gcID_WRAPMODEON, StcTextEditor::onWrapmodeOn )
		EVT_MENU( stced::gcID_CHARSETANSI, StcTextEditor::onUseCharset )
		EVT_MENU( stced::gcID_CHARSETMAC, StcTextEditor::onUseCharset )
		EVT_MENU( stced::gcID_CONVERTCR, StcTextEditor::onConvertEOL )
		EVT_MENU( stced::gcID_CONVERTCRLF, StcTextEditor::onConvertEOL )
		EVT_MENU( stced::gcID_CONVERTLF, StcTextEditor::onConvertEOL )
		EVT_MENU_RANGE( stced::gcID_HILIGHTFIRST, stced::gcID_HILIGHTLAST, StcTextEditor::onHighlightLang )
		EVT_STC_MARGINCLICK( wxID_ANY, StcTextEditor::onMarginClick )
	END_EVENT_TABLE()

	void StcTextEditor::onSize( wxSizeEvent & event )
	{
		int x = GetClientSize().x
			+ ( m_context.lineNumberEnable ? m_lineNrMargin : 0 )
			+ ( m_context.foldEnable ? m_foldingMargin : 0 );

		if ( x > 0 )
		{
			SetScrollWidth( x );
		}

		event.Skip();
	}

	void StcTextEditor::onFind( wxCommandEvent & WXUNUSED( event ) )
	{
	}

	void StcTextEditor::onFindNext( wxCommandEvent & WXUNUSED( event ) )
	{
	}

	void StcTextEditor::onReplace( wxCommandEvent & WXUNUSED( event ) )
	{
	}

	void StcTextEditor::onReplaceNext( wxCommandEvent & WXUNUSED( event ) )
	{
	}

	void StcTextEditor::onBraceMatch( wxCommandEvent & WXUNUSED( event ) )
	{
		int min = GetCurrentPos();
		int max = BraceMatch( min );

		if ( max > ( min + 1 ) )
		{
			BraceHighlight( min + 1, max );
			SetSelection( min + 1, max );
		}
		else
		{
			BraceBadLight( min );
		}
	}

	void StcTextEditor::onGoto( wxCommandEvent & WXUNUSED( event ) )
	{
	}

	void StcTextEditor::onHighlightLang( wxCommandEvent & event )
	{
		doInitializePrefs();
	}

	void StcTextEditor::onDisplayEOL( wxCommandEvent & WXUNUSED( event ) )
	{
		SetViewEOL( !GetViewEOL() );
	}

	void StcTextEditor::onIndentGuide( wxCommandEvent & WXUNUSED( event ) )
	{
		if ( GetIndentationGuides() )
		{
			SetIndentationGuides( 0 );
		}
		else
		{
			SetIndentationGuides( 1 );
		}
	}

	void StcTextEditor::onLineNumber( wxCommandEvent & WXUNUSED( event ) )
	{
		SetMarginWidth( m_lineNrID, GetMarginWidth( m_lineNrID ) == 0 ? m_lineNrMargin : 0 );
	}

	void StcTextEditor::onLongLineOn( wxCommandEvent & WXUNUSED( event ) )
	{
		SetEdgeMode( GetEdgeMode() == 0 ? wxSTC_EDGE_LINE : wxSTC_EDGE_NONE );
	}

	void StcTextEditor::onWhiteSpace( wxCommandEvent & WXUNUSED( event ) )
	{
		SetViewWhiteSpace( GetViewWhiteSpace() == 0 ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE );
	}

	void StcTextEditor::onFoldToggle( wxCommandEvent & WXUNUSED( event ) )
	{
		ToggleFold( GetFoldParent( GetCurrentLine() ) );
	}

	void StcTextEditor::onSetOverType( wxCommandEvent & WXUNUSED( event ) )
	{
		SetOvertype( !GetOvertype() );
	}

	void StcTextEditor::onSetReadOnly( wxCommandEvent & WXUNUSED( event ) )
	{
		SetReadOnly( !GetReadOnly() );
	}

	void StcTextEditor::onWrapmodeOn( wxCommandEvent & WXUNUSED( event ) )
	{
		SetWrapMode( GetWrapMode() == 0 ? wxSTC_WRAP_WORD : wxSTC_WRAP_NONE );
	}

	void StcTextEditor::onUseCharset( wxCommandEvent & event )
	{
		int charset = GetCodePage();

		switch ( event.GetId() )
		{
		case stced::gcID_CHARSETANSI:
			charset = wxSTC_CHARSET_ANSI;
			break;

		case stced::gcID_CHARSETMAC:
			charset = wxSTC_CHARSET_ANSI;
			break;
		}

		for ( int i = 0; i < wxSTC_STYLE_LASTPREDEFINED; ++i )
		{
			StyleSetCharacterSet( i, charset );
		}

		SetCodePage( charset );
	}

	void StcTextEditor::onConvertEOL( wxCommandEvent & event )
	{
		int eolMode = GetEOLMode();

		switch ( event.GetId() )
		{
		case stced::gcID_CONVERTCR:
			eolMode = wxSTC_EOL_CR;
			break;

		case stced::gcID_CONVERTCRLF:
			eolMode = wxSTC_EOL_CRLF;
			break;

		case stced::gcID_CONVERTLF:
			eolMode = wxSTC_EOL_LF;
			break;
		}

		ConvertEOLs( eolMode );
		SetEOLMode( eolMode );
	}

	void StcTextEditor::onMarginClick( wxStyledTextEvent & event )
	{
		if ( event.GetMargin() == 2 )
		{
			int lineClick = LineFromPosition( event.GetPosition() );
			int levelClick = GetFoldLevel( lineClick );

			if ( ( levelClick & wxSTC_FOLDLEVELHEADERFLAG ) > 0 )
			{
				ToggleFold( lineClick );
			}
		}
	}
}
