/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_SceneFileDialog_HPP___
#define ___ARIA_SceneFileDialog_HPP___

#include "Prerequisites.hpp"
#include "StcContext.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/fdrepdlg.h>
#include <wx/frame.h>
#include <wx/aui/auibook.h>
#pragma warning( pop )

namespace aria
{
	class SceneFileDialog
		: public wxFrame
	{
	public:
		SceneFileDialog( Plugin const & plugin
			, wxString const & filename
			, wxString const & title
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 800 ) );
		~SceneFileDialog()override;

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

	private:
		Plugin const & m_plugin;
		wxString m_filename;
		wxAuiManager m_auiManager;
		StcContext m_stcContext;
		wxAuiNotebook * m_editors{};
		wxFindReplaceData m_findReplaceData{};
		wxFindReplaceDialog * m_findDialog{};
		wxFindReplaceDialog * m_replaceDialog{};
	};
}

#endif
