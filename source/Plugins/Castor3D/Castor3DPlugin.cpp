#include "Castor3DPlugin.hpp"

#include "Editor/SceneFileDialog.hpp"

#include <Aria/Options.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/stdpaths.h>
#pragma warning( pop )

namespace aria::c3d
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
#if Aria_UseAsync
		static auto constexpr ExecMode = wxEXEC_ASYNC;
#else
		static auto constexpr ExecMode = wxEXEC_SYNC;
#endif

		namespace lg
		{
			static const wxString Launcher{ wxT( "launcher" ) };
			static const wxString Viewer{ wxT( "viewer" ) };
			static const wxString Engine{ wxT( "engine" ) };
		}

		namespace st
		{
			static const wxString Launcher{ wxT( "l" ) };
			static const wxString Viewer{ wxT( "v" ) };
			static const wxString Engine{ wxT( "e" ) };
		}

		namespace dc
		{
			static const wxString Launcher{ _( "Path to test launcher application." ) };
			static const wxString Viewer{ _( "Path to text viewer application." ) };
			static const wxString Engine{ _( "Specifies the path to the 3D engine's shared library." ) };
		}
	}

	//*********************************************************************************************

	void C3dPluginConfig::fillParser( wxCmdLineParser & parser )const
	{
		parser.AddOption( option::st::Launcher
			, option::lg::Launcher
			, option::dc::Launcher
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( option::st::Viewer
			, option::lg::Viewer
			, option::dc::Viewer
			, wxCMD_LINE_VAL_STRING, 0 );
		parser.AddOption( option::st::Engine
			, option::lg::Engine
			, option::dc::Engine
			, wxCMD_LINE_VAL_STRING, 0 );
	}

	void C3dPluginConfig::fillDialog( wxDialog & dialog
		, wxSizer & parentSizer )
	{
		addFileField( dialog
			, parentSizer
			, wxT( "Test launcher executable" )
			, wxT( "The executable that will be used to run a single test." )
			, launcher );
		addFileField( dialog
			, parentSizer
			, wxT( "Test viewer executable" )
			, wxT( "The executable that will be used to view a single test." )
			, viewer );
		addFileField( dialog
			, parentSizer
			, wxT( "Engine main file" )
			, wxT( "The engine file that will be used to tell if a test is out of date, engine wise." )
			, engine );
	}

	void C3dPluginConfig::setup( Options const & options )
	{
		auto executableDir = wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath();
		launcher = options.getFileName( option::lg::Launcher
			, false
			, executableDir / ( wxT( "CastorTestLauncher" ) + BinExt ) );
		viewer = options.getFileName( option::lg::Viewer
			, false
			, executableDir / ( wxT( "CastorViewer" ) + BinExt ) );
		engine = options.getFileName( option::lg::Engine
			, false
			, executableDir / ( DynlibPre + wxT( "Castor3D" ) + DynlibExt ) );
	}

	void C3dPluginConfig::init()
	{
		wxLogMessage( "Engine: " + engine.GetFullPath() );
		wxLogMessage( "Launcher: " + launcher.GetFullPath() );
		wxLogMessage( "Viewer: " + viewer.GetFullPath() );

		if ( wxFileExists( engine.GetFullPath() ) )
		{
			updateEngineRefDate();
		}
	}

	void C3dPluginConfig::write( wxFileConfig & configFile )const
	{
		configFile.Write( option::lg::Launcher
			, launcher.GetFullPath() );
		configFile.Write( option::lg::Viewer
			, viewer.GetFullPath() );
		configFile.Write( option::lg::Engine
			, engine.GetFullPath() );
	}

	db::DateTime const & C3dPluginConfig::getEngineRefDate()const
	{
		return engineRefDate;
	}

	void C3dPluginConfig::updateEngineRefDate()
	{
		engineRefDate = getFileDate( engine );
		assert( engineRefDate.IsValid() );
	}

	//*********************************************************************************************

	std::string const C3dPlugin::Name = "Castor3D";

	C3dPlugin::C3dPlugin()
		: aria::Plugin{ aria::makeWxString( Name )
			, std::make_unique< C3dPluginConfig >() }
	{
	}

	aria::PluginPtr C3dPlugin::create()
	{
		return std::make_unique< C3dPlugin >();
	}

	std::unique_ptr< aria::PluginConfig > C3dPlugin::createConfig()const
	{
		return std::make_unique< C3dPluginConfig >();
	}

	long C3dPlugin::viewTest( wxProcess * process
		, wxString const & fileName
		, wxString const & rendererName
		, bool async )const
	{
		auto & pluginConfig = static_cast< C3dPluginConfig & >( *m_pluginConfig );
		wxString command = pluginConfig.viewer.GetFullPath();
		command << " " << fileName
			<< " -l 1"
			<< " -a";

		if ( !async )
		{
			command << " -s";
			command << " -f 25";
		}

		if ( !rendererName.empty() )
		{
			command << " -" << rendererName;
		}

		return wxExecute( command
			, wxEXEC_ASYNC
			, process );
	}

	long C3dPlugin::runTest( wxProcess * process
		, wxString const & fileName
		, wxString const & rendererName )const
	{
		auto & pluginConfig = static_cast< C3dPluginConfig & >( *m_pluginConfig );
		wxString command = pluginConfig.launcher.GetFullPath();
		command << " " << fileName;
		command << " -f " << 100u;
		command << " -d";
		command << " -" << rendererName;

		return wxExecute( command
			, option::ExecMode
			, process );
	}

	void C3dPlugin::viewSceneFile( wxWindow * parent
		, wxFileName const & filePath )const
	{
		auto editor = new SceneFileDialog{ *this
			, filePath.GetFullPath()
			, filePath.GetName()
			, parent };
		editor->Show();
	}

	wxFileName C3dPlugin::getOldSceneName( Test const & test )
	{
		return wxFileName{ test.name + ".cscn" };
	}

	wxFileName C3dPlugin::getSceneName( Test const & test )const
	{
		return wxFileName{ toTestPrefix( test.id ) + ".cscn" };
	}

	bool C3dPlugin::isOutOfEngineDate( TestRun const & test )const
	{
		auto & pluginConfig = static_cast< C3dPluginConfig & >( *m_pluginConfig );
		return ( !test.engineDate.IsValid() )
			|| test.engineDate.IsEarlierThan( getFileDate( pluginConfig.engine ) );
	}

	//*********************************************************************************************
}

#if !defined( _WIN32 )
#	define AriaC3D_API
#else
#	ifdef Castor3D_EXPORTS
#		define AriaC3D_API __declspec( dllexport )
#	else
#		define AriaC3D_API __declspec( dllimport )
#	endif
#endif

extern "C"
{
	AriaC3D_API void onLoad( aria::PluginFactory * factory )
	{
		factory->registerPlugin( aria::c3d::C3dPlugin::Name
			, aria::c3d::C3dPlugin::create );
	}

	AriaC3D_API void onUnload( aria::PluginFactory * factory )
	{
		factory->unregisterPlugin( aria::c3d::C3dPlugin::Name );
	}
}

//*************************************************************************************************