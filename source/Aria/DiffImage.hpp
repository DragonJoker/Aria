/* See LICENSE file in root folder */
#ifndef ___ARIA__DiffImage_HPP___
#define ___ARIA__DiffImage_HPP___

#include "Prerequisites.hpp"

#include <wx/image.h>

#include <array>
#include <vector>

namespace aria
{
	struct DiffOptions
	{
		PathArray outputs;
		wxFileName input;
		double acceptableThreshold = 0.1;
		double negligibleThreshold = 0.001;
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

	DiffResult compareImages( DiffOptions const & options
		, DiffConfig const & config
		, wxFileName const & compFile );
	double compareImages( wxImage const & reference
		, wxImage const & toTest
		, wxImage & diffImg );
}

#endif
