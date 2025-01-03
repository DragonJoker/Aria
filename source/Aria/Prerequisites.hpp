/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Prerequisites_HPP___
#define ___CTP_Prerequisites_HPP___

#include <AriaLib/Prerequisites.hpp>

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <wx/event.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria
{
	enum class DiffMode
	{
		eRaw,
		eLogarithmic,
		eFlip,
	};

	class CategoryPanel;
	class LayeredPanel;
	class MainFrame;
	class TestsMainPanel;
	class RendererPage;
	class TestPanel;
	class TestTreeModelNode;
	class TestTreeModel;

	namespace run
	{
		class RunTreeModelNode;
		class RunTreeModel;
	}

	struct StatusName
	{
		NodeType type;
		RendererTestsCounts const * rendererCounts;
		TestsCounts const * categoryCounts;
		std::string name;
		TestStatus status{};
		bool outOfEngineDate{};
		bool outOfSceneDate{};
		bool ignored{};

		static uint32_t getStatusIndex( bool ignoreResult
			, TestStatus status );
		static uint32_t getTestStatusIndex( Config const & config
			, DatabaseTest const & test );
	};

	struct Menus
	{
		enum IDs
		{
			eID_TEST_RUN,
			eID_TEST_RUN_5,
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
			eID_TEST_DELETE_REFERENCE,
			eID_CATEGORY_RUN_TESTS_ALL,
			eID_CATEGORY_RUN_TESTS_ALL_5,
			eID_CATEGORY_RUN_TESTS_NOTRUN,
			eID_CATEGORY_RUN_TESTS_ACCEPTABLE,
			eID_CATEGORY_RUN_TESTS_UNACCEPTABLE,
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
			eID_RENDERER_RUN_TESTS_ALL_5,
			eID_RENDERER_RUN_TESTS_NOTRUN,
			eID_RENDERER_RUN_TESTS_ACCEPTABLE,
			eID_RENDERER_RUN_TESTS_UNACCEPTABLE,
			eID_RENDERER_RUN_TESTS_CRASHED,
			eID_RENDERER_RUN_TESTS_ALL_BUT_NEGLIGIBLE,
			eID_RENDERER_RUN_TESTS_OUTDATED,
			eID_RENDERER_UPDATE_ENGINE,
			eID_RENDERER_UPDATE_SCENE,
			eID_RENDERER_CREATE_CATEGORY,
			eID_CANCEL_RUNS,
		};
		struct SubMenus
		{
			wxMenu * test;
			wxMenu * category;
			wxMenu * renderer;

			void bind( wxObjectEventFunction testFunc
				, wxObjectEventFunction catFunc
				, wxObjectEventFunction rendFunc
				, wxEvtHandler * handler )const;
			void unbind( wxObjectEventFunction testFunc
				, wxObjectEventFunction catFunc
				, wxObjectEventFunction rendFunc
				, wxEvtHandler * handler )const;
		};

		SubMenus base;
		SubMenus busy;
		SubMenus bar;
		wxStatusBar * statusBar;

		void bind( wxObjectEventFunction testFunc
			, wxObjectEventFunction catFunc
			, wxObjectEventFunction rendFunc
			, wxEvtHandler * handler )const;
		void unbind( wxObjectEventFunction testFunc
			, wxObjectEventFunction catFunc
			, wxObjectEventFunction rendFunc
			, wxEvtHandler * handler )const;
	};

	using TestTreeModelNodePtrArray = std::vector< TestTreeModelNode * >;

	struct TestNode
	{
		DatabaseTest * test;
		TestStatus status;
		TestTreeModelNode * node;
	};

	bool isTestNode( TestTreeModelNode const & node );
	bool isCategoryNode( TestTreeModelNode const & node );
	bool isRendererNode( TestTreeModelNode const & node );
}

#endif
