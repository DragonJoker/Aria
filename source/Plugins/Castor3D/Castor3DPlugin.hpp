/*
See LICENSE file in root folder
*/
#ifndef ___CTPC3D_Plugin_HPP___
#define ___CTPC3D_Plugin_HPP___

#include <AriaLib/Plugin.hpp>

namespace aria::c3d
{
	class C3dPluginConfig
		: public aria::PluginConfig
	{
	public:
		void fillParser( wxCmdLineParser & parser )const override;
		void fillDialog( wxDialog & dialog
			, wxSizer & parentSizer )override;
		void setup( Options const & options )override;
		void init()override;
		void write( wxFileConfig & configFile )const override;

		wxDateTime const & getEngineRefDate()const override;
		void updateEngineRefDate()override;

		wxFileName launcher;
		wxFileName viewer;
		wxFileName engine;
		wxDateTime engineRefDate;
	};

	class C3dPlugin
		: public aria::Plugin
	{
	public:
		C3dPlugin();

		static aria::PluginPtr create();

		std::unique_ptr< aria::PluginConfig > createConfig()const override;
		void createTest( Test const & test
			, FileSystem & fileSystem )const override;
		void deleteTest( Test const & test
			, FileSystem & fileSystem )const override;
		void changeTestCategory( Test const & test
			, Category oldCategory
			, Category newCategory
			, FileSystem & fileSystem )override;
		long viewTest( wxProcess * process
			, Test const & test
			, wxString const & rendererName
			, bool async )const override;
		long runTest( wxProcess * process
			, Test const & test
			, wxString const & rendererName )const override;
		void editTest( wxWindow * parent
			, Test const & test )const override;
		wxDateTime getTestDate( Test const & test )const override;
		wxFileName getTestFileName( Test const & test )const override;
		wxFileName getTestName( Test const & test )const override;
		bool isOutOfEngineDate( TestRun const & test )const override;
		bool isOutOfTestDate( TestRun const & test )const override;
		bool isSceneFile( wxString const & test )const override;

		wxFileName getSceneFile( Test const & test )const;
		wxFileName getSceneFile( TestRun const & test )const;

		static std::string const Name;
	};
}

#endif
