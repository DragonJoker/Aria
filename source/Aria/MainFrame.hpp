/*
See LICENSE file in root folder
*/
#ifndef ___CTP_MainFrame_HPP___
#define ___CTP_MainFrame_HPP___

#include "RendererPage.hpp"

#include <AriaLib/Plugin.hpp>
#include <AriaLib/Database/DbConnection.hpp>
#include <AriaLib/Database/DbStatement.hpp>
#include <AriaLib/Database/TestDatabase.hpp>
#include <AriaLib/FileSystem/FileSystem.hpp>

#pragma warning( push )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 4464 )
#include <wx/frame.h>
#include <wx/dataview.h>
#include <wx/menu.h>
#include <wx/process.h>
#include <wx/aui/framemanager.h>
#include <wx/aui/auibook.h>

#include <atomic>
#include <list>
#include <map>
#include <thread>
#pragma warning( pop )

class wxGauge;
class wxStaticText;

namespace aria
{
	class MainFrame
		: public wxFrame
	{
	public:
		enum ID
		{
			eID_TIMER_TEST_UPDATER,
			eID_TIMER_CATEGORY_UPDATER,
			eID_TIMER_KILL_RUN,
			eID_DETAIL,
			eID_TEST_RUN,
			eID_TEST_VIEW_SYNC,
			eID_TEST_VIEW_ASYNC,
			eID_TEST_SET_REF,
			eID_TEST_IGNORE_RESULT,
			eID_TEST_UPDATE_ENGINE,
			eID_TEST_UPDATE_SCENE,
			eID_TEST_COPY_FILE_NAME,
			eID_TEST_VIEW_FILE,
			eID_TEST_CHANGE_CATEGORY,
			eID_TEST_CHANGE_NAME,
			eID_TEST_DELETE,
			eID_CATEGORY_RUN_TESTS_ALL,
			eID_CATEGORY_RUN_TESTS_NOTRUN,
			eID_CATEGORY_RUN_TESTS_ACCEPTABLE,
			eID_CATEGORY_RUN_TESTS_CRASHED,
			eID_CATEGORY_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_CATEGORY_RUN_TESTS_OUTDATED,
			eID_CATEGORY_UPDATE_ENGINE,
			eID_CATEGORY_UPDATE_SCENE,
			eID_CATEGORY_ADD_NUMPREFIX,
			eID_CATEGORY_REMOVE_NUMPREFIX,
			eID_CATEGORY_CHANGE_NAME,
			eID_CATEGORY_CREATE_TEST,
			eID_CATEGORY_DELETE,
			eID_RENDERER_RUN_TESTS_ALL,
			eID_RENDERER_RUN_TESTS_NOTRUN,
			eID_RENDERER_RUN_TESTS_ACCEPTABLE,
			eID_RENDERER_RUN_TESTS_CRASHED,
			eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RENDERER_RUN_TESTS_OUTDATED,
			eID_RENDERER_UPDATE_ENGINE,
			eID_RENDERER_UPDATE_SCENE,
			eID_RENDERER_CREATE_CATEGORY,
			eID_ALL_RUN_TESTS_ALL,
			eID_ALL_RUN_TESTS_NOTRUN,
			eID_ALL_RUN_TESTS_ACCEPTABLE,
			eID_ALL_RUN_TESTS_CRASHED,
			eID_ALL_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_ALL_RUN_TESTS_OUTDATED,
			eID_ALL_UPDATE_ENGINE,
			eID_ALL_UPDATE_SCENE,
			eID_CANCEL,
			eID_TESTS_BOOK,
			eID_DB_NEW_RENDERER,
			eID_DB_NEW_CATEGORY,
			eID_DB_NEW_TEST,
			eID_DB_EXPORT_LATEST_TIMES,
			eID_EDIT_CONFIG,
			eID_GIT,
		};

		class TestProcess
			: public wxProcess
		{
		public:
			TestProcess( MainFrame * mainframe
				, int flags );

			void OnTerminate( int pid, int status )override;

		private:
			MainFrame * m_mainframe;
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
		explicit MainFrame( PluginPtr plugin );
		~MainFrame()override;

		void initialise();

		TestTreeModelNode * getTestNode( DatabaseTest const & test );
		wxDataViewItem getTestItem( DatabaseTest const & test );

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
		void doInitMenus();
		void doInitMenuBar();
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
		void doRunAllTests();
		void doRunTests( TestStatus filter );
		void doRunAllTestsBut( TestStatus filter );
		void doRunAllOutdatedTests();
		void doUpdateAllEngineDate();
		void doUpdateAllSceneDate();
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
		void doEditConfig();
		void onTestRunEnd( int status );
		void onTestDisplayEnd( int status );
		void onTestDiffEnd( TestTimes const & times );
		bool onTestProcessEnd( int pid, int status );

		void onClose( wxCloseEvent & evt );
		void onTestsPageChange( wxAuiNotebookEvent & evt );
		void onTestsMenuOption( wxCommandEvent & evt );
		void onDatabaseMenuOption( wxCommandEvent & evt );
		void onConfigMenuOption( wxCommandEvent & evt );
		void onProcessEnd( wxProcessEvent & evt );
		void onTestUpdateTimer( wxTimerEvent & evt );
		void onCategoryUpdateTimer( wxTimerEvent & evt );
		void onSize( wxSizeEvent & evt );

	private:
		wxAuiManager m_auiManager;
		PluginPtr m_plugin;
		Config & m_config;
		FileSystemPtr m_fileSystem;
		TestDatabase m_database;
		Tests m_tests;
		std::map< Renderer, RendererPage *, LessIdValue > m_testsPages;
		wxAuiNotebook * m_testsBook{};
		RendererPage * m_selectedPage{};
		wxStaticText * m_statusText{};
		wxGauge * m_testProgress{};
		std::unique_ptr< wxMenu > m_testMenu{};
		std::unique_ptr< wxMenu > m_categoryMenu{};
		std::unique_ptr< wxMenu > m_rendererMenu{};
		std::unique_ptr< wxMenu > m_allMenu{};
		std::unique_ptr< wxMenu > m_busyTestMenu{};
		std::unique_ptr< wxMenu > m_busyCategoryMenu{};
		std::unique_ptr< wxMenu > m_busyRendererMenu{};
		std::unique_ptr< wxMenu > m_busyAllMenu{};
		wxMenu * m_barTestMenu{};
		wxMenu * m_barCategoryMenu{};
		wxMenu * m_barRendererMenu{};
		wxMenu * m_barAllMenu{};
		RunningTest m_runningTest;
		wxTimer * m_timerKillRun{};
		std::atomic_bool m_cancelled;
		wxTimer * m_testUpdater;
		wxTimer * m_categoriesUpdater;
	};
}

#endif
