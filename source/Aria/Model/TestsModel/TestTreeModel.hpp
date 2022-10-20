/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestTreeModel_HPP___
#define ___CTP_TestTreeModel_HPP___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/dataview.h>
#pragma warning( pop )

namespace aria
{
	/*
	TestTreeModel
	Implements the following data model:
		Status + Category/Name		RunDate		RunTime		
	--------------------------------------------------------
	1:	bitmap + Common/001-test	2020-11-06	15:35:22	
	*/
	class TestTreeModel
		: public wxDataViewModel
	{
	public:
		enum class Column
		{
			eStatusName,
			eRunDate,
			eRunTime,
			eCount,
		};

	public:
		TestTreeModel( Renderer renderer
			, RendererTestsCounts & counts );
		~TestTreeModel()override;

		TestTreeModelNode * addCategory( Category category
			, TestsCounts const & counts
			, bool newCategory = false );
		void renameCategory( Category category
			, wxString const & oldName );
		void removeCategory( Category category );
		TestTreeModelNode * addTest( DatabaseTest & test
			, bool newTest = false );
		TestTreeModelNode * getTestNode( DatabaseTest const & test )const;
		void removeTest( DatabaseTest const & test );
		void expandRoots( wxDataViewCtrl * view );
		void instantiate( wxDataViewCtrl * view );
		void resize( wxDataViewCtrl * view
			, wxSize const & size );

		// helper method for wxLog
		std::string getName( wxDataViewItem const & item )const;

		TestTreeModelNode * GetRootNode()const
		{
			return m_root;
		}

		// helper methods to change the model
		void deleteItem( wxDataViewItem const & item );

		// Custom comparison
		int Compare( wxDataViewItem const & item1
			, wxDataViewItem const & item2
			, unsigned int column
			, bool ascending )const override;

		// implementation of base class virtuals to define model
		unsigned int GetColumnCount()const override;
		wxString GetColumnType( unsigned int col )const override;
		void GetValue( wxVariant & variant
			, wxDataViewItem const & item
			, unsigned int col )const override;
		bool SetValue( wxVariant const & variant
			, wxDataViewItem const & item
			, unsigned int col ) override;
		wxDataViewItem GetParent( wxDataViewItem const & item )const override;
		bool IsContainer( wxDataViewItem const & item )const override;
		unsigned int GetChildren( wxDataViewItem const & parent
			, wxDataViewItemArray & array )const override;
		bool HasContainerColumns( const wxDataViewItem & item )const override;

	private:
		Renderer m_renderer;
		TestTreeModelNode * m_root;
		std::map< std::string, TestTreeModelNode * > m_categories;
	};
}

#endif
