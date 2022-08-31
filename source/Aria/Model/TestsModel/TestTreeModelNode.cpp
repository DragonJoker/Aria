#include "Model/TestsModel/TestTreeModelNode.hpp"

#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

namespace aria
{
	TestTreeModelNode::TestTreeModelNode( Renderer renderer
		, RendererTestsCounts & counts )
		: renderer{ renderer }
		, statusName{ NodeType::eRenderer
			, &counts
			, nullptr
			, renderer->name }
		, rendererCounts{ &counts }
		, m_container{ true }
	{
	}

	TestTreeModelNode::TestTreeModelNode( TestTreeModelNode * parent
		, Renderer renderer
		, Category category
		, CategoryTestsCounts const & counts )
		: renderer{ renderer }
		, category{ category }
		, statusName{ NodeType::eCategory
			, nullptr
			, &counts
			, category->name }
		, categoryCounts{ &counts }
		, m_container{ true }
		, m_parent{ parent }
	{
	}

	TestTreeModelNode::TestTreeModelNode( TestTreeModelNode * parent
		, DatabaseTest & test )
		: test{ &test }
		, renderer{ test.getRenderer() }
		, category{ test.getCategory() }
		, statusName{ NodeType::eTestRun
			, nullptr
			, nullptr
			, test.getName() }
		, m_container{ false }
		, m_parent{ parent }
	{
	}

	TestTreeModelNode::~TestTreeModelNode()
	{
		// free all our children nodes
		size_t count = m_children.size();

		for( size_t i = 0; i < count; i++ )
		{
			delete m_children[i];
		}

		m_children.clear();
	}

	void TestTreeModelNode::Remove( TestTreeModelNode * node )
	{
		auto it = std::find( m_children.begin()
			, m_children.end()
			, node );

		if( it != m_children.end() )
		{
			m_children.erase( it );
		}
	}
}
