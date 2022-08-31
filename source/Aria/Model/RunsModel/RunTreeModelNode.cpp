#include "Model/RunsModel/RunTreeModelNode.hpp"

#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

namespace aria::run
{
	RunTreeModelNode::RunTreeModelNode()
	{
	}

	RunTreeModelNode::RunTreeModelNode( RunTreeModelNode * parent
		, Run prun )
		: run{ prun }
		, m_parent{ parent }
	{
	}

	RunTreeModelNode::~RunTreeModelNode()
	{
		// free all our children nodes
		Clear();
	}

	void RunTreeModelNode::Clear()
	{
		size_t count = m_children.size();

		for ( size_t i = 0; i < count; i++ )
		{
			delete m_children[i];
		}

		m_children.clear();
	}

	void RunTreeModelNode::Remove( RunTreeModelNode * node )
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
