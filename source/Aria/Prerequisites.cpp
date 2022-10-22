#include "Prerequisites.hpp"

#include "Model/TestsModel/TestTreeModelNode.hpp"

#include <AriaLib/Database/DatabaseTest.hpp>

#include <wx/dir.h>
#include <wx/filefn.h>

#if !defined( WIN32 )
#	include <strings.h>
#endif

#include <sstream>

namespace aria
{
	uint32_t StatusName::getStatusIndex( bool ignoreResult
		, TestStatus status )
	{
		uint32_t result{};

		if ( ( !ignoreResult )
			|| ( !isPending( status ) && !isRunning( status ) ) )
		{
			result = uint32_t( status ) + uint32_t( AdditionalIndices );
		}
		else if ( ignoreResult )
		{
			result = IgnoredIndex;
		}

		return ( result << 2 );
	}

	uint32_t StatusName::getTestStatusIndex( Config const & config
		, DatabaseTest const & test )
	{
		return getStatusIndex( test.getIgnoreResult(), test.getStatus() )
			| ( test.checkOutOfEngineDate()
				? 0x01u
				: 0x00u )
			| ( test.checkOutOfTestDate()
				? 0x02u
				: 0x00u );
	}

	bool isTestNode( TestTreeModelNode const & node )
	{
		return node.test != nullptr;
	}

	bool isCategoryNode( TestTreeModelNode const & node )
	{
		return node.category
			&& node.renderer
			&& !node.isRootNode();
	}

	bool isRendererNode( TestTreeModelNode const & node )
	{
		return node.renderer
			&& node.isRootNode();
	}

	void Menus::SubMenus::bind( wxObjectEventFunction testFunc
		, wxObjectEventFunction catFunc
		, wxObjectEventFunction rendFunc
		, wxEvtHandler * handler )const
	{
		test->Connect( wxEVT_COMMAND_MENU_SELECTED
			, testFunc
			, nullptr
			, handler );
		category->Connect( wxEVT_COMMAND_MENU_SELECTED
			, catFunc
			, nullptr
			, handler );
		renderer->Connect( wxEVT_COMMAND_MENU_SELECTED
			, rendFunc
			, nullptr
			, handler );
	}

	void Menus::SubMenus::unbind( wxObjectEventFunction testFunc
		, wxObjectEventFunction catFunc
		, wxObjectEventFunction rendFunc
		, wxEvtHandler * handler )const
	{
		test->Disconnect( wxEVT_COMMAND_MENU_SELECTED
			, testFunc
			, nullptr
			, handler );
		category->Disconnect( wxEVT_COMMAND_MENU_SELECTED
			, catFunc
			, nullptr
			, handler );
		renderer->Disconnect( wxEVT_COMMAND_MENU_SELECTED
			, rendFunc
			, nullptr
			, handler );
	}

	void Menus::bind( wxObjectEventFunction testFunc
		, wxObjectEventFunction catFunc
		, wxObjectEventFunction rendFunc
		, wxEvtHandler * handler )const
	{
		base.bind( testFunc, catFunc, rendFunc, handler );
		busy.bind( testFunc, catFunc, rendFunc, handler );
		bar.bind( testFunc, catFunc, rendFunc, handler );
	}

	void Menus::unbind( wxObjectEventFunction testFunc
		, wxObjectEventFunction catFunc
		, wxObjectEventFunction rendFunc
		, wxEvtHandler * handler )const
	{
		base.unbind( testFunc, catFunc, rendFunc, handler );
		busy.unbind( testFunc, catFunc, rendFunc, handler );
		bar.unbind( testFunc, catFunc, rendFunc, handler );
	}
}
