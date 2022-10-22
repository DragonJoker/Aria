/*
See LICENSE file in root folder
*/
#ifndef ___Aria_Plugin_HPP___
#define ___Aria_Plugin_HPP___

#include "Prerequisites.hpp"

#include <functional>

#define Aria_UseAsync 1

namespace aria
{
	class PluginFactory
	{
		using Creator = std::function< PluginPtr() >;
	public:
		AriaLib_API void registerPlugin( std::string name
			, Creator creator );
		AriaLib_API void unregisterPlugin( std::string const & name );

		AriaLib_API PluginPtr create( std::string const & name )const;

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
		AriaLib_API virtual ~PluginConfig() = default;
		AriaLib_API virtual void fillParser( wxCmdLineParser & parser )const = 0;
		AriaLib_API virtual void fillDialog( wxDialog & dialog
			, wxSizer & parentSizer ) = 0;
		AriaLib_API virtual void setup( TestsOptions const & options ) = 0;
		AriaLib_API virtual void init() = 0;
		AriaLib_API virtual void write( wxFileConfig & configFile )const = 0;

		AriaLib_API virtual db::DateTime const & getEngineRefDate()const = 0;
		AriaLib_API virtual void updateEngineRefDate() = 0;
	};

	class Plugin
	{
	protected:
		AriaLib_API Plugin( wxString name
			, std::unique_ptr< PluginConfig > pluginConfig );

	public:
		AriaLib_API virtual ~Plugin() = default;

		AriaLib_API void initConfig()const;
		AriaLib_API void updateConfig( std::unique_ptr< PluginConfig > pluginConfig );

		AriaLib_API virtual std::unique_ptr< PluginConfig > createConfig()const = 0;
		AriaLib_API virtual void createTest( Test const & test
			, FileSystem & fileSystem )const = 0;
		AriaLib_API virtual void deleteTest( Test const & test
			, FileSystem & fileSystem )const = 0;
		AriaLib_API virtual void changeTestCategory( Test const & test
			, Category oldCategory
			, Category newCategory
			, FileSystem & fileSystem ) = 0;
		AriaLib_API virtual long viewTest( wxProcess * process
			, Test const & test
			, wxString const & rendererName
			, bool async )const = 0;
		AriaLib_API virtual long runTest( wxProcess * process
			, Test const & test
			, wxString const & rendererName )const = 0;
		AriaLib_API virtual void editTest( wxWindow * parent
			, Test const & test )const = 0;
		AriaLib_API virtual db::DateTime getTestDate( Test const & test )const = 0;
		AriaLib_API virtual wxFileName getTestFileName( Test const & test )const = 0;
		AriaLib_API virtual wxFileName getTestName( Test const & test )const = 0;
		AriaLib_API virtual bool isOutOfEngineDate( TestRun const & test )const = 0;
		AriaLib_API virtual bool isOutOfTestDate( TestRun const & test )const = 0;
		AriaLib_API virtual bool isSceneFile( wxString const & test )const = 0;

		AriaLib_API long viewTest( wxProcess * process
			, TestRun const & test
			, wxString const & rendererName
			, bool async )const;
		AriaLib_API long viewTest( wxProcess * process
			, DatabaseTest const & test
			, wxString const & rendererName
			, bool async )const;
		AriaLib_API long runTest( wxProcess * process
			, TestRun const & test
			, wxString const & rendererName )const;
		AriaLib_API long runTest( wxProcess * process
			, DatabaseTest const & test
			, wxString const & rendererName )const;
		AriaLib_API void editTest( wxWindow * parent
			, TestRun const & test )const;
		AriaLib_API void editTest( wxWindow * parent
			, DatabaseTest const & test )const;

		AriaLib_API db::DateTime getTestDate( TestRun const & test )const;
		AriaLib_API wxFileName getTestName( TestRun const & test )const;
		AriaLib_API wxFileName getTestFileName( DatabaseTest const & test )const;

		long viewTest( Test const & test
			, bool async )const
		{
			return viewTest( nullptr
				, test
				, wxEmptyString
				, async );
		}

		long viewTest( TestRun const & test
			, bool async )const
		{
			return viewTest( nullptr
				, test
				, wxEmptyString
				, async );
		}

		long viewTest( DatabaseTest const & test
			, bool async )const
		{
			return viewTest( nullptr
				, test
				, wxEmptyString
				, async );
		}

		bool isOutOfDate( TestRun const & test )const
		{
			return isOutOfTestDate( test )
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

	AriaLib_API void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value );
	AriaLib_API void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value );
}

#endif
