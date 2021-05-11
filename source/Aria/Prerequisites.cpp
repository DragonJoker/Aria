#include "Prerequisites.hpp"

#include "DatabaseTest.hpp"
#include "StringUtils.hpp"
#include "TestDatabase.hpp"
#include "TestsCounts.hpp"
#include "Database/DbDateTimeHelpers.hpp"
#include "Model/TreeModelNode.hpp"

#include <wx/dir.h>
#include <wx/filefn.h>

#if !defined( WIN32 )
#	include <strings.h>
#endif

namespace aria
{
	namespace
	{
		static const std::string FOLDER_DATETIME = "%Y-%m-%d_%H-%M-%S";
		static constexpr size_t FOLDER_DATETIME_SIZE = 4u + 3u + 3u + 3u + 3u + 3u;
	}

	size_t HashNoCase::operator()( std::string const & v )const
	{
		return std::hash< std::string >{}( lowerCase( v ) );
	}

	bool LessNoCase::operator()( const char lhs, const char rhs )const
	{
		return operator()( &lhs, &rhs, 1 );
	}

	bool LessNoCase::operator()( const char * lhs, const char * rhs, size_t minSize )const
	{
#if defined( WIN32 )
		return _strnicmp( lhs, rhs, minSize ) < 0;
#else
		return strncasecmp( lhs, rhs, minSize ) < 0;
#endif
	}

	bool LessNoCase::operator()( const char * lhs, const char * rhs )const
	{
		return operator()( lhs, rhs, std::min( strlen( lhs ), strlen( rhs ) ) );
	}

	bool LessNoCase::operator()( std::string const & lhs, std::string const & rhs )const
	{
		return operator()( lhs.data(), rhs.data(), std::min( lhs.size(), rhs.size() ) );
	}

	bool EqualNoCase::operator()( const char lhs, const char rhs )const
	{
		return operator()( &lhs, &rhs, 1 );
	}
	
	bool EqualNoCase::operator()( const char * lhs, const char * rhs, size_t minSize )const
	{
#if defined( WIN32 )
		return _strnicmp( lhs, rhs, minSize ) == 0;
#else
		return strncasecmp( lhs, rhs, minSize ) == 0;
#endif
	}

	bool EqualNoCase::operator()( const char * lhs, const char * rhs )const
	{
		return operator()( lhs, rhs, std::min( strlen( lhs ), strlen( rhs ) ) );
	}

	bool EqualNoCase::operator()( std::string const & lhs, std::string const & rhs )const
	{
		return operator()( lhs.data(), rhs.data(), std::min( lhs.size(), rhs.size() ) );
	}

	wxString getExtension( wxString const & name )
	{
		auto find = name.find_last_of( "." );
		return find != wxString::npos
			? name.substr( find + 1u )
			: wxEmptyString;
	}

	wxFileName getFolderName( TestStatus value )
	{
		switch ( value )
		{
		case TestStatus::eNotRun:
			return wxFileName{ "NotRun" };
		case TestStatus::eNegligible:
			return wxFileName{ "Negligible" };
		case TestStatus::eAcceptable:
			return wxFileName{ "Acceptable" };
		case TestStatus::eUnacceptable:
			return wxFileName{ "Unacceptable" };
		case TestStatus::eUnprocessed:
			return wxFileName{ "Unprocessed" };
		case TestStatus::eCrashed:
			return wxFileName{ "Crashed" };
		default:
			assert( false );
			return wxFileName{};
		}
	}

	TestStatus getStatus( std::string const & name )
	{
		static std::map< std::string, TestStatus > const folders
		{
			{ "Negligible", TestStatus::eNegligible },
			{ "Acceptable", TestStatus::eAcceptable },
			{ "Unacceptable", TestStatus::eUnacceptable },
			{ "Unprocessed", TestStatus::eUnprocessed },
		};
		auto it = folders.find( name );

		if ( it == folders.end() )
		{
			return TestStatus::eUnprocessed;
		}

		return it->second;
	}

	//*********************************************************************************************

	uint32_t StatusName::getStatusIndex( bool ignoreResult
		, TestStatus status )
	{
		uint32_t result{};

		if ( ( !ignoreResult )
			|| ( !isPending( status ) && !isRunning( status ) ) )
		{
			result = size_t( status ) + AdditionalIndices;
		}
		else if ( ignoreResult )
		{
			result = IgnoredIndex;
		}

		return ( result << 2 );
	}

	uint32_t StatusName::getTestStatusIndex( Config const & config
		, DatabaseTest const & test )
	{
		return getStatusIndex( test.getIgnoreResult(), test.getStatus() )
			| ( test.checkOutOfCastorDate()
				? 0x01u
				: 0x00u )
			| ( test.checkOutOfSceneDate()
				? 0x02u
				: 0x00u );
	}

	//*********************************************************************************************

	db::DateTime getSceneDate( Config const & config, Test const & test )
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	db::DateTime getSceneDate( Config const & config, TestRun const & test )
	{
		return getFileDate( config.test / getSceneFile( test ) );
	}

	bool isOutOfEngineDate( Config const & config, TestRun const & test )
	{
		return ( !db::date_time::isValid( test.engineData ) )
			|| test.engineData < getFileDate( config.engine );
	}

	bool isOutOfSceneDate( Config const & config, TestRun const & test )
	{
		return ( !db::date_time::isValid( test.sceneDate ) )
			|| test.sceneDate < getSceneDate( config, test );
	}

	bool isOutOfDate( Config const & config, TestRun const & test )
	{
		return isOutOfSceneDate( config, test )
			|| isOutOfEngineDate( config, test );
	}

	void updateEngineRefDate( Config & config )
	{
		config.engineRefDate = getFileDate( config.engine );
		assert( db::date_time::isValid( config.engineRefDate ) );
	}

	wxFileName getSceneFile( Test const & test )
	{
		return wxFileName{ test.category->name } / getSceneName( test );
	}

	wxFileName getSceneFile( TestRun const & test )
	{
		return getSceneFile( *test.test );
	}

	wxFileName getSceneName( Test const & test )
	{
		return wxFileName{ test.name + ".cscn" };
	}

	wxFileName getSceneName( TestRun const & test )
	{
		return getSceneName( *test.test );
	}

	wxFileName getResultFolder( Test const & test )
	{
		return getResultFolder( test, test.category );
	}

	wxFileName getResultFolder( Test const & test
		, Category category )
	{
		return wxFileName{ "Result" } / category->name;
	}

	wxFileName getResultFolder( TestRun const & test )
	{
		return getResultFolder( *test.test, test.test->category ) / getFolderName( test.status );
	}

	wxFileName getResultFolder( TestRun const & test
		, Category category )
	{
		return getResultFolder( *test.test, category ) / getFolderName( test.status );
	}

	wxFileName getResultName( TestRun const & test )
	{
		return wxFileName{ test.test->name + "_" + test.renderer->name + ".png" };
	}

	wxFileName getCompareFolder( Test const & test )
	{
		return wxFileName{ test.category->name } / wxT( "Compare" );
	}

	wxFileName getCompareFolder( TestRun const & test )
	{
		return getCompareFolder( *test.test ) / getFolderName( test.status );
	}

	wxFileName getCompareName( TestRun const & test )
	{
		return wxFileName{ test.test->name + "_" + test.renderer->name + ".png" };
	}

	wxFileName getReferenceFolder( Test const & test )
	{
		return wxFileName{ test.category->name };
	}

	wxFileName getReferenceFolder( TestRun const & test )
	{
		return getReferenceFolder( *test.test );
	}

	wxFileName getReferenceName( Test const & test )
	{
		return wxFileName{ test.name + "_ref.png" };
	}

	wxFileName getReferenceName( TestRun const & test )
	{
		return getReferenceName( *test.test );
	}

	std::string getDetails( Test const & test )
	{
		return test.category->name
			+ " - " + test.name;
	}

	wxString getProgressDetails( Test const & test )
	{
		return getProgressDetails( test.category->name, test.name );
	}

	wxString getProgressDetails( wxString const & catName
		, wxString const & testName )
	{
		return wxT( "- Category: " ) + catName
			+ wxT( "\n" ) + wxT( "- Test: " ) + testName;
	}

	void traverseDirectory( wxFileName const & folderPath
		, TraverseDirFunction directoryFunction
		, HitFileFunction fileFunction )
	{
		class DirTraverser
			: public wxDirTraverser
		{
		public:
			DirTraverser( TraverseDirFunction directoryFunction
				, HitFileFunction fileFunction )
				: m_directoryFunction{ directoryFunction }
				, m_fileFunction{ fileFunction }
			{
			}

			wxDirTraverseResult OnFile( const wxString & filename )override
			{
				wxFileName filePath{ filename };
				m_fileFunction( filePath.GetPath(), filePath.GetName() + "." + filePath.GetExt() );
				return wxDIR_CONTINUE;
			}

			wxDirTraverseResult OnDir( const wxString & dirname )override
			{
				return m_directoryFunction( dirname );
			}

		private:
			TraverseDirFunction m_directoryFunction;
			HitFileFunction m_fileFunction;
		};

		DirTraverser traverser{ directoryFunction, fileFunction };
		wxDir dir{ folderPath.GetFullPath() };
		dir.Traverse( traverser );
	}

	PathArray filterDirectoryFiles( wxFileName const & folderPath
		, FileFilterFunction onFile
		, bool recursive )
	{
		PathArray files;
		HitFileFunction fileFunction = [&files, onFile]( wxString const & folder
			, wxString const & name )
		{
			if ( onFile( folder, name ) )
			{
				files.push_back( folder / name );
			}

			return true;
		};
		TraverseDirFunction directoryFunction;

		if ( recursive )
		{
			directoryFunction = [&fileFunction, &directoryFunction]( wxString const & path )
			{
				return wxDIR_CONTINUE;
			};
		}
		else
		{
			directoryFunction = []( wxString const & path )
			{
				return wxDIR_IGNORE;
			};
		}

		traverseDirectory( folderPath
			, directoryFunction
			, fileFunction );
		return files;
	}

	PathArray findTestResults( Test const & test
		, wxFileName const & work )
	{
		std::vector< TestStatus > statuses
		{
			TestStatus::eNegligible,
			TestStatus::eAcceptable,
			TestStatus::eUnacceptable,
			TestStatus::eUnprocessed,
		};
		FileFilterFunction filterFile = [&test]( wxString const & folder
			, wxString const & name )
		{
			return name.find( test.name ) == ( FOLDER_DATETIME_SIZE + 1u );
		};
		PathArray result;
		auto baseFolder = work / getResultFolder( test );

		for ( auto & status : statuses )
		{
			auto matches = filterDirectoryFiles( baseFolder / getFolderName( status )
				, filterFile
				, true );
			result.insert( result.end(), matches.begin(), matches.end() );
		}

		return result;
	}

	std::string getDetails( TestRun const & test )
	{
		return test.renderer->name
			+ " - " + getDetails( *test.test )
			+ " - " + ( db::date_time::isValid( test.runDate )
				? db::date_time::format( test.runDate, DISPLAY_DATETIME )
				: std::string{} );
	}

	wxString getProgressDetails( DatabaseTest const & test )
	{
		return getProgressDetails( makeWxString( test.getCategory()->name )
			, makeWxString( test.getName() )
			, makeWxString( test->renderer->name )
			, test->runDate );
	}

	wxString getProgressDetails( wxString const & catName
		, wxString const & testName
		, wxString const & rendName
		, db::DateTime const & runDate )
	{
		return wxT( "- Renderer: " ) + rendName
			+ wxT( "\n" ) + getProgressDetails( catName, testName )
			+ wxT( "\n- Run date: " ) + ( db::date_time::isValid( runDate )
				? db::date_time::format( runDate, DISPLAY_DATETIME )
				: std::string{} );
	}

	PathArray findTestResults( TestRun const & test
		, wxFileName const & work )
	{
		std::vector< TestStatus > statuses
		{
			TestStatus::eNegligible,
			TestStatus::eAcceptable,
			TestStatus::eUnacceptable,
			TestStatus::eUnprocessed,
		};
		FileFilterFunction filterFile = [&test]( wxString const & folder
			, wxString const & name )
		{
			return name.find( test.test->name ) == ( FOLDER_DATETIME_SIZE + 1u );
		};
		PathArray result;
		auto baseFolder = work / getResultFolder( *test.test );

		for ( auto & status : statuses )
		{
			auto matches = filterDirectoryFiles( baseFolder / getFolderName( status )
				, filterFile
				, true );
			result.insert( result.end(), matches.begin(), matches.end() );
		}

		return result;
	}

	wxString makeWxString( std::string const & in )
	{
		return wxString{ in.c_str(), wxMBConvUTF8{} };
	}

	std::string makeStdString( wxString const & in )
	{
		return in.char_str( wxMBConvUTF8{} ).data();
	}

	db::DateTime getFileDate( wxFileName const & imgPath )
	{
		return imgPath.GetModificationTime();
	}

	bool isDateTime( wxString const & value
		, db::DateTime & result )
	{
		return db::date_time::isDateTime( value, FOLDER_DATETIME, result );
	}

	bool isTestNode( TreeModelNode const & node )
	{
		return node.test != nullptr;
	}

	bool isCategoryNode( TreeModelNode const & node )
	{
		return node.category
			&& node.renderer
			&& !node.isRootNode();
	}

	bool isRendererNode( TreeModelNode const & node )
	{
		return node.renderer
			&& node.isRootNode();
	}

	wxFileName getTestFileName( wxFileName const & folder
		, Test const & test )
	{
		return folder / getSceneFile( test );
	}

	wxFileName getTestFileName( wxFileName const & folder
		, DatabaseTest const & test )
	{
		return getTestFileName( folder, *test->test );
	}

	wxFileName operator/( wxString const & lhs, wxString const & rhs )
	{
		return lhs + wxFileName::GetPathSeparator() + rhs;
	}

	wxFileName operator/( wxFileName const & lhs, wxString const & rhs )
	{
		return lhs.GetFullPath() + wxFileName::GetPathSeparator() + rhs;
	}

	wxFileName operator/( wxFileName const & lhs, wxFileName const & rhs )
	{
		return lhs.GetFullPath() + wxFileName::GetPathSeparator() + rhs.GetFullPath();
	}
}
