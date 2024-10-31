/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestRunsList_HPP___
#define ___CTP_TestRunsList_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/aui/framemanager.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

class wxMenu;

namespace aria
{
	class TestRunsPanel
		: public wxPanel
	{
	public:
		TestRunsPanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, TestDatabase & database
			, wxMenu * contextMenu );
		~TestRunsPanel()override;

		void setTest( DatabaseTest const & test );
		void deleteRun( uint32_t runId );
		void updateRunHost( uint32_t runId );
		void updateRunStatus( uint32_t runId );
		wxDataViewItemArray getSelection()const;

		DatabaseTest const * getTest()const
		{
			return m_test;
		}

	private:
		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );

	private:
		TestDatabase & m_database;
		wxMenu * m_contextMenu;
		DatabaseTest const * m_test{};
		wxAuiManager m_auiManager;
		wxObjectDataPtr< run::RunTreeModel > m_model;
		wxDataViewCtrl * m_view{};
	};
}

#endif
