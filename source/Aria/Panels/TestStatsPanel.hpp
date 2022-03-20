/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestStatsPanel_HPP___
#define ___CTP_TestStatsPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>

namespace aria
{
	class TestStatsPanel
		: public wxPanel
	{
	public:
		TestStatsPanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, TestDatabase & database );

		void refresh();
		void setTest( DatabaseTest & test );
		void deleteRun( uint32_t runId );

		DatabaseTest * getTest()const
		{
			return m_test;
		}

	private:
		TestDatabase & m_database;
		DatabaseTest * m_test{};
		wxPanel * m_totalPanel{};
		wxPanel * m_framePanel{};
	};
}

#endif
