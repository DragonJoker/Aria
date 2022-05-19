/*
See LICENSE file in root folder
*/
#ifndef ___CTPC3D_Plugin_HPP___
#define ___CTPC3D_Plugin_HPP___

#include <Aria/Plugin.hpp>

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

		aria::db::DateTime const & getEngineRefDate()const override;
		void updateEngineRefDate()override;

		wxFileName launcher;
		wxFileName viewer;
		wxFileName engine;
		aria::db::DateTime engineRefDate;
	};

	class C3dPlugin
		: public aria::Plugin
	{
	public:
		C3dPlugin();

		static aria::PluginPtr create();

		std::unique_ptr< aria::PluginConfig > createConfig()const override;
		long viewTest( wxProcess * process
			, wxString const & fileName
			, wxString const & rendererName
			, bool async )const override;
		long runTest( wxProcess * process
			, wxString const & fileName
			, wxString const & rendererName )const override;
		void viewSceneFile( wxWindow * parent
			, wxFileName const & filePath )const override;
		wxFileName getOldSceneName( Test const & test ) override;
		wxFileName getSceneName( Test const & test )const override;
		bool isOutOfEngineDate( TestRun const & test )const override;

		static std::string const Name;
	};
}

#endif
