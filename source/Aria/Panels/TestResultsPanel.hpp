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
			eNone,
			eSource,
			eDiffRaw,
			eDiffLog,
			eDiffFlip,
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
		void onRefSave( wxCommandEvent & evt );
		void onResSave( wxCommandEvent & evt );

	private:
		Config const & m_config;
		DatabaseTest * m_test{};
		wxImagePanel * m_ref{};
		wxImagePanel * m_result{};
		ImgIndex m_currentRef{ eNone };
		ImgIndex m_currentRes{ eNone };
		wxImage m_refImage;
		wxImage m_resImage;
		wxImage m_refToResImageRaw;
		wxImage m_refToResImageLog;
		wxImage m_refToResImageFlip;
		wxImage m_resToRefImageRaw;
		wxImage m_resToRefImageLog;
		wxImage m_resToRefImageFlip;
	};
}

#endif
