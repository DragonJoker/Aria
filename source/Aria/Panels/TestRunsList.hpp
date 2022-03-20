/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestRunsList_HPP___
#define ___CTP_TestRunsList_HPP___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/aui/framemanager.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#pragma warning( pop )

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
		~TestRunsPanel();

		void setTest( DatabaseTest const & test );
		void deleteRun( uint32_t runId );
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
