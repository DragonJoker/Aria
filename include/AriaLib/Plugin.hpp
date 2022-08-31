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
		void registerPlugin( std::string name
			, Creator creator );
		void unregisterPlugin( std::string const & name );

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
		virtual ~PluginConfig() = default;
		virtual void fillParser( wxCmdLineParser & parser )const = 0;
		virtual void fillDialog( wxDialog & dialog
			, wxSizer & parentSizer ) = 0;
		virtual void setup( Options const & options ) = 0;
		virtual void init() = 0;
		virtual void write( wxFileConfig & configFile )const = 0;

		virtual db::DateTime const & getEngineRefDate()const = 0;
		virtual void updateEngineRefDate() = 0;
	};

	class Plugin
	{
	protected:
		Plugin( wxString name
			, std::unique_ptr< PluginConfig > pluginConfig );

	public:
		virtual ~Plugin() = default;

		void initConfig()const;
		void updateConfig( std::unique_ptr< PluginConfig > pluginConfig );

		virtual std::unique_ptr< PluginConfig > createConfig()const = 0;
		virtual void createTest( Test const & test
			, FileSystem & fileSystem )const = 0;
		virtual void deleteTest( Test const & test
			, FileSystem & fileSystem )const = 0;
		virtual void changeTestCategory( Test const & test
			, Category oldCategory
			, Category newCategory
			, FileSystem & fileSystem ) = 0;
		virtual long viewTest( wxProcess * process
			, Test const & test
			, wxString const & rendererName
			, bool async )const = 0;
		virtual long runTest( wxProcess * process
			, Test const & test
			, wxString const & rendererName )const = 0;
		virtual void editTest( wxWindow * parent
			, Test const & test )const = 0;
		virtual db::DateTime getTestDate( Test const & test )const = 0;
		virtual wxFileName getTestFileName( Test const & test )const = 0;
		virtual wxFileName getTestName( Test const & test )const = 0;
		virtual bool isOutOfEngineDate( TestRun const & test )const = 0;
		virtual bool isOutOfTestDate( TestRun const & test )const = 0;

		long viewTest( wxProcess * process
			, TestRun const & test
			, wxString const & rendererName
			, bool async )const;
		long viewTest( wxProcess * process
			, DatabaseTest const & test
			, wxString const & rendererName
			, bool async )const;
		long runTest( wxProcess * process
			, TestRun const & test
			, wxString const & rendererName )const;
		long runTest( wxProcess * process
			, DatabaseTest const & test
			, wxString const & rendererName )const;
		void editTest( wxWindow * parent
			, TestRun const & test )const;
		void editTest( wxWindow * parent
			, DatabaseTest const & test )const;

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

		db::DateTime getTestDate( TestRun const & test )const;
		wxFileName getTestName( TestRun const & test )const;
		wxFileName getTestFileName( DatabaseTest const & test )const;

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

	void addFileField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value );
	void addDirField( wxWindow & parent
		, wxSizer & parentSizer
		, wxString const & name
		, wxString const & tip
		, wxFileName & value );
}

#endif
