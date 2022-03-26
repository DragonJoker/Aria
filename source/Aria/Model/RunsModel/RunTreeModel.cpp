#include "Model/RunsModel/RunTreeModel.hpp"

#include "TestsCounts.hpp"
#include "Database/DatabaseTest.hpp"
#include "Database/TestDatabase.hpp"
#include "Model/RunsModel/RunStatusRenderer.hpp"
#include "Model/RunsModel/RunTreeModelNode.hpp"

#include <wx/dc.h>

namespace aria::run
{
	//*********************************************************************************************

	namespace
	{
		int getColumnSize( RunTreeModel::Column col )
		{
			switch ( col )
			{
			case RunTreeModel::Column::eStatus:
				return 50;
			case RunTreeModel::Column::eRunDateTime:
				return 170;
			case RunTreeModel::Column::ePlatformName:
				return 200;
			case RunTreeModel::Column::eCpuName:
				return 200;
			case RunTreeModel::Column::eGpuName:
				return 200;
			case RunTreeModel::Column::eTotalTime:
				return 100;
			case RunTreeModel::Column::eAvgTime:
				return 100;
			case RunTreeModel::Column::eLastTime:
				return 100;
			default:
				return 100;
			}
		}

		int getColumnMinSize( RunTreeModel::Column col
			, int maxWidth )
		{
			return getColumnSize( col );
		}

		wxString getColumnName( RunTreeModel::Column col )
		{
			switch ( col )
			{
			case RunTreeModel::Column::eStatus:
				return wxT( "Status" );
			case RunTreeModel::Column::eRunDateTime:
				return wxT( "Run Date" );
			case RunTreeModel::Column::ePlatformName:
				return wxT( "Platform Name" );
			case RunTreeModel::Column::eCpuName:
				return wxT( "CPU Name" );
			case RunTreeModel::Column::eGpuName:
				return wxT( "GPU Name" );
			case RunTreeModel::Column::eTotalTime:
				return wxT( "Total Time" );
			case RunTreeModel::Column::eAvgTime:
				return wxT( "Average Time" );
			case RunTreeModel::Column::eLastTime:
				return wxT( "Last Frame Time" );
			default:
				return wxT( "string" );
			}
		}

		wxString getColumnType( RunTreeModel::Column col )
		{
			switch ( col )
			{
			case RunTreeModel::Column::eStatus:
				return RunStatusRenderer::GetDefaultType();
			default:
				return wxT( "string" );
			}
		}

		wxDataViewRenderer * getColumnRenderer( RunTreeModel::Column col
			, wxDataViewCtrl * view )
		{
			switch ( col )
			{
			case RunTreeModel::Column::eStatus:
				return new RunStatusRenderer{ view, getColumnType( col ) };
			default:
				return new wxDataViewTextRenderer{ getColumnType( col ) };
			}
		}

		template< typename ValueT >
		int compare( ValueT const & lhs, ValueT const & rhs )
		{
			if ( lhs < rhs )
			{
				return -1;
			}

			if ( lhs == rhs )
			{
				return 0;
			}

			return 1;
		}
	}

	//*********************************************************************************************

	RunTreeModel::RunTreeModel()
		: m_root( new RunTreeModelNode{} )
	{
	}

	RunTreeModel::~RunTreeModel()
	{
		delete m_root;
	}

	void RunTreeModel::addRun( Run run )
	{
		RunTreeModelNode * node = new RunTreeModelNode{ m_root, std::move( run ) };
		m_root->Append( node );
		ItemAdded( wxDataViewItem{ m_root }, wxDataViewItem{ node } );
	}

	RunTreeModelNode * RunTreeModel::getRunNode( uint32_t runId )const
	{
		RunTreeModelNode * result{};
		auto nodeIt = std::find_if( m_root->GetChildren().begin()
			, m_root->GetChildren().end()
			, [runId]( RunTreeModelNode * lookup )
			{
				return lookup->run.id == runId;
			} );

		if ( nodeIt != m_root->GetChildren().end() )
		{
			result = *nodeIt;
		}

		return result;
	}

	void RunTreeModel::removeRun( uint32_t runId )
	{
		auto node = getRunNode( runId );
		m_root->Remove( node );
		ItemDeleted( wxDataViewItem{ m_root }, wxDataViewItem{ node } );
	}

	void RunTreeModel::clear()
	{
		for ( auto node : m_root->GetChildren() )
		{
			m_root->Remove( node );
		}

		Cleared();
	}

	void RunTreeModel::expandRoots( wxDataViewCtrl * view )
	{
		view->Expand( wxDataViewItem{ m_root } );
	}

	void RunTreeModel::instantiate( wxDataViewCtrl * view )
	{
		uint32_t flags = wxCOL_SORTABLE | wxCOL_RESIZABLE;

		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			wxDataViewColumn * col = new wxDataViewColumn( getColumnName( column )
				, getColumnRenderer( column, view )
				, uint32_t( i )
				, getColumnSize( column )
				, wxALIGN_LEFT
				, int( flags ) );
			col->SetMinWidth( getColumnSize( column ) );
			view->AppendColumn( col );
		}
	}

	void RunTreeModel::resize( wxDataViewCtrl * view
		, wxSize const & size )
	{
		for ( int i = 0; i < int( Column::eCount ); ++i )
		{
			auto column = Column( i );
			auto col = view->GetColumn( uint32_t( i ) );
			col->SetMinWidth( getColumnMinSize( column, size.GetWidth() ) );
			col->SetWidth( getColumnMinSize( column, size.GetWidth() ) );
		}

		view->Refresh();
	}

	void RunTreeModel::deleteItem( wxDataViewItem const & item )
	{
		auto node = static_cast< RunTreeModelNode * >( item.GetID() );

		if ( node )
		{
			wxDataViewItem parent( node->GetParent() );

			if ( parent.IsOk() )
			{
				// first remove the node from the parent's array of children;
				// NOTE: MyMusicRunTreeModelNodePtrArray is only an array of _pointers_
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

	int RunTreeModel::Compare( wxDataViewItem const & item1
		, wxDataViewItem const & item2
		, unsigned int column
		, bool ascending )const
	{
		int result = 0;
		wxASSERT( item1.IsOk() && item2.IsOk() );
		auto node1 = static_cast< RunTreeModelNode const * >( item1.GetID() );
		auto node2 = static_cast< RunTreeModelNode const * >( item2.GetID() );

		switch ( Column( column ) )
		{
		case RunTreeModel::Column::eStatus:
			result = compare( node1->run.status, node2->run.status );
			break;
		case RunTreeModel::Column::eRunDateTime:
			result = compare( node1->run.runDate, node2->run.runDate );
			break;
		case RunTreeModel::Column::ePlatformName:
			result = compare( node1->run.host->platform->id, node2->run.host->platform->id );
			break;
		case RunTreeModel::Column::eCpuName:
			result = compare( node1->run.host->cpu->id, node2->run.host->cpu->id );
			break;
		case RunTreeModel::Column::eGpuName:
			result = compare( node1->run.host->gpu->id, node2->run.host->gpu->id );
			break;
		case RunTreeModel::Column::eTotalTime:
			result = compare( node1->run.totalTime, node2->run.totalTime );
			break;
		case RunTreeModel::Column::eAvgTime:
			result = compare( node1->run.avgTime, node2->run.avgTime );
			break;
		case RunTreeModel::Column::eLastTime:
			result = compare( node1->run.lastTime, node2->run.lastTime );
			break;
		default:
			break;
		}

		result = ascending
			? result
			: -result;
		return result;
	}

	unsigned int RunTreeModel::GetColumnCount()const
	{
		return uint32_t( Column::eCount );
	}

	wxString RunTreeModel::GetColumnType( unsigned int col )const
	{
		return getColumnType( Column( col ) );
	}

	void RunTreeModel::GetValue( wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )const
	{
		wxASSERT( item.IsOk() );
		auto node = static_cast< RunTreeModelNode * >( item.GetID() );

		if ( !node )
		{
			return;
		}

		switch ( Column( col ) )
		{
		case Column::eStatus:
			variant = long( node->run.status );
			break;
		case Column::eRunDateTime:
			variant = node->run.runDate.IsValid()
				? ( node->run.runDate.FormatISODate() + wxT( " " ) + node->run.runDate.FormatISOTime() )
				: wxString{};
			break;
		case Column::ePlatformName:
			variant = makeWxString( node->run.host->platform->name );
			break;
		case Column::eCpuName:
			variant = makeWxString( node->run.host->cpu->name );
			break;
		case Column::eGpuName:
			variant = makeWxString( node->run.host->gpu->name );
			break;
		case Column::eTotalTime:
			variant = makeWxStringS( node->run.totalTime );
			break;
		case Column::eAvgTime:
			variant = makeWxStringMs( node->run.avgTime );
			break;
		case Column::eLastTime:
			variant = makeWxStringMs( node->run.lastTime );
			break;
		default:
			wxLogError( wxString() << "RunTreeModel::GetValue: wrong column " << col );
			break;
		}
	}

	bool RunTreeModel::SetValue( const wxVariant & variant
		, wxDataViewItem const & item
		, unsigned int col )
	{
		bool result = false;
		wxASSERT( item.IsOk() );
		auto node = static_cast< RunTreeModelNode * >( item.GetID() );

		if ( !node )
		{
			return result;
		}

		switch ( Column( col ) )
		{
		case Column::eStatus:
		case Column::eRunDateTime:
		case Column::ePlatformName:
		case Column::eCpuName:
		case Column::eGpuName:
		case Column::eTotalTime:
		case Column::eAvgTime:
		case Column::eLastTime:
			break;

		default:
			wxLogError( wxString() << "RunTreeModel::SetValue: wrong column " << col );
			break;
		}

		return result;
	}

	wxDataViewItem RunTreeModel::GetParent( wxDataViewItem const & item )const
	{
		wxDataViewItem result( nullptr );

		if ( item.IsOk() )
		{
			auto node = static_cast< RunTreeModelNode * >( item.GetID() );

			if ( node && m_root != node )
			{
				result = wxDataViewItem{ node->GetParent() };
			}
		}

		return result;
	}

	bool RunTreeModel::IsContainer( wxDataViewItem const & item )const
	{
		bool result = false;

		if ( !item.IsOk() )
		{
			result = true;
		}
		else
		{
			auto node = static_cast< RunTreeModelNode * >( item.GetID() );
			return node == m_root;
		}

		return result;
	}

	unsigned int RunTreeModel::GetChildren( const wxDataViewItem & parent
		, wxDataViewItemArray & array )const
	{
		unsigned int result = 0;
		auto node = static_cast< RunTreeModelNode * >( parent.GetID() );

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

	bool RunTreeModel::HasContainerColumns( const wxDataViewItem & item )const
	{
		return false;
	}

	//*********************************************************************************************
}
