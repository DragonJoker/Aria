/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestStatsPanel_HPP___
#define ___CTP_TestStatsPanel_HPP___

#include "Prerequisites.hpp"

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

	private:
		TestDatabase & m_database;
		DatabaseTest * m_test{};
		Host const & m_host;
		wxPanel * m_hostPanel{};
		wxPanel * m_totalPanel{};
		wxPanel * m_framePanel{};
		TestStatus m_maxStatus{ TestStatus::eAcceptable };
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
