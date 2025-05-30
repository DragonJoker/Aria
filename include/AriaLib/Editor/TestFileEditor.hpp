/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_TestFileEditor_H___
#define ___ARIA_TestFileEditor_H___

#include "AriaLib/Editor/EditorModule.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/aui/framemanager.h>
#include <wx/panel.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

class wxFindReplaceData;

namespace aria
{
	class TestFileEditor
		: public wxPanel
	{
	public:
		TestFileEditor( StcContext & stcContext
			, wxString const & filename
			, wxWindow * parent
			, wxWindowID editId
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~TestFileEditor()override;

		bool isModified()const;
		wxString getFileName()const;
		bool saveFile();
		void findFirst( wxFindReplaceData const & data );
		void findNext( wxFindReplaceData const & data );
		void replace( wxFindReplaceData const & data );
		void replaceAll( wxFindReplaceData const & data );

	private:
		void doInitialiseLayout( wxString const & filename
			, wxWindowID editId );
		void doCleanup();

	protected:
		wxAuiManager m_auiManager;
		StcContext & m_stcContext;
		StcTextEditor * m_editor{};
		wxString m_source;
		int m_currentIter{};
	};
}

#endif
