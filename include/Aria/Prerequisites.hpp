/*
See LICENSE file in root folder
*/
#ifndef ___CTP_Prerequisites_HPP___
#define ___CTP_Prerequisites_HPP___

#include "Database/DbPrerequisites.hpp"

#pragma warning( push )
#pragma warning( disable: 4251 )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4464 )
#pragma warning( disable: 4371 )
#include <wx/wx.h>
#include <wx/cmdline.h>
#include <wx/colour.h>
#include <wx/datetime.h>
#include <wx/dir.h>
#include <wx/dynlib.h>
#include <wx/fileconf.h>
#include <wx/string.h>

#include <chrono>
#include <functional>
#include <list>
#include <map>
#include <ostream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#pragma warning( pop )

#if !defined( _WIN32 )
#	define Aria_API
#else
#	if defined( Aria_EXPORTS )
#		define Aria_API __declspec( dllexport )
#	else
#		define Aria_API __declspec( dllimport )
#	endif
#endif

class wxFileName;

namespace aria
{
	using Microseconds = std::chrono::microseconds;

	enum class DiffMode
	{
		eRaw,
		eLogarithmic,
		eFlip,
	};

	enum class TestStatus : uint32_t
	{
		eNotRun,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eUnprocessed,
		eCrashed,
		ePending,
		eRunning_0,
		eRunning_1,
		eRunning_2,
		eRunning_3,
		eRunning_4,
		eRunning_5,
		eRunning_6,
		eRunning_7,
		eRunning_8,
		eRunning_9,
		eRunning_10,
		eRunning_11,
		eCount,
		eRunning_Begin = eRunning_0,
		eRunning_End = eRunning_11,
	};
	static_assert( uint32_t( TestStatus::eCount ) == uint32_t( TestStatus::eRunning_End ) + 1u
		, "Running status should always end TestStatus enumeration" );

	enum class RunStatus
	{
		eNotRun,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eUnprocessed,
		eCrashed,
		eCount,
	};

	struct Host;
	struct Run
	{
		uint32_t id;
		Host * host;
		RunStatus status;
		db::DateTime runDate;
		Microseconds totalTime;
		Microseconds avgTime;
		Microseconds lastTime;
	};

	using RunMap = std::map< wxDateTime, Run >;

	wxString getExtension( wxString const & name );
	wxFileName getFolderName( TestStatus value );
	TestStatus getStatus( std::string const & name );

	inline bool isCrashed( TestStatus value )
	{
		return value == TestStatus::eCrashed;
	}

	inline bool isPending( TestStatus value )
	{
		return value == TestStatus::ePending;
	}

	inline bool isRunning( TestStatus value )
	{
		return value >= TestStatus::eRunning_Begin
			&& value <= TestStatus::eRunning_End;
	}

	inline std::string getName( TestStatus status )
	{
		switch ( status )
		{
		case aria::TestStatus::eNotRun:
			return "not_run";
		case aria::TestStatus::eNegligible:
			return "negligible";
		case aria::TestStatus::eAcceptable:
			return "acceptable";
		case aria::TestStatus::eUnacceptable:
			return "unacceptable";
		case aria::TestStatus::eUnprocessed:
			return "unprocessed";
		case aria::TestStatus::eCrashed:
			return "crashed";
		case aria::TestStatus::ePending:
			return "pending";
		case aria::TestStatus::eRunning_0:
			return "running_0";
		case aria::TestStatus::eRunning_1:
			return "running_1";
		case aria::TestStatus::eRunning_2:
			return "running_2";
		case aria::TestStatus::eRunning_3:
			return "running_3";
		case aria::TestStatus::eRunning_4:
			return "running_4";
		case aria::TestStatus::eRunning_5:
			return "running_5";
		case aria::TestStatus::eRunning_6:
			return "running_6";
		case aria::TestStatus::eRunning_7:
			return "running_7";
		case aria::TestStatus::eRunning_8:
			return "running_8";
		case aria::TestStatus::eRunning_9:
			return "running_9";
		case aria::TestStatus::eRunning_10:
			return "running_10";
		case aria::TestStatus::eRunning_11:
			return "running_11";
		default:
			return "unknown";
		}
	}

	enum TestsCountsType : uint32_t
	{
		eNotRun,
		eNegligible,
		eAcceptable,
		eUnacceptable,
		eUnprocessed,
		eCrashed,
		ePending,
		eRunning,
		eIgnored,
		eOutdated,
		eAll,
		eCount,
		eCountedInAllEnd = eIgnored,
	};

	inline TestsCountsType getType( TestStatus status )
	{
		if ( isRunning( status ) )
		{
			return TestsCountsType::eRunning;
		}

		return TestsCountsType( status );
	}

	enum class NodeType
	{
		eRenderer,
		eCategory,
		eTestRun,
	};

	struct IdValue
	{
		IdValue( int32_t id
			, std::string name )
			: id{ id }
			, name{ std::move( name ) }
		{
		}

		int32_t id;
		std::string name;
	};
	using IdValuePtr = std::unique_ptr< IdValue >;

	struct LessIdValue
	{
		size_t operator()( IdValue const & lhs, IdValue const & rhs )const
		{
			return lhs.id < rhs.id;
		}

		size_t operator()( IdValue const * lhs, IdValue const * rhs )const
		{
			return operator()( *lhs, *rhs );
		}

		size_t operator()( IdValuePtr const & lhs, IdValuePtr const & rhs )const
		{
			return operator()( *lhs, *rhs );
		}
	};

	struct HashNoCase
	{
		size_t operator()( std::string const & v )const;
	};

	struct LessNoCase
	{
		bool operator()( const char lhs, const char rhs )const;
		bool operator()( const char * lhs, const char * rhs, size_t minSize )const;
		bool operator()( const char * lhs, const char * rhs )const;
		bool operator()( std::string const & lhs, std::string const & rhs )const;
	};

	struct EqualNoCase
	{
		bool operator()( const char lhs, const char rhs )const;
		bool operator()( const char * lhs, const char * rhs, size_t minSize )const;
		bool operator()( const char * lhs, const char * rhs )const;
		bool operator()( std::string const & lhs, std::string const & rhs )const;
	};

	using ByteArray = db::ByteArray;
	using PathArray = std::vector< wxFileName >;

	template< typename SignalT >
	class Connection;
	template< typename Function >
	class Signal;

	class CategoryPanel;
	class DatabaseTest;
	class RendererTestRuns;
	class AllTestRuns;
	class FileSystem;
	class LayeredPanel;
	class MainFrame;
	class Plugin;
	class PluginConfig;
	class PluginFactory;
	class RendererPage;
	class TestDatabase;
	class TestPanel;
	class TestTreeModelNode;
	class TestTreeModel;

	struct Config;
	struct IdValue;
	struct StatusName;
	struct Test;
	struct AllTestsCounts;
	struct Options;
	struct PluginLib;
	struct RendererTestsCounts;
	struct CategoryTestsCounts;
	struct TestNode;
	struct TestRun;

	using PluginPtr = std::unique_ptr< Plugin >;

	namespace run
	{
		class RunTreeModelNode;
		class RunTreeModel;
	}

	using FilterFunc = std::function< bool( DatabaseTest const & ) >;

	using TraverseDirFunction = std::function< wxDirTraverseResult( wxString const & path ) >;
	using HitFileFunction = std::function< void( wxString const & folder, wxString const & name ) >;
	Aria_API void traverseDirectory( wxFileName const & folderPath
		, TraverseDirFunction directoryFunction
		, HitFileFunction fileFunction );
	using FileFilterFunction = std::function< bool( wxString const & folder, wxString const & name ) >;
	Aria_API PathArray filterDirectoryFiles( wxFileName const & folderPath
		, FileFilterFunction onFile
		, bool recursive = false );
	Aria_API PathArray listDirectoryFiles( wxFileName const & folderPath
		, bool recursive = false );

	using Platform = IdValue *;
	using Cpu = IdValue *;
	using Gpu = IdValue *;

	struct Host
	{
		int32_t id;
		Platform platform;
		Cpu cpu;
		Gpu gpu;
	};

	using HostPtr = std::unique_ptr< Host >;
	using FileSystemPtr = std::unique_ptr< FileSystem >;
	using TestPtr = std::unique_ptr< Test >;
	using AllTestsCountsPtr = std::shared_ptr< AllTestsCounts >;
	using AllTestRunsPtr = std::shared_ptr< AllTestRuns >;
	using Renderer = IdValue *;
	using Category = IdValue *;
	using Keyword = IdValue *;
	using RendererMap = std::unordered_map< std::string, IdValuePtr >;
	using CategoryMap = std::unordered_map< std::string, IdValuePtr >;
	using KeywordMap = std::unordered_map< std::string, IdValuePtr, HashNoCase >;
	using PlatformMap = std::unordered_map< std::string, IdValuePtr, HashNoCase >;
	using CpuMap = std::unordered_map< std::string, IdValuePtr, HashNoCase >;
	using GpuMap = std::unordered_map< std::string, IdValuePtr, HashNoCase >;
	using HostMap = std::unordered_map< int32_t, HostPtr >;
	using TestArray = std::vector< TestPtr >;
	using TestMap = std::map< Category, TestArray, LessIdValue >;
	using TestsCountsCategoryMap = std::map< Category, CategoryTestsCounts, LessIdValue >;
	using TestsCountsRendererMap = std::map< Renderer, RendererTestsCounts, LessIdValue >;

	template< typename ValueT >
	struct CountedValueT;
	template< typename ValueT >
	using CountedValueSignalFuncT = std::function< void( CountedValueT< ValueT > const & ) >;
	template< typename ValueT >
	using CountedValueSignalT = Signal< CountedValueSignalFuncT< ValueT > >;
	template< typename ValueT >
	using CountedValueConnectionT = Connection< CountedValueSignalT< ValueT > >;

	using CountedUInt = CountedValueT< uint32_t >;
	using CountedUIntSignal = CountedValueSignalT< uint32_t >;
	using CountedUIntConnection = CountedValueConnectionT< uint32_t >;

	struct TestTimes
	{
		Host const * host{};
		Microseconds total{};
		Microseconds avg{};
		Microseconds last{};
	};

	struct StatusName
	{
		NodeType type;
		RendererTestsCounts const * rendererCounts;
		CategoryTestsCounts const * categoryCounts;
		std::string name;
		TestStatus status{};
		bool outOfCastorDate{};
		bool outOfSceneDate{};
		bool ignored{};

		static uint32_t getStatusIndex( bool ignoreResult
			, TestStatus status );
		static uint32_t getTestStatusIndex( Config const & config
			, DatabaseTest const & test );
	};

	struct Tests
	{
		TestMap tests;
		AllTestRunsPtr runs;
		AllTestsCountsPtr counts;
	};

	static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
	static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
	static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
	static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
	static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
	static const wxColour ACTIVE_TAB_COLOUR = wxColour( 51, 153, 255, 255 );
	static const wxColour ACTIVE_TEXT_COLOUR = wxColour( 255, 255, 255, 255 );

	static const std::string DISPLAY_DATETIME_TIME = "%02d:%02d:%02d";
	static constexpr size_t DISPLAY_DATETIME_TIME_SIZE = 2u + 3u + 3u;
	static const std::string DISPLAY_DATETIME_DATE = "%04d-%02d-%02d";
	static constexpr size_t DISPLAY_DATETIME_DATE_SIZE = 4u + 3u + 3u;
	static const std::string DISPLAY_DATETIME = "%Y-%m-%d %H:%M:%S";
	static constexpr size_t DISPLAY_DATETIME_SIZE = 4u + 3u + 3u + 3u + 3u + 3u;

	using TestTreeModelNodePtrArray = std::vector< TestTreeModelNode * >;

	struct Config
	{
		Config( PluginConfig & plugin )
			: pluginConfig{ &plugin }
		{
		}

		PluginConfig * pluginConfig;
		wxFileName test;
		wxFileName work;
		wxFileName database;
		std::vector< wxString > renderers{ wxT( "vk" ), wxT( "gl" ), wxT( "d3d11" ) };
		bool initFromFolder{};
		uint32_t maxFrameCount{ 10u };
		wxString plugin;
	};

	struct TestRun
	{
		TestRun( Test * test
			, Renderer renderer
			, db::DateTime runDate
			, TestStatus status
			, db::DateTime engineDate
			, db::DateTime testDate
			, TestTimes times )
			: test{ test }
			, renderer{ renderer }
			, runDate{ std::move( runDate ) }
			, status{ std::move( status ) }
			, engineDate{ std::move( engineDate ) }
			, testDate{ std::move( testDate ) }
			, times{ std::move( times ) }
		{
		}

		Test * test;
		int32_t id{};
		Renderer renderer{};
		db::DateTime runDate;
		TestStatus status;
		db::DateTime engineDate;
		db::DateTime testDate;
		TestTimes times;
	};

	struct Test
		: IdValue
	{
		Test( int32_t id = {}
			, std::string name = {}
			, Category category = {}
			, bool ignoreResult = {} )
			: IdValue{ id, std::move( name ) }
			, category{ category }
			, ignoreResult{ ignoreResult }
		{
		}

		Category category{};
		bool ignoreResult{};
	};

	static constexpr size_t IgnoredIndex = 0u;
	static constexpr size_t AdditionalIndices = 1u;

	Aria_API wxFileName getResultFolder( Test const & test );
	Aria_API wxFileName getResultFolder( Test const & test
		, Category category );
	Aria_API wxFileName getCompareFolder( Test const & test );
	Aria_API wxFileName getReferenceFolder( Test const & test );
	Aria_API wxFileName getReferenceName( Test const & test );
	Aria_API PathArray findTestResults( Test const & test
		, wxFileName const & work );
	Aria_API std::string getDetails( Test const & test );
	Aria_API wxString getProgressDetails( Test const & test );
	Aria_API wxString getProgressDetails( wxString const & catName
		, wxString const & testName );
	Aria_API std::string toTestPrefix( int32_t id );
	Aria_API wxFileName getResultFolder( TestRun const & test );
	Aria_API wxFileName getResultFolder( TestRun const & test
		, Category category );
	Aria_API wxFileName getResultName( TestRun const & test );
	Aria_API wxFileName getCompareFolder( TestRun const & test );
	Aria_API wxFileName getCompareName( TestRun const & test );
	Aria_API wxFileName getReferenceFolder( TestRun const & test );
	Aria_API wxFileName getReferenceName( TestRun const & test );
	Aria_API PathArray findTestResults( TestRun const & test
		, wxFileName const & work );
	Aria_API std::string getDetails( TestRun const & test );
	Aria_API wxString getProgressDetails( DatabaseTest const & test );
	Aria_API wxString getProgressDetails( wxString const & catName
		, wxString const & testName
		, wxString const & rendName
		, db::DateTime const & runDate );

	struct TestNode
	{
		DatabaseTest * test;
		TestStatus status;
		TestTreeModelNode * node;
	};

	Aria_API wxString makeWxString( std::string const & in );
	Aria_API std::string makeStdString( wxString const & in );

	Aria_API db::DateTime getFileDate( wxFileName const & imgPath );

	bool isTestNode( TestTreeModelNode const & node );
	bool isCategoryNode( TestTreeModelNode const & node );
	bool isRendererNode( TestTreeModelNode const & node );

	Aria_API wxFileName operator/( wxString const & lhs, wxString const & rhs );
	Aria_API wxFileName operator/( wxFileName const & lhs, wxString const & rhs );
	Aria_API wxFileName operator/( wxFileName const & lhs, wxFileName const & rhs );
	Aria_API std::ostream & operator<<( std::ostream & stream, wxFileName const & value );
	Aria_API wxString & operator<<( wxString & stream, wxFileName const & value );
}

#endif
