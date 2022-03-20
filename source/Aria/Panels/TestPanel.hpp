/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestPanel_HPP___
#define ___CTP_TestPanel_HPP___

#include "TestResultsPanel.hpp"
#include "TestStatsPanel.hpp"
#include "TestRunsList.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/panel.h>
#include <wx/aui/auibook.h>
#include <wx/aui/framemanager.h>
#pragma warning( pop )

namespace aria
{
	class TestPanel
		: public wxPanel
	{
	public:
		TestPanel( wxWindow * parent
			, Config const & config
			, TestDatabase & database );
		~TestPanel()override;

		void refresh();
		void setTest( DatabaseTest & test );

		DatabaseTest * getTest()const
		{
			return m_test;
		}

	private:
		void doDeleteRun();
		void onMenuOption( wxCommandEvent & evt );

	private:
		Config const & m_config;
		TestDatabase & m_database;
		wxAuiManager m_auiManager;
		DatabaseTest * m_test{};
		wxAuiNotebook * m_pages{};
		wxMenu * m_runsListMenu;
		TestResultsPanel * m_results{};
		TestStatsPanel * m_stats{};
		TestRunsPanel * m_runs{};
	};
}

#endif
