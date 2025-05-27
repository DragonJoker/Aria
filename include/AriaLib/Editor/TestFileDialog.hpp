/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_TestFileDialog_HPP___
#define ___ARIA_TestFileDialog_HPP___

#include "AriaLib/Editor/StcContext.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/fdrepdlg.h>
#include <wx/frame.h>
#include <wx/aui/auibook.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	class TestFileDialog
		: public wxFrame
	{
	public:
		AriaLib_API TestFileDialog( Plugin const & plugin
			, Test const & test
			, LanguageInfoPtr language
			, wxString const & filename
			, wxString const & title
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 800 ) );
		AriaLib_API ~TestFileDialog()override;

	private:
		void doInitialiseLayout( wxString const & filename );
		void doPopulateMenu();
		void doCleanup();
		void doOpenFile();
		void doCloseFile();
		void doSaveFile();
		void doFind();
		void doReplace();
		void doFindFirst();
		void doFindNext();
		void doReplaceOne();
		void doReplaceAll();
		void onFindReplace( wxFindDialogEvent & event );
		void doLoadPage( wxString const & filename );

		DECLARE_EVENT_TABLE()
		void onModified( wxStyledTextEvent & event );

	private:
		Plugin const & m_plugin;
		Test const & m_test;
		wxString m_filename;
		wxString m_title;
		wxAuiManager m_auiManager;
		StcContext m_stcContext;
		wxAuiNotebook * m_editors{};
		wxFindReplaceData m_findReplaceData{};
		wxFindReplaceDialog * m_findDialog{};
		wxFindReplaceDialog * m_replaceDialog{};
	};
}

#endif
