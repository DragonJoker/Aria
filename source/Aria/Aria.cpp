#include "Aria.hpp"
#include "ConfigurationDialog.hpp"
#include "MainFrame.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/clipbrd.h>
#include <wx/cmdline.h>
#include <wx/filefn.h> 
#include <wx/fileconf.h>
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
#if defined( _WIN32 )
		static wxString const BinExt = wxT( ".exe" );
		static wxString const DynlibExt = wxT( ".dll" );
		static wxString const DynlibPre;
#else
		static wxString const BinExt;
		static wxString const DynlibExt = wxT( ".so" );
		static wxString const DynlibPre = wxT( "lib" );
#endif

		namespace option
		{
			namespace lg
			{
				static const wxString Help{ wxT( "help" ) };
				static const wxString Force{ wxT( "force" ) };
				static const wxString ConfigFile{ wxT( "config" ) };
				static const wxString Database{ wxT( "database" ) };
				static const wxString Test{ wxT( "test" ) };
				static const wxString Launcher{ wxT( "launcher" ) };
				static const wxString Viewer{ wxT( "viewer" ) };
				static const wxString Work{ wxT( "work" ) };
				static const wxString Engine{ wxT( "engine" ) };
				static const wxString FrameCount{ wxT( "frames" ) };
			}

			namespace st
			{
				static const wxString Help{ wxT( "h" ) };
				static const wxString Force{ wxT( "f" ) };
				static const wxString ConfigFile{ wxT( "c" ) };
				static const wxString Database{ wxT( "d" ) };
				static const wxString Test{ wxT( "t" ) };
				static const wxString Launcher{ wxT( "l" ) };
				static const wxString Viewer{ wxT( "v" ) };
				static const wxString Work{ wxT( "w" ) };
				static const wxString Engine{ wxT( "e" ) };
				static const wxString FrameCount{ wxT( "a" ) };
			}

			namespace df
			{
				static const uint32_t FrameCount{ 10u };
			}
		}

		struct Options
		{
			Options( int argc, wxCmdLineArgsArray const & argv )
				: parser{ argc, argv }
			{
				static const wxString Help{ _( "Displays this help." ) };
				static const wxString Force{ _( "Force database initialisation." ) };
				static const wxString ConfigFile{ _( "Specifies the tests config file." ) };
				static const wxString Database{ _( "Specifies the database file." ) };
				static const wxString Test{ _( "Specifies the tests directory." ) };
				static const wxString Launcher{ _( "Path to test launcher application." ) };
				static const wxString Viewer{ _( "Path to text viewer application." ) };
				static const wxString Work{ _( "Specifies the working directory." ) };
				static const wxString Engine{ _( "Specifies the path to the 3D engine's shared library." ) };
				static const wxString FrameCount{ _( "The number of frames to let run before capture." ) };

				parser.AddSwitch( option::st::Help, option::lg::Help, Help );
				parser.AddSwitch( option::st::Force, option::lg::Force, Force );
				parser.AddOption( option::st::ConfigFile, option::lg::ConfigFile, ConfigFile, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::Database, option::lg::Database, Database, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::Test, option::lg::Test, Test, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::Launcher, option::lg::Launcher, Launcher, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::Viewer, option::lg::Viewer, Viewer, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::Work, option::lg::Work, Work, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::Engine, option::lg::Engine, Engine, wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( option::st::FrameCount, option::lg::FrameCount, FrameCount, wxCMD_LINE_VAL_NUMBER );

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

			~Options()
			{
				delete configFile;
			}

			bool has( wxString const & option )
			{
				return parser.Found( option );
			}

			template< typename ValueT >
			ValueT getLong( wxString const & option
				, bool mandatory
				, ValueT defaultValue )
			{
				long value;
				ValueT result;

				if ( parser.Found( option, &value ) )
				{
					result = ValueT( value );
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

			wxFileName get( wxString const & option
				, bool mandatory
				, wxFileName const & defaultValue = wxFileName{} )
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

			void write( Config const & config )
			{
				configFile->Write( option::lg::Test, config.test.GetFullPath() );
				configFile->Write( option::lg::Work, config.work.GetFullPath() );
				configFile->Write( option::lg::Database, config.database.GetFullPath() );
				configFile->Write( option::lg::Launcher, config.launcher.GetFullPath() );
				configFile->Write( option::lg::Viewer, config.viewer.GetFullPath() );
				configFile->Write( option::lg::Engine, config.engine.GetFullPath() );
				configFile->Write( option::lg::FrameCount, config.maxFrameCount );
			}

			static wxString findConfigFile( wxCmdLineParser const & parser )
			{
				wxString cfg;
				parser.Found( wxT( 'c' ), &cfg );
				return cfg;
			}

		private:
			wxCmdLineParser parser;
			wxFileConfig * configFile{ nullptr };
		};
	}

	Aria::Aria()
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	bool Aria::doParseCommandLine( Config & config )
	{
		wxAppConsole::SetAppName( wxT( "aria" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )
		wxAppConsole::SetAppDisplayName( wxString{ wxT( "Aria Tests Monitor v" ) } << Aria_VERSION_MAJOR << "." << Aria_VERSION_MINOR << "." << Aria_VERSION_BUILD );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		app::Options options{ wxApp::argc, wxApp::argv };

		try
		{
			auto executableDir = wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath();
			config.test = options.get( app::option::lg::Test, true );
			config.work = options.get( app::option::lg::Work, false, config.test );
			config.maxFrameCount = options.getLong( app::option::lg::FrameCount, false, app::option::df::FrameCount );
			config.database = options.get( app::option::lg::Database, false, config.work / wxT( "db.sqlite" ) );
			config.launcher = options.get( app::option::lg::Launcher, false, executableDir / ( wxT( "CastorTestLauncher" ) + app::BinExt ) );
			config.viewer = options.get( app::option::lg::Viewer, false, executableDir / ( wxT( "CastorViewer" ) + app::BinExt ) );
			config.engine = options.get( app::option::lg::Engine, false, executableDir / ( app::DynlibPre + wxT( "Castor3D" ) + app::DynlibExt ) );
			config.initFromFolder = options.has( wxT( 'f' ) );
			options.write( config );
		}
		catch ( bool )
		{
			ConfigurationDialog dialog{ nullptr, config };

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
		Config config;
		auto result = doParseCommandLine( config );

		if ( result )
		{
			result = false;
			wxInitAllImageHandlers();
			wxLogMessage( "Test folder: " + config.test.GetFullPath() );
			wxLogMessage( "Work folder: " + config.work.GetFullPath() );
			wxLogMessage( "Database: " + config.database.GetFullPath() );
			wxLogMessage( "Engine: " + config.engine.GetFullPath() );
			wxLogMessage( "Launcher: " + config.launcher.GetFullPath() );
			wxLogMessage( "Viewer: " + config.viewer.GetFullPath() );

			if ( wxFileExists( config.engine.GetFullPath() ) )
			{
				updateEngineRefDate( config );
			}

			try
			{
				MainFrame * mainFrame{ new MainFrame{ std::move( config ) } };
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
