#include "Plugin.hpp"

#include "Database/DatabaseTest.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/filepicker.h>
#include <wx/dirctrl.h>
#include <wx/filectrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	//*********************************************************************************************

	namespace config
	{
		static constexpr int MinWidth = 600;
		static constexpr int MinHeight = 25;

		template< typename PickerT >
		static void addPickerField( wxWindow & parent
			, wxSizer & sizer
			, wxString const & name
			, wxString const & tip
			, wxFileName & value
			, wxEventTypeTag< wxFileDirPickerEvent > const & evt
			, int style
			, int topBorder )
		{
			auto fieldSizer = new wxBoxSizer( wxVERTICAL );
			auto label = new wxStaticText{ &parent, wxID_ANY, name };
			fieldSizer->Add( label, wxSizerFlags{} );

			auto tooltip = new wxToolTip{ tip };
			label->SetToolTip( tooltip );

			auto picker = new PickerT{ &parent, wxID_ANY, value.GetFullPath(), wxT( "Choose the " ) + name };
			picker->SetSize( wxSize( MinWidth, MinHeight ) );
			picker->SetMinSize( wxSize( MinWidth, MinHeight ) );
			fieldSizer->Add( picker, wxSizerFlags{}.FixedMinSize() );
			picker->Bind( evt
				, [&value]( wxFileDirPickerEvent const & event )
				{
					auto result = event.GetPath();

					if ( !result.empty() )
					{
						value = event.GetPath();
					}
				} );

			sizer.Add( fieldSizer, wxSizerFlags{}.Border( wxUP, topBorder ) );
		}
	}

	void addTextField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxString & value
		, int style
		, int topBorder )
	{
		auto fieldSizer = new wxBoxSizer( wxVERTICAL );
		auto label = new wxStaticText{ &parent, wxID_ANY, name };
		fieldSizer->Add( label, wxSizerFlags{} );

		auto tooltip = new wxToolTip{ tip };
		label->SetToolTip( tooltip );

		auto picker = new wxTextCtrl{ &parent, wxID_ANY, value };
		picker->SetSize( wxSize( config::MinWidth, config::MinHeight ) );
		picker->SetMinSize( wxSize( config::MinWidth, config::MinHeight ) );
		fieldSizer->Add( picker, wxSizerFlags{}.FixedMinSize() );
		picker->Bind( wxEVT_TEXT
			, [&value]( wxCommandEvent const & event )
			{
				if ( auto result = event.GetString();
					!result.empty() )
				{
					value = result;
				}
			} );

		parentSizer.Add( fieldSizer, wxSizerFlags{}.Border( wxUP, topBorder ) );
	}

	void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value
		, int style
		, int topBorder )
	{
		config::addPickerField< wxFilePickerCtrl >( parent
			, parentSizer
			, name
			, tip
			, value
			, wxEVT_FILEPICKER_CHANGED
			, wxFLP_USE_TEXTCTRL | style
			, topBorder );
	}

	void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value
		, int style
		, int topBorder )
	{
		config::addPickerField< wxDirPickerCtrl >( parent
			, parentSizer
			, name
			, tip
			, value
			, wxEVT_DIRPICKER_CHANGED
			, wxDIRP_USE_TEXTCTRL | style
			, topBorder );
	}

	void addTextField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxString & value
		, int topBorder )
	{
		addTextField( parent
			, parentSizer
			, name
			, tip
			, value
			, wxFLP_DEFAULT_STYLE
			, topBorder );
	}

	void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value
		, int topBorder )
	{
		addFileField( parent
			, parentSizer
			, name
			, tip
			, value
			, wxFLP_DEFAULT_STYLE
			, topBorder );
	}

	void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value
		, int topBorder )
	{
		addDirField( parent
			, parentSizer
			, name
			, tip
			, value
			, wxDIRP_DEFAULT_STYLE
			, topBorder );
	}

	//*********************************************************************************************

	void PluginFactory::registerPlugin( std::string name
		, Creator creator )
	{
		m_registered.emplace( std::move( name ), std::move( creator ) );
	}

	void PluginFactory::unregisterPlugin( std::string const & name )
	{
		auto it = m_registered.find( name );

		if ( it != m_registered.end() )
		{
			m_registered.erase( it );
		}
	}

	PluginPtr PluginFactory::create( std::string const & name )const
	{
		auto it = m_registered.find( name );

		if ( it != m_registered.end() )
		{
			return it->second();
		}

		return nullptr;
	}

	//*********************************************************************************************

	PluginConfig::PluginConfig( std::vector< wxString > supportedRenderers )noexcept
		: m_supportedRenderers{ std::move( supportedRenderers ) }
	{
	}

	//*********************************************************************************************

	Plugin::Plugin( wxString name
		, std::unique_ptr< PluginConfig > pluginConfig )
		: m_name{ std::move( name ) }
		, m_pluginConfig{ std::move( pluginConfig ) }
		, config{ *m_pluginConfig }
	{
	}

	void Plugin::initConfig()const
	{
		m_pluginConfig->init();
	}

	void Plugin::updateConfig( std::unique_ptr< PluginConfig > pluginConfig )
	{
		m_pluginConfig = std::move( pluginConfig );
		config.pluginConfig = m_pluginConfig.get();
	}

	long Plugin::viewTest( wxProcess * process
		, TestRun const & test
		, wxString const & rendererName
		, bool async )const
	{
		return viewTest( process
			, *test.test
			, rendererName
			, async );
	}

	long Plugin::viewTest( wxProcess * process
		, DatabaseTest const & test
		, wxString const & rendererName
		, bool async )const
	{
		return viewTest( process
			, *test
			, rendererName
			, async );
	}

	long Plugin::runTest( wxProcess * process
		, TestRun const & test
		, wxString const & rendererName )const
	{
		return runTest( process
			, *test.test
			, rendererName );
	}

	long Plugin::runTest( wxProcess * process
		, DatabaseTest const & test
		, wxString const & rendererName )const
	{
		return runTest( process
			, *test
			, rendererName );
	}

	void Plugin::editTest( wxWindow * parent
		, TestRun const & test )const
	{
		editTest( parent, *test.test );
	}

	void Plugin::editTest( wxWindow * parent
		, DatabaseTest const & test )const
	{
		editTest( parent, *test );
	}

	db::DateTime Plugin::getTestDate( TestRun const & test )const
	{
		return getTestDate( *test.test );
	}

	wxFileName Plugin::getTestName( TestRun const & test )const
	{
		return getTestName( *test.test );
	}

	wxFileName Plugin::getTestFileName( DatabaseTest const & test )const
	{
		return getTestFileName( *test->test );
	}

	//*********************************************************************************************
}
