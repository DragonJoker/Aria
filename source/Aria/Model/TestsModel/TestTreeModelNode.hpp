/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestTreeModelNode_HPP___
#define ___CTP_TestTreeModelNode_HPP___

#include "Prerequisites.hpp"

namespace aria
{
	class TestTreeModelNode
	{
		friend TestTreeModel;

	public:
		TestTreeModelNode( Renderer renderer
			, RendererTestsCounts & counts );
		TestTreeModelNode( TestTreeModelNode * parent
			, Renderer renderer
			, Category category
			, CategoryTestsCounts & counts );
		TestTreeModelNode( TestTreeModelNode * parent
			, DatabaseTest & test );
		~TestTreeModelNode();

		bool isRootNode()const
		{
			return GetParent() == nullptr;
		}
	
		void Remove( TestTreeModelNode * node );

		TestTreeModelNode * GetParent()const
		{
			return m_parent;
		}

		TestTreeModelNodePtrArray & GetChildren()
		{
			return m_children;
		}

		TestTreeModelNode * GetNthChild( size_t n )const
		{
			return m_children[n];
		}

		void Insert( TestTreeModelNode * child, size_t n )
		{
			m_children.insert( m_children.begin() + ptrdiff_t( n ), child );
		}

		void Append( TestTreeModelNode * child )
		{
			m_children.push_back( child );
		}

		size_t GetChildCount()const
		{
			return m_children.size();
		}

		bool IsContainer()const
		{
			return m_container;
		}

	public:
		DatabaseTest * test{};
		Renderer renderer{};
		Category category{};
		StatusName statusName{};
		AllTestsCounts const * allCounts{};
		RendererTestsCounts const * rendererCounts{};
		CategoryTestsCounts const * categoryCounts{};

	private:
		bool m_container{};
		TestTreeModelNode * m_parent{};
		TestTreeModelNodePtrArray m_children;
	};
}

#endif
