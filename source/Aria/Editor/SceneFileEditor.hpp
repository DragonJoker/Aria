/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_SceneFileEditor_H___
#define ___ARIA_SceneFileEditor_H___

#include "EditorModule.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/aui/framemanager.h>
#include <wx/panel.h>
#pragma warning( pop )

class wxFindReplaceData;

namespace aria
{
	class SceneFileEditor
		: public wxPanel
	{
	public:
		SceneFileEditor( StcContext & stcContext
			, wxString const & filename
			, wxWindow * parent
			, wxPoint const & position = wxDefaultPosition
			, const wxSize size = wxSize( 800, 600 ) );
		~SceneFileEditor()override;

		bool saveFile();
		void findFirst( wxFindReplaceData const & data );
		void findNext( wxFindReplaceData const & data );
		void replace( wxFindReplaceData const & data );
		void replaceAll( wxFindReplaceData const & data );

	private:
		void doInitialiseLayout( wxString const & filename );
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
