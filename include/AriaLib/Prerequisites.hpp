/*
See LICENSE file in root folder
*/
#ifndef ___Aria_Prerequisites_HPP___
#define ___Aria_Prerequisites_HPP___

#include "Database/DbPrerequisites.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
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
#include "AriaLib/EndExternHeaderGuard.hpp"

#pragma warning( push )
#pragma warning( disable: 5264 )

class wxFileName;

namespace aria
{
	static const wxColour PANEL_BACKGROUND_COLOUR = wxColour( 30, 30, 30 );
	static const wxColour PANEL_FOREGROUND_COLOUR = wxColour( 220, 220, 220 );
	static const wxColour BORDER_COLOUR = wxColour( 90, 90, 90 );
	static const wxColour INACTIVE_TAB_COLOUR = wxColour( 60, 60, 60 );
	static const wxColour INACTIVE_TEXT_COLOUR = wxColour( 200, 200, 200 );
	static const wxColour ACTIVE_TAB_COLOUR = wxColour( 51, 153, 255, 255 );
	static const wxColour ACTIVE_TEXT_COLOUR = wxColour( 255, 255, 255, 255 );

	static constexpr int GridLineSize = 22;

	using Microseconds = std::chrono::microseconds;

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
		AriaLib_API size_t operator()( std::string const & v )const;
	};

	struct LessNoCase
	{
		AriaLib_API bool operator()( const char lhs, const char rhs )const;
		AriaLib_API bool operator()( const char * lhs, const char * rhs, size_t minSize )const;
		AriaLib_API bool operator()( const char * lhs, const char * rhs )const;
		AriaLib_API bool operator()( std::string const & lhs, std::string const & rhs )const;
	};

	struct EqualNoCase
	{
		AriaLib_API bool operator()( const char lhs, const char rhs )const;
		AriaLib_API bool operator()( const char * lhs, const char * rhs, size_t minSize )const;
		AriaLib_API bool operator()( const char * lhs, const char * rhs )const;
		AriaLib_API bool operator()( std::string const & lhs, std::string const & rhs )const;
	};

	using ByteArray = db::ByteArray;
	using PathArray = std::vector< wxFileName >;

	class DatabaseTest;
	class RendererTestRuns;
	class AllTestRuns;
	class FileSystem;
	class Plugin;
	class PluginConfig;
	class PluginFactory;
	class TestDatabase;

	struct Config;
	struct IdValue;
	struct StatusName;
	struct Test;
	struct AllTestsCounts;
	struct Options;
	struct PluginLib;
	struct RendererTestsCounts;
	struct TestsCounts;
	struct TestNode;
	struct TestRun;
	struct TestsOptions;

	template< typename SignalT >
	class Connection;
	template< typename Function >
	class Signal;

	template< typename ValueT >
	struct CountedValueT;

	using CountedUInt = CountedValueT< uint32_t >;

	using OptionsPtr = std::unique_ptr< Options >;
	using PluginPtr = std::unique_ptr< Plugin >;

	using FilterFunc = std::function< bool( DatabaseTest const & ) >;

	using TraverseDirFunction = std::function< wxDirTraverseResult( wxString const & path ) >;
	using HitFileFunction = std::function< void( wxString const & folder, wxString const & name ) >;
	using FileFilterFunction = std::function< bool( wxString const & folder, wxString const & name ) >;

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
	using TestsCountsCategoryMap = std::map< Category, TestsCounts, LessIdValue >;
	using TestsCountsRendererMap = std::map< Renderer, RendererTestsCounts, LessIdValue >;
	using DatabaseTestArray = std::vector< DatabaseTest * >;

	struct TestTimes
	{
		Host const * host{};
		Microseconds total{};
		Microseconds avg{};
		Microseconds last{};
	};

	struct Tests
	{
		TestMap tests;
		AllTestRunsPtr runs;
		AllTestsCountsPtr counts;
	};

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

	AriaLib_API wxString getExtension( wxString const & name );
	AriaLib_API wxFileName getFolderName( TestStatus value );
	AriaLib_API TestStatus getStatus( std::string const & name );

	AriaLib_API void traverseDirectory( wxFileName const & folderPath
		, TraverseDirFunction directoryFunction
		, HitFileFunction fileFunction );
	AriaLib_API PathArray filterDirectoryFiles( wxFileName const & folderPath
		, FileFilterFunction onFile
		, bool recursive = false );
	AriaLib_API PathArray listDirectoryFiles( wxFileName const & folderPath
		, bool recursive = false );

	AriaLib_API wxFileName getResultFolder( Test const & test );
	AriaLib_API wxFileName getResultFolder( Test const & test
		, Category category );
	AriaLib_API wxFileName getCompareFolder( Test const & test );
	AriaLib_API wxFileName getReferenceFolder( Test const & test );
	AriaLib_API wxFileName getReferenceName( Test const & test );
	AriaLib_API PathArray findTestResults( Test const & test
		, wxFileName const & work );
	AriaLib_API std::string getDetails( Test const & test );
	AriaLib_API wxString getProgressDetails( Test const & test );
	AriaLib_API wxString getProgressDetails( wxString const & catName
		, wxString const & testName );
	AriaLib_API std::string toTestPrefix( int32_t id );
	AriaLib_API wxFileName getResultFolder( TestRun const & test );
	AriaLib_API wxFileName getResultFolder( TestRun const & test
		, Category category );
	AriaLib_API wxFileName getResultName( TestRun const & test );
	AriaLib_API wxFileName getCompareFolder( TestRun const & test );
	AriaLib_API wxFileName getCompareName( TestRun const & test );
	AriaLib_API wxFileName getReferenceFolder( TestRun const & test );
	AriaLib_API wxFileName getReferenceName( TestRun const & test );
	AriaLib_API PathArray findTestResults( TestRun const & test
		, wxFileName const & work );
	AriaLib_API std::string getDetails( TestRun const & test );
	AriaLib_API wxString getProgressDetails( DatabaseTest const & test );
	AriaLib_API wxString getProgressDetails( wxString const & catName
		, wxString const & testName
		, wxString const & rendName
		, db::DateTime const & runDate );

	AriaLib_API wxString makeWxString( std::string const & in );
	AriaLib_API std::string makeStdString( wxString const & in );

	AriaLib_API db::DateTime getFileDate( wxFileName const & imgPath );

	AriaLib_API wxFileName operator/( wxString const & lhs, wxString const & rhs );
	AriaLib_API wxFileName operator/( wxFileName const & lhs, wxString const & rhs );
	AriaLib_API wxFileName operator/( wxFileName const & lhs, wxFileName const & rhs );
	AriaLib_API std::ostream & operator<<( std::ostream & stream, wxFileName const & value );
	AriaLib_API wxString & operator<<( wxString & stream, wxFileName const & value );

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

	inline TestsCountsType getType( TestStatus status )
	{
		if ( isRunning( status ) )
		{
			return TestsCountsType::eRunning;
		}

		return TestsCountsType( status );
	}
}

#pragma warning( pop )

#endif
