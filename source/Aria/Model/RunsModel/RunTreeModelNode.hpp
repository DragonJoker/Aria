/*
See LICENSE file in root folder
*/
#ifndef ___CTP_RunTreeModelNode_HPP___
#define ___CTP_RunTreeModelNode_HPP___

#include "RunsModelPrerequisites.hpp"

namespace aria::run
{
	class RunTreeModelNode
	{
		friend RunTreeModel;

	public:
		RunTreeModelNode();
		RunTreeModelNode( RunTreeModelNode * parent
			, Run run );
		~RunTreeModelNode();

		bool isRootNode()const
		{
			return GetParent() == nullptr;
		}
	
		void Remove( RunTreeModelNode * node );
		void Clear();

		RunTreeModelNode * GetParent()const
		{
			return m_parent;
		}

		RunTreeModelNodePtrArray & GetChildren()
		{
			return m_children;
		}

		RunTreeModelNode * GetNthChild( size_t n )const
		{
			return m_children[n];
		}

		void Insert( RunTreeModelNode * child, size_t n )
		{
			m_children.insert( m_children.begin() + ptrdiff_t( n ), child );
		}

		void Append( RunTreeModelNode * child )
		{
			m_children.push_back( child );
		}

		size_t GetChildCount()const
		{
			return m_children.size();
		}

	public:
		Run run{};

	private:
		RunTreeModelNode * m_parent{};
		RunTreeModelNodePtrArray m_children;
	};
}

#endif
