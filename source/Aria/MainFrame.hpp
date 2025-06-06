/*
See LICENSE file in root folder
*/
#ifndef ___CTP_MainFrame_HPP___
#define ___CTP_MainFrame_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/Options.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/dataview.h>
#include <wx/frame.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

class wxGauge;
class wxStaticText;

namespace aria
{
	class MainFrame
		: public wxFrame
	{
	public:
		enum ID
		{
			eID_GIT = aria::Menus::eID_CANCEL_RUNS + 1,
			eID_CONFIG_EDIT,
			eID_CONFIG_NEW,
			eID_CONFIG_IMPORT,
			eID_CONFIG_SELECT_,
		};

	public:
		explicit MainFrame( OptionsPtr options );

		TestTreeModelNode * getTestNode( DatabaseTest const & test );
		wxDataViewItem getTestItem( DatabaseTest const & test );

		void loadConfiguration( Plugin * plugin );
		void unloadConfiguration();

		bool areTestsRunning()const;
		TestDatabase & getDatabase();

	private:
		void doInitMenus();
		void doInitMenuBar();

		void onClose( wxCloseEvent & evt );
		void onRendererMenuOption( wxCommandEvent & evt );
		void onCategoryMenuOption( wxCommandEvent & evt );
		void onTestMenuOption( wxCommandEvent & evt );
		void onDatabaseMenuOption( wxCommandEvent & evt );
		void onConfigMenuOption( wxCommandEvent & evt );

	private:
		OptionsPtr m_options;
		PathArray m_configs;
		std::unique_ptr< wxMenu > m_testMenu{};
		std::unique_ptr< wxMenu > m_categoryMenu{};
		std::unique_ptr< wxMenu > m_rendererMenu{};
		std::unique_ptr< wxMenu > m_busyTestMenu{};
		std::unique_ptr< wxMenu > m_busyCategoryMenu{};
		std::unique_ptr< wxMenu > m_busyRendererMenu{};
		wxMenu * m_barTestMenu{};
		wxMenu * m_barCategoryMenu{};
		wxMenu * m_barRendererMenu{};
		Menus m_menus{};
		TestsMainPanel * m_configurationPanel{};
	};
}

#endif
