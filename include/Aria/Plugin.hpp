/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Plugin_HPP___
#define ___CTP_Plugin_HPP___

#include "Prerequisites.hpp"

#if !defined( _WIN32 )
#	define Aria_API
#else
#	if defined( Aria_EXPORTS )
#		define Aria_API __declspec( dllexport )
#	else
#		define Aria_API __declspec( dllimport )
#	endif
#endif

#define Aria_UseAsync 1

namespace aria
{
	class PluginVisitor
	{
	};

	class PluginConfig
	{
	public:
		Aria_API void fillParser( wxCmdLineParser & parser )const;
		Aria_API void fillDialog( wxDialog & dialog
			, wxSizer & parentSizer );
		Aria_API void setup( Options const & options );
		Aria_API void init();
		Aria_API void write( wxFileConfig & configFile )const;

		Aria_API db::DateTime const & getEngineRefDate()const;
		Aria_API void updateEngineRefDate();

		wxFileName launcher;
		wxFileName viewer;
		wxFileName engine;
		db::DateTime engineRefDate;
	};

	class Plugin
	{
	public:
		Aria_API Plugin();
		Aria_API virtual ~Plugin() = default;

		Aria_API void accept( PluginVisitor * visitor );

		Aria_API void initConfig()const;
		Aria_API void updateConfig( std::unique_ptr< PluginConfig > pluginConfig );
		Aria_API std::unique_ptr< PluginConfig > createConfig()const;

		Aria_API long viewTest( wxProcess * process
			, wxString const & fileName
			, wxString const & rendererName
			, bool async )const;
		Aria_API long runTest( wxProcess * process
			, wxString const & fileName
			, wxString const & rendererName )const;

		long viewTest( wxString const & fileName
			, bool async )const
		{
			return viewTest( nullptr
				, fileName
				, wxEmptyString
				, async );
		}

		// Scene File handling
		Aria_API void viewSceneFile( wxWindow * parent
			, wxFileName const & filePath )const;
		Aria_API wxFileName getOldSceneName( Test const & test );
		Aria_API db::DateTime getSceneDate( Test const & test )const;
		Aria_API db::DateTime getSceneDate( TestRun const & test )const;
		Aria_API wxFileName getSceneFile( Test const & test )const;
		Aria_API wxFileName getSceneFile( TestRun const & test )const;
		Aria_API wxFileName getSceneName( Test const & test )const;
		Aria_API wxFileName getSceneName( TestRun const & test )const;
		Aria_API wxFileName getTestFileName( wxFileName const & folder
			, Test const & test )const;
		Aria_API wxFileName getTestFileName( wxFileName const & folder
			, DatabaseTest const & test )const;

		Aria_API bool isOutOfSceneDate( TestRun const & test )const;
		Aria_API bool isOutOfEngineDate( TestRun const & test )const;
		Aria_API bool isOutOfDate( TestRun const & test )const;

		Aria_API db::DateTime const & getEngineRefDate()const;
		Aria_API void updateEngineRefDate()const;

	private:
		std::unique_ptr< PluginConfig > m_pluginConfig;

	public:
		Config config;
	};
}

#endif
