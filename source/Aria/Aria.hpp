/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Aria_HPP___
#define ___CTP_Aria_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/Plugin.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/app.h>

#include <fstream>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	class Plugin;

	class Aria
		: public wxApp
	{
	public:
		Aria();

		MainFrame * getMainFrame()const
		{
			return m_mainFrame;
		}

	private:
		OptionsPtr doParseCommandLine();

		bool OnInit()override;
		int OnExit()override;

	private:
		PluginFactory m_factory;
		std::vector< PluginLib > m_pluginsLibs;
		MainFrame * m_mainFrame{ nullptr };
		std::ofstream m_outStream;
		std::unique_ptr< wxLogStream > m_logStream;
	};
}

wxDECLARE_APP( aria::Aria );

#endif
