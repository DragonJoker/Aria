#include "Plugin.hpp"

#include "Database/DatabaseTest.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/filepicker.h>
#include <wx/dirctrl.h>
#include <wx/filectrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/tooltip.h>
#pragma warning( pop )

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
			, wxEventTypeTag< wxFileDirPickerEvent > const & evt )
		{
			auto label = new wxStaticText{ &parent, wxID_ANY, name };
			sizer.Add( label, wxSizerFlags{} );

			auto tooltip = new wxToolTip{ tip };
			label->SetToolTip( tooltip );

			auto picker = new PickerT{ &parent, wxID_ANY, value.GetFullPath(), wxT( "Choose the " ) + name };
			picker->SetSize( wxSize( MinWidth, MinHeight ) );
			picker->SetMinSize( wxSize( MinWidth, MinHeight ) );
			sizer.Add( picker, wxSizerFlags{}.Border( wxBOTTOM, 5 ).FixedMinSize() );
			picker->Bind( evt
				, [&value]( wxFileDirPickerEvent & event )
				{
					auto result = event.GetPath();

					if ( !result.empty() )
					{
						value = event.GetPath();
					}
				} );
		}
	}

	void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value )
	{
		config::addPickerField< wxFilePickerCtrl >( parent
			, parentSizer
			, name
			, tip
			, value
			, wxEVT_FILEPICKER_CHANGED );
	}

	void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value )
	{
		config::addPickerField< wxDirPickerCtrl >( parent
			, parentSizer
			, name
			, tip
			, value
			, wxEVT_DIRPICKER_CHANGED );
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
