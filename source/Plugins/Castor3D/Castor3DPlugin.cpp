#include "Castor3DPlugin.hpp"

#include "Editor/SceneFileDialog.hpp"

#include <AriaLib/FileSystem/FileSystem.hpp>
#include <AriaLib/Options.hpp>

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

	wxDateTime const & C3dPluginConfig::getEngineRefDate()const
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

	void C3dPlugin::createTest( Test const & test
		, FileSystem & fileSystem )const
	{
		fileSystem.addFile( test.name
			, getTestFileName( test ) );
		fileSystem.commit( "Created test [" + test.name + "]" );
	}

	void C3dPlugin::deleteTest( Test const & test
		, FileSystem & fileSystem )const
	{
		fileSystem.removeFile( test.name, getTestFileName( test ), true );
		fileSystem.commit( "Deleted test [" + test.name + "]" );
	}

	void C3dPlugin::changeTestCategory( Test const & test
		, Category oldCategory
		, Category newCategory
		, FileSystem & fileSystem )
	{
		auto sceneName = getTestName( test );
		fileSystem.moveFile( test.name
			, config.test / oldCategory->name
			, config.test / newCategory->name
			, sceneName
			, sceneName
			, true );
		auto referenceName = getReferenceName( test );
		fileSystem.moveFile( test.name
			, config.test / oldCategory->name
			, config.test / newCategory->name
			, referenceName
			, referenceName
			, true );
		fileSystem.commit( wxString{} << "Changed test [" << test.name << "] category to [" << newCategory->name << "]." );
	}

	long C3dPlugin::viewTest( wxProcess * process
		, Test const & test
		, wxString const & rendererName
		, bool async )const
	{
		auto filePath = getTestFileName( test );
		auto & pluginConfig = static_cast< C3dPluginConfig & >( *m_pluginConfig );
		wxString command = pluginConfig.viewer.GetFullPath();
		command << " " << filePath
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
		, Test const & test
		, wxString const & rendererName )const
	{
		auto filePath = getTestFileName( test );
		auto & pluginConfig = static_cast< C3dPluginConfig & >( *m_pluginConfig );
		wxString command = pluginConfig.launcher.GetFullPath();
		command << " " << filePath;
		command << " -f " << 100u;
		command << " -d";
		command << " -" << rendererName;

		return wxExecute( command
			, option::ExecMode
			, process );
	}

	void C3dPlugin::editTest( wxWindow * parent
		, Test const & test )const
	{
		auto filePath = getTestFileName( test );

		if ( !filePath.Exists() )
		{
			if ( auto file = fopen( makeStdString( filePath.GetFullPath() ).c_str(), "w" ) )
			{
				fclose( file );
			}
		}

		auto editor = new SceneFileDialog{ *this
			, test
			, filePath.GetFullPath()
			, filePath.GetName() + " - " + test.name
			, parent };
		editor->Show();
	}

	wxDateTime C3dPlugin::getTestDate( Test const & test )const
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	wxFileName C3dPlugin::getTestFileName( Test const & test )const
	{
		return config.test / getSceneFile( test );
	}

	wxFileName C3dPlugin::getTestName( Test const & test )const
	{
		return wxFileName{ toTestPrefix( test.id ) + ".cscn" };
	}

	bool C3dPlugin::isOutOfEngineDate( TestRun const & test )const
	{
		auto & pluginConfig = static_cast< C3dPluginConfig & >( *m_pluginConfig );
		return ( !test.engineDate.IsValid() )
			|| test.engineDate.IsEarlierThan( getFileDate( pluginConfig.engine ) );
	}

	bool C3dPlugin::isOutOfTestDate( TestRun const & test )const
	{
		return ( !test.testDate.IsValid() )
			|| test.testDate.IsEarlierThan( getTestDate( *test.test ) );
	}

	bool C3dPlugin::isSceneFile( wxString const & test )const
	{
		return getExtension( test ) == wxT( "cscn" );
	}

	wxFileName C3dPlugin::getSceneFile( Test const & test )const
	{
		return test.category->name.empty()
			? wxFileName{ getTestName( test ) }
			: wxFileName{ test.category->name } / getTestName( test );
	}

	wxFileName C3dPlugin::getSceneFile( TestRun const & test )const
	{
		return getSceneFile( *test.test );
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

#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma clang diagnostic ignored "-Wmissing-prototypes"

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
