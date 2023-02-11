/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestsMainPanel_HPP___
#define ___CTP_TestsMainPanel_HPP___

#include "RendererPage.hpp"

#include <AriaLib/Plugin.hpp>
#include <AriaLib/Database/DbConnection.hpp>
#include <AriaLib/Database/DbStatement.hpp>
#include <AriaLib/Database/TestDatabase.hpp>
#include <AriaLib/FileSystem/FileSystem.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/menu.h>
#include <wx/process.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>

#include <map>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	class TestsMainPanel
		: public wxPanel
	{
	public:
		enum ID
		{
			eID_TIMER_TEST_UPDATER,
			eID_TIMER_CATEGORY_UPDATER,
			eID_TIMER_KILL_RUN,
			eID_DETAIL,
			eID_TESTS_BOOK,
			eID_DB_NEW_RENDERER,
			eID_DB_NEW_CATEGORY,
			eID_DB_NEW_TEST,
			eID_DB_EXPORT_LATEST_TIMES,
			eID_GIT,
		};

		class TestProcess
			: public wxProcess
		{
		public:
			TestProcess( wxEvtHandler * mainframe
				, int flags );

			void OnTerminate( int pid, int status )override;

		private:
			wxEvtHandler * m_mainframe;
		};

		struct RunningTest
		{
			std::unique_ptr< wxProcess > genProcess{};
			std::unique_ptr< wxProcess > disProcess{};
			wxProcess * currentProcess{};

			TestNode current();
			void push( TestNode node );
			TestNode next();
			void end();
			void clear();
			bool empty()const;
			size_t size()const;
			bool isRunning()const;

		private:
			std::list< TestNode > pending{};
			TestNode running{};
		};

	public:
		TestsMainPanel( wxFrame * parent
			, Plugin * plugin
			, Menus const & menus );
		~TestsMainPanel()override;

		void initialise();

		TestTreeModelNode * getTestNode( DatabaseTest const & test );
		wxDataViewItem getTestItem( DatabaseTest const & test );
		void editConfig();
		void onRendererMenuOption( wxCommandEvent & evt );
		void onCategoryMenuOption( wxCommandEvent & evt );
		void onTestMenuOption( wxCommandEvent & evt );
		void onDatabaseMenuOption( wxCommandEvent & evt );

		bool areTestsRunning()const
		{
			return !m_runningTest.empty();
		}

		TestDatabase & getDatabase()
		{
			return m_database;
		}

	private:
		wxWindow * doInitTestsLists();
		void doInitTestsList( Renderer renderer );
		void doInitGui();
		void doFillLists( wxProgressDialog & progress, int & index );
		void doFillList( Renderer renderer
			, wxProgressDialog & progress
			, int & index );
		RendererPage * doGetPage( wxDataViewItem const & item );

		uint32_t doGetAllTestsRange()const;
		void doProcessTest();
		void doStartTests();
		void doPushTest( wxDataViewItem & item );
		void doClearRunning();
		void doRunTest();
		void doCopyTestFileName();
		void doViewTestSceneFile();
		void doViewTest( bool async );
		void doSetRef();
		void doIgnoreTestResult();
		void doUpdateEngineDate();
		void doUpdateSceneDate();
		void doRunAllCategoryTests();
		void doRunCategoryTests( TestStatus filter );
		void doRunAllCategoryTestsBut( TestStatus filter );
		void doRunAllCategoryOutdatedTests();
		void doUpdateCategoryEngineDate();
		void doUpdateCategorySceneDate();
		void doAddCategoryNumPrefix();
		void doRemoveCategoryNumPrefix();
		void doRunAllRendererTests();
		void doRunRendererTests( TestStatus filter );
		void doRunAllRendererTestsBut( TestStatus filter );
		void doRunAllRendererOutdatedTests();
		void doUpdateRendererEngineDate();
		void doUpdateRendererSceneDate();
		std::vector< wxDataViewItem > doListAllTests( FilterFunc filter );
		void doCancelTest( DatabaseTest & test
			, TestStatus status );
		void doCancel();
		void doNewRenderer();
		void doNewCategory();
		void doNewTest( Category category = nullptr );
		void doExportLatestTimes();
		void doChangeTestCategory();
		void doRenameTest( DatabaseTest & dbTest
			, std::string const & newName
			, std::string const & commitText
			, bool commit );
		void doChangeTestName();
		void doRemoveTest( DatabaseTest & dbTest
			, std::string const & commitText
			, bool commit );
		void doDeleteTest();
		void doRenameCategory( Category category
			, std::string const & newName
			, std::string const & commitText
			, bool commit );
		void doChangeCategoryName();
		void doRemoveCategory( Category category
			, std::string const & commitText
			, bool commit );
		void doDeleteCategory();
		void onTestRunEnd( int status );
		void onTestDisplayEnd( int status );
		void onTestDiffEnd( TestTimes const & times );
		bool onTestProcessEnd( int pid, int status );

		void onTestsPageChange( wxAuiNotebookEvent & evt );
		void onProcessEnd( wxProcessEvent & evt );
		void onTestUpdateTimer( wxTimerEvent & evt );
		void onCategoryUpdateTimer( wxTimerEvent & evt );
		void onSize( wxSizeEvent & evt );

	private:
		Plugin * m_plugin;
		Config & m_config;
		Menus const & m_menus;
		wxAuiManager m_auiManager;
		FileSystemPtr m_fileSystem;
		TestDatabase m_database;
		Tests m_tests;
		std::map< Renderer, RendererPage *, LessIdValue > m_testsPages;
		wxAuiNotebook * m_testsBook{};
		RendererPage * m_selectedPage{};
		wxStaticText * m_statusText{};
		wxGauge * m_testProgress{};
		RunningTest m_runningTest;
		wxTimer * m_timerKillRun{};
		std::atomic_bool m_cancelled;
		wxTimer * m_testUpdater;
		wxTimer * m_categoriesUpdater;
	};
}

#endif
