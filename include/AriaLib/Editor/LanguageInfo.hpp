/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_LanguageInfo_H___
#define ___ARIA_LanguageInfo_H___

#include "AriaLib/Editor/StyleInfo.hpp"

#include <array>

namespace aria
{
	/**
	\~english
	\brief Defines language informations (name, file patterns, Scintilla lexer, ...)
	\~french
	\brief Définit les informations d'un langage (nom, extension de fichier, lexer Scintilla, ...)
	*/
	class LanguageInfo
	{
	private:
		using WordArray = std::array< wxString, 9u >;

	public:
		AriaLib_API LanguageInfo( int lexer, wxString name, wxString wildcard, wxString description );
		AriaLib_API virtual ~LanguageInfo() = default;

		WordArray const & getKeywords()const
		{
			return m_keywords;
		}

		wxString const & getKeyword( uint32_t index )const
		{
			return m_keywords[index];
		}

		StyleInfo const & getStyle( int type )const
		{
			return m_styles.at( type );
		}

		StyleInfo const & getDefaultStyle()const
		{
			return getStyle( 0 );
		}

		StyleInfoMap const & getStyles()const
		{
			return m_styles;
		}

		int getLexer()const
		{
			return m_lexer;
		}

		int getFoldFlags()const
		{
			return m_foldFlags;
		}

		int getFontSize()const
		{
			return m_fontSize;
		}

		wxString const & getWildcard()const
		{
			return m_wildcard;
		}

		wxString const & getDescription()const
		{
			return m_description;
		}

		wxString const & getFontName()const
		{
			return m_fontName;
		}

	protected:
		int m_lexer;
		wxString m_name;
		wxString m_wildcard;
		wxString m_description;
		WordArray m_keywords;
		StyleInfoMap m_styles;
		int m_foldFlags{ 0 };
		int32_t m_fontSize{ 10 };
		wxString m_fontName;
	};
}

#endif
