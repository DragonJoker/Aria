#include "Plugin.hpp"

#include "Database/DatabaseTest.hpp"

namespace aria
{
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
