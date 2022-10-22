/*
See LICENSE file in root folder
*/
#ifndef ___Aria_Options_HPP___
#define ___Aria_Options_HPP___

#include "Plugin.hpp"

namespace aria
{
	namespace option
	{
		namespace lg
		{
			static const wxString Help{ wxT( "help" ) };
			static const wxString Force{ wxT( "force" ) };
			static const wxString ConfigFile{ wxT( "config" ) };
		}

		namespace st
		{
			static const wxString Help{ wxT( "h" ) };
			static const wxString Force{ wxT( "f" ) };
			static const wxString ConfigFile{ wxT( "c" ) };
		}

		namespace df
		{
			static const uint32_t FrameCount{ 10u };
		}

		AriaLib_API wxString selectPlugin( PluginFactory const & factory );
	}

	using PFN_OnLoad = void ( * )( aria::PluginFactory * factory );
	using PFN_OnUnload = void ( * )( aria::PluginFactory * factory );

	struct PluginLib
	{
		AriaLib_API PluginLib( PluginLib const & ) = delete;
		AriaLib_API PluginLib & operator=( PluginLib const & ) = delete;
		AriaLib_API PluginLib( PluginFactory & factory
			, wxDynamicLibrary * plib );
		AriaLib_API PluginLib( PluginLib && rhs );
		AriaLib_API PluginLib & operator=( PluginLib && rhs );
		AriaLib_API ~PluginLib();

	private:
		PluginFactory * m_factory;
		wxDynamicLibrary * m_lib;
		PFN_OnLoad m_onLoad;
		PFN_OnUnload m_onUnload;
	};

	struct TestsOptions
	{
		TestsOptions( TestsOptions const & ) = delete;
		TestsOptions & operator=( TestsOptions const & ) = delete;
		AriaLib_API explicit TestsOptions( PluginPtr plugin
			, wxFileName const & configFileName );
		AriaLib_API TestsOptions( PluginFactory & factory
			, wxFileName const & configFileName );
		AriaLib_API TestsOptions( TestsOptions && rhs );
		AriaLib_API TestsOptions & operator=( TestsOptions && rhs );
		AriaLib_API ~TestsOptions() = default;

		AriaLib_API wxString write();

		AriaLib_API wxString getString( wxString const & option
			, bool mandatory
			, wxString const & defaultValue = wxString{} )const;
		AriaLib_API wxFileName getFileName( wxString const & option
			, bool mandatory
			, wxFileName const & defaultValue = wxFileName{} )const;

		template< typename ValueT >
		ValueT getLong( wxString const & option
			, bool mandatory
			, ValueT defaultValue )const
		{
			long value;
			ValueT result;
			wxString str = getString( option, false );

			if ( str.IsNumber() )
			{
				str.ToLong( &value );
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

		Plugin * getPlugin()
		{
			return plugin.get();
		}

		wxString getFilePath()const
		{
			return configPath.GetFullPath();
		}

	private:
		wxFileName configPath;
		wxFileConfig configFile;
		PluginPtr plugin;
		Plugin * pluginPtr;
	};

	struct Options
	{
		Options( Options const & ) = delete;
		Options & operator=( Options const & ) = delete;
		Options( Options && ) = delete;
		Options & operator=( Options && ) = delete;
		AriaLib_API Options( PluginFactory & factory
			, std::vector< PluginLib > & pluginsLibs
			, int argc
			, wxCmdLineArgsArray const & argv );
		AriaLib_API ~Options() = default;

		AriaLib_API bool has( wxString const & option )const;

		AriaLib_API wxString getString( wxString const & option
			, bool mandatory
			, wxString const & defaultValue = wxString{} )const;
		AriaLib_API wxFileName getFileName( wxString const & option
			, bool mandatory
			, wxFileName const & defaultValue = wxFileName{} )const;

		AriaLib_API void load( PluginPtr plugin
			, wxFileName const & fileName );
		AriaLib_API void load( wxFileName const & fileName );
		AriaLib_API void select( wxFileName const & fileName );
		AriaLib_API void write();

		AriaLib_API PathArray listConfigs()const;

		bool hasPlugin()const
		{
			return plugin != nullptr;
		}

		Plugin * getPlugin()
		{
			return plugin;
		}

		auto & getTestsOptions()const
		{
			return testsOptions;
		}

		auto & getFactory()const
		{
			return m_factory;
		}

	private:
		PluginFactory & m_factory;
		wxCmdLineParser parser;
		std::vector< TestsOptions > testsOptions;
		wxFileConfig configFile;
		Plugin * plugin{};
	};
}

#endif
