/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Aria_HPP___
#define ___CTP_Aria_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/Plugin.hpp>

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/app.h>
#pragma warning( pop )

#include <fstream>

namespace aria
{
	class MainFrame;
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
		PluginPtr doParseCommandLine();

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
