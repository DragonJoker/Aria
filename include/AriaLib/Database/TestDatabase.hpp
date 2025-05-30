/*
See LICENSE file in root folder
*/
#ifndef ___Aria_TestDatabase_HPP___
#define ___Aria_TestDatabase_HPP___

#include "Prerequisites.hpp"
#include "DbConnection.hpp"
#include "DbStatement.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <list>
#include <map>
#include "AriaLib/EndExternHeaderGuard.hpp"

class wxProgressDialog;

namespace aria
{
	class TestDatabase
	{
		friend class DatabaseTest;

	public:
		AriaLib_API explicit TestDatabase( Plugin & plugin
			, FileSystem & fileSystem );
		AriaLib_API ~TestDatabase();

		AriaLib_API void initialise( wxProgressDialog & progress
			, int & index );

		AriaLib_API db::Transaction beginTransaction( std::string const & name );

		AriaLib_API void moveResultFile( DatabaseTest const & test
			, TestStatus oldStatus
			, TestStatus newStatus
			, wxFileName const & work );
		AriaLib_API void moveResultImage( DatabaseTest const & test
			, Category oldCategory
			, Category newCategory );
		AriaLib_API bool updateReferenceFile( DatabaseTest const & test
			, TestStatus status );

		AriaLib_API Renderer createRenderer( std::string const & name );

		AriaLib_API Category createCategory( std::string const & name );
		AriaLib_API void deleteCategory( Category category );
		AriaLib_API void updateCategoryName( Category category
			, wxString const & name );

		AriaLib_API Keyword createKeyword( std::string const & name );

		AriaLib_API TestMap listTests();
		AriaLib_API TestMap listTests( wxProgressDialog & progress
			, int & index );
		AriaLib_API void listTests( TestMap & result );
		AriaLib_API void listTests( TestMap & result
			, wxProgressDialog & progress
			, int & index );
		AriaLib_API void deleteTest( uint32_t testId );
		AriaLib_API void updateTestName( Test const & test
			, wxString const & name );
		AriaLib_API void updateTestCategory( Test const & test
			, Category category );

		AriaLib_API AllTestRuns listLatestRuns( TestMap const & tests );
		AriaLib_API AllTestRuns listLatestRuns( TestMap const & tests
			, wxProgressDialog & progress
			, int & index );
		AriaLib_API void listLatestRuns( TestMap const & tests
			, AllTestRuns & result );
		AriaLib_API void listLatestRuns( TestMap const & tests
			, AllTestRuns & result
			, wxProgressDialog & progress
			, int & index );
		AriaLib_API void listLatestRuns( Renderer renderer
			, TestMap const & tests
			, RendererTestRuns & result );
		AriaLib_API void listLatestRuns( Renderer renderer
			, TestMap const & tests
			, RendererTestRuns & result
			, wxProgressDialog & progress
			, int & index );
		AriaLib_API RunMap listRuns( int testId );
		AriaLib_API void deleteRun( uint32_t runId );
		AriaLib_API void updateRunHost( uint32_t runId, int32_t hostId );
		AriaLib_API void updateRunStatus( uint32_t runId, RunStatus status );
		AriaLib_API std::vector< Host const * > listTestHosts( Test const & test
			, Renderer const & renderer );
		AriaLib_API std::map< wxDateTime, TestTimes > listTestTimes( Test const & test
			, Renderer const & renderer
			, Host const & host
			, TestStatus maxStatus );

		AriaLib_API void insertTest( Test & test
			, bool moveFiles = true );
		AriaLib_API void updateRunsEngineDate( db::DateTime const & date );
		AriaLib_API Host * getHost( std::string const & platform
			, std::string const & cpu
			, std::string const & gpu );

		RendererMap const & getRenderers()const
		{
			return m_renderers;
		}

		CategoryMap const & getCategories()const
		{
			return m_categories;
		}

		PlatformMap const & getPlatforms()const
		{
			return m_platforms;
		}

		CpuMap const & getCpus()const
		{
			return m_cpus;
		}

		GpuMap const & getGpus()const
		{
			return m_gpus;
		}

		Plugin const & getPlugin()const
		{
			return *m_plugin;
		}

		Config const & getConfig()const
		{
			return m_config;
		}

		FileSystem & getFileSystem()const
		{
			return m_fileSystem;
		}

	public:
		struct InsertIdValue
		{
		protected:
			InsertIdValue() = default;
			explicit InsertIdValue( std::string const & tableName
				, uint32_t nameSize
				, db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO " + tableName + " (Name) VALUES (?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM " + tableName + " WHERE Name=?;" ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, nameSize ) }
				, sName{ select->createParameter( "Name", db::FieldType::eVarchar, nameSize ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create Insert" + tableName + " INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create Insert" + tableName + " SELECT statement." };
				}
			}

		public:
			int32_t insert( std::string const & name );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * name{};
			db::Parameter * sName{};
		};

		struct InsertIdId
		{
		protected:
			InsertIdId() = default;
			explicit InsertIdId( std::string const & tableName
				, std::string const & lhsIdName
				, std::string const & rhsIdName
				, db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO " + tableName + " (" + lhsIdName + ", " + rhsIdName + ") VALUES (?, ?);" ) }
				, lhsId{ stmt->createParameter( lhsIdName, db::FieldType::eSint32 ) }
				, rhsId{ stmt->createParameter( rhsIdName, db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create Insert" + tableName + " INSERT statement." };
				}
			}

		public:
			void insert( int32_t lhs, int32_t rhs );

		private:
			db::StatementPtr stmt;
			db::Parameter * lhsId{};
			db::Parameter * rhsId{};
		};

		struct InsertRenderer
			: InsertIdValue
		{
			InsertRenderer() = default;
			explicit InsertRenderer( db::Connection & connection )
				: InsertIdValue{ "Renderer", 10u, connection }
			{
			}
		};

		struct InsertCategory
			: InsertIdValue
		{
			InsertCategory() = default;
			explicit InsertCategory( db::Connection & connection )
				: InsertIdValue{ "Category", 50u, connection }
			{
			}
		};

		struct InsertKeyword
			: InsertIdValue
		{
			InsertKeyword() = default;
			explicit InsertKeyword( db::Connection & connection )
				: InsertIdValue{ "Keyword", 50u, connection }
			{
			}
		};

		struct InsertPlatform
			: InsertIdValue
		{
			InsertPlatform() = default;
			explicit InsertPlatform( db::Connection & connection )
				: InsertIdValue{ "Platform", 256u, connection }
			{
			}
		};

		struct InsertCpu
			: InsertIdValue
		{
			InsertCpu() = default;
			explicit InsertCpu( db::Connection & connection )
				: InsertIdValue{ "CPU", 256u, connection }
			{
			}
		};

		struct InsertGpu
			: InsertIdValue
		{
			InsertGpu() = default;
			explicit InsertGpu( db::Connection & connection )
				: InsertIdValue{ "GPU", 256u, connection }
			{
			}
		};

		struct InsertCategoryKeyword
			: InsertIdId
		{
			InsertCategoryKeyword() = default;
			explicit InsertCategoryKeyword( db::Connection & connection )
				: InsertIdId{ "CategoryKeyword", "CategoryId", "KeywordId", connection }
			{
			}
		};

		struct InsertTestKeyword
			: InsertIdId
		{
			InsertTestKeyword() = default;
			explicit InsertTestKeyword( db::Connection & connection )
				: InsertIdId{ "TestKeyword", "TestId", "KeywordId", connection }
			{
			}
		};

		struct InsertTest
		{
			InsertTest() = default;
			explicit InsertTest( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO Test (CategoryId, Name, IgnoreResult) VALUES (?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM Test WHERE CategoryId=? AND Name=?;" ) }
				, categoryId{ stmt->createParameter( "CategoryId", db::FieldType::eSint32 ) }
				, sCategoryId{ select->createParameter( "CategoryId", db::FieldType::eSint32 ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, sName{ select->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, ignoreResult{ stmt->createParameter( "IgnoreResult", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertTest INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertTest SELECT statement." };
				}
			}

			int32_t insert( int32_t categoryId
				, std::string const & name
				, bool ignoreResult = false );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * categoryId{};
			db::Parameter * sCategoryId{};
			db::Parameter * name{};
			db::Parameter * sName{};
			db::Parameter * ignoreResult{};
		};

		struct InsertHost
			: InsertIdValue
		{
			InsertHost() = default;
			explicit InsertHost( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO Host (PlatformId, CpuId, GpuId) VALUES (?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM Host WHERE PlatformId=? AND CpuId=? AND GpuId=?;" ) }
				, platformId{ stmt->createParameter( "PlatformId", db::FieldType::eSint32 ) }
				, sPlatformId{ select->createParameter( "PlatformId", db::FieldType::eSint32 ) }
				, cpuId{ stmt->createParameter( "CpuId", db::FieldType::eSint32 ) }
				, sCpuId{ select->createParameter( "CpuId", db::FieldType::eSint32 ) }
				, gpuId{ stmt->createParameter( "GpuId", db::FieldType::eSint32 ) }
				, sGpuId{ select->createParameter( "GpuId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertHost INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertHost SELECT statement." };
				}
			}

			int32_t insert( int32_t platformId
				, int32_t cpuId
				, int32_t gpuId );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * platformId{};
			db::Parameter * sPlatformId{};
			db::Parameter * cpuId{};
			db::Parameter * sCpuId{};
			db::Parameter * gpuId{};
			db::Parameter * sGpuId{};
		};

		struct InsertRunV2
		{
			InsertRunV2() = default;
			explicit InsertRunV2( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO TestRun (TestId, RendererId, RunDate, Status, EngineDate, SceneDate) VALUES (?, ?, ?, ?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM TestRun WHERE TestId=? AND RendererId=? AND RunDate=? AND Status=? AND EngineDate=? AND SceneDate=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, sTestId{ select->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, sRendererId{ select->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, sRunDate{ select->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, sStatus{ select->createParameter( "Status", db::FieldType::eSint32 ) }
				, engineDate{ stmt->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, sEngineDate{ select->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, testDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, sTestDate{ select->createParameter( "SceneDate", db::FieldType::eDatetime ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertRun INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertRun SELECT statement." };
				}
			}

			int32_t insert( int32_t id
				, int32_t inRendererId
				, db::DateTime dateRun
				, TestStatus status
				, db::DateTime const & dateEngine
				, db::DateTime const & dateScene );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * testId{};
			db::Parameter * sTestId{};
			db::Parameter * rendererId{};
			db::Parameter * sRendererId{};
			db::Parameter * runDate{};
			db::Parameter * sRunDate{};
			db::Parameter * status{};
			db::Parameter * sStatus{};
			db::Parameter * engineDate{};
			db::Parameter * sEngineDate{};
			db::Parameter * testDate{};
			db::Parameter * sTestDate{};
		};

		struct InsertRun
		{
			InsertRun() = default;
			explicit InsertRun( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO TestRun (TestId, RendererId, RunDate, Status, EngineDate, SceneDate, TotalTime, AvgFrameTime, LastFrameTime, HostId) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM TestRun WHERE TestId=? AND RendererId=? AND RunDate=? AND Status=? AND EngineDate=? AND SceneDate=? AND TotalTime=? AND AvgFrameTime=? AND LastFrameTime=? AND HostId=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, sTestId{ select->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, sRendererId{ select->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, sRunDate{ select->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, sStatus{ select->createParameter( "Status", db::FieldType::eSint32 ) }
				, engineDate{ stmt->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, sEngineDate{ select->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, testDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, sTestDate{ select->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, totalTime{ stmt->createParameter( "TotalTime", db::FieldType::eUint32 ) }
				, sTotalTime{ select->createParameter( "TotalTime", db::FieldType::eUint32 ) }
				, avgFrameTime{ stmt->createParameter( "AvgFrameTime", db::FieldType::eUint32 ) }
				, sAvgFrameTime{ select->createParameter( "AvgFrameTime", db::FieldType::eUint32 ) }
				, lastFrameTime{ stmt->createParameter( "LastFrameTime", db::FieldType::eUint32 ) }
				, sLastFrameTime{ select->createParameter( "LastFrameTime", db::FieldType::eUint32 ) }
				, hostId{ stmt->createParameter( "HostId", db::FieldType::eSint32 ) }
				, sHostId{ select->createParameter( "HostId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertRun INSERT statement." };
				}

				if ( !select->initialise() )
				{
					throw std::runtime_error{ "Couldn't create InsertRun SELECT statement." };
				}
			}

			int32_t insert( int32_t id
				, int32_t inRendererId
				, db::DateTime dateRun
				, TestStatus status
				, db::DateTime const & dateEngine
				, db::DateTime const & dateScene
				, Microseconds totalTime
				, Microseconds avgFrameTime
				, Microseconds lastFrameTime
				, Host const & host );

		private:
			db::StatementPtr stmt;
			db::StatementPtr select;
			db::Parameter * testId{};
			db::Parameter * sTestId{};
			db::Parameter * rendererId{};
			db::Parameter * sRendererId{};
			db::Parameter * runDate{};
			db::Parameter * sRunDate{};
			db::Parameter * status{};
			db::Parameter * sStatus{};
			db::Parameter * engineDate{};
			db::Parameter * sEngineDate{};
			db::Parameter * testDate{};
			db::Parameter * sTestDate{};
			db::Parameter * totalTime{};
			db::Parameter * sTotalTime{};
			db::Parameter * avgFrameTime{};
			db::Parameter * sAvgFrameTime{};
			db::Parameter * lastFrameTime{};
			db::Parameter * sLastFrameTime{};
			db::Parameter * hostId{};
			db::Parameter * sHostId{};
		};

		struct UpdateTestIgnoreResult
		{
			UpdateTestIgnoreResult() = default;
			explicit UpdateTestIgnoreResult( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET IgnoreResult=? WHERE Id=?;" ) }
				, ignore{ stmt->createParameter( "IgnoreResult", db::FieldType::eSint32 ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateTestIgnoreResult UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * ignore{};
			db::Parameter * id{};
		};

		struct UpdateRunStatus
		{
			UpdateRunStatus() = default;
			explicit UpdateRunStatus( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET Status=?, EngineDate=?, SceneDate=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, engineDate{ stmt->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, testDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunStatus UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * status{};
			db::Parameter * engineDate{};
			db::Parameter * testDate{};
			db::Parameter * id{};
		};

		struct UpdateRunDates
		{
			UpdateRunDates() = default;
			explicit UpdateRunDates( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET EngineDate=?, SceneDate=? WHERE Id=?;" ) }
				, engineDate{ stmt->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, testDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunDates UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineDate{};
			db::Parameter * testDate{};
			db::Parameter * id{};
		};

		struct UpdateRunEngineDate
		{
			UpdateRunEngineDate() = default;
			explicit UpdateRunEngineDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET EngineDate=? WHERE Id=?;" ) }
				, engineDate{ stmt->createParameter( "EngineDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunEngineDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineDate{};
			db::Parameter * id{};
		};

		struct UpdateRunSceneDate
		{
			UpdateRunSceneDate() = default;
			explicit UpdateRunSceneDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET SceneDate=? WHERE Id=?;" ) }
				, testDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunSceneDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * testDate{};
			db::Parameter * id{};
		};

		struct CheckTableExists
		{
			CheckTableExists() = default;
			explicit CheckTableExists( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT name FROM sqlite_master WHERE type = 'table' AND name=?;" ) }
				, tableName{ stmt->createParameter( "TableName", db::FieldType::eVarchar, 255 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create CheckTableExists SELECT statement." };
				}
			}

			bool checkTable( std::string const & name );

		private:
			db::StatementPtr stmt;
			db::Parameter * tableName{};
		};

		struct ListCategories
		{
			ListCategories() = default;
			explicit ListCategories( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, Name FROM Category" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListCategories SELECT statement." };
				}
			}

			void listCategories( CategoryMap & categories );

		private:
			db::StatementPtr stmt;
		};

		struct ListPlatforms
		{
			ListPlatforms() = default;
			explicit ListPlatforms( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, Name FROM Platform" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListPlatforms SELECT statement." };
				}
			}

			void list( PlatformMap & platforms );

		private:
			db::StatementPtr stmt;
		};

		struct ListCpus
		{
			ListCpus() = default;
			explicit ListCpus( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, Name FROM CPU" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListCpus SELECT statement." };
				}
			}

			void list( CpuMap & cpus );

		private:
			db::StatementPtr stmt;
		};

		struct ListGpus
		{
			ListGpus() = default;
			explicit ListGpus( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, Name FROM GPU" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListGpus SELECT statement." };
				}
			}

			void list( GpuMap & cpus );

		private:
			db::StatementPtr stmt;
		};

		struct ListHosts
		{
			ListHosts() = default;
			explicit ListHosts( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, PlatformId, CpuId, GpuId FROM Host" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListHosts SELECT statement." };
				}
			}

			void list( PlatformMap const & platforms
				, CpuMap const & cpus
				, GpuMap const & gpus
				, HostMap & hosts );

		private:
			db::StatementPtr stmt;
		};

		struct ListTests
		{
			ListTests() = default;
			explicit ListTests( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, CategoryId, Name, IgnoreResult FROM Test ORDER BY CategoryId, Name" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListTests SELECT statement." };
				}
			}

			TestMap listTests( CategoryMap & categories
				, wxProgressDialog & progress
				, int & index );
			void listTests( CategoryMap & categories
				, TestMap & result
				, wxProgressDialog & progress
				, int & index );

		private:
			db::StatementPtr stmt;
		};

		struct ListLatestTestRun
		{
			ListLatestTestRun() = default;
			explicit ListLatestTestRun( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Id, RendererId, MAX(RunDate) AS RunDate, Status, EngineDate, SceneDate FROM TestRun WHERE TestId=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListLatestTestRun SELECT statement." };
				}
			}

			db::ResultPtr listTests( int32_t id );

		private:
			db::StatementPtr stmt;
			db::Parameter * testId{};
		};

		struct ListLatestRendererTests
		{
			ListLatestRendererTests() = default;
			explicit ListLatestRendererTests( TestDatabase * database )
				: database{ database }
				, stmt{ database->m_database.createStatement( "SELECT CategoryId, TestId, TestRun.Id, MAX(RunDate) AS RunDate, HostId, Status, EngineDate, SceneDate, TotalTime, AvgFrameTime, LastFrameTime FROM Test, TestRun WHERE Test.Id=TestRun.TestId AND RendererId=? GROUP BY CategoryId, TestId ORDER BY CategoryId, TestId; " ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListLatestRendererTests SELECT statement." };
				}
			}

			RendererTestRuns listTests( TestMap const & tests
				, HostMap & hosts
				, CategoryMap & categories
				, Renderer renderer
				, wxProgressDialog & progress
				, int & index );
			void listTests( TestMap const & tests
				, HostMap & hosts
				, CategoryMap & categories
				, Renderer renderer
				, RendererTestRuns & result
				, wxProgressDialog & progress
				, int & index );

		private:
			TestDatabase * database{};
			db::StatementPtr stmt;
			db::Parameter * rendererId{};
		};

		struct ListTestRuns
		{
			ListTestRuns() = default;
			explicit ListTestRuns( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "SELECT TestRun.Id, Status, RunDate, HostId, TotalTime, AvgFrameTime, LastFrameTime FROM TestRun WHERE TestId=? ORDER BY RunDate DESC;" ) }
				, id{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListTestRuns SELECT statement." };
				}
			}

			RunMap listRuns( HostMap const & hosts
				, int testId );

		private:
			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct DeleteRun
		{
			DeleteRun() = default;
			explicit DeleteRun( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "DELETE FROM TestRun WHERE Id=?;" ) }
				, id{ stmt->createParameter( "RunId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create DeleteRun DELETE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct DeleteTest
		{
			DeleteTest() = default;
			explicit DeleteTest( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "DELETE FROM Test WHERE Id=?;" ) }
				, id{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create DeleteTest DELETE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct DeleteTestRuns
		{
			DeleteTestRuns() = default;
			explicit DeleteTestRuns( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "DELETE FROM TestRun WHERE TestId=?;" ) }
				, id{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create DeleteTestRuns DELETE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct DeleteCategory
		{
			DeleteCategory() = default;
			explicit DeleteCategory( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "DELETE FROM Category WHERE Id=?;" ) }
				, id{ stmt->createParameter( "CategoryId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create DeleteCategory DELETE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct DeleteCategoryTests
		{
			DeleteCategoryTests() = default;
			explicit DeleteCategoryTests( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "DELETE FROM Test WHERE CategoryId=?;" ) }
				, id{ stmt->createParameter( "CategoryId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create DeleteCategoryTests DELETE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct DeleteCategoryTestsRuns
		{
			DeleteCategoryTestsRuns() = default;
			explicit DeleteCategoryTestsRuns( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "DELETE FROM TestRun WHERE TestId IN (SELECT Id FROM Test WHERE CategoryId=?);" ) }
				, id{ stmt->createParameter( "CategoryId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create DeleteCategoryTestsRuns DELETE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * id{};
		};

		struct UpdateRunsEngineDate
		{
			UpdateRunsEngineDate() = default;
			explicit UpdateRunsEngineDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET EngineDate=? WHERE Id IN (SELECT MAX(Id) FROM TestRun GROUP BY TestId, RendererId);" ) }
				, engineDate{ stmt->createParameter( "EngineDate", db::FieldType::eDatetime ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunsEngineDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineDate{};
		};

		struct UpdateTestCategory
		{
			UpdateTestCategory() = default;
			explicit UpdateTestCategory( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET CategoryId=? WHERE Id=?;" ) }
				, categoryId{ stmt->createParameter( "CategoryId", db::FieldType::eSint32 ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateTestCategory UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * categoryId{};
			db::Parameter * id{};
		};

		struct UpdateTestName
		{
			UpdateTestName() = default;
			explicit UpdateTestName( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Test SET Name=? WHERE Id=?;" ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateTestName UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * name{};
			db::Parameter * id{};
		};

		struct UpdateCategoryName
		{
			UpdateCategoryName() = default;
			explicit UpdateCategoryName( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE Category SET Name=? WHERE Id=?;" ) }
				, name{ stmt->createParameter( "Name", db::FieldType::eVarchar, 1024 ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateCategoryName UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * name{};
			db::Parameter * id{};
		};

		struct UpdateHost
		{
			UpdateHost() = default;
			explicit UpdateHost( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET HostId=? WHERE Id=?;" ) }
				, hostId{ stmt->createParameter( "HostId", db::FieldType::eSint32 ) }
				, runId{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateHost UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * hostId{};
			db::Parameter * runId{};
		};

		struct UpdateStatus
		{
			UpdateStatus() = default;
			explicit UpdateStatus( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET Status=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, runId{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateStatus UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * status{};
			db::Parameter * runId{};
		};

		struct GetDatabaseVersion
		{
			GetDatabaseVersion() = default;
			explicit GetDatabaseVersion( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT Version FROM TestsDatabase;" ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create GetDatabaseVersion SELECT statement." };
				}
			}

			uint32_t get();

			db::StatementPtr stmt;
		};

		struct ListTestHosts
		{
			ListTestHosts() = default;
			explicit ListTestHosts( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT HostId FROM TestRun WHERE TestId=? AND RendererId=? AND TotalTime > 0 GROUP BY HostId;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "ListTestHosts", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListTestHosts SELECT statement." };
				}
			}

			std::vector< Host const * > list( Test const & test
				, Renderer const & renderer
				, HostMap const & hosts );

			db::StatementPtr stmt;

		private:
			db::Parameter * testId{};
			db::Parameter * rendererId{};
		};

		struct ListAllTimes
		{
			ListAllTimes() = default;
			explicit ListAllTimes( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT RunDate, TotalTime, AvgFrameTime, LastFrameTime FROM TestRun WHERE TestId=? AND RendererId=? AND HostId=? AND Status <= ? AND TotalTime > 0 ORDER BY RunDate;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, hostId{ stmt->createParameter( "HostId", db::FieldType::eSint32 ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListAllTimes SELECT statement." };
				}
			}

			std::map< wxDateTime, TestTimes > listTimes( Test const & test
				, Renderer const & renderer
				, Host const & host
				, TestStatus maxStatus );

			db::StatementPtr stmt;

		private:
			db::Parameter * testId{};
			db::Parameter * rendererId{};
			db::Parameter * hostId{};
			db::Parameter * status{};
		};

	private:
		void insertRun( TestRun & run
			, bool moveFiles = true );
		void updateTestIgnoreResult( Test const & test
			, bool ignore );
		void updateRunStatus( TestRun const & run );
		void updateRunEngineDate( TestRun const & run );
		void updateRunTestDate( TestRun const & run );
		void doCreateV1( wxProgressDialog & progress, int & index );
		void doCreateV2( wxProgressDialog & progress, int & index );
		void doCreateV3( wxProgressDialog & progress, int & index );
		void doCreateV4( wxProgressDialog & progress, int & index );
		void doCreateV5( wxProgressDialog & progress, int & index );
		void doCreateV6( wxProgressDialog & progress, int & index );
		void doUpdateCategories();
		void doUpdateRenderers();
		void doListCategories( wxProgressDialog & progress, int & index );
		void doFillDatabase( wxProgressDialog & progress, int & index );
		void doAssignTestKeywords( db::Result const & testNames, wxProgressDialog & progress, int & index );

	private:
		Plugin * m_plugin;
		Config & m_config;
		FileSystem & m_fileSystem;
		db::Connection m_database;
		RendererMap m_renderers;
		CategoryMap m_categories;
		KeywordMap m_keywords;
		PlatformMap m_platforms;
		CpuMap m_cpus;
		GpuMap m_gpus;
		HostMap m_hosts;
		InsertRenderer m_insertRenderer;
		InsertCategory m_insertCategory;
		InsertKeyword m_insertKeyword;
		InsertPlatform m_insertPlatform;
		InsertCpu m_insertCpu;
		InsertGpu m_insertGpu;
		InsertHost m_insertHost;
		InsertCategoryKeyword m_insertCategoryKeyword;
		InsertTestKeyword m_insertTestKeyword;
		InsertTest m_insertTest;
		InsertRunV2 m_insertRunV2;
		InsertRun m_insertRun;
		CheckTableExists m_checkTableExists;
		UpdateRunStatus m_updateRunStatus;
		UpdateTestIgnoreResult m_updateTestIgnoreResult;
		UpdateRunDates m_updateRunDates;
		UpdateRunEngineDate m_updateRunEngineDate;
		UpdateRunSceneDate m_updateRunSceneDate;
		ListCategories m_listCategories;
		ListPlatforms m_listPlatforms;
		ListCpus m_listCpus;
		ListGpus m_listGpus;
		ListHosts m_listHosts;
		ListTests m_listTests;
		ListLatestTestRun m_listLatestRun;
		ListLatestRendererTests m_listLatestRendererRuns;
		ListTestRuns m_listTestRuns;
		DeleteRun m_deleteRun;
		DeleteTest m_deleteTest;
		DeleteTestRuns m_deleteTestRuns;
		DeleteCategory m_deleteCategory;
		DeleteCategoryTests m_deleteCategoryTests;
		DeleteCategoryTestsRuns m_deleteCategoryTestsRuns;
		UpdateRunsEngineDate m_updateRunsEngineDate;
		UpdateTestCategory m_updateTestCategory;
		UpdateTestName m_updateTestName;
		UpdateCategoryName m_updateCategoryName;
		UpdateHost m_updateHost;
		UpdateStatus m_updateStatus;
		GetDatabaseVersion m_getDatabaseVersion;
		ListTestHosts m_listTestHosts;
		ListAllTimes m_listAllTimes;
	};
}

#endif
