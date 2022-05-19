#include "Aria.hpp"
#include "ConfigurationDialog.hpp"
#include "MainFrame.hpp"
#include "Plugin.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/clipbrd.h>
#include <wx/filefn.h> 
#include <wx/image.h>
#include <wx/stdpaths.h>
#pragma warning( pop )

#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif

wxIMPLEMENT_APP( aria::Aria );

namespace aria
{
	namespace app
	{
		namespace option
		{
			namespace lg
			{
				static const wxString Help{ wxT( "help" ) };
				static const wxString Force{ wxT( "force" ) };
				static const wxString ConfigFile{ wxT( "config" ) };
				static const wxString Database{ wxT( "database" ) };
				static const wxString Test{ wxT( "test" ) };
				static const wxString Work{ wxT( "work" ) };
				static const wxString FrameCount{ wxT( "frames" ) };
			}

			namespace st
			{
				static const wxString Help{ wxT( "h" ) };
				static const wxString Force{ wxT( "f" ) };
				static const wxString ConfigFile{ wxT( "c" ) };
				static const wxString Database{ wxT( "d" ) };
				static const wxString Test{ wxT( "t" ) };
				static const wxString Work{ wxT( "w" ) };
				static const wxString FrameCount{ wxT( "a" ) };
			}

			namespace df
			{
				static const uint32_t FrameCount{ 10u };
			}
		}
	}

	//*********************************************************************************************

	Options::Options( PluginConfig & pluginConfig
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

		parser.AddSwitch( app::option::st::Help
			, app::option::lg::Help
			, Help );
		parser.AddSwitch( app::option::st::Force
			, app::option::lg::Force
			, Force );
		parser.AddOption( app::option::st::ConfigFile
			, app::option::lg::ConfigFile
			, ConfigFile
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( app::option::st::Database
			, app::option::lg::Database
			, Database
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( app::option::st::Test
			, app::option::lg::Test
			, Test
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( app::option::st::Work
			, app::option::lg::Work
			, Work
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( app::option::st::FrameCount
			, app::option::lg::FrameCount
			, FrameCount
			, wxCMD_LINE_VAL_NUMBER );
		pluginConfig.fillParser( parser );

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
	}

	Options::~Options()
	{
		delete configFile;
	}

	bool Options::has( wxString const & option )const
	{
		return parser.Found( option );
	}

	wxFileName Options::get( wxString const & option
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

	void Options::write( Config const & config )
	{
		configFile->Write( app::option::lg::Test, config.test.GetFullPath() );
		configFile->Write( app::option::lg::Work, config.work.GetFullPath() );
		configFile->Write( app::option::lg::Database, config.database.GetFullPath() );
		configFile->Write( app::option::lg::FrameCount, config.maxFrameCount );
		config.pluginConfig->write( *configFile );
	}

	wxString Options::findConfigFile( wxCmdLineParser const & parser )
	{
		wxString cfg;
		parser.Found( wxT( 'c' ), &cfg );
		return cfg;
	}

	//*********************************************************************************************

	Aria::Aria()
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	bool Aria::doParseCommandLine( Plugin & plugin )
	{
		wxAppConsole::SetAppName( wxT( "aria" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )
		wxAppConsole::SetAppDisplayName( wxString{ wxT( "Aria Tests Monitor v" ) } << Aria_VERSION_MAJOR << "." << Aria_VERSION_MINOR << "." << Aria_VERSION_BUILD );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		auto & config = plugin.config;
		Options options{ *config.pluginConfig
			, wxApp::argc
			, wxApp::argv };

		try
		{
			config.test = options.get( app::option::lg::Test, true );
			config.work = options.get( app::option::lg::Work, false, config.test );
			config.maxFrameCount = options.getLong( app::option::lg::FrameCount, false, app::option::df::FrameCount );
			config.database = options.get( app::option::lg::Database, false, config.work / wxT( "db.sqlite" ) );
			config.initFromFolder = options.has( wxT( 'f' ) );
			config.pluginConfig->setup( options );
			options.write( config );
		}
		catch ( bool )
		{
			ConfigurationDialog dialog{ nullptr, plugin };

			if ( dialog.ShowModal() == wxID_OK )
			{
				options.write( config );
			}
		}

		return true;
	}

	bool Aria::OnInit()
	{
		wxConvCurrent = &wxConvUTF8;
		wxFileName executableDir{ wxStandardPaths::Get().GetExecutablePath() };
		m_outStream = std::ofstream{ makeStdString( ( executableDir.GetPath() / "Result.log" ).GetFullPath() ) };
		m_logStream = std::make_unique< wxLogStream >( &m_outStream );
		wxLog::SetActiveTarget( m_logStream.get() );
		auto plugin = std::make_unique< Plugin >();
		auto & config = plugin->config;
		auto result = doParseCommandLine( *plugin );

		if ( result )
		{
			result = false;
			wxInitAllImageHandlers();
			wxLogMessage( "Test folder: " + config.test.GetFullPath() );
			wxLogMessage( "Work folder: " + config.work.GetFullPath() );
			wxLogMessage( "Database: " + config.database.GetFullPath() );
			plugin->initConfig();

			try
			{
				MainFrame * mainFrame{ new MainFrame{ std::move( plugin ) } };
				SetTopWindow( mainFrame );
				mainFrame->Show();
				mainFrame->initialise();
				result = true;
			}
			catch ( std::exception & exc )
			{
				wxLogError( wxString() << "Initialisation failed : " << exc.what() );
			}
		}

		if ( !result )
		{
			wxLogMessage( wxT( "Stop" ) );
			wxImage::CleanUpHandlers();
		}

		return result;
	}

	int Aria::OnExit()
	{
		wxTheClipboard->Flush();
		wxLogMessage( wxT( "Stop" ) );
		wxImage::CleanUpHandlers();
		wxLog::SetActiveTarget( nullptr );
		return 0;
	}
}
