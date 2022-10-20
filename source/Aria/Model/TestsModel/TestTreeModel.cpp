#include "Model/TestsModel/TestTreeModel.hpp"

#include "Model/TestsModel/TestStatusRenderer.hpp"
#include "Model/TestsModel/TestTreeModelNode.hpp"

#include <AriaLib/TestsCounts.hpp>
#include <AriaLib/Database/DatabaseTest.hpp>
#include <AriaLib/Database/TestDatabase.hpp>

#include <wx/dc.h>

namespace aria
{
	//*********************************************************************************************

	namespace testmdl
	{
		static int getColumnSize( TestTreeModel::Column col )
		{
			switch ( col )
			{
			case aria::TestTreeModel::Column::eStatusName:
				return 400;
			case aria::TestTreeModel::Column::eRunDate:
				return 80;
			case aria::TestTreeModel::Column::eRunTime:
				return 90;
			default:
				return 100;
			}
		}

		static int getColumnMinSize( TestTreeModel::Column col
			, int maxWidth )
		{
			switch ( col )
			{
			case aria::TestTreeModel::Column::eStatusName:
				return maxWidth
					- getColumnMinSize( TestTreeModel::Column::eRunDate, maxWidth )
					- getColumnMinSize( TestTreeModel::Column::eRunTime, maxWidth );
			case aria::TestTreeModel::Column::eRunDate:
				return 80;
			case aria::TestTreeModel::Column::eRunTime:
				return 90;
			default:
				return 100;
			}
		}

		static wxString getColumnName( TestTreeModel::Column col )
		{
			switch ( col )
			{
			case aria::TestTreeModel::Column::eStatusName:
				return wxT( "Name" );
			case aria::TestTreeModel::Column::eRunDate:
				return wxT( "Run Date" );
			case aria::TestTreeModel::Column::eRunTime:
				return wxT( "Run Time" );
			default:
				return wxT( "string" );
			}
		}

		static wxString getColumnType( TestTreeModel::Column col )
		{
			switch ( col )
			{
			case TestTreeModel::Column::eStatusName:
				return TestStatusRenderer::GetDefaultType();
			default:
				return wxT( "string" );
			}
		}

		static wxDataViewRenderer * getColumnRenderer( TestTreeModel::Column col
			, wxDataViewCtrl * view )
		{
			switch ( col )
			{
			case aria::TestTreeModel::Column::eStatusName:
				return new TestStatusRenderer{ view, getColumnType( col ) };
			default:
				return new wxDataViewTextRenderer{ getColumnType( col ) };
			}
		}

		template< typename CountsT >
		static void updateStatusT( TestTreeModelNode * node
			, CountsT const * catRenCounts )
		{
			node->statusName.status = ( ( catRenCounts->getValue( TestsCountsType::eRunning ) != 0 )
				? std::max( TestStatus::eRunning_Begin, std::min( node->statusName.status, TestStatus::eRunning_End ) )
				: ( ( catRenCounts->getValue( TestsCountsType::ePending ) != 0 )
					? TestStatus::ePending
					: ( ( catRenCounts->getValue( TestsCountsType::eCrashed ) != 0 )
						? TestStatus::eCrashed
						: ( ( catRenCounts->getValue( TestsCountsType::eUnacceptable ) != 0 )
							? TestStatus::eUnacceptable
							: ( ( catRenCounts->getValue( TestsCountsType::eAcceptable ) != 0 )
								? TestStatus::eAcceptable
								: ( ( catRenCounts->getValue( TestsCountsType::eNegligible ) > catRenCounts->getValue( TestsCountsType::eNotRun ) )
									? TestStatus::eNegligible
									: TestStatus::eNotRun ) ) ) ) ) );
			node->statusName.outOfEngineDate = ( catRenCounts->getValue( TestsCountsType::eOutdated ) != 0 );
		}
	}

	//*********************************************************************************************

	TestTreeModel::TestTreeModel( Renderer renderer
		, RendererTestsCounts & counts )
		: m_renderer{ renderer }
		, m_root( new TestTreeModelNode{ renderer, counts } )
	{
	}

	TestTreeModel::~TestTreeModel()
	{
		delete m_root;
	}

	TestTreeModelNode * TestTreeModel::addCategory( Category category
		, TestsCounts const & counts
		, bool newCategory )
	{
		TestTreeModelNode * node = new TestTreeModelNode{ m_root, m_renderer, category, counts };
		m_categories[category->name] = node;

		if ( m_root )
		{
			m_root->Append( node );
		}

		if ( newCategory )
		{
			ItemAdded( wxDataViewItem{ m_root }, wxDataViewItem{ node } );
		}

		return node;
	}

	void TestTreeModel::renameCategory( Category category
		, wxString const & oldName )
	{
		auto stdName = makeStdString( oldName );
		auto nodeIt = m_categories.find( stdName );

		if ( nodeIt != m_categories.end() )
		{
			auto node = nodeIt->second;
			auto counts = node->categoryCounts;

			m_categories.erase( nodeIt );
			
			if ( m_root )
			{
				m_root->Remove( node );
			}

			ItemDeleted( wxDataViewItem{ m_root }, wxDataViewItem{ node } );

			node = new TestTreeModelNode{ m_root, m_renderer, category, *counts };
			m_categories[category->name] = node;
			
			if ( m_root )
			{
				m_root->Append( node );
			}

			ItemAdded( wxDataViewItem{ m_root }, wxDataViewItem{ node } );
		}
	}

	void TestTreeModel::removeCategory( Category category )
	{
		auto nodeIt = m_categories.find( category->name );

		if ( nodeIt != m_categories.end() )
		{
			auto node = nodeIt->second;
			m_categories.erase( nodeIt );

			if ( m_root )
			{
				m_root->Remove( node );
			}

			ItemDeleted( wxDataViewItem{ m_root }, wxDataViewItem{ node } );
		}
	}

	TestTreeModelNode * TestTreeModel::addTest( DatabaseTest & test
		, bool newTest )
	{
		auto it = m_categories.find( test->test->category->name );
		wxASSERT( m_categories.end() != it );
		TestTreeModelNode * node = new TestTreeModelNode{ it->second, test };
		it->second->Append( node );

		if ( newTest )
		{
			ItemAdded( wxDataViewItem{ it->second }, wxDataViewItem{ node } );
		}

		return node;
	}

	TestTreeModelNode * TestTreeModel::getTestNode( DatabaseTest const & test )const
	{
		TestTreeModelNode * result{};
		auto it = m_categories.begin();

		while ( !result && it != m_categories.end() )
		{
			auto nodeIt = std::find_if( it->second->GetChildren().begin()
				, it->second->GetChildren().end()
				, [&test]( TestTreeModelNode * lookup )
				{
					return lookup->test
						&& lookup->test->getTestId() == test.getTestId();
				} );

			if ( nodeIt != it->second->GetChildren().end() )
			{
				result = *nodeIt;
			}

			++it;
		}

		return result;
	}

	void TestTreeModel::removeTest( DatabaseTest const & test )
	{
		auto node = getTestNode( test );
		auto it = m_categories.find( node->category->name );
		wxASSERT( m_categories.end() != it );
		it->second->Remove( node );
		ItemDeleted( wxDataViewItem{ it->second }, wxDataViewItem{ node } );
	}

	void TestTreeModel::expandRoots( wxDataViewCtrl * view )
	{
		view->Expand( wxDataViewItem{ m_root } );
	}

	void TestTreeModel::instantiate( wxDataViewCtrl * view )
	{
		uint32_t flags = wxCOL_SORTABLE | wxCOL_RESIZABLE;

		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			wxDataViewColumn * col = new wxDataViewColumn( testmdl::getColumnName( column )
				, testmdl::getColumnRenderer( column, view )
				, uint32_t( i )
				, testmdl::getColumnSize( column )
				, wxALIGN_LEFT
				, int( flags ) );
			col->SetMinWidth( testmdl::getColumnSize( column ) );
			view->AppendColumn( col );
		}
	}

	void TestTreeModel::resize( wxDataViewCtrl * view
		, wxSize const & size )
	{
		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			auto col = view->GetColumn( uint32_t( i ) );
			col->SetMinWidth( testmdl::getColumnMinSize( column, size.GetWidth() ) );
			col->SetWidth( testmdl::getColumnMinSize( column, size.GetWidth() ) );
		}

		view->Refresh();
	}

	std::string TestTreeModel::getName( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TestTreeModelNode * >( item.GetID() );

		if ( node )
		{
			if ( node->test )
			{
				result = node->test->getName();
			}
			else if ( node->category )
			{
				result = node->category->name;
			}
			else if ( node->renderer )
			{
				result = node->renderer->name;
			}
		}

		return result;
	}

	void TestTreeModel::deleteItem( wxDataViewItem const & item )
	{
		auto node = static_cast< TestTreeModelNode * >( item.GetID() );

		if ( node )
		{
			wxDataViewItem parent( node->GetParent() );

			if ( parent.IsOk() )
			{
				// first remove the node from the parent's array of children;
				// NOTE: MyMusicTestTreeModelNodePtrArray is only an array of _pointers_
				//       thus removing the node from it doesn't result in freeing it
				node->GetParent()->Remove( node );

				// free the node
				delete node;

				// notify control
				ItemDeleted( parent, item );
			}
			else
			{
				wxASSERT( node == m_root );
				// don't make the control completely empty:
				wxLogError( "Cannot remove the root item!" );
			}
		}
	}

	int TestTreeModel::Compare( wxDataViewItem const & item1
		, wxDataViewItem const & item2
		, unsigned int column
		, bool ascending )const
	{
		int result = 0;
		wxASSERT( item1.IsOk() && item2.IsOk() );

		if ( IsContainer( item1 ) && IsContainer( item2 ) )
		{
			wxString str1 = getName( item1 );
			wxString str2 = getName( item2 );
			int res = str1.Cmp( str2 );

			if ( res )
			{
				result = res;
			}
			else
			{
				// items must be different
				auto litem1 = wxUIntPtr( item1.GetID() );
				auto litem2 = wxUIntPtr( item2.GetID() );
				result = int( litem1 - litem2 );
			}

			result = ascending
				? result
				: -result;
		}
		else if ( column == int( Column::eStatusName ) )
		{
			auto node1 = static_cast< TestTreeModelNode const * >( item1.GetID() );
			auto node2 = static_cast< TestTreeModelNode const * >( item2.GetID() );
			result = ( node1->test->getStatus() < node2->test->getStatus()
				? -1
				: ( node1->test->getStatus() == node2->test->getStatus()
					? makeWxString( node1->test->getName() ).Cmp( node2->test->getName() )
					: 1 ) );
			result = ascending
				? result
				: -result;
		}
		else
		{
			result = wxDataViewModel::Compare( item1, item2, column, ascending );
		}

		return result;
	}

	unsigned int TestTreeModel::GetColumnCount()const
	{
		return uint32_t( Column::eCount );
	}

	wxString TestTreeModel::GetColumnType( unsigned int col )const
	{
		return testmdl::getColumnType( Column( col ) );
	}

	void TestTreeModel::GetValue( wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )const
	{
		if ( !item.IsOk() )
		{
			return;
		}

		auto node = static_cast< TestTreeModelNode * >( item.GetID() );

		if ( !node )
		{
			return;
		}

		if ( node->test )
		{
			switch ( Column( col ) )
			{
			case Column::eStatusName:
				node->statusName.categoryCounts = &node->test->getCounts();
				node->statusName.status = node->test->getStatus();
				node->statusName.outOfEngineDate = node->test->checkOutOfEngineDate();
				node->statusName.outOfSceneDate = node->test->checkOutOfTestDate();
				node->statusName.ignored = node->test->getIgnoreResult();
				node->statusName.name = getName( item );
				variant = reinterpret_cast< void * >( &node->statusName );
				break;

			case Column::eRunDate:
				variant = node->test->getRunDate().IsValid()
					? node->test->getRunDate().FormatISODate()
					: wxString{};
				break;

			case Column::eRunTime:
				variant = node->test->getRunDate().IsValid()
					? node->test->getRunDate().FormatISOTime()
					: wxString{};
				break;

			default:
				wxLogError( wxString() << "TestTreeModel::GetValue: wrong column " << col );
				break;
			}
		}
		else
		{
			switch ( Column( col ) )
			{
			case Column::eStatusName:
				if ( node->category )
				{
					testmdl::updateStatusT( node, node->categoryCounts );
				}
				else if ( node->renderer )
				{
					testmdl::updateStatusT( node, node->rendererCounts );
				}
				else
				{
					testmdl::updateStatusT( node, node->allCounts );
				}
				node->statusName.outOfSceneDate = false;
				node->statusName.ignored = false;
				node->statusName.name = getName( item );
				variant = reinterpret_cast< void * >( &node->statusName );
				break;

			case Column::eRunDate:
			case Column::eRunTime:
				variant = wxEmptyString;
				break;

			default:
				wxLogError( wxString() << "TestTreeModel::GetValue: wrong column " << col );
				break;
			}
		}
	}

	bool TestTreeModel::SetValue( const wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )
	{
		bool result = false;

		if ( !item.IsOk() )
		{
			return result;
		}

		auto node = static_cast< TestTreeModelNode * >( item.GetID() );

		if ( !node )
		{
			return result;
		}

		switch ( Column( col ) )
		{
		case Column::eStatusName:
		case Column::eRunDate:
		case Column::eRunTime:
			break;

		default:
			wxLogError( wxString() << "TestTreeModel::SetValue: wrong column " << col );
			break;
		}

		return result;
	}

	wxDataViewItem TestTreeModel::GetParent( wxDataViewItem const & item )const
	{
		wxDataViewItem result( nullptr );

		if ( item.IsOk() )
		{
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( node && m_root != node )
			{
				result = wxDataViewItem{ node->GetParent() };
			}
		}

		return result;
	}

	bool TestTreeModel::IsContainer( wxDataViewItem const & item )const
	{
		bool result = false;

		if ( !item.IsOk() )
		{
			result = true;
		}
		else
		{
			auto node = static_cast< TestTreeModelNode * >( item.GetID() );

			if ( node )
			{
				result = node->IsContainer();
			}
		}

		return result;
	}

	unsigned int TestTreeModel::GetChildren( const wxDataViewItem & parent
		, wxDataViewItemArray & array )const
	{
		unsigned int result = 0;
		auto node = static_cast< TestTreeModelNode * >( parent.GetID() );

		if ( !node )
		{
			array.Add( wxDataViewItem{ m_root } );
			result = 1;
		}
		else
		{
			if ( node->GetChildCount() > 0 )
			{
				auto count = static_cast< unsigned int >( node->GetChildCount() );

				for ( unsigned int pos = 0; pos < count; pos++ )
				{
					auto child = node->GetNthChild( pos );
					array.Add( wxDataViewItem{ child } );
				}

				result = count;
			}
		}

		return result;
	}

	bool TestTreeModel::HasContainerColumns( const wxDataViewItem & item )const
	{
		return false;
	}

	//*********************************************************************************************
}
