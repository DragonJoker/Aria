/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_ConfigurationDialog_HPP___
#define ___ARIA_ConfigurationDialog_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/Plugin.hpp>

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/dialog.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	class ConfigurationDialog
		: public wxDialog
	{
	public:
		ConfigurationDialog( wxWindow * parent
			, Plugin & plugin );

	private:
		Plugin & m_plugin;
		std::unique_ptr< PluginConfig > m_newPluginConfig;
		Config m_newConfig;
		Config & m_config;
	};
}

#endif
