#include "AriaLib/Editor/LanguageInfo.hpp"

namespace aria
{
	LanguageInfo::LanguageInfo( int lexer, wxString name, wxString wildcard, wxString description )
		: m_lexer{ lexer }
		, m_name{ name }
		, m_wildcard{ wildcard }
		, m_description{ description }
	{
	}
}
