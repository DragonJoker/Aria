/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Prerequisites_HPP___
#define ___CTP_Prerequisites_HPP___

#include <AriaLib/Prerequisites.hpp>

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
		CategoryTestsCounts const * categoryCounts;
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

	static const std::string DISPLAY_DATETIME_TIME = "%02d:%02d:%02d";
	static constexpr size_t DISPLAY_DATETIME_TIME_SIZE = 2u + 3u + 3u;
	static const std::string DISPLAY_DATETIME_DATE = "%04d-%02d-%02d";
	static constexpr size_t DISPLAY_DATETIME_DATE_SIZE = 4u + 3u + 3u;
	static const std::string DISPLAY_DATETIME = "%Y-%m-%d %H:%M:%S";
	static constexpr size_t DISPLAY_DATETIME_SIZE = 4u + 3u + 3u + 3u + 3u + 3u;

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
