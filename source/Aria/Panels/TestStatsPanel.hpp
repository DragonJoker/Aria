/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestStatsPanel_HPP___
#define ___CTP_TestStatsPanel_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/panel.h>
#include <wx/aui/auibook.h>
#include <wx/aui/framemanager.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	class HostTestStatsPanel
		: public wxPanel
	{
	public:
		HostTestStatsPanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, TestDatabase & database
			, Host const & host );

		void refresh();
		void setTest( DatabaseTest & test );
		void filterTests( TestStatus maxStatus );
		void deleteRun( uint32_t runId );
		int getSelection();
		void setSelection( int sel );

	private:
		TestDatabase & m_database;
		DatabaseTest * m_test{};
		Host const & m_host;
		wxPanel * m_hostPanel{};
		wxStaticText * m_platformDest;
		wxStaticText * m_cpuDest;
		wxStaticText * m_gpuDest;
		wxAuiNotebook * m_pages{};
		TestStatus m_maxStatus{ TestStatus::eNegligible };
	};

	class TestStatsPanel
		: public wxPanel
	{
	public:
		TestStatsPanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, TestDatabase & database );
		~TestStatsPanel()override;

		void refresh();
		void setTest( DatabaseTest & test );
		void deleteRun( uint32_t hostId
			, uint32_t runId );

		DatabaseTest * getTest()const
		{
			return m_test;
		}

	private:
		void onPageChange( wxAuiNotebookEvent & evt );

	private:
		TestDatabase & m_database;
		DatabaseTest * m_test{};
		wxAuiManager m_auiManager;
		wxAuiNotebook * m_pages{};
		std::map< int32_t, HostTestStatsPanel * > m_hosts;
		std::map< int, int32_t > m_hostPages;
	};
}

#endif
