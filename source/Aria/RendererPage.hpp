/*
See LICENSE file in root folder
*/
#ifndef ___ARIA_RendererPage_HPP___
#define ___ARIA_RendererPage_HPP___

#include "Prerequisites.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/aui/framemanager.h>
#include <wx/dataview.h>
#include <wx/panel.h>
#pragma warning( pop )

#include <functional>
#include <map>

class wxMenu;
class wxProgressDialog;
class wxStaticText;
class wxString;

namespace aria
{
	class RendererPage
		: public wxPanel
	{
	public:
		struct ToMove
		{
			Category originalCategory;
			int32_t id;
		};
		using ToMoveArray = std::vector< ToMove >;
		struct Selection
		{
			wxDataViewItemArray items;
			bool allTests{};
			bool allCategories{};
			bool allRenderers{};
		};

	public:
		RendererPage( Plugin const & plugin
			, Renderer renderer
			, RendererTestRuns & runs
			, RendererTestsCounts & counts
			, wxWindow * parent
			, MainFrame * frame
			, wxMenu * testMenu
			, wxMenu * categoryMenu
			, wxMenu * rendererMenu
			, wxMenu * allMenu
			, wxMenu * busyTestMenu
			, wxMenu * busyCategoryMenu
			, wxMenu * busyRendererMenu
			, wxMenu * busyAllMenu );
		~RendererPage()override;

		TestTreeModelNode * getTestNode( DatabaseTest const & test )const;
		void refreshView()const;
		void resizeModel( wxSize const & size );
		void listLatestRuns( TestDatabase & database
			, TestMap const & tests
			, AllTestsCounts & counts
			, wxProgressDialog & progress
			, int & index );
		void updateTest( TestTreeModelNode * node );
		std::vector< wxDataViewItem > listRendererTests( Renderer renderer
			, FilterFunc filter )const;
		std::vector< wxDataViewItem > listRenderersTests( FilterFunc filter )const;
		std::vector< wxDataViewItem > listCategoryTests( Category category
			, FilterFunc filter )const;
		std::vector< wxDataViewItem > listCategoriesTests( FilterFunc filter )const;
		std::vector< wxDataViewItem > listSelectedTests()const;
		std::vector< wxDataViewItem > listSelectedCategories()const;
		void copyTestFileName()const;
		void viewTestSceneFile();
		void viewTest( wxProcess * process
			, wxStaticText * statusText
			, bool async )const;
		void setTestsReferences( AllTestsCounts & counts );
		void ignoreTestsResult( bool ignore );
		void updateTestsEngineDate();
		void updateTestsSceneDate();
		void addCategory( Category category
			, CategoryTestsCounts & catCounts );
		void addTest( DatabaseTest & dbTest );
		void removeTest( DatabaseTest const & dbTest );
		void updateTestView( DatabaseTest const & test
			, AllTestsCounts & counts );
		void preChangeTestName( Test const & test
			, wxString const & newName );
		void postChangeTestName( Test const & test
			, wxString const & oldName );
		void preChangeTestName( DatabaseTest & test
			, wxString const & newName );
		void postChangeTestName( DatabaseTest & test
			, wxString const & oldName );
		void removeCategory( Category category );
		void postChangeCategoryName( Category category
			, wxString const & oldName );
		void changeTestsCategory( ToMoveArray const & tests
			, Category newCategory );

	private:
		void doInitLayout( wxWindow * frame );
		void doUpdateTestStatus( DatabaseTest & test
			, AllTestsCounts & counts
			, TestStatus newStatus
			, bool reference );
		void doListCategoryTests( Category category
			, FilterFunc filter
			, std::vector< wxDataViewItem > & result )const;
		void doListRendererTests( Renderer renderer
			, FilterFunc filter
			, std::vector< wxDataViewItem > & result )const;
		void onSelectionChange( wxDataViewEvent & evt );
		void onItemContextMenu( wxDataViewEvent & evt );

	private:
		MainFrame * m_mainFrame;
		Plugin const & m_plugin;
		Renderer m_renderer;
		wxMenu * m_testMenu;
		wxMenu * m_categoryMenu;
		wxMenu * m_rendererMenu;
		wxMenu * m_allMenu;
		wxMenu * m_busyTestMenu;
		wxMenu * m_busyCategoryMenu;
		wxMenu * m_busyRendererMenu;
		wxMenu * m_busyAllMenu;
		wxAuiManager m_auiManager;
		RendererTestRuns & m_runs;
		RendererTestsCounts & m_counts;
		wxObjectDataPtr< TestTreeModel > m_model;
		wxDataViewCtrl * m_view{};
		LayeredPanel * m_generalViews{};
		LayeredPanel * m_detailViews{};
		TestPanel * m_testView{};
		CategoryPanel * m_allView{};
		CategoryPanel * m_categoryView{};
		Selection m_selected;
	};
}

#endif
