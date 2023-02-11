/* See LICENSE file in root folder */
#ifndef ___ARIA__DiffImage_HPP___
#define ___ARIA__DiffImage_HPP___

#include "Prerequisites.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/image.h>

#include <array>
#include <vector>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	struct DiffOptions
	{
		PathArray outputs;
		wxFileName input;
		double acceptableThreshold = 0.1;
		double negligibleThreshold = 0.001;
		DiffMode mode = DiffMode::eLogarithmic;
	};

	enum class DiffResult
	{
		eUnprocessed,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eCount,
	};

	struct DiffConfig
	{
		explicit DiffConfig( DiffOptions const & options );

		wxImage reference;
		std::array< wxFileName, size_t( DiffResult::eCount ) > dirs;
	};

	wxImage loadImage( wxFileName const & filePath );
	DiffResult compareImages( DiffOptions const & options
		, DiffConfig const & config
		, wxFileName const & compFile );
	wxImage getImageDiff( DiffMode mode
		, wxFileName const & reference
		, wxFileName const & toTest );
}

#endif
