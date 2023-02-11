#include "Options.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/choicdlg.h>
#include <wx/stdpaths.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	//*********************************************************************************************

	namespace option
	{
		static const wxString FrameCount{ wxT( "frames" ) };
		static const wxString Database{ wxT( "database" ) };
		static const wxString Test{ wxT( "test" ) };
		static const wxString Work{ wxT( "work" ) };
		static const wxString Plugin{ wxT( "plugin" ) };
		static const wxString KnownConfig{ _( "knownConfig" ) };

#if defined( _WIN32 )
		static wxString const BinExt = wxT( ".exe" );
		static wxString const DynlibExt = wxT( ".dll" );
		static wxString const DynlibPre;
#elif defined( __APPLE__ )
		static wxString const BinExt;
		static wxString const DynlibExt = wxT( ".dylib" );
		static wxString const DynlibPre = wxT( "lib" );
#else
		static wxString const BinExt;
		static wxString const DynlibExt = wxT( ".so" );
		static wxString const DynlibPre = wxT( "lib" );
#endif

		static void listPlugins( std::vector< PluginLib > & pluginsLibs
			, PluginFactory & factory )
		{
			wxFileName pluginsDir{ wxStandardPaths::Get().GetExecutablePath() };
#if !defined( _WIN32 )
			pluginsDir.RemoveLastDir();
			pluginsDir.AppendDir( wxT( "lib" ) );
#endif
			auto plugins = filterDirectoryFiles( pluginsDir.GetPath() / wxT( "Aria" )
				, []( wxString const & folder, wxString const & name )
				{
					return name.EndsWith( DynlibExt )
						&& name.StartsWith( "aria" );
				} );

			for ( auto & plugin : plugins )
			{
				auto pluginLib = new wxDynamicLibrary{ plugin.GetFullPath() };

				if ( pluginLib->IsLoaded() )
				{
					pluginsLibs.emplace_back( factory, pluginLib );
				}
			}
		}

		wxString selectPlugin( PluginFactory const & factory )
		{
			wxArrayString choices;

			for ( auto it : factory )
			{
				choices.push_back( it.first );
			}

			wxSingleChoiceDialog dialog{ nullptr
				, wxT( "Select your plugin" )
				, wxT( "Plugin selection" )
				, choices };

			if ( dialog.ShowModal() == wxID_OK )
			{
				return dialog.GetStringSelection();
			}

			return wxString{};
		}
	}

	//*********************************************************************************************

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconditionally-supported"
	PluginLib::PluginLib( PluginFactory & factory
		, wxDynamicLibrary * lib )
		: m_factory{ &factory }
		, m_lib{ lib }
		, m_onLoad{ PFN_OnLoad( lib->GetSymbol( "onLoad" ) ) }
		, m_onUnload{ PFN_OnUnload( lib->GetSymbol( "onUnload" ) ) }
	{
		if ( m_onLoad && m_onUnload )
		{
			m_onLoad( &factory );
		}
	}
#pragma GCC diagnostic pop

	PluginLib::PluginLib( PluginLib && rhs )
		: m_factory{ rhs.m_factory }
		, m_lib{ rhs.m_lib }
		, m_onLoad{ rhs.m_onLoad }
		, m_onUnload{ rhs.m_onUnload }
	{
		rhs.m_factory = nullptr;
		rhs.m_lib = nullptr;
		rhs.m_onLoad = nullptr;
		rhs.m_onUnload = nullptr;
	}

	PluginLib & PluginLib::operator=( PluginLib && rhs )
	{
		m_factory = rhs.m_factory;
		m_lib = rhs.m_lib;
		m_onLoad = rhs.m_onLoad;
		m_onUnload = rhs.m_onUnload;

		rhs.m_factory = nullptr;
		rhs.m_lib = nullptr;
		rhs.m_onLoad = nullptr;
		rhs.m_onUnload = nullptr;

		return *this;
	}

	PluginLib::~PluginLib()
	{
		if ( m_factory && m_onLoad && m_onUnload )
		{
			m_onUnload( m_factory );
		}

		delete m_lib;
	}

	//*********************************************************************************************

	TestsOptions::TestsOptions( PluginFactory & factory
		, wxFileName const & configFileName )
		: configPath{ configFileName }
		, configFile{ wxEmptyString
			, wxEmptyString
			, configPath.GetFullPath()
			, wxEmptyString
			, wxCONFIG_USE_LOCAL_FILE }
		, plugin{ factory.create( makeStdString( getString( option::Plugin, true ) ) ) }
		, pluginPtr{ plugin.get() }
	{
		pluginPtr->config.test = getFileName( option::Test, true );
		pluginPtr->config.work = getFileName( option::Work, false, pluginPtr->config.test );
		pluginPtr->config.maxFrameCount = getLong( option::FrameCount, false, option::df::FrameCount );
		pluginPtr->config.database = getFileName( option::Database, false, pluginPtr->config.work / wxT( "db.sqlite" ) );
		pluginPtr->config.plugin = pluginPtr->getName();
		pluginPtr->config.pluginConfig->setup( *this );
	}

	TestsOptions::TestsOptions( PluginPtr pplugin
		, wxFileName const & configFileName )
		: configPath{ configFileName }
		, configFile{ wxEmptyString
			, wxEmptyString
			, configPath.GetFullPath()
			, wxEmptyString
			, wxCONFIG_USE_LOCAL_FILE }
		, plugin{ std::move( pplugin ) }
		, pluginPtr{ plugin.get() }
	{
		if ( !pluginPtr->config.database.IsOk() )
		{
			pluginPtr->config.database = pluginPtr->config.work / wxT( "db.sqlite" );
		}

		if ( pluginPtr->config.plugin.empty() )
		{
			pluginPtr->config.plugin = pluginPtr->getName();
		}
	}

	TestsOptions::TestsOptions( TestsOptions && rhs )
		: configPath{ std::move( rhs.configPath ) }
		, configFile{ wxEmptyString
			, wxEmptyString
			, configPath.GetFullPath()
			, wxEmptyString
			, wxCONFIG_USE_LOCAL_FILE }
		, plugin{ std::move( rhs.plugin ) }
		, pluginPtr{ plugin.get() }
	{
	}

	TestsOptions & TestsOptions::operator=( TestsOptions && rhs )
	{
		configPath = std::move( rhs.configPath );
		configFile.SetPath( configFile.GetPath() );
		plugin = std::move( rhs.plugin );
		pluginPtr = plugin.get();

		return *this;
	}

	wxString TestsOptions::getString( wxString const & option
		, bool mandatory
		, wxString const & defaultValue )const
	{
		wxString result;

		if ( configFile.HasEntry( option ) )
		{
			configFile.Read( option, &result );
		}
		else if ( mandatory )
		{
			throw false;
		}
		else
		{
			result = defaultValue;
		}

		return result;
	}

	wxFileName TestsOptions::getFileName( wxString const & option
		, bool mandatory
		, wxFileName const & defaultValue )const
	{
		wxString value;
		wxFileName result;

		if ( configFile.HasEntry( option ) )
		{
			configFile.Read( option, &value );
			result = wxFileName( value.mb_str( wxConvUTF8 ).data() );
		}
		else if ( mandatory )
		{
			throw false;
		}
		else
		{
			result = defaultValue;
		}

		if ( mandatory
			&& !wxDirExists( result.GetFullPath() ) )
		{
			throw false;
		}

		return result;
	}

	wxString TestsOptions::write()
	{
		configFile.Write( option::Test, pluginPtr->config.test.GetFullPath() );
		configFile.Write( option::Work, pluginPtr->config.work.GetFullPath() );
		configFile.Write( option::Database, pluginPtr->config.database.GetFullPath() );
		configFile.Write( option::FrameCount, pluginPtr->config.maxFrameCount );
		configFile.Write( option::Plugin, pluginPtr->config.plugin );
		pluginPtr->config.pluginConfig->write( configFile );
		configFile.Flush();
		return ( pluginPtr->config.work / "aria.ini" ).GetFullPath();
	}

	//*********************************************************************************************

	Options::Options( PluginFactory & factory
		, std::vector< PluginLib > & pluginsLibs
		, int argc
		, wxCmdLineArgsArray const & argv )
		: m_factory{ factory }
		, parser{ argc, argv }
		, configFile{ wxT( "Aria" )
			, wxT( "DragonJoker" )
			, wxEmptyString
			, wxEmptyString
			, wxCONFIG_USE_LOCAL_FILE }
	{
		static const wxString Help{ _( "Displays this help." ) };
		static const wxString Force{ _( "Force database initialisation." ) };
		static const wxString ConfigFile{ _( "Specifies the tests config file." ) };

		parser.AddSwitch( option::st::Help
			, option::lg::Help
			, Help );
		parser.AddSwitch( option::st::Force
			, option::lg::Force
			, Force );
		parser.AddOption( option::st::ConfigFile
			, option::lg::ConfigFile
			, ConfigFile
			, wxCMD_LINE_VAL_STRING, 0 );

		if ( ( parser.Parse( false ) != 0 )
			|| parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			throw false;
		}

		option::listPlugins( pluginsLibs, m_factory );
		wxString entry;
		long index;

		if ( configFile.GetFirstEntry( entry, index ) )
		{
			do
			{
				if ( entry.StartsWith( option::KnownConfig ) )
				{
					wxString value;
					configFile.Read( entry, &value );
					load( wxFileName{ value } );
				}
			}
			while ( configFile.GetNextEntry( entry, index ) );
		}

		if ( has( option::lg::ConfigFile ) )
		{
			auto cfg = getFileName( option::lg::ConfigFile, true );
			select( cfg );
		}
	}

	bool Options::has( wxString const & option )const
	{
		return parser.Found( option )
			|| configFile.HasEntry( option );
	}

	wxFileName Options::getFileName( wxString const & option
		, bool mandatory
		, wxFileName const & defaultValue )const
	{
		wxString value;
		wxFileName result;

		if ( parser.Found( option, &value ) )
		{
			result = wxFileName( value.mb_str( wxConvUTF8 ).data() );
		}
		else if ( configFile.HasEntry( option ) )
		{
			configFile.Read( option, &value );
			result = wxFileName( value.mb_str( wxConvUTF8 ).data() );
		}
		else if ( mandatory )
		{
			throw false;
		}
		else
		{
			result = defaultValue;
		}

		if ( mandatory
			&& !wxFileExists( result.GetFullPath() ) )
		{
			throw false;
		}

		return result;
	}

	wxString Options::getString( wxString const & option
		, bool mandatory
		, wxString const & defaultValue )const
	{
		wxString result;

		if ( parser.Found( option, &result ) )
		{
		}
		else if ( configFile.HasEntry( option ) )
		{
			configFile.Read( option, &result );
		}
		else if ( mandatory )
		{
			throw false;
		}
		else
		{
			result = defaultValue;
		}

		return result;
	}

	void Options::load( PluginPtr pplugin
		, wxFileName const & fileName )
	{
		try
		{
			testsOptions.push_back( TestsOptions{ std::move( pplugin ), fileName } );
		}
		catch ( bool )
		{
		}
	}

	void Options::load( wxFileName const & fileName )
	{
		try
		{
			testsOptions.push_back( TestsOptions{ m_factory, fileName } );
		}
		catch ( bool )
		{
		}
	}

	void Options::select( wxFileName const & fileName )
	{
		auto it = std::find_if( testsOptions.begin()
			, testsOptions.end()
			, [&fileName]( TestsOptions const & lookup )
			{
				return lookup.getFilePath() == fileName.GetFullPath();
			} );

		if ( it != testsOptions.end() )
		{
			plugin = it->getPlugin();

			auto & config = plugin->config;
			config.initFromFolder = config.initFromFolder
				|| has( option::st::Force );
			wxLogMessage( "Test folder: " + config.test.GetFullPath() );
			wxLogMessage( "Work folder: " + config.work.GetFullPath() );
			wxLogMessage( "Database: " + config.database.GetFullPath() );
			plugin->initConfig();
		}
	}

	void Options::write()
	{
		int index{};

		for ( auto & options : testsOptions )
		{
			configFile.Write( option::KnownConfig + ( wxString{} << ++index ), options.write() );
		}

		configFile.Flush();
	}

	PathArray Options::listConfigs()const
	{
		PathArray result;

		for ( auto & options : testsOptions )
		{
			result.push_back( options.getFilePath() );
		}

		return result;
	}

	//*********************************************************************************************
}
