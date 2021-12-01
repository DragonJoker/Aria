/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_ConfigurationDialog_HPP___
#define ___ARIA_ConfigurationDialog_HPP___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/dialog.h>
#pragma warning( pop )

namespace aria
{
	class ConfigurationDialog
		: public wxDialog
	{
	public:
		ConfigurationDialog( wxWindow * parent
			, Config & config );

	private:
		Config m_newConfig;
		Config & m_config;
	};
}

#endif
