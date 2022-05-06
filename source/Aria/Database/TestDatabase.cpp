#include "Database/TestDatabase.hpp"

#include "Database/DatabaseTest.hpp"
#include "Database/DbResult.hpp"
#include "Database/DbStatement.hpp"
#include "FileSystem/FileSystem.hpp"

#pragma warning( push )
#pragma warning( disable:4251 )
#pragma warning( disable:4365 )
#pragma warning( disable:4371 )
#pragma warning( disable:4464 )
#include <wx/dir.h>
#include <wx/progdlg.h>
#pragma warning( pop )

#include <set>
#include <unordered_map>

namespace aria
{
	//*********************************************************************************************

	namespace testdb
	{
		template< typename HashT >
		static IdValue * getIdValue( std::string const & name
			, std::unordered_map< std::string, IdValuePtr, HashT > & map
			, TestDatabase::InsertIdValue & insertIdValue )
		{
			auto ires = map.emplace( name, nullptr );

			if ( ires.second )
			{
				ires.first->second = std::make_unique< IdValue >( 0, name );
			}

			auto idValue = ires.first->second.get();

			if ( idValue->id == 0 )
			{
				idValue->id = insertIdValue.insert( idValue->name );
			}

			return idValue;
		}

		template< typename HashT >
		static auto findId( std::unordered_map< std::string, IdValuePtr, HashT > const & map
			, int32_t id )
		{
			return std::find_if( map.begin()
				, map.end()
				, [id]( auto & lookup )
				{
					return lookup.second->id == id;
				} );
		}

		static Host const * findHost( HostMap const & hosts
			, int32_t id )
		{
			auto it = std::find_if( hosts.begin()
				, hosts.end()
				, [id]( auto & lookup )
				{
					return lookup.second->id == id;
				} );

			if ( it == hosts.end() )
			{
				return nullptr;
			}

			return it->second.get();
		}

		template< typename HashT >
		static IdValue * getIdValue( int32_t id
			, std::unordered_map< std::string, IdValuePtr, HashT > & map )
		{
			auto it = std::find_if( map.begin()
				, map.end()
				, [id]( RendererMap::value_type const & lookup )
				{
					return lookup.second->id == id;
				} );

			if ( it == map.end() )
			{
				return nullptr;
			}

			return it->second.get();
		}

		static Renderer getRenderer( std::string const & name
			, RendererMap & values
			, TestDatabase::InsertRenderer & insert )
		{
			return getIdValue( name, values, insert );
		}

		static Category getCategory( std::string const & name
			, CategoryMap & values
			, TestDatabase::InsertCategory & insert )
		{
			return getIdValue( name, values, insert );
		}

		static IdValue * getCategory( int32_t id
			, CategoryMap & values )
		{
			return getIdValue( id, values );
		}

		static Keyword getKeyword( std::string const & name
			, KeywordMap & values
			, TestDatabase::InsertKeyword & insert )
		{
			return getIdValue( name, values, insert );
		}

		static Platform getPlatform( std::string const & name
			, PlatformMap & values
			, TestDatabase::InsertPlatform & insert )
		{
			return getIdValue( name, values, insert );
		}

		static Cpu getCpu( std::string const & name
			, CpuMap & values
			, TestDatabase::InsertCpu & insert )
		{
			return getIdValue( name, values, insert );
		}

		static Gpu getGpu( std::string const & name
			, GpuMap & values
			, TestDatabase::InsertGpu & insert )
		{
			return getIdValue( name, values, insert );
		}

		static PathArray listTestCategories( wxFileName const & folder )
		{
			PathArray result;
			traverseDirectory( folder
				, [&result]( wxFileName const & fdr )
				{
					result.push_back( fdr );
					return wxDIR_CONTINUE;
				}
				, []( wxString const & fdr
					, wxString const & name )
				{
				} );
			return result;
		}

		static PathArray listScenes( wxFileName const & categoryPath )
		{
			return filterDirectoryFiles( categoryPath
				, []( wxString const & fdr, wxString const & name )
				{
					return getExtension( name ) == wxT( "cscn" );
				} );
		}

		static TestArray::iterator findTest( TestArray & result
			, std::string const & name )
		{
			return std::find_if( result.begin()
				, result.end()
				, [&name]( TestPtr const & lookup )
				{
					return lookup->name == name;
				} );
		}

		static void makeTestRun( Config const & config
			, TestDatabase::InsertRenderer & insertRenderer
			, TestDatabase::InsertTest & insertTest
			, TestDatabase::InsertRunV2 & insertRun
			, IdValue * category
			, TestArray & categoryTests
			, wxFileName const & imgPath
			, TestStatus status
			, RendererMap & renderers )
		{
			auto name = makeStdString( imgPath.GetName() + "." + imgPath.GetExt() );
			auto prevDotIdx = name.find_last_of( "." );

			if ( prevDotIdx == std::string::npos
				|| name.substr( prevDotIdx + 1 ) != "diff" )
			{
				auto rendererIdx = name.find_last_of( "_" );

				if ( rendererIdx != std::string::npos )
				{
					auto renderer = getRenderer( name.substr( rendererIdx + 1 )
						, renderers
						, insertRenderer );
					name = name.substr( 0, rendererIdx );
					auto runDate = getFileDate( imgPath );

					if ( runDate.IsValid() )
					{
						auto it = findTest( categoryTests, name );

						if ( it == categoryTests.end() )
						{
							it = categoryTests.emplace( it
								, std::make_unique< Test >( 0
									, name
									, category ) );
							( *it )->id = insertTest.insert( category->id, name );
						}

						Test & test = *( *it );
						insertRun.insert( test.id
							, renderer->id
							, runDate
							, status
							, runDate
							, runDate );
					}
				}
			}
		}

		static void listAllResults( Config const & config
			, TestDatabase::InsertRenderer & insertRenderer
			, TestDatabase::InsertTest & insertTest
			, TestDatabase::InsertRunV2 & insertRun
			, wxFileName const & categoryPath
			, PathArray const & folders
			, Category category
			, TestArray & categoryTests
			, RendererMap & renderers )
		{
			for ( auto & status : folders )
			{
				auto testStatus = getStatus( makeStdString( status.GetFullPath() ) );
				filterDirectoryFiles( status
					, [&config, &insertRenderer, &insertTest, &insertRun, &renderers, &categoryTests, category, testStatus]( wxString const & folder, wxString const & name )
					{
						auto result = getExtension( name ) == wxT( "png" );

						if ( result )
						{
							makeTestRun( config
								, insertRenderer
								, insertTest
								, insertRun
								, category
								, categoryTests
								, folder / name
								, testStatus
								, renderers );
						}

						return result;
					} );
			}
		}

		static TestArray listCategoryTestFiles( Config const & config
			, TestDatabase::InsertRenderer & insertRenderer
			, TestDatabase::InsertTest & insertTest
			, TestDatabase::InsertRunV2 & insertRun
			, wxFileName const & categoryPath
			, Category category
			, RendererMap & renderers )
		{
			getRenderer( "vk", renderers, insertRenderer );
			auto compareFolder = categoryPath / wxT( "Compare" );
			PathArray folders
			{
				compareFolder / "Negligible",
				compareFolder / "Acceptable",
				compareFolder / "Unacceptable",
				compareFolder / "Unprocessed",
			};
			auto scenes = listScenes( categoryPath );
			TestArray result;

			for ( auto & testScene : scenes )
			{
				auto sceneName = makeStdString( testScene.GetName() );
				auto it = findTest( result, sceneName );

				if ( it == result.end() )
				{
					auto test = std::make_unique< Test >( 0
						, sceneName
						, category );
					test->id = insertTest.insert( test->category->id, test->name );
					result.push_back( std::move( test ) );
				}
			}

			listAllResults( config
				, insertRenderer
				, insertTest
				, insertRun
				, categoryPath
				, folders
				, category
				, result
				, renderers );

			return result;
		}
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertIdValue::insert( std::string const & inName )
	{
		name->setValue( inName );
		sName->setValue( inName );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	void TestDatabase::InsertIdId::insert( int32_t lhs, int32_t rhs )
	{
		lhsId->setValue( lhs );
		rhsId->setValue( rhs );

		if ( !stmt->executeUpdate() )
		{
			throw std::runtime_error{ "Couldn't insert entry" };
		}
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertTest::insert( int32_t inCategoryId
		, std::string const & inName
		, bool inIgnoreResult )
	{
		categoryId->setValue( inCategoryId );
		sCategoryId->setValue( inCategoryId );
		name->setValue( inName );
		sName->setValue( inName );
		ignoreResult->setValue( inIgnoreResult ? 1 : 0 );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertHost::insert( int32_t inPlatformId
		, int32_t inCpuId
		, int32_t inGpuId )
	{
		platformId->setValue( inPlatformId );
		sPlatformId->setValue( inPlatformId );
		cpuId->setValue( inCpuId );
		sCpuId->setValue( inCpuId );
		gpuId->setValue( inGpuId );
		sGpuId->setValue( inGpuId );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertRunV2::insert( int32_t id
		, int32_t inRendererId
		, db::DateTime dateRun
		, TestStatus inStatus
		, db::DateTime const & dateCastor
		, db::DateTime const & dateScene )
	{
		testId->setValue( id );
		sTestId->setValue( id );
		rendererId->setValue( inRendererId );
		sRendererId->setValue( inRendererId );
		runDate->setValue( dateRun );
		sRunDate->setValue( dateRun );
		status->setValue( int32_t( inStatus ) );
		sStatus->setValue( int32_t( inStatus ) );
		engineData->setValue( dateCastor );
		sCastorDate->setValue( dateCastor );
		sceneDate->setValue( dateScene );
		sSceneDate->setValue( dateScene );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	int32_t TestDatabase::InsertRun::insert( int32_t id
		, int32_t inRendererId
		, db::DateTime dateRun
		, TestStatus inStatus
		, db::DateTime const & dateCastor
		, db::DateTime const & dateScene
		, Microseconds timeTotal
		, Microseconds timeAvgFrame
		, Microseconds timeLastFrame
		, Host const & host )
	{
		testId->setValue( id );
		sTestId->setValue( id );
		rendererId->setValue( inRendererId );
		sRendererId->setValue( inRendererId );
		runDate->setValue( dateRun );
		sRunDate->setValue( dateRun );
		status->setValue( int32_t( inStatus ) );
		sStatus->setValue( int32_t( inStatus ) );
		engineData->setValue( dateCastor );
		sCastorDate->setValue( dateCastor );
		sceneDate->setValue( dateScene );
		sSceneDate->setValue( dateScene );
		totalTime->setValue( uint32_t( timeTotal.count() ) );
		sTotalTime->setValue( uint32_t( timeTotal.count() ) );
		avgFrameTime->setValue( uint32_t( timeAvgFrame.count() ) );
		sAvgFrameTime->setValue( uint32_t( timeAvgFrame.count() ) );
		lastFrameTime->setValue( uint32_t( timeLastFrame.count() ) );
		sLastFrameTime->setValue( uint32_t( timeLastFrame.count() ) );
		hostId->setValue( host.id );
		sHostId->setValue( host.id );

		if ( !stmt->executeUpdate() )
		{
			return -1;
		}

		auto result = select->executeSelect();

		if ( !result || result->empty() )
		{
			return -1;
		}

		return result->begin()->getField( 0 ).getValue< int32_t >();
	}

	//*********************************************************************************************

	bool TestDatabase::CheckTableExists::checkTable( std::string const & name )
	{
		tableName->setValue( name );
		auto result = stmt->executeSelect();
		return result && !result->empty();
	}

	//*********************************************************************************************

	void TestDatabase::ListCategories::listCategories( CategoryMap & categories )
	{
		if ( auto res = stmt->executeSelect() )
		{
			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto name = row.getField( 1 ).getValue< std::string >();
				categories.emplace( name, std::make_unique< IdValue >( id, name ) );
			}
		}
	}

	//*********************************************************************************************

	void TestDatabase::ListPlatforms::list( PlatformMap & result )
	{
		if ( auto res = stmt->executeSelect() )
		{
			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto name = row.getField( 1 ).getValue< std::string >();
				result.emplace( name, std::make_unique< IdValue >( id, name ) );
			}
		}
	}

	//*********************************************************************************************

	void TestDatabase::ListCpus::list( CpuMap & result )
	{
		if ( auto res = stmt->executeSelect() )
		{
			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto name = row.getField( 1 ).getValue< std::string >();
				result.emplace( name, std::make_unique< IdValue >( id, name ) );
			}
		}
	}

	//*********************************************************************************************

	void TestDatabase::ListGpus::list( GpuMap & result )
	{
		if ( auto res = stmt->executeSelect() )
		{
			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto name = row.getField( 1 ).getValue< std::string >();
				result.emplace( name, std::make_unique< IdValue >( id, name ) );
			}
		}
	}

	//*********************************************************************************************

	void TestDatabase::ListHosts::list( PlatformMap const & platforms
		, CpuMap const & cpus
		, GpuMap const & gpus
		, HostMap & hosts )
	{
		if ( auto res = stmt->executeSelect() )
		{
			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto platformId = row.getField( 1 ).getValue< int32_t >();
				auto cpuId = row.getField( 2 ).getValue< int32_t >();
				auto gpuId = row.getField( 3 ).getValue< int32_t >();
				auto platformIt = testdb::findId( platforms, platformId );
				assert( platformIt != platforms.end() );
				auto cpuIt = testdb::findId( cpus, cpuId );
				assert( cpuIt != cpus.end() );
				auto gpuIt = testdb::findId( gpus, gpuId );
				assert( gpuIt != gpus.end() );
				hosts.emplace( id, std::make_unique< Host >( Host{ id
					, platformIt->second.get()
					, cpuIt->second.get()
					, gpuIt->second.get() } ) );
			}
		}
	}

	//*********************************************************************************************

	TestMap TestDatabase::ListTests::listTests( CategoryMap & categories
		, wxProgressDialog & progress
		, int & index )
	{
		TestMap result;
		listTests( categories, result, progress, index );
		return result;
	}

	void TestDatabase::ListTests::listTests( CategoryMap & categories
		, TestMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		for ( auto & category : categories )
		{
			result.emplace( category.second.get(), TestArray{} );
		}

		if ( auto res = stmt->executeSelect() )
		{
			progress.SetRange( int( progress.GetRange() + res->size() ) );

			for ( auto & row : *res )
			{
				auto id = row.getField( 0 ).getValue< int32_t >();
				auto catId = row.getField( 1 ).getValue< int32_t >();
				auto name = row.getField( 2 ).getValue< std::string >();
				auto ignoreResult = row.getField( 3 ).getValue< int32_t >();
				auto category = testdb::getCategory( catId, categories );
				auto catIt = result.emplace( category, TestArray{} ).first;
				catIt->second.emplace_back( std::make_unique< Test >( id, name, category, ignoreResult != 0 ) );
				progress.Update( index++
					, _( "Listing tests" )
					+ wxT( "\n" ) + getDetails( *catIt->second.back() ) );
				progress.Fit();
			}
		}
		else
		{
			throw std::runtime_error{ "Couldn't list tests" };
		}
	}

	//*********************************************************************************************

	db::ResultPtr TestDatabase::ListLatestTestRun::listTests( int32_t inId )
	{
		testId->setValue( inId );
		return stmt->executeSelect();
	}

	//*********************************************************************************************

	RendererTestRuns TestDatabase::ListLatestRendererTests::listTests( TestMap const & tests
		, HostMap & hosts
		, CategoryMap & categories
		, Renderer renderer
		, wxProgressDialog & progress
		, int & index )
	{
		RendererTestRuns result{ *database };
		listTests( tests, hosts, categories, renderer, result, progress, index );
		return result;
	}

	void TestDatabase::ListLatestRendererTests::listTests( TestMap const & tests
		, HostMap & hosts
		, CategoryMap & categories
		, Renderer renderer
		, RendererTestRuns & result
		, wxProgressDialog & progress
		, int & index )
	{
		// Prefill result with "not run" entries.
		for ( auto & cat : tests )
		{
			for ( auto & test : cat.second )
			{
				result.addTest( TestRun{ test.get()
					, renderer
					, db::DateTime{}
					, TestStatus::eNotRun
					, db::DateTime{}
					, db::DateTime{}
					, TestTimes{} } );
			}
		}

		rendererId->setValue( renderer->id );

		if ( auto res = stmt->executeSelect() )
		{
			progress.SetRange( int( progress.GetRange() + res->size() ) );

			for ( auto & row : *res )
			{
				auto catId = row.getField( 0 ).getValue< int32_t >();
				auto catIt = tests.find( testdb::getCategory( catId, categories ) );

				if ( catIt != tests.end() )
				{
					auto testId = row.getField( 1 ).getValue< int32_t >();
					auto testIt = std::find_if( catIt->second.begin()
						, catIt->second.end()
						, [testId]( TestPtr const & lookup )
						{
							return lookup->id == testId;
						} );

					if ( testIt != catIt->second.end() )
					{
						auto & test = *testIt->get();
						auto runId = row.getField( 2 ).getValue< int32_t >();
						auto runDate = row.getField( 3 ).getValue< db::DateTime >();
						auto hostId = row.getField( 4 ).getValue< int32_t >();
						auto status = TestStatus( row.getField( 5 ).getValue< int32_t >() );
						auto engineData = row.getField( 6 ).getValue< db::DateTime >();
						auto sceneDate = row.getField( 7 ).getValue< db::DateTime >();
						auto totalTime = Microseconds{ uint64_t( row.getField( 8 ).getValue< int32_t >() ) };
						auto avgFrameTime = Microseconds{ uint64_t( row.getField( 9 ).getValue< int32_t >() ) };
						auto lastFrameTime = Microseconds{ uint64_t( row.getField( 10 ).getValue< int32_t >() ) };
						auto it = std::find_if( result.begin()
							, result.end()
							, [testId]( DatabaseTest const & lookup )
							{
								return lookup->test->id == testId;
							} );
						auto hostIt = hosts.find( hostId );
						assert( hostIt != hosts.end() );

						if ( it == result.end() )
						{
							assert( false );
							auto & dbTest = result.addTest( TestRun{ &test
								, renderer
								, runDate
								, status
								, engineData
								, sceneDate
								, TestTimes{ hostIt->second.get(), totalTime, avgFrameTime, lastFrameTime } } );
							dbTest.update( runId );
							progress.Update( index++
								, _( "Listing latest runs" )
								+ wxT( "\n" ) + getProgressDetails( dbTest ) );
							progress.Fit();
						}
						else
						{
							assert( it->getStatus() == TestStatus::eNotRun );
							it->update( runId
								, runDate
								, status
								, engineData
								, sceneDate
								, TestTimes{ hostIt->second.get(), totalTime, avgFrameTime, lastFrameTime } );
							progress.Update( index++
								, _( "Listing latest runs" )
								+ wxT( "\n" ) + getProgressDetails( *it ) );
							progress.Fit();
						}
					}
				}
			}
		}
		else
		{
			throw std::runtime_error{ "Couldn't list tests runs" };
		}
	}

	//*********************************************************************************************

	RunMap TestDatabase::ListTestRuns::listRuns( HostMap const & hosts
		, int testId )
	{
		RunMap result;
		id->setValue( testId );

		if ( auto res = stmt->executeSelect() )
		{
			for ( auto & row : *res )
			{
				Run run;
				run.id = uint32_t( row.getField( 0 ).getValue< int32_t >() );
				run.status = RunStatus( row.getField( 1 ).getValue< int32_t >() );
				run.runDate = row.getField( 2 ).getValue< db::DateTime >();
				auto hostId = row.getField( 3 ).getValue< int32_t >();
				auto hostIt = hosts.find( hostId );
				assert( hostIt != hosts.end() );
				run.host = hostIt->second.get();
				run.totalTime = Microseconds{ uint64_t( row.getField( 4 ).getValue< int32_t >() ) };
				run.avgTime = Microseconds{ uint64_t( row.getField( 5 ).getValue< int32_t >() ) };
				run.lastTime = Microseconds{ uint64_t( row.getField( 6 ).getValue< int32_t >() ) };
				result.insert( { run.runDate, run } );
			}
		}

		return result;
	}

	//*********************************************************************************************

	uint32_t TestDatabase::GetDatabaseVersion::get()
	{
		auto result = stmt->executeSelect();

		if ( !result || result->empty() )
		{
			throw std::runtime_error{ "Couldn't retrieve database version" };
		}

		auto row = result->begin();
		auto value = row->getField( 0 ).getValue< int32_t >();
		return uint32_t( value );
	}

	//*********************************************************************************************

	std::vector< Host const * > TestDatabase::ListTestHosts::list( Test const & test
		, Renderer const & renderer
		, HostMap const & hosts )
	{
		testId->setValue( test.id );
		rendererId->setValue( renderer->id );
		auto result = stmt->executeSelect();

		if ( !result )
		{
			throw std::runtime_error{ "Couldn't retrieve hosts list" };
		}

		std::vector< Host const * > ret;

		for ( auto & row : *result )
		{
			ret.emplace_back( testdb::findHost( hosts
				, row.getField( 0 ).getValue< int32_t >() ) );
		}

		return ret;
	}

	//*********************************************************************************************

	std::map< wxDateTime, TestTimes > TestDatabase::ListAllTimes::listTimes( Test const & test
		, Renderer const & renderer
		, Host const & host )
	{
		testId->setValue( test.id );
		rendererId->setValue( renderer->id );
		hostId->setValue( host.id );
		auto result = stmt->executeSelect();

		if ( !result )
		{
			throw std::runtime_error{ "Couldn't retrieve times list" };
		}

		std::map< wxDateTime, TestTimes > ret;

		for ( auto & row : *result )
		{
			ret.emplace( row.getField( 0 ).getValue< db::DateTime >()
				, TestTimes{ &host
					, Microseconds{ row.getField( 1 ).getValue< int32_t >() }
					, Microseconds{ row.getField( 2 ).getValue< int32_t >() }
					, Microseconds{ row.getField( 3 ).getValue< int32_t >() } } );
		}

		return ret;
	}

	//*********************************************************************************************

	TestDatabase::TestDatabase( Config config
		, FileSystem & fileSystem )
		: m_config{ std::move( config ) }
		, m_fileSystem{ fileSystem }
		, m_database{ m_config.database }
	{
	}

	TestDatabase::~TestDatabase()
	{
	}

	void TestDatabase::initialise( wxProgressDialog & progress
		, int & index )
	{
		// Necessary database initialisation
		m_checkTableExists = CheckTableExists{ m_database };
		auto catRenInit = false;

		if ( !m_checkTableExists.checkTable( "Test" ) )
		{
			doCreateV1( progress, index );
			m_fileSystem.touchDb( m_config.database );
		}

		if ( !m_checkTableExists.checkTable( "TestsDatabase" ) )
		{
			doCreateV2( progress, index );
			m_fileSystem.touchDb( m_config.database );
			catRenInit = true;
		}
		else
		{
			m_insertTest = InsertTest{ m_database };
			m_insertRunV2 = InsertRunV2{ m_database };
			m_insertCategory = InsertCategory{ m_database };
			m_insertRenderer = InsertRenderer{ m_database };
		}

		if ( !m_checkTableExists.checkTable( "Keyword" ) )
		{
			doCreateV3( progress, index );
			m_fileSystem.touchDb( m_config.database );
		}
		else
		{
			m_insertKeyword = InsertKeyword{ m_database };
			m_insertTestKeyword = InsertTestKeyword{ m_database };
			m_insertCategoryKeyword = InsertCategoryKeyword{ m_database };
			m_getDatabaseVersion = GetDatabaseVersion{ m_database };
		}

		auto version = m_getDatabaseVersion.get();

		if ( version < 4 )
		{
			doCreateV4( progress, index );
		}

		bool initCpuGpu{ false };

		if ( version < 5 )
		{
			doCreateV5( progress, index );
		}
		else
		{
			m_insertPlatform = InsertPlatform{ m_database };
			m_insertCpu = InsertCpu{ m_database };
			m_insertGpu = InsertGpu{ m_database };
			m_insertHost = InsertHost{ m_database };
			initCpuGpu = true;
		}

		m_insertRun = InsertRun{ m_database };
		m_updateRunStatus = UpdateRunStatus{ m_database };
		m_updateTestIgnoreResult = UpdateTestIgnoreResult{ m_database };
		m_updateRunDates = UpdateRunDates{ m_database };
		m_updateRunCastorDate = UpdateRunCastorDate{ m_database };
		m_updateRunSceneDate = UpdateRunSceneDate{ m_database };
		m_listCategories = ListCategories{ m_database };
		m_listTests = ListTests{ m_database };
		m_listLatestRun = ListLatestTestRun{ m_database };
		m_listLatestRendererRuns = ListLatestRendererTests{ this };
		m_listTestRuns = ListTestRuns{ this };
		m_deleteRun = DeleteRun{ this };
		m_updateRunsCastorDate = UpdateRunsCastorDate{ m_database };
		m_updateTestCategory = UpdateTestCategory{ m_database };
		m_updateTestName = UpdateTestName{ m_database };
		m_listAllTimes = ListAllTimes{ m_database };
		m_listPlatforms = ListPlatforms{ m_database };
		m_listCpus = ListCpus{ m_database };
		m_listGpus = ListGpus{ m_database };
		m_listHosts = ListHosts{ m_database };
		m_listHosts = ListHosts{ m_database };
		m_listTestHosts = ListTestHosts{ m_database };

		if ( m_config.initFromFolder )
		{
			doFillDatabase( progress, index );
			m_fileSystem.touchDb( m_config.database );
			catRenInit = true;
		}

		if ( !catRenInit )
		{
			if ( auto result = m_database.executeSelect( "SELECT Id, Name FROM Category;" ) )
			{
				for ( auto & row : *result )
				{
					auto id = row.getField( 0 ).getValue< int32_t >();
					auto name = row.getField( 1 ).getValue< std::string >();
					m_categories.emplace( name, std::make_unique< IdValue >( id, name ) );
				}
			}
			else
			{
				throw std::runtime_error{ "Couldn't list categories" };
			}

			if ( auto result = m_database.executeSelect( "SELECT Id, Name FROM Renderer;" ) )
			{
				for ( auto & row : *result )
				{
					auto id = row.getField( 0 ).getValue< int32_t >();
					auto name = row.getField( 1 ).getValue< std::string >();
					m_renderers.emplace( name, std::make_unique< IdValue >( id, name ) );
				}
			}
			else
			{
				throw std::runtime_error{ "Couldn't list renderers" };
			}
		}

		if ( initCpuGpu )
		{
			m_listPlatforms.list( m_platforms );
			m_listCpus.list( m_cpus );
			m_listGpus.list( m_gpus );
			m_listHosts.list( m_platforms, m_cpus, m_gpus, m_hosts );
		}
	}

	void TestDatabase::moveResultImage( DatabaseTest const & test
		, Category oldCategory
		, Category newCategory )
	{
		auto srcFolder = m_config.work / getResultFolder( *test, oldCategory );
		auto dstFolder = m_config.work / getResultFolder( *test, newCategory );
		auto resultName = getResultName( *test );
		m_fileSystem.moveResultFile( test.getName()
			, srcFolder
			, dstFolder
			, resultName
			, resultName );
	}

	void TestDatabase::moveResultFile( DatabaseTest const & test
		, TestStatus oldStatus
		, TestStatus newStatus
		, wxFileName const & work )
	{
		if ( oldStatus == TestStatus::eNotRun
			|| newStatus == TestStatus::eNotRun
			|| oldStatus == newStatus )
		{
			return;
		}

		auto resultFolder = work / getResultFolder( *( *test ).test );
		auto resultName = getResultName( *test );
		m_fileSystem.moveResultFile( test.getName()
			, resultFolder / getFolderName( oldStatus )
			, resultFolder / getFolderName( newStatus )
			, resultName
			, resultName );
	}

	bool TestDatabase::updateReferenceFile( DatabaseTest const & test
		, TestStatus status )
	{
		return m_fileSystem.updateSceneFile( test.getName()
			, m_config.work / getResultFolder( *( *test ).test ) / getFolderName( status )
			, m_config.test / getReferenceFolder( *test )
			, getResultName( *test )
			, getReferenceName( *test ) );
	}

	Renderer TestDatabase::createRenderer( std::string const & name )
	{
		m_fileSystem.touchDb( m_config.database );
		return testdb::getRenderer( name, m_renderers, m_insertRenderer );
	}

	Category TestDatabase::createCategory( std::string const & name )
	{
		m_fileSystem.touchDb( m_config.database );
		return testdb::getCategory( name, m_categories, m_insertCategory );
	}

	Keyword TestDatabase::createKeyword( std::string const & name )
	{
		m_fileSystem.touchDb( m_config.database );
		return testdb::getKeyword( name, m_keywords, m_insertKeyword );
	}

	TestMap TestDatabase::listTests()
	{
		TestMap result;
		listTests( result );
		return result;
	}

	TestMap TestDatabase::listTests( wxProgressDialog & progress
		, int & index )
	{
		TestMap result;
		listTests( result, progress, index );
		return result;
	}

	void TestDatabase::listTests( TestMap & result )
	{
		wxProgressDialog progress{ wxT( "Listing tests" )
			, wxT( "Listing tests..." )
			, 1
			, nullptr };
		int index = 0;
		listTests( result, progress, index );
	}

	void TestDatabase::listTests( TestMap & result
		, wxProgressDialog & progress
		, int & index )
	{
		wxLogMessage( "Listing tests" );
		progress.SetTitle( _( "Listing tests" ) );
		progress.Update( index
			, _( "Listing tests" )
			+ wxT( "\n" ) + _( "..." ) );
		progress.Fit();
		m_listCategories.listCategories( m_categories );
		m_listTests.listTests( m_categories, result, progress, index );
	}

	AllTestRuns TestDatabase::listLatestRuns( TestMap const & tests )
	{
		AllTestRuns result{ *this };
		listLatestRuns( tests, result );
		return result;
	}

	AllTestRuns TestDatabase::listLatestRuns( TestMap const & tests
		, wxProgressDialog & progress
		, int & index )
	{
		AllTestRuns result{ *this };
		listLatestRuns( tests, result, progress, index );
		return result;
	}

	void TestDatabase::listLatestRuns( TestMap const & tests
		, AllTestRuns & result )
	{
		wxProgressDialog progress{ wxT( "Listing latest runs" )
			, wxT( "Listing latest runs..." )
			, 1
			, nullptr };
		int index = 0;
		listLatestRuns( tests, result, progress, index );
	}

	void TestDatabase::listLatestRuns( TestMap const & tests
		, AllTestRuns & result
		, wxProgressDialog & progress
		, int & index )
	{
		wxLogMessage( "Listing latest runs" );
		progress.SetTitle( _( "Listing latest runs" ) );
		progress.Update( index, _( "Listing latest runs\n..." ) );
		progress.Fit();

		for ( auto & renderer : m_renderers )
		{
			auto & rendCounts = result.addRenderer( renderer.second.get() );
			listLatestRuns( renderer.second.get(), tests, rendCounts, progress, index );
		}
	}

	void TestDatabase::listLatestRuns( Renderer renderer
		, TestMap const & tests
		, RendererTestRuns & result )
	{
		wxProgressDialog progress{ wxT( "Listing latest renderer runs" )
			, wxT( "Listing latest renderer runs..." )
			, 1
			, nullptr };
		int index = 0;
		listLatestRuns( renderer, tests, result, progress, index );
	}

	void TestDatabase::listLatestRuns( Renderer renderer
		, TestMap const & tests
		, RendererTestRuns & result
		, wxProgressDialog & progress
		, int & index )
	{
		wxLogMessage( "Listing latest renderer runs" );
		progress.SetTitle( _( "Listing latest renderer runs" ) );
		progress.Update( index, _( "Listing latest renderer runs\n..." ) );
		progress.Fit();
		m_listLatestRendererRuns.listTests( tests, m_hosts, m_categories, renderer, result, progress, index );
	}

	RunMap TestDatabase::listRuns( int testId )
	{
		wxLogMessage( wxString{} << "Listing test " << testId << " runs" );
		return m_listTestRuns.listRuns( m_hosts, testId );
	}

	void TestDatabase::deleteRun( uint32_t runId )
	{
		wxLogMessage( wxString{} << "Deleting run " << runId );
		m_deleteRun.id->setValue( int32_t( runId ) );
		m_deleteRun.stmt->executeUpdate();
		m_fileSystem.touchDb( m_config.database );
	}

	std::vector< Host const * > TestDatabase::listTestHosts( Test const & test
		, Renderer const & renderer )
	{
		wxLogMessage( "Listing test runs hosts" );
		return m_listTestHosts.list( test, renderer, m_hosts );
	}

	std::map< wxDateTime, TestTimes > TestDatabase::listTestTimes( Test const & test
		, Renderer const & renderer
		, Host const & host )
	{
		wxLogMessage( "Listing latest test times" );
		return m_listAllTimes.listTimes( test, renderer, host );
	}

	void TestDatabase::insertTest( Test & test
		, bool moveFiles )
	{
		test.id = m_insertTest.insert( test.category->id
			, test.name );
		wxLogMessage( wxString() << "Inserted: " + getDetails( test ) );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateRunsCastorDate( db::DateTime const & date )
	{
		m_updateRunsCastorDate.engineData->setValue( date );
		m_updateRunsCastorDate.stmt->executeUpdate();
		wxLogMessage( "Updated Castor3D date for all runs" );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateTestCategory( Test const & test
		, Category category )
	{
		m_updateTestCategory.categoryId->setValue( category->id );
		m_updateTestCategory.id->setValue( test.id );
		m_updateTestCategory.stmt->executeUpdate();
		wxLogMessage( wxString() << "Updated category for test " + test.name );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateTestName( Test const & test
		, wxString const & name )
	{
		m_updateTestName.id->setValue( test.id );
		m_updateTestName.name->setValue( makeStdString( name ) );
		m_updateTestName.stmt->executeUpdate();
		wxLogMessage( wxString() << "Updated category for test " << test.id );
		m_fileSystem.touchDb( m_config.database );
	}

	Host * TestDatabase::getHost( std::string const & platformName
		, std::string const & cpuName
		, std::string const & gpuName )
	{
		auto platform = testdb::getIdValue( platformName, m_platforms, m_insertPlatform );
		auto cpu = testdb::getIdValue( cpuName, m_cpus, m_insertCpu );
		auto gpu = testdb::getIdValue( gpuName, m_gpus, m_insertGpu );
		auto it = std::find_if( m_hosts.begin()
			, m_hosts.end()
			, [platform, cpu, gpu]( auto & lookup )
			{
				return lookup.second->platform == platform
					&& lookup.second->cpu == cpu
					&& lookup.second->gpu == gpu;
			} );

		if ( it == m_hosts.end() )
		{
			auto id = m_insertHost.insert( platform->id, cpu->id, gpu->id );
			it = m_hosts.emplace( id, std::make_unique< Host >( Host{ id
				, platform
				, cpu
				, gpu } ) ).first;
		}

		return it->second.get();
	}

	void TestDatabase::insertRun( TestRun & run
		, bool moveFiles )
	{
		run.id = m_insertRun.insert( run.test->id
			, run.renderer->id
			, run.runDate
			, run.status
			, run.engineDate
			, run.sceneDate
			, run.times.total
			, run.times.avg
			, run.times.last
			, *run.times.host );

		if ( moveFiles )
		{
			if ( run.status != TestStatus::eNotRun
				&& run.status != TestStatus::eCrashed )
			{
				auto srcFolder = m_config.test / getCompareFolder( run );
				auto dstFolder = m_config.work / getResultFolder( run );
				m_fileSystem.moveResultFile( run.test->name
					, srcFolder
					, dstFolder
					, getCompareName( run )
					, getResultName( run ) );
			}
		}

		wxLogMessage( wxString() << "Inserted: " + getDetails( run ) );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateTestIgnoreResult( Test const & test
		, bool ignore )
	{
		m_updateTestIgnoreResult.ignore->setValue( ignore ? 1 : 0 );
		m_updateTestIgnoreResult.id->setValue( int32_t( test.id ) );
		m_updateTestIgnoreResult.stmt->executeUpdate();
		wxLogMessage( wxString() << "Updated ignore result for: " + getDetails( test ) );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateRunStatus( TestRun const & run )
	{
		m_updateRunStatus.status->setValue( int32_t( run.status ) );
		m_updateRunStatus.engineData->setValue( run.engineDate );
		m_updateRunStatus.sceneDate->setValue( run.sceneDate );
		m_updateRunStatus.id->setValue( int32_t( run.id ) );
		m_updateRunStatus.stmt->executeUpdate();
		wxLogMessage( wxString() << "Updated status for: " + getDetails( run ) );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateRunCastorDate( TestRun const & run )
	{
		m_updateRunCastorDate.engineData->setValue( run.engineDate );
		m_updateRunCastorDate.id->setValue( int32_t( run.id ) );
		m_updateRunCastorDate.stmt->executeUpdate();
		wxLogMessage( wxString() << "Updated Castor3D date for: " + getDetails( run ) );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::updateRunSceneDate( TestRun const & run )
	{
		m_updateRunSceneDate.sceneDate->setValue( getSceneDate( m_config, run ) );
		m_updateRunSceneDate.id->setValue( int32_t( run.id ) );
		m_updateRunSceneDate.stmt->executeUpdate();
		wxLogMessage( wxString() << "Updated Scene date for: " + getDetails( run ) );
		m_fileSystem.touchDb( m_config.database );
	}

	void TestDatabase::doCreateV1( wxProgressDialog & progress, int & index )
	{
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Creating tests database" ) + wxT( " V3" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			std::string createTableTest = "CREATE TABLE Test";
			createTableTest += "( Id INTEGER PRIMARY KEY\n";
			createTableTest += "\t, Name VARCHAR(1024)\n";
			createTableTest += "\t, RunDate DATETIME\n";
			createTableTest += "\t, Status INTEGER\n";
			createTableTest += "\t, Renderer VARCHAR(10)\n";
			createTableTest += "\t, Category VARCHAR(50)\n";
			createTableTest += "\t, IgnoreResult INTEGER\n";
			createTableTest += "\t, CastorDate DATETIME\n";
			createTableTest += "\t, SceneDate DATETIME\n";
			createTableTest += ");";
			m_database.executeUpdate( createTableTest );

			progress.Update( index++
				, _( "Creating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			progress.Fit();
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doCreateV2( wxProgressDialog & progress, int & index )
	{
		static int constexpr NonTestsCount = 7;
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Updating tests database to V3" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating tests database" ) );
			progress.Fit();
			progress.SetRange( NonTestsCount );
			std::string query = "CREATE TABLE TestsDatabase( Id INTEGER PRIMARY KEY, Version INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create TestsDatabase table." };
			}

			query = "INSERT INTO TestsDatabase (Version) VALUES (2);";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create version 2 entry." };
			}

			{
				// Renderer table
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( "\n" ) + _( "Creating Renderer table" ) );
				progress.Fit();
				query = "CREATE TABLE Renderer";
				query += "( Id INTEGER PRIMARY KEY\n";
				query += "\t, Name VARCHAR(10)\n";
				query += ");";

				if ( !m_database.executeUpdate( query ) )
				{
					throw std::runtime_error{ "Couldn't create Renderer table." };
				}

				m_insertRenderer = InsertRenderer{ m_database };
			}

			{
				// Category table
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( "\n" ) + _( "Creating Category table" ) );
				progress.Fit();

				query = "CREATE TABLE Category";
				query += "( Id INTEGER PRIMARY KEY\n";
				query += "\t, Name VARCHAR(50)\n";
				query += ");";

				if ( !m_database.executeUpdate( query ) )
				{
					throw std::runtime_error{ "Couldn't create Category table." };
				}

				m_insertCategory = InsertCategory{ m_database };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating Test table" ) );
			progress.Fit();

			query = "ALTER TABLE Test\n";
			query += "RENAME TO TestOld;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't rename Test table." };
			}

			query = "CREATE TABLE Test";
			query += "( Id INTEGER PRIMARY KEY\n";
			query += "\t, CategoryId INTEGER\n";
			query += "\t, Name VARCHAR(1024)\n";
			query += "\t, IgnoreResult INTEGER\n";
			query += ");";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't re-create Test table." };
			}

			query = "CREATE TABLE TestRun";
			query += "( Id INTEGER PRIMARY KEY\n";
			query += "\t, TestId INTEGER\n";
			query += "\t, RendererId INTEGER\n";
			query += "\t, RunDate DATETIME\n";
			query += "\t, Status INTEGER\n";
			query += "\t, CastorDate DATETIME\n";
			query += "\t, SceneDate DATETIME\n";
			query += ");";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create TestRun table." };
			}

			m_insertTest = InsertTest{ m_database };
			m_insertRunV2 = InsertRunV2{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Listing tests" ) );
			progress.Fit();

			query = "SELECT Category, Name, Renderer, RunDate, Status, CastorDate, SceneDate\n";
			query += "FROM TestOld\n";
			query += "ORDER BY Category, Name, Renderer, RunDate;";
			auto testsList = m_database.executeSelect( query );

			if ( !testsList )
			{
				throw std::runtime_error{ "Couldn't list existing tests." };
			}

			progress.SetRange( NonTestsCount + int( testsList->size() ) );
			std::vector< TestPtr > tests;
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( " - " ) + _( "Listing tests" )
				+ wxT( "\n" ) + _( "..." ) );
			progress.Fit();

			std::string prvCatName;
			std::string prvTestName;
			int testId = 0;

			for ( auto & testInstance : *testsList )
			{
				auto catName = testInstance.getField( 0u ).getValue< std::string >();
				auto testName = testInstance.getField( 1u ).getValue< std::string >();
				auto rendName = testInstance.getField( 2u ).getValue< std::string >();
				auto runDate = testInstance.getField( 3u ).getValue< db::DateTime >();
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( " - " ) + _( "Listing tests" )
					+ wxT( "\n" ) + getProgressDetails( catName, testName, rendName, runDate ) );
				progress.Fit();

				if ( catName != prvCatName
					|| testName != prvTestName )
				{
					prvCatName = catName;
					prvTestName = testName;
					auto category = testdb::getCategory( catName, m_categories, m_insertCategory );
					testId = m_insertTest.insert( category->id, testName );
				}

				auto status = TestStatus( testInstance.getField( 4u ).getValue< int32_t >() );
				auto engineData = testInstance.getField( 5u ).getValue< db::DateTime >();
				auto sceneDate = testInstance.getField( 6u ).getValue < db::DateTime >();
				auto renderer = testdb::getRenderer( rendName, m_renderers, m_insertRenderer );
				m_insertRunV2.insert( testId, renderer->id, runDate, status, engineData, sceneDate );
			}

			query = "DROP TABLE TestOld;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't drop TestOld table." };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			progress.Fit();

			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doCreateV3( wxProgressDialog & progress, int & index )
	{
		static int constexpr NonTestsCount = 7;
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Updating tests database to V3" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating Keyword table" ) );
			progress.Fit();
			progress.SetRange( NonTestsCount );
			std::string query = "CREATE TABLE Keyword( Id INTEGER PRIMARY KEY, Name VARCHAR(50) );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create Keyword table." };
			}

			m_insertKeyword = InsertKeyword{ m_database };
			std::set< std::string, LessNoCase > keywords
			{
				"Light",
				"Directional",
				"Point",
				"Spot",
				"Shadow",
				"PCF",
				"VSM",
				"Reflection",
				"Refraction",
				"Phong",
				"PBR",
				"GlobalIllumination",
				"LPV",
				"Texturing",
				"Albedo",
				"Diffuse",
				"Roughness",
				"Glossiness",
				"Shininess",
				"Metallic",
				"Specular",
				"Emissive",
				"Opacity",
				"Normal",
				"Occlusion",
				"Height",
			};

			for ( auto & keyword : keywords )
			{
				testdb::getKeyword( keyword, m_keywords, m_insertKeyword );
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating CategoryKeyword table" ) );
			progress.Fit();
			query = "CREATE TABLE CategoryKeyword( CategoryId INTEGER, KeywordId INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create CategoryKeyword table." };
			}

			m_insertCategoryKeyword = InsertCategoryKeyword{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating TestKeyword table" ) );
			progress.Fit();
			query = "CREATE TABLE TestKeyword( TestId INTEGER, KeywordId INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create TestKeyword table." };
			}

			m_insertTestKeyword = InsertTestKeyword{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Listing test names" ) );
			progress.Fit();
			query = "SELECT Id, Name FROM Test ORDER BY Name;";
			auto testNames = m_database.executeSelect( query );

			if ( !testNames )
			{
				throw std::runtime_error{ "Couldn't list test names." };
			}

			progress.SetRange( int( progress.GetRange() + testNames->size() ) );
			auto findSubstr = []( const std::string & str1
				, const std::string & str2 )
			{
				auto it = std::search( str1.begin()
					, str1.end()
					, str2.begin()
					, str2.end()
					, EqualNoCase{} );
				int result = -1;

				if ( it != str1.end() )
				{
					result = int( std::distance( str1.begin(), it ) );
				}

				return result;
			};

			for ( auto & test : *testNames )
			{
				auto id = test.getField( 0 ).getValue< int32_t >();
				auto name = test.getField( 1 ).getValue< std::string >();
				progress.Update( index++
					, _( "Updating tests database" )
					+ wxT( "\n" ) + _( "Listing test names" )
					+ wxT( "\n" ) + _( "- Test:" ) + name );
				progress.Fit();

				for ( auto & keyword : m_keywords )
				{
					if ( findSubstr( name, keyword.first ) != -1 )
					{
						m_insertTestKeyword.insert( id, keyword.second->id );
					}
				}
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Updating database version number" ) );
			progress.Fit();
			query = "UPDATE TestsDatabase SET Version=3;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't update database version number to 3." };
			}

			m_getDatabaseVersion = GetDatabaseVersion{ m_database };
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			progress.Fit();
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doCreateV4( wxProgressDialog & progress, int & index )
	{
		static int constexpr NonTestsCount = 2;
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Updating tests database to V4" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate4" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Updating version number" ) );
			progress.Fit();
			progress.SetRange( NonTestsCount );
			std::string query = "UPDATE TestsDatabase SET Version=4;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't update version number." };
			}

			query = "ALTER TABLE TestRun ADD COLUMN TotalTime INTEGER DEFAULT 0;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't add TotalTime column." };
			}

			query = "ALTER TABLE TestRun ADD COLUMN AvgFrameTime INTEGER DEFAULT 0;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't add AvgFrameTime column." };
			}

			query = "ALTER TABLE TestRun ADD COLUMN LastFrameTime INTEGER DEFAULT 0;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't add LastFrameTime column." };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			progress.Fit();
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doCreateV5( wxProgressDialog & progress, int & index )
	{
		static int constexpr UpdatesCount = 6;
		auto saveRange = progress.GetRange();
		auto saveIndex = index;
		progress.SetTitle( _( "Updating tests database to V5" ) );
		index = 0;
		auto transaction = m_database.beginTransaction( "DatabaseUpdate5" );

		if ( !transaction )
		{
			throw std::runtime_error{ "Couldn't begin a transaction." };
		}

		try
		{
			progress.SetRange( UpdatesCount );
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Updating version number" ) );
			progress.Fit();
			std::string query = "UPDATE TestsDatabase SET Version=5;";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't update version number." };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating Platform table" ) );
			progress.Fit();
			query = "CREATE TABLE Platform( Id INTEGER PRIMARY KEY, Name VARCHAR(128) );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create Platform table." };
			}

			m_insertPlatform = InsertPlatform{ m_database };
			std::vector< std::string > platforms
			{
				"Unknown",
				"Windows 10 or greater",
			};

			for ( auto & platform : platforms )
			{
				testdb::getPlatform( platform, m_platforms, m_insertPlatform );
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating CPU table" ) );
			progress.Fit();
			query = "CREATE TABLE CPU( Id INTEGER PRIMARY KEY, Name VARCHAR(256) );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create CPU table." };
			}

			m_insertCpu = InsertCpu{ m_database };
			std::vector< std::string > cpus
			{
				"Unknown",
				"AMD Ryzen 5 5600 6-Core Processor",
				"AMD Ryzen 9 5950X 16-Core Processor",
			};

			for ( auto & cpu : cpus )
			{
				testdb::getCpu( cpu, m_cpus, m_insertCpu );
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating GPU table" ) );
			progress.Fit();
			query = "CREATE TABLE GPU( Id INTEGER PRIMARY KEY, Name VARCHAR(256) );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create GPU table." };
			}

			m_insertGpu = InsertGpu{ m_database };
			std::vector< std::string > gpus
			{
				"Unknown",
				"NVIDIA GeForce GTX 960",
				"NVIDIA GeForce RTX 3070",
			};

			for ( auto & gpu : gpus )
			{
				testdb::getGpu( gpu, m_gpus, m_insertGpu );
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Creating Host table" ) );
			progress.Fit();
			query = "CREATE TABLE Host( Id INTEGER PRIMARY KEY, PlatformId INTEGER, CpuId INTEGER, GpuId INTEGER );";

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't create Host table." };
			}

			m_insertHost = InsertHost{ m_database };

			for ( auto & platform : m_platforms )
			{
				for ( auto & cpu : m_cpus )
				{
					for ( auto & gpu : m_gpus )
					{
						auto id = m_insertHost.insert( platform.second->id, cpu.second->id, gpu.second->id );
						m_hosts.emplace( id, std::make_unique< Host >( Host{ id
							, platform.second.get()
							, cpu.second.get()
							, gpu.second.get() } ) );
					}
				}
			}

			auto host = getHost( "Unknown", "Unknown", "Unknown" );
			query = "ALTER TABLE TestRun ADD COLUMN HostId INTEGER DEFAULT " + std::to_string( host->id ) + ";";
			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Adding CpuId column" ) );
			progress.Fit();

			if ( !m_database.executeUpdate( query ) )
			{
				throw std::runtime_error{ "Couldn't add CpuId column." };
			}

			progress.Update( index++
				, _( "Updating tests database" )
				+ wxT( "\n" ) + _( "Validating changes" ) );
			progress.Fit();
			transaction.commit();
			progress.SetRange( saveRange );
			index = saveIndex;
		}
		catch ( std::exception & )
		{
			transaction.rollback();
			progress.SetRange( saveRange );
			index = saveIndex;
			throw;
		}
	}

	void TestDatabase::doUpdateCategories()
	{
		for ( auto & category : m_categories )
		{
			category.second->id = m_insertCategory.insert( category.first );
		}
	}

	void TestDatabase::doUpdateRenderers()
	{
		for ( auto & renderer : m_renderers )
		{
			renderer.second->id = m_insertRenderer.insert( renderer.first );
		}
	}

	void TestDatabase::doListCategories( wxProgressDialog & progress
		, int & index )
	{
		TestMap result;
		auto categoryPaths = testdb::listTestCategories( m_config.test );
		doUpdateCategories();
		wxLogMessage( "Listing Test files" );
		progress.SetTitle( _( "Listing Test files" ) );
		progress.SetRange( progress.GetRange() + int( categoryPaths.size() ) );
		progress.Update( index, _( "Listing Test files\n..." ) );
		progress.Fit();

		for ( auto & categoryPath : categoryPaths )
		{
			auto categoryName = categoryPath.GetName();
			progress.Update( index++
				, _( "Listing Test files" )
				+ wxT( "\n" ) + wxT( "- Category: " ) + categoryName + wxT( "..." ) );
			progress.Fit();
			auto category = testdb::getCategory( makeStdString( categoryName ), m_categories, m_insertCategory );
			result.emplace( category
				, testdb::listCategoryTestFiles( m_config
					, m_insertRenderer
					, m_insertTest
					, m_insertRunV2
					, categoryPath
					, category
					, m_renderers ) );
		}
	}

	void TestDatabase::doFillDatabase( wxProgressDialog & progress
		, int & index )
	{
		doListCategories( progress, index );
	}

	//*********************************************************************************************
}
