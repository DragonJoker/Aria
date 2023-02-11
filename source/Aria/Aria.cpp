#include "Aria.hpp"
#include "ConfigurationDialog.hpp"
#include "MainFrame.hpp"

#include <AriaLib/Options.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/clipbrd.h>
#include <wx/filefn.h> 
#include <wx/image.h>
#include <wx/stdpaths.h>

#if defined( __WXGTK__ )
#	include <X11/Xlib.h>
#endif
#include <AriaLib/EndExternHeaderGuard.hpp>

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

	OptionsPtr Aria::doParseCommandLine()
	{
		wxAppConsole::SetAppName( wxT( "aria" ) );
		wxAppConsole::SetVendorName( wxT( "dragonjoker" ) );

#if wxCHECK_VERSION( 2, 9, 0 )
		wxAppConsole::SetAppDisplayName( wxString{ wxT( "Aria Tests Monitor v" ) } << Aria_VERSION_MAJOR << "." << Aria_VERSION_MINOR << "." << Aria_VERSION_BUILD );
		wxAppConsole::SetVendorDisplayName( wxT( "DragonJoker" ) );
#endif

		try
		{
			return std::make_unique< Options >( m_factory
				, m_pluginsLibs
				, wxApp::argc
				, wxApp::argv );
		}
		catch ( bool )
		{
			return nullptr;
		}
	}

	bool Aria::OnInit()
	{
		wxConvCurrent = &wxConvUTF8;
		wxFileName executableDir{ wxStandardPaths::Get().GetExecutablePath() };
		m_outStream = std::ofstream{ makeStdString( ( executableDir.GetPath() / "Result.log" ).GetFullPath() ) };
		m_logStream = std::make_unique< wxLogStream >( &m_outStream );
		wxLog::SetActiveTarget( m_logStream.get() );
		bool result{};

		if ( auto options = doParseCommandLine() )
		{
			options->write();
			wxInitAllImageHandlers();

			try
			{
				MainFrame * mainFrame{ new MainFrame{ std::move( options ) } };
				SetTopWindow( mainFrame );
				mainFrame->Show();
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
			wxLog::SetActiveTarget( nullptr );
			m_pluginsLibs.clear();
		}

		return result;
	}

	int Aria::OnExit()
	{
		wxClipboard::Get()->Flush();
		wxLogMessage( wxT( "Stop" ) );
		wxImage::CleanUpHandlers();
		wxLog::SetActiveTarget( nullptr );
		m_pluginsLibs.clear();
		return 0;
	}
}
