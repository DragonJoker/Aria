/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Plugin_HPP___
#define ___CTP_Plugin_HPP___

#include "Prerequisites.hpp"

#include <functional>

#define Aria_UseAsync 1

namespace aria
{
	class PluginFactory
	{
		using Creator = std::function< PluginPtr() >;
	public:
		Aria_API void registerPlugin( std::string name
			, Creator creator );
		Aria_API void unregisterPlugin( std::string const & name );

		PluginPtr create( std::string const & name )const;

		auto begin()const
		{
			return m_registered.begin();
		}

		auto end()const
		{
			return m_registered.end();
		}

	private:
		std::map< std::string, Creator > m_registered;
	};

	class PluginConfig
	{
	public:
		Aria_API virtual ~PluginConfig() = default;
		Aria_API virtual void fillParser( wxCmdLineParser & parser )const = 0;
		Aria_API virtual void fillDialog( wxDialog & dialog
			, wxSizer & parentSizer ) = 0;
		Aria_API virtual void setup( Options const & options ) = 0;
		Aria_API virtual void init() = 0;
		Aria_API virtual void write( wxFileConfig & configFile )const = 0;

		Aria_API virtual db::DateTime const & getEngineRefDate()const = 0;
		Aria_API virtual void updateEngineRefDate() = 0;
	};

	class Plugin
	{
	protected:
		Aria_API Plugin( wxString name
			, std::unique_ptr< PluginConfig > pluginConfig );

	public:
		Aria_API virtual ~Plugin() = default;

		Aria_API void initConfig()const;
		Aria_API void updateConfig( std::unique_ptr< PluginConfig > pluginConfig );

		Aria_API virtual std::unique_ptr< PluginConfig > createConfig()const = 0;
		Aria_API virtual long viewTest( wxProcess * process
			, wxString const & fileName
			, wxString const & rendererName
			, bool async )const = 0;
		Aria_API virtual long runTest( wxProcess * process
			, wxString const & fileName
			, wxString const & rendererName )const = 0;
		Aria_API virtual void viewSceneFile( wxWindow * parent
			, wxFileName const & filePath )const = 0;
		Aria_API virtual wxFileName getOldSceneName( Test const & test ) = 0;
		Aria_API virtual wxFileName getSceneName( Test const & test )const = 0;
		Aria_API virtual bool isOutOfEngineDate( TestRun const & test )const = 0;

		long viewTest( wxString const & fileName
			, bool async )const
		{
			return viewTest( nullptr
				, fileName
				, wxEmptyString
				, async );
		}

		Aria_API db::DateTime getSceneDate( Test const & test )const;
		Aria_API db::DateTime getSceneDate( TestRun const & test )const;
		Aria_API wxFileName getSceneFile( Test const & test )const;
		Aria_API wxFileName getSceneFile( TestRun const & test )const;
		Aria_API wxFileName getSceneName( TestRun const & test )const;
		Aria_API wxFileName getTestFileName( wxFileName const & folder
			, Test const & test )const;
		Aria_API wxFileName getTestFileName( wxFileName const & folder
			, DatabaseTest const & test )const;
		Aria_API bool isOutOfSceneDate( TestRun const & test )const;

		bool isOutOfDate( TestRun const & test )const
		{
			return isOutOfSceneDate( test )
				|| isOutOfEngineDate( test );
		}

		db::DateTime const & getEngineRefDate()const
		{
			return m_pluginConfig->getEngineRefDate();
		}

		void updateEngineRefDate()const
		{
			m_pluginConfig->updateEngineRefDate();
		}

		wxString const & getName()const
		{
			return m_name;
		}

	protected:
		wxString m_name;
		std::unique_ptr< PluginConfig > m_pluginConfig;

	public:
		Config config;
	};

	Aria_API void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value );
	Aria_API void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value );
}

#endif
