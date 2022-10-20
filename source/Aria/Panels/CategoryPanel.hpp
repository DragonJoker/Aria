/*
See LICENSE file in root folder
*/
#ifndef ___CTP_CategoryPanel_HPP___
#define ___CTP_CategoryPanel_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/TestsCounts.hpp>

#include <wx/panel.h>

class wxStaticText;

namespace aria
{
	class CategoryPanel
		: public wxPanel
	{
	public:
		CategoryPanel( wxWindow * parent
			, wxPoint const & position
			, wxSize const & size );

		void refresh();
		void update( wxString const & name
			, AllTestsCounts & counts );
		void update( wxString const & name
			, RendererTestsCounts & counts );
		void update( wxString const & name
			, TestsCounts & counts );

	private:
		AllTestsCounts * m_allCounts{};
		RendererTestsCounts * m_rendererCounts{};
		TestsCounts * m_categoryCounts{};
		wxString m_name;
		std::array< wxStaticText *, TestsCountsType::eCount > m_values{};
	};
}

#endif
