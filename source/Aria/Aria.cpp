#include "Aria.hpp"
#include "ConfigurationDialog.hpp"
#include "MainFrame.hpp"
#include "Options.hpp"

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
	//*********************************************************************************************

	Aria::Aria()
	{
#if defined( __WXGTK__ )
		XInitThreads();
#endif
	}

	PluginPtr Aria::doParseCommandLine()
	{
		wxAppConsole::SetAppName( wxT( "aria" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )
		wxAppConsole::SetAppDisplayName( wxString{ wxT( "Aria Tests Monitor v" ) } << Aria_VERSION_MAJOR << "." << Aria_VERSION_MINOR << "." << Aria_VERSION_BUILD );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		Options options{ m_factory
			, m_pluginsLibs
			, wxApp::argc
			, wxApp::argv };
		auto result = options.getPlugin();

		if ( result )
		{
			auto & config = result->config;

			try
			{
				config.test = options.getFileName( option::lg::Test, true );
				config.work = options.getFileName( option::lg::Work, false, config.test );
				config.maxFrameCount = options.getLong( option::lg::FrameCount, false, option::df::FrameCount );
				config.database = options.getFileName( option::lg::Database, false, config.work / wxT( "db.sqlite" ) );
				config.plugin = result->getName();
				config.initFromFolder = options.has( wxT( 'f' ) );
				config.pluginConfig->setup( options );
				options.write( config );
			}
			catch ( bool )
			{
				ConfigurationDialog dialog{ nullptr, *result };

				if ( dialog.ShowModal() == wxID_OK )
				{
					options.write( config );
				}
			}
		}

		return result;
	}

	bool Aria::OnInit()
	{
		wxConvCurrent = &wxConvUTF8;
		wxFileName executableDir{ wxStandardPaths::Get().GetExecutablePath() };
		m_outStream = std::ofstream{ makeStdString( ( executableDir.GetPath() / "Result.log" ).GetFullPath() ) };
		m_logStream = std::make_unique< wxLogStream >( &m_outStream );
		wxLog::SetActiveTarget( m_logStream.get() );
		auto plugin = doParseCommandLine();
		auto result = plugin != nullptr;

		if ( result )
		{
			result = false;
			wxInitAllImageHandlers();
			auto & config = plugin->config;
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
		m_pluginsLibs.clear();
		return 0;
	}
}
