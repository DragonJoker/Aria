#include "Aria.hpp"
#include "ConfigurationDialog.hpp"
#include "MainFrame.hpp"

#include <wx/clipbrd.h>
#include <wx/cmdline.h>
#include <wx/filefn.h> 
#include <wx/fileconf.h>
#include <wx/image.h>
#include <wx/stdpaths.h>

#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif

wxIMPLEMENT_APP( aria::Aria );

namespace aria
{
	namespace
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
			static const wxString Database{ wxT( "database" ) };
			static const wxString Diff{ wxT( "diff" ) };
			static const wxString Test{ wxT( "test" ) };
			static const wxString Launcher{ wxT( "launcher" ) };
			static const wxString Viewer{ wxT( "viewer" ) };
			static const wxString Work{ wxT( "work" ) };
			static const wxString Engine{ wxT( "engine" ) };
		}

		struct Options
		{
			Options( Options const & ) = delete;
			Options & operator=( Options const & ) = delete;
			Options( Options && ) = default;
			Options & operator=( Options && ) = default;

			Options( int argc, wxCmdLineArgsArray const & argv )
				: parser{ argc, argv }
			{
				parser.AddSwitch( wxT( "h" ), wxT( "help" ), _( "Displays this help." ) );
				parser.AddSwitch( wxT( "f" ), wxT( "force" ), _( "Force database initialisation." ) );
				parser.AddOption( wxT( "c" ), wxT( "config" ), _( "Specifies the tests config file." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "d" ), option::Database, _( "Specifies the database file." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "i" ), option::Diff, _( "Path to DiffImage." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "t" ), option::Test, _( "Specifies the tests directory." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "l" ), option::Launcher, _( "Path to CastorTestLauncher." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "v" ), option::Viewer, _( "Path to CastorViewer." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "w" ), option::Work, _( "Specifies the working directory." ), wxCMD_LINE_VAL_STRING, 0 );
				parser.AddOption( wxT( "a" ), option::Engine, _( "Specifies the path to the 3D engine's shared library." ), wxCMD_LINE_VAL_STRING, 0 );

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
				configFile->Write( option::Test, config.test.GetFullPath() );
				configFile->Write( option::Work, config.work.GetFullPath() );
				configFile->Write( option::Database, config.database.GetFullPath() );
				configFile->Write( option::Launcher, config.launcher.GetFullPath() );
				configFile->Write( option::Viewer, config.viewer.GetFullPath() );
				configFile->Write( option::Engine, config.engine.GetFullPath() );
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
		wxAppConsole::SetAppDisplayName( wxT( "Castor3D Tests Monitor" ) );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		try
		{
			auto executableDir = wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath();
			Options options{ wxApp::argc, wxApp::argv };
			config.test = options.get( option::Test, true );
			config.work = options.get( option::Work, false, config.test );
			config.database = options.get( option::Database, false, config.work / wxT( "db.sqlite" ) );
			config.launcher = options.get( option::Launcher, false, executableDir / ( wxT( "CastorTestLauncher" ) + BinExt ) );
			config.viewer = options.get( option::Viewer, false, executableDir / ( wxT( "CastorViewer" ) + BinExt ) );
			config.engine = options.get( option::Engine, false, executableDir / ( DynlibPre + wxT( "Castor3D" ) + DynlibExt ) );
			config.initFromFolder = options.has( wxT( 'f' ) );
			options.write( config );
		}
		catch ( bool )
		{
			ConfigurationDialog dialog{ nullptr, config };
			dialog.ShowModal();
		}

		return true;
	}

	bool Aria::OnInit()
	{
		wxFileName executableDir{ wxStandardPaths::Get().GetExecutablePath() };
		m_outStream = std::ofstream{ ( executableDir.GetPath() / "Result.log" ).GetFullPath().ToStdString() };
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
