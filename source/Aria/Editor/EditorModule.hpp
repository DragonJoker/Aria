/*
See LICENSE file in root folder
*/
#ifndef ___CTP_EditorModule_HPP___
#define ___CTP_EditorModule_HPP___

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/colour.h>
#include <wx/stc/stc.h>
#pragma warning( pop )

#include <map>
#include <memory>

namespace aria
{
	class LanguageFileContext;
	class LanguageFileParser;
	class LanguageInfo;
	class SceneFileDialog;
	class SceneFileEditor;
	class StcContext;
	class StcTextEditor;
	class StyleInfo;

	using LanguageInfoPtr = std::shared_ptr< LanguageInfo >;
	using LanguageFileContextPtr = std::shared_ptr< LanguageFileContext >;
	using StyleInfoMap = std::map< int, StyleInfo >;
}

#endif
