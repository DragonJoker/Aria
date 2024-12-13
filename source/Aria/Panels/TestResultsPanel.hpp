/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestResultsPanel_HPP___
#define ___CTP_TestResultsPanel_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/panel.h>
#include <wx/image.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

#include <array>

class wxComboBox;

namespace aria
{
	class TestResultsFullSizePanel;
	class TestResultsSideBySidePanel;
	class wxImagePanel;

	class TestResultsPanel
		: public wxPanel
	{
	public:
		enum AllImgIndex : size_t
		{
			eResult,
			eReference,
			eDiffResToRefRaw,
			eDiffResToRefLog,
			eDiffResToRefFlip,
			eDiffRefToResRaw,
			eDiffRefToResLog,
			eDiffRefToResFlip,
			eCount,
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
		void onDisplayMode( wxCommandEvent & evt );

	private:
		Config const & m_config;
		DatabaseTest * m_test{};
		TestResultsSideBySidePanel * m_sideBySide{};
		TestResultsFullSizePanel * m_fullSize{};
		LayeredPanel * m_layers{};
		size_t m_layer{ 1 };
		std::array< wxImage, eCount > m_images;
	};

	class TestResultsSideBySidePanel
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
		TestResultsSideBySidePanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, Config const & config
			, std::array< wxImage, TestResultsPanel::eCount > & images );

		void refresh();
		void setTest( DatabaseTest & test );

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
		std::array< wxImage, TestResultsPanel::eCount > * m_images;
	};

	class TestResultsFullSizePanel
		: public wxPanel
	{
		enum ImgIndex : int
		{
			eNone,
			eResult,
			eReference,
			eDiffRaw,
			eDiffLog,
			eDiffFlip,
		};

	public:
		TestResultsFullSizePanel( wxWindow * parent
			, wxWindowID id
			, wxSize const & size
			, Config const & config
			, std::array< wxImage, TestResultsPanel::eCount > & images );

		void refresh();
		void setTest( DatabaseTest & test );

	private:
		void load( int index );
		void onSelect( wxCommandEvent & evt );
		void onSave( wxCommandEvent & evt );

	private:
		Config const & m_config;
		DatabaseTest * m_test{};
		wxImagePanel * m_result{};
		ImgIndex m_current{ eNone };
		std::array< wxImage, TestResultsPanel::eCount > * m_images;
	};
}

#endif
