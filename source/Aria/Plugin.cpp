#include "Plugin.hpp"

#include "ConfigurationDialog.hpp"
#include "Editor/SceneFileDialog.hpp"
#include "Database/DatabaseTest.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/stdpaths.h>
#pragma warning( pop )

namespace aria
{
	namespace option
	{
#if Aria_UseAsync
		static auto constexpr ExecMode = wxEXEC_ASYNC;
#else
		static auto constexpr ExecMode = wxEXEC_SYNC;
#endif

#if defined( _WIN32 )
		static wxString const BinExt = wxT( ".exe" );
		static wxString const DynlibExt = wxT( ".dll" );
		static wxString const DynlibPre;
#else
		static wxString const BinExt;
		static wxString const DynlibExt = wxT( ".so" );
		static wxString const DynlibPre = wxT( "lib" );
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

	void PluginConfig::fillParser( wxCmdLineParser & parser )const
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

	void PluginConfig::fillDialog( wxDialog & dialog
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

	void PluginConfig::setup( Options const & options )
	{
		auto executableDir = wxFileName{ wxStandardPaths::Get().GetExecutablePath() }.GetPath();
		launcher = options.get( option::lg::Launcher
			, false
			, executableDir / ( wxT( "CastorTestLauncher" ) + option::BinExt ) );
		viewer = options.get( option::lg::Viewer
			, false
			, executableDir / ( wxT( "CastorViewer" ) + option::BinExt ) );
		engine = options.get( option::lg::Engine
			, false
			, executableDir / ( option::DynlibPre + wxT( "Castor3D" ) + option::DynlibExt ) );
	}

	void PluginConfig::init()
	{
		wxLogMessage( "Engine: " + engine.GetFullPath() );
		wxLogMessage( "Launcher: " + launcher.GetFullPath() );
		wxLogMessage( "Viewer: " + viewer.GetFullPath() );

		if ( wxFileExists( engine.GetFullPath() ) )
		{
			updateEngineRefDate();
		}
	}

	void PluginConfig::write( wxFileConfig & configFile )const
	{
		configFile.Write( option::lg::Launcher
			, launcher.GetFullPath() );
		configFile.Write( option::lg::Viewer
			, viewer.GetFullPath() );
		configFile.Write( option::lg::Engine
			, engine.GetFullPath() );
	}

	db::DateTime const & PluginConfig::getEngineRefDate()const
	{
		return engineRefDate;
	}

	void PluginConfig::updateEngineRefDate()
	{
		engineRefDate = getFileDate( engine );
		assert( engineRefDate.IsValid() );
	}

	//*********************************************************************************************

	Plugin::Plugin()
		: m_pluginConfig{ std::make_unique< PluginConfig >() }
		, config{ *m_pluginConfig }
	{
	}

	void Plugin::initConfig()const
	{
		m_pluginConfig->init();
	}

	void Plugin::updateConfig( std::unique_ptr< PluginConfig > pluginConfig )
	{
		m_pluginConfig = std::move( pluginConfig );
		config.pluginConfig = m_pluginConfig.get();
	}

	std::unique_ptr< PluginConfig > Plugin::createConfig()const
	{
		return std::make_unique< PluginConfig >();
	}

	long Plugin::viewTest( wxProcess * process
		, wxString const & fileName
		, wxString const & rendererName
		, bool async )const
	{
		wxString command = m_pluginConfig->viewer.GetFullPath();
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

	long Plugin::runTest( wxProcess * process
		, wxString const & fileName
		, wxString const & rendererName )const
	{
		wxString command = m_pluginConfig->launcher.GetFullPath();
		command << " " << fileName;
		command << " -f " << 100u;
		command << " -d";
		command << " -" << rendererName;

		return wxExecute( command
			, option::ExecMode
			, process );
	}

	void Plugin::viewSceneFile( wxWindow * parent
		, wxFileName const & filePath )const
	{
		auto editor = new SceneFileDialog{ *this
			, filePath.GetFullPath()
			, filePath.GetName()
			, parent };
		editor->Show();
	}

	wxFileName Plugin::getOldSceneName( Test const & test )
	{
		return wxFileName{ test.name + ".cscn" };
	}

	db::DateTime Plugin::getSceneDate( Test const & test )const
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	db::DateTime Plugin::getSceneDate( TestRun const & test )const
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	wxFileName Plugin::getSceneFile( Test const & test )const
	{
		return wxFileName{ test.category->name } / getSceneName( test );
	}

	wxFileName Plugin::getSceneFile( TestRun const & test )const
	{
		return getSceneFile( *test.test );
	}

	wxFileName Plugin::getSceneName( Test const & test )const
	{
		return wxFileName{ toTestPrefix( test.id ) + ".cscn" };
	}

	wxFileName Plugin::getSceneName( TestRun const & test )const
	{
		return getSceneName( *test.test );
	}

	wxFileName Plugin::getTestFileName( wxFileName const & folder
		, Test const & test )const
	{
		return folder / getSceneFile( test );
	}

	wxFileName Plugin::getTestFileName( wxFileName const & folder
		, DatabaseTest const & test )const
	{
		return getTestFileName( folder, *test->test );
	}

	bool Plugin::isOutOfSceneDate( TestRun const & test )const
	{
		return ( !test.sceneDate.IsValid() )
			|| test.sceneDate.IsEarlierThan( getSceneDate( test ) );
	}

	bool Plugin::isOutOfEngineDate( TestRun const & test )const
	{
		return ( !test.engineDate.IsValid() )
			|| test.engineDate.IsEarlierThan( getFileDate( m_pluginConfig->engine ) );
	}

	bool Plugin::isOutOfDate( TestRun const & test )const
	{
		return isOutOfSceneDate( test )
			|| isOutOfEngineDate( test );
	}

	db::DateTime const & Plugin::getEngineRefDate()const
	{
		return m_pluginConfig->getEngineRefDate();
	}

	void Plugin::updateEngineRefDate()const
	{
		m_pluginConfig->updateEngineRefDate();
	}

	//*********************************************************************************************
}
