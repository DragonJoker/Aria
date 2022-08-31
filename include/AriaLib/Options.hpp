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
			static const wxString Database{ wxT( "database" ) };
			static const wxString Test{ wxT( "test" ) };
			static const wxString Work{ wxT( "work" ) };
			static const wxString FrameCount{ wxT( "frames" ) };
			static const wxString Plugin{ wxT( "plugin" ) };
		}

		namespace st
		{
			static const wxString Help{ wxT( "h" ) };
			static const wxString Force{ wxT( "f" ) };
			static const wxString ConfigFile{ wxT( "c" ) };
			static const wxString Database{ wxT( "d" ) };
			static const wxString Test{ wxT( "t" ) };
			static const wxString Work{ wxT( "w" ) };
			static const wxString FrameCount{ wxT( "a" ) };
			static const wxString Plugin{ wxT( "p" ) };
		}

		namespace df
		{
			static const uint32_t FrameCount{ 10u };
		}
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

	struct Options
	{
		AriaLib_API Options( PluginFactory & factory
			, std::vector< PluginLib > & pluginsLibs
			, int argc
			, wxCmdLineArgsArray const & argv );
		AriaLib_API ~Options();

		AriaLib_API bool has( wxString const & option )const;

		template< typename ValueT >
		ValueT getLong( wxString const & option
			, bool mandatory
			, ValueT defaultValue )const
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

		AriaLib_API wxString getString( wxString const & option
			, bool mandatory
			, wxString const & defaultValue = wxString{} )const;
		AriaLib_API wxFileName getFileName( wxString const & option
			, bool mandatory
			, wxFileName const & defaultValue = wxFileName{} )const;

		AriaLib_API void write( Config const & config );

		AriaLib_API static wxString findConfigFile( wxCmdLineParser const & parser );

		PluginPtr getPlugin()
		{
			return std::move( plugin );
		}

	private:
		wxCmdLineParser parser;
		wxFileConfig * configFile{ nullptr };
		PluginPtr plugin;
	};
}

#endif
