/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TreeModel_HPP___
#define ___CTP_TreeModel_HPP___

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
	TreeModel
	Implements the following data model:
		Status + Category/Name		RunDate		RunTime		
	--------------------------------------------------------
	1:	bitmap + Common/001-test	2020-11-06	15:35:22	
	*/
	class TreeModel
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
		TreeModel( Config const & config
			, Renderer renderer
			, RendererTestsCounts & counts );
		~TreeModel()override;

		TreeModelNode * addCategory( Category category
			, CategoryTestsCounts & counts
			, bool newCategory = false );
		TreeModelNode * addTest( DatabaseTest & test
			, bool newTest = false );
		TreeModelNode * getTestNode( DatabaseTest const & test )const;
		void removeTest( DatabaseTest const & test );
		void expandRoots( wxDataViewCtrl * view );
		void instantiate( wxDataViewCtrl * view );
		void resize( wxDataViewCtrl * view
			, wxSize const & size );

		// helper method for wxLog
		std::string getName( wxDataViewItem const & item )const;

		TreeModelNode * GetRootNode()const
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
		Config const & m_config;
		Renderer m_renderer;
		TreeModelNode * m_root;
		std::map< std::string, TreeModelNode * > m_categories;
	};
}

#endif