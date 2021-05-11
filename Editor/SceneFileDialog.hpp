/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_SceneFileDialog_HPP___
#define ___ARIA_SceneFileDialog_HPP___

#include "Prerequisites.hpp"
#include "StcContext.hpp"

#include <wx/frame.h>
#include <wx/aui/auibook.h>

namespace aria
{
	class SceneFileDialog
		: public wxFrame
	{
	public:
		SceneFileDialog( Config const & config
			, wxString const & filename
			, wxString const & title
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 800 ) );
		~SceneFileDialog();

	private:
		void doInitialiseLayout( wxString const & filename );
		void doPopulateMenu();
		void doCleanup();
		void doOpenFile();
		void doCloseFile();
		void doSaveFile();
		void doLoadPage( wxString const & filename );

	private:
		Config const & m_config;
		wxString m_filename;
		wxAuiManager m_auiManager;
		StcContext m_stcContext;
		wxAuiNotebook * m_editors{};
	};
}

#endif
