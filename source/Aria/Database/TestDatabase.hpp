/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestDatabase_HPP___
#define ___CTP_TestDatabase_HPP___

#include "Prerequisites.hpp"
#include "Database/DbConnection.hpp"
#include "Database/DbStatement.hpp"

#include <list>
#include <map>

class wxProgressDialog;

namespace aria
{
	class TestDatabase
	{
		friend class DatabaseTest;

	public:
		explicit TestDatabase( Plugin & plugin
			, FileSystem & fileSystem );
		~TestDatabase();

		void initialise( wxProgressDialog & progress
			, int & index );

		void moveResultFile( DatabaseTest const & test
			, TestStatus oldStatus
			, TestStatus newStatus
			, wxFileName const & work );
		void moveResultImage( DatabaseTest const & test
			, Category oldCategory
			, Category newCategory );
		bool updateReferenceFile( DatabaseTest const & test
			, TestStatus status );

		Renderer createRenderer( std::string const & name );
		Category createCategory( std::string const & name );
		Keyword createKeyword( std::string const & name );

		TestMap listTests();
		TestMap listTests( wxProgressDialog & progress
			, int & index );
		void listTests( TestMap & result );
		void listTests( TestMap & result
			, wxProgressDialog & progress
			, int & index );

		AllTestRuns listLatestRuns( TestMap const & tests );
		AllTestRuns listLatestRuns( TestMap const & tests
			, wxProgressDialog & progress
			, int & index );
		void listLatestRuns( TestMap const & tests
			, AllTestRuns & result );
		void listLatestRuns( TestMap const & tests
			, AllTestRuns & result
			, wxProgressDialog & progress
			, int & index );
		void listLatestRuns( Renderer renderer
			, TestMap const & tests
			, RendererTestRuns & result );
		void listLatestRuns( Renderer renderer
			, TestMap const & tests
			, RendererTestRuns & result
			, wxProgressDialog & progress
			, int & index );
		RunMap listRuns( int testId );
		void deleteRun( uint32_t runId );
		std::vector< Host const * > listTestHosts( Test const & test
			, Renderer const & renderer );
		std::map< wxDateTime, TestTimes > listTestTimes( Test const & test
			, Renderer const & renderer
			, Host const & host );

		void insertTest( Test & test
			, bool moveFiles = true );
		void updateRunsEngineDate( db::DateTime const & date );
		void updateTestCategory( Test const & test
			, Category category );
		void updateTestName( Test const & test
			, wxString const & name );
		Host * getHost( std::string const & platform
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
				: stmt{ connection.createStatement( "INSERT INTO TestRun (TestId, RendererId, RunDate, Status, CastorDate, SceneDate) VALUES (?, ?, ?, ?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM TestRun WHERE TestId=? AND RendererId=? AND RunDate=? AND Status=? AND CastorDate=? AND SceneDate=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, sTestId{ select->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, sRendererId{ select->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, sRunDate{ select->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, sStatus{ select->createParameter( "Status", db::FieldType::eSint32 ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sCastorDate{ select->createParameter( "CastorDate", db::FieldType::eDatetime ) }
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
				, db::DateTime const & dateCastor
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
			db::Parameter * engineData{};
			db::Parameter * sCastorDate{};
			db::Parameter * testDate{};
			db::Parameter * sTestDate{};
		};

		struct InsertRun
		{
			InsertRun() = default;
			explicit InsertRun( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO TestRun (TestId, RendererId, RunDate, Status, CastorDate, SceneDate, TotalTime, AvgFrameTime, LastFrameTime, HostId) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM TestRun WHERE TestId=? AND RendererId=? AND RunDate=? AND Status=? AND CastorDate=? AND SceneDate=? AND TotalTime=? AND AvgFrameTime=? AND LastFrameTime=? AND HostId=?;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, sTestId{ select->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, sRendererId{ select->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, runDate{ stmt->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, sRunDate{ select->createParameter( "RunDate", db::FieldType::eDatetime ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, sStatus{ select->createParameter( "Status", db::FieldType::eSint32 ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sCastorDate{ select->createParameter( "CastorDate", db::FieldType::eDatetime ) }
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
				, db::DateTime const & dateCastor
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
			db::Parameter * engineData{};
			db::Parameter * sCastorDate{};
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
				: stmt{ connection.createStatement( "UPDATE TestRun SET Status=?, CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, status{ stmt->createParameter( "Status", db::FieldType::eSint32 ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
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
			db::Parameter * engineData{};
			db::Parameter * testDate{};
			db::Parameter * id{};
		};

		struct UpdateRunDates
		{
			UpdateRunDates() = default;
			explicit UpdateRunDates( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, testDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunDates UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineData{};
			db::Parameter * testDate{};
			db::Parameter * id{};
		};

		struct UpdateRunCastorDate
		{
			UpdateRunCastorDate() = default;
			explicit UpdateRunCastorDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=? WHERE Id=?;" ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunCastorDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineData{};
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
				: stmt{ connection.createStatement( "SELECT Id, RendererId, MAX(RunDate) AS RunDate, Status, CastorDate, SceneDate FROM TestRun WHERE TestId=?;" ) }
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
				, stmt{ database->m_database.createStatement( "SELECT CategoryId, TestId, TestRun.Id, MAX(RunDate) AS RunDate, HostId, Status, CastorDate, SceneDate, TotalTime, AvgFrameTime, LastFrameTime FROM Test, TestRun WHERE Test.Id=TestRun.TestId AND RendererId=? GROUP BY CategoryId, TestId ORDER BY CategoryId, TestId; " ) }
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
			TestDatabase * database;
			db::StatementPtr stmt;
			db::Parameter * rendererId{};
		};

		struct ListTestRuns
		{
			ListTestRuns() = default;
			explicit ListTestRuns( TestDatabase * database )
				: stmt{ database->m_database.createStatement( "SELECT TestRun.Id, Status, RunDate, HostId, TotalTime, AvgFrameTime, LastFrameTime FROM TestRun WHERE TestId=?;" ) }
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

		struct UpdateRunsCastorDate
		{
			UpdateRunsCastorDate() = default;
			explicit UpdateRunsCastorDate( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=? WHERE Id IN (SELECT MAX(Id) FROM TestRun GROUP BY TestId, RendererId);" ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunsCastorDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineData;
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
			db::Parameter * categoryId;
			db::Parameter * id;
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
			db::Parameter * name;
			db::Parameter * id;
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
			db::Parameter * testId;
			db::Parameter * rendererId;
		};

		struct ListAllTimes
		{
			ListAllTimes() = default;
			explicit ListAllTimes( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT RunDate, TotalTime, AvgFrameTime, LastFrameTime FROM TestRun WHERE TestId=? AND RendererId=? AND HostId=? AND TotalTime > 0 ORDER BY RunDate;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
				, hostId{ stmt->createParameter( "HostId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListAllTimes SELECT statement." };
				}
			}

			std::map< wxDateTime, TestTimes > listTimes( Test const & test
				, Renderer const & renderer
				, Host const & host );

			db::StatementPtr stmt;

		private:
			db::Parameter * testId;
			db::Parameter * rendererId;
			db::Parameter * hostId;
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
		void doUpdateCategories();
		void doUpdateRenderers();
		void doListCategories( wxProgressDialog & progress, int & index );
		void doFillDatabase( wxProgressDialog & progress, int & index );

	private:
		Plugin * m_plugin;
		Config & m_config;
		FileSystem & m_fileSystem;
		db::Connection m_database;
		InsertRenderer m_insertRenderer;
		InsertCategory m_insertCategory;
		InsertKeyword m_insertKeyword;
		InsertPlatform m_insertPlatform;
		InsertCpu m_insertCpu;
		InsertGpu m_insertGpu;
		InsertHost m_insertHost;
		InsertCategoryKeyword m_insertCategoryKeyword;
		InsertTestKeyword m_insertTestKeyword;
		RendererMap m_renderers;
		CategoryMap m_categories;
		KeywordMap m_keywords;
		PlatformMap m_platforms;
		CpuMap m_cpus;
		GpuMap m_gpus;
		HostMap m_hosts;
		InsertTest m_insertTest;
		InsertRunV2 m_insertRunV2;
		InsertRun m_insertRun;
		CheckTableExists m_checkTableExists;
		UpdateRunStatus m_updateRunStatus;
		UpdateTestIgnoreResult m_updateTestIgnoreResult;
		UpdateRunDates m_updateRunDates;
		UpdateRunCastorDate m_updateRunEngineDate;
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
		UpdateRunsCastorDate m_updateRunsCastorDate;
		UpdateTestCategory m_updateTestCategory;
		UpdateTestName m_updateTestName;
		GetDatabaseVersion m_getDatabaseVersion;
		ListTestHosts m_listTestHosts;
		ListAllTimes m_listAllTimes;
	};
}

#endif
