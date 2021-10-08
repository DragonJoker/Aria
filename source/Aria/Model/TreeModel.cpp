#include "Model/TreeModel.hpp"

#include "TestsCounts.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"
#include "Model/DataViewTestStatusRenderer.hpp"
#include "Model/TreeModelNode.hpp"

#include <wx/dc.h>

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		int getColumnSize( TreeModel::Column col )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return 400;
			case aria::TreeModel::Column::eRunDate:
				return 80;
			case aria::TreeModel::Column::eRunTime:
				return 90;
			default:
				return 100;
			}
		}

		int getColumnMinSize( TreeModel::Column col
			, int maxWidth )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return maxWidth
					- getColumnMinSize( TreeModel::Column::eRunDate, maxWidth )
					- getColumnMinSize( TreeModel::Column::eRunTime, maxWidth );
			case aria::TreeModel::Column::eRunDate:
				return 80;
			case aria::TreeModel::Column::eRunTime:
				return 90;
			default:
				return 100;
			}
		}

		wxString getColumnName( TreeModel::Column col )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return wxT( "Name" );
			case aria::TreeModel::Column::eRunDate:
				return wxT( "Run Date" );
			case aria::TreeModel::Column::eRunTime:
				return wxT( "Run Time" );
			default:
				return wxT( "string" );
			}
		}

		wxString getColumnType( TreeModel::Column col )
		{
			switch ( col )
			{
			case TreeModel::Column::eStatusName:
				return DataViewTestStatusRenderer::GetDefaultType();
			default:
				return wxT( "string" );
			}
		}

		wxDataViewRenderer * getColumnRenderer( TreeModel::Column col
			, wxDataViewCtrl * view )
		{
			switch ( col )
			{
			case aria::TreeModel::Column::eStatusName:
				return new DataViewTestStatusRenderer{ view, getColumnType( col ) };
			default:
				return new wxDataViewTextRenderer{ getColumnType( col ) };
			}
		}

		template< typename CountsT >
		void updateStatusT( TreeModelNode * node
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
			node->statusName.outOfCastorDate = ( catRenCounts->getValue( TestsCountsType::eOutdated ) != 0 );
		}
	}

	//*********************************************************************************************

	TreeModel::TreeModel( Config const & config
		, Renderer renderer
		, RendererTestsCounts & counts )
		: m_config{ config }
		, m_renderer{ renderer }
		, m_root( new TreeModelNode{ renderer, counts } )
	{
	}

	TreeModel::~TreeModel()
	{
		delete m_root;
	}

	TreeModelNode * TreeModel::addCategory( Category category
		, CategoryTestsCounts & counts
		, bool newCategory )
	{
		TreeModelNode * node = new TreeModelNode{ m_root, m_renderer, category, counts };
		m_categories[category->name] = node;
		m_root->Append( node );

		if ( newCategory )
		{
			ItemAdded( wxDataViewItem{ m_root }, wxDataViewItem{ node } );
		}

		return node;
	}

	TreeModelNode * TreeModel::addTest( DatabaseTest & test
		, bool newTest )
	{
		auto it = m_categories.find( test->test->category->name );
		wxASSERT( m_categories.end() != it );
		TreeModelNode * node = new TreeModelNode{ it->second, test };
		it->second->Append( node );

		if ( newTest )
		{
			ItemAdded( wxDataViewItem{ it->second }, wxDataViewItem{ node } );
		}

		return node;
	}

	TreeModelNode * TreeModel::getTestNode( DatabaseTest const & test )const
	{
		TreeModelNode * result{};
		auto it = m_categories.begin();

		while ( !result && it != m_categories.end() )
		{
			auto nodeIt = std::find_if( it->second->GetChildren().begin()
				, it->second->GetChildren().end()
				, [&test]( TreeModelNode * lookup )
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

	void TreeModel::removeTest( DatabaseTest const & test )
	{
		auto node = getTestNode( test );
		auto it = m_categories.find( node->category->name );
		wxASSERT( m_categories.end() != it );
		it->second->Remove( node );
		ItemDeleted( wxDataViewItem{ it->second }, wxDataViewItem{ node } );
	}

	void TreeModel::expandRoots( wxDataViewCtrl * view )
	{
		view->Expand( wxDataViewItem{ m_root } );
	}

	void TreeModel::instantiate( wxDataViewCtrl * view )
	{
		uint32_t flags = wxCOL_SORTABLE | wxCOL_RESIZABLE;

		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			wxDataViewColumn * col = new wxDataViewColumn( getColumnName( column )
				, getColumnRenderer( column, view )
				, i
				, getColumnSize( column )
				, wxALIGN_LEFT, flags );
			col->SetMinWidth( getColumnSize( column ) );
			view->AppendColumn( col );
		}
	}

	void TreeModel::resize( wxDataViewCtrl * view
		, wxSize const & size )
	{
		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			auto col = view->GetColumn( i );
			col->SetMinWidth( getColumnMinSize( column, size.GetWidth() ) );
			col->SetWidth( getColumnMinSize( column, size.GetWidth() ) );
		}

		view->Refresh();
	}

	std::string TreeModel::getName( wxDataViewItem const & item )const
	{
		std::string result{};
		auto node = static_cast< TreeModelNode * >( item.GetID() );

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

	void TreeModel::Delete( wxDataViewItem const & item )
	{
		auto node = static_cast< TreeModelNode * >( item.GetID() );

		if ( node )
		{
			wxDataViewItem parent( node->GetParent() );

			if ( parent.IsOk() )
			{
				// first remove the node from the parent's array of children;
				// NOTE: MyMusicTreeModelNodePtrArray is only an array of _pointers_
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

	int TreeModel::Compare( wxDataViewItem const & item1
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
				result = litem1 - litem2;
			}

			result = ascending
				? result
				: -result;
		}
		else if ( column == int( Column::eStatusName ) )
		{
			auto node1 = static_cast< TreeModelNode const * >( item1.GetID() );
			auto node2 = static_cast< TreeModelNode const * >( item2.GetID() );
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

	unsigned int TreeModel::GetColumnCount()const
	{
		return uint32_t( Column::eCount );
	}

	wxString TreeModel::GetColumnType( unsigned int col )const
	{
		return getColumnType( Column( col ) );
	}

	void TreeModel::GetValue( wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )const
	{
		wxASSERT( item.IsOk() );
		auto node = static_cast< TreeModelNode * >( item.GetID() );

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
				node->statusName.outOfCastorDate = node->test->checkOutOfCastorDate();
				node->statusName.outOfSceneDate = node->test->checkOutOfSceneDate();
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
				wxLogError( wxString() << "TreeModel::GetValue: wrong column " << col );
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
					updateStatusT( node, node->categoryCounts );
				}
				else if ( node->renderer )
				{
					updateStatusT( node, node->rendererCounts );
				}
				else
				{
					updateStatusT( node, node->allCounts );
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
				wxLogError( wxString() << "TreeModel::GetValue: wrong column " << col );
				break;
			}
		}
	}

	bool TreeModel::SetValue( const wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )
	{
		bool result = false;
		wxASSERT( item.IsOk() );
		auto node = static_cast< TreeModelNode * >( item.GetID() );

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
			wxLogError( wxString() << "TreeModel::SetValue: wrong column " << col );
			break;
		}

		return result;
	}

	wxDataViewItem TreeModel::GetParent( wxDataViewItem const & item )const
	{
		wxDataViewItem result( 0 );

		if ( item.IsOk() )
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( node && m_root != node )
			{
				result = wxDataViewItem{ node->GetParent() };
			}
		}

		return result;
	}

	bool TreeModel::IsContainer( wxDataViewItem const & item )const
	{
		bool result = false;

		if ( !item.IsOk() )
		{
			result = true;
		}
		else
		{
			auto node = static_cast< TreeModelNode * >( item.GetID() );

			if ( node )
			{
				result = node->IsContainer();
			}
		}

		return result;
	}

	unsigned int TreeModel::GetChildren( const wxDataViewItem & parent
		, wxDataViewItemArray & array )const
	{
		unsigned int result = 0;
		auto node = static_cast< TreeModelNode * >( parent.GetID() );

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

	bool TreeModel::HasContainerColumns( const wxDataViewItem & item )const
	{
		return false;
	}

	//*********************************************************************************************
}
