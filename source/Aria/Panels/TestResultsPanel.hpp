/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestResultsPanel_HPP___
#define ___CTP_TestResultsPanel_HPP___

#include "Prerequisites.hpp"

#include <wx/panel.h>
#include <wx/image.h>

class wxComboBox;

namespace aria
{
	class wxImagePanel;

	class TestResultsPanel
		: public wxPanel
	{
		enum ImgIndex : int
		{
			None,
			Source,
			Diff,
		};

	public:
		TestResultsPanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, Config const & config );

		void refresh();
		void setTest( DatabaseTest & test );

		DatabaseTest * getTest()const
		{
			return m_test;
		}

	private:
		void loadRef( int index );
		void loadRes( int index );
		void onRefSelect( wxCommandEvent & evt );
		void onResSelect( wxCommandEvent & evt );

	private:
		Config const & m_config;
		DatabaseTest * m_test{};
		wxImagePanel * m_ref{};
		wxImagePanel * m_result{};
		ImgIndex m_currentRef{ None };
		ImgIndex m_currentRes{ None };
		wxImage m_refImage;
		wxImage m_resImage;
		wxImage m_refToResImage;
		wxImage m_resToRefImage;
	};
}

#endif
