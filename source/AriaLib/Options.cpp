#include "Options.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/choicdlg.h>
#include <wx/stdpaths.h>
#pragma warning( pop )

namespace aria
{
	//*********************************************************************************************

	namespace option
	{
#if defined( _WIN32 )
		static wxString const BinExt = wxT( ".exe" );
		static wxString const DynlibExt = wxT( ".dll" );
		static wxString const DynlibPre;
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

		static wxString selectPlugin( PluginFactory const & factory )
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

	Options::Options( PluginFactory & factory
		, std::vector< PluginLib > & pluginsLibs
		, int argc
		, wxCmdLineArgsArray const & argv )
		: parser{ argc, argv }
	{
		static const wxString Help{ _( "Displays this help." ) };
		static const wxString Force{ _( "Force database initialisation." ) };
		static const wxString ConfigFile{ _( "Specifies the tests config file." ) };
		static const wxString Database{ _( "Specifies the database file." ) };
		static const wxString Test{ _( "Specifies the tests directory." ) };
		static const wxString Work{ _( "Specifies the working directory." ) };
		static const wxString FrameCount{ _( "The number of frames to let run before capture." ) };
		static const wxString Plugin{ _( "The plugin name." ) };

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
		parser.AddOption( option::st::Database
			, option::lg::Database
			, Database
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( option::st::Test
			, option::lg::Test
			, Test
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( option::st::Work
			, option::lg::Work
			, Work
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( option::st::FrameCount
			, option::lg::FrameCount
			, FrameCount
			, wxCMD_LINE_VAL_NUMBER );
		parser.AddOption( option::st::Plugin
			, option::lg::Plugin
			, Plugin
			, wxCMD_LINE_VAL_STRING, 0 );

		if ( ( parser.Parse( false ) != 0 )
			|| parser.Found( wxT( 'h' ) ) )
		{
			parser.Usage();
			throw false;
		}

		configFile = new wxFileConfig{ wxEmptyString
			, wxEmptyString
			, findConfigFile( parser )
			, wxEmptyString
			, wxCONFIG_USE_LOCAL_FILE };
		option::listPlugins( pluginsLibs, factory );
		wxString pluginName = getString( option::lg::Plugin, false );

		if ( pluginName.empty() )
		{
			pluginName = option::selectPlugin( factory );
		}

		if ( !pluginName.empty() )
		{
			plugin = factory.create( makeStdString( pluginName ) );
			plugin->config.pluginConfig->fillParser( parser );

			if ( ( parser.Parse( false ) != 0 ) )
			{
				parser.Usage();
				throw false;
			}
		}
	}

	Options::~Options()
	{
		delete configFile;
	}

	bool Options::has( wxString const & option )const
	{
		return parser.Found( option );
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
		else if ( configFile->HasEntry( option ) )
		{
			configFile->Read( option, &value );
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

	wxString Options::getString( wxString const & option
		, bool mandatory
		, wxString const & defaultValue )const
	{
		wxString result;

		if ( parser.Found( option, &result ) )
		{
		}
		else if ( configFile->HasEntry( option ) )
		{
			configFile->Read( option, &result );
		}
		else if ( mandatory )
		{
			throw false;
		}
		else
		{
			result = defaultValue;
		}

		if ( mandatory )
		{
			throw false;
		}

		return result;
	}

	void Options::write( Config const & config )
	{
		configFile->Write( option::lg::Test, config.test.GetFullPath() );
		configFile->Write( option::lg::Work, config.work.GetFullPath() );
		configFile->Write( option::lg::Database, config.database.GetFullPath() );
		configFile->Write( option::lg::FrameCount, config.maxFrameCount );
		configFile->Write( option::lg::Plugin, config.plugin );
		config.pluginConfig->write( *configFile );
	}

	wxString Options::findConfigFile( wxCmdLineParser const & parser )
	{
		wxString cfg;
		parser.Found( wxT( 'c' ), &cfg );
		return cfg;
	}

	//*********************************************************************************************
}
