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

	db::DateTime Plugin::getSceneDate( Test const & test )const
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	db::DateTime Plugin::getSceneDate( TestRun const & test )const
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	wxFileName Plugin::getSceneFile( Test const & test )const
	{
		return wxFileName{ test.category->name } / getSceneName( test );
	}

	wxFileName Plugin::getSceneFile( TestRun const & test )const
	{
		return getSceneFile( *test.test );
	}

	wxFileName Plugin::getSceneName( TestRun const & test )const
	{
		return getSceneName( *test.test );
	}

	wxFileName Plugin::getTestFileName( wxFileName const & folder
		, Test const & test )const
	{
		return folder / getSceneFile( test );
	}

	wxFileName Plugin::getTestFileName( wxFileName const & folder
		, DatabaseTest const & test )const
	{
		return getTestFileName( folder, *test->test );
	}

	bool Plugin::isOutOfSceneDate( TestRun const & test )const
	{
		return ( !test.sceneDate.IsValid() )
			|| test.sceneDate.IsEarlierThan( getSceneDate( test ) );
	}

	//*********************************************************************************************
}
