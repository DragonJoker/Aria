/*
See LICENSE file in root folder
*/
#ifndef ___CTP_RunsModelPrerequisites_HPP___
#define ___CTP_RunsModelPrerequisites_HPP___

#include "Model/ModelPrerequisites.hpp"
#include "Prerequisites.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <chrono>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria::run
{
	using Microseconds = std::chrono::microseconds;

	class RunTreeModelNode;
	class RunTreeModel;

	typedef std::vector< RunTreeModelNode * > RunTreeModelNodePtrArray;

	inline wxString makeWxStringS( Microseconds const & time )
	{
		wxString result;
		result << ( double( time.count() ) / 1000000.0 ) << " s";
		return result;
	}

	inline wxString makeWxStringMs( Microseconds const & time )
	{
		wxString result;
		result << ( double( time.count() ) / 1000.0 ) << " ms";
		return result;
	}
}

#endif
