/* See LICENSE file in root folder */
#ifndef ___DIL_DiffImage_HPP___
#define ___DIL_DiffImage_HPP___

#include <wx/filename.h>
#include <wx/image.h>

#include <array>
#include <vector>

namespace diffimg
{
	using Path = wxFileName;
	using PathArray = std::vector< Path >;

	struct Options
	{
		PathArray outputs;
		Path input;
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

	struct Config
	{
		explicit Config( Options const & options );

		wxImage reference;
		std::array< Path, size_t( DiffResult::eCount ) > dirs;
	};

	DiffResult compareImages( Options const & options
		, Config const & config
		, Path const & compFile );
}

#endif
