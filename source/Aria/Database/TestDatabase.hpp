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
		explicit TestDatabase( Config config
			, FileSystem & fileSystem );
		~TestDatabase();

		void initialise( wxProgressDialog & progress
			, int & index );

		void moveResultFile( DatabaseTest const & test
			, TestStatus oldStatus
			, TestStatus newStatus
			, wxFileName const & work );
		void moveResultImage( DatabaseTest const & test
			, wxString const & oldName
			, wxString const & newName );
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
		std::map< wxDateTime, TestTimes > listTestTimes( Test const & test
			, Renderer const & renderer );

		void insertTest( Test & test
			, bool moveFiles = true );
		void updateRunsCastorDate( db::DateTime const & date );
		void updateTestCategory( Test const & test
			, Category category );
		void updateTestName( Test const & test
			, wxString const & name );

		RendererMap const & getRenderers()const
		{
			return m_renderers;
		}

		CategoryMap const & getCategories()const
		{
			return m_categories;
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
			InsertIdValue( InsertIdValue const & ) = default;
			InsertIdValue & operator=( InsertIdValue const & ) = default;
			InsertIdValue( InsertIdValue && ) = default;
			InsertIdValue & operator=( InsertIdValue && ) = default;
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
			InsertIdId( InsertIdId const & ) = default;
			InsertIdId & operator=( InsertIdId const & ) = default;
			InsertIdId( InsertIdId && ) = default;
			InsertIdId & operator=( InsertIdId && ) = default;
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
			InsertRenderer( InsertRenderer const & ) = default;
			InsertRenderer & operator=( InsertRenderer const & ) = default;
			InsertRenderer( InsertRenderer && ) = default;
			InsertRenderer & operator=( InsertRenderer && ) = default;
			InsertRenderer() = default;
			explicit InsertRenderer( db::Connection & connection )
				: InsertIdValue{ "Renderer", 10u, connection }
			{
			}
		};

		struct InsertCategory
			: InsertIdValue
		{
			InsertCategory( InsertCategory const & ) = default;
			InsertCategory & operator=( InsertCategory const & ) = default;
			InsertCategory( InsertCategory && ) = default;
			InsertCategory & operator=( InsertCategory && ) = default;
			InsertCategory() = default;
			explicit InsertCategory( db::Connection & connection )
				: InsertIdValue{ "Category", 50u, connection }
			{
			}
		};

		struct InsertKeyword
			: InsertIdValue
		{
			InsertKeyword( InsertKeyword const & ) = default;
			InsertKeyword & operator=( InsertKeyword const & ) = default;
			InsertKeyword( InsertKeyword && ) = default;
			InsertKeyword & operator=( InsertKeyword && ) = default;
			InsertKeyword() = default;
			explicit InsertKeyword( db::Connection & connection )
				: InsertIdValue{ "Keyword", 50u, connection }
			{
			}
		};

		struct InsertCategoryKeyword
			: InsertIdId
		{
			InsertCategoryKeyword( InsertCategoryKeyword const & ) = default;
			InsertCategoryKeyword & operator=( InsertCategoryKeyword const & ) = default;
			InsertCategoryKeyword( InsertCategoryKeyword && ) = default;
			InsertCategoryKeyword & operator=( InsertCategoryKeyword && ) = default;
			InsertCategoryKeyword() = default;
			explicit InsertCategoryKeyword( db::Connection & connection )
				: InsertIdId{ "CategoryKeyword", "CategoryId", "KeywordId", connection }
			{
			}
		};

		struct InsertTestKeyword
			: InsertIdId
		{
			InsertTestKeyword( InsertTestKeyword const & ) = default;
			InsertTestKeyword & operator=( InsertTestKeyword const & ) = default;
			InsertTestKeyword( InsertTestKeyword && ) = default;
			InsertTestKeyword & operator=( InsertTestKeyword && ) = default;
			InsertTestKeyword() = default;
			explicit InsertTestKeyword( db::Connection & connection )
				: InsertIdId{ "TestKeyword", "TestId", "KeywordId", connection }
			{
			}
		};

		struct InsertTest
		{
			InsertTest( InsertTest const & ) = default;
			InsertTest & operator=( InsertTest const & ) = default;
			InsertTest( InsertTest && ) = default;
			InsertTest & operator=( InsertTest && ) = default;
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

		struct InsertRunV2
		{
			InsertRunV2( InsertRunV2 const & ) = default;
			InsertRunV2 & operator=( InsertRunV2 const & ) = default;
			InsertRunV2( InsertRunV2 && ) = default;
			InsertRunV2 & operator=( InsertRunV2 && ) = default;
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
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, sSceneDate{ select->createParameter( "SceneDate", db::FieldType::eDatetime ) }
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
			db::Parameter * sceneDate{};
			db::Parameter * sSceneDate{};
		};

		struct InsertRun
		{
			InsertRun( InsertRun const & ) = default;
			InsertRun & operator=( InsertRun const & ) = default;
			InsertRun( InsertRun && ) = default;
			InsertRun & operator=( InsertRun && ) = default;
			InsertRun() = default;
			explicit InsertRun( db::Connection & connection )
				: stmt{ connection.createStatement( "INSERT INTO TestRun (TestId, RendererId, RunDate, Status, CastorDate, SceneDate, TotalTime, AvgFrameTime, LastFrameTime) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);" ) }
				, select{ connection.createStatement( "SELECT Id FROM TestRun WHERE TestId=? AND RendererId=? AND RunDate=? AND Status=? AND CastorDate=? AND SceneDate=? AND TotalTime=? AND AvgFrameTime=? AND LastFrameTime=?;" ) }
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
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, sSceneDate{ select->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, totalTime{ stmt->createParameter( "TotalTime", db::FieldType::eUint32 ) }
				, sTotalTime{ select->createParameter( "TotalTime", db::FieldType::eUint32 ) }
				, avgFrameTime{ stmt->createParameter( "AvgFrameTime", db::FieldType::eUint32 ) }
				, sAvgFrameTime{ select->createParameter( "AvgFrameTime", db::FieldType::eUint32 ) }
				, lastFrameTime{ stmt->createParameter( "LastFrameTime", db::FieldType::eUint32 ) }
				, sLastFrameTime{ select->createParameter( "LastFrameTime", db::FieldType::eUint32 ) }
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
				, Microseconds lastFrameTime );

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
			db::Parameter * sceneDate{};
			db::Parameter * sSceneDate{};
			db::Parameter * totalTime{};
			db::Parameter * sTotalTime{};
			db::Parameter * avgFrameTime{};
			db::Parameter * sAvgFrameTime{};
			db::Parameter * lastFrameTime{};
			db::Parameter * sLastFrameTime{};
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
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
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
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct UpdateRunDates
		{
			UpdateRunDates() = default;
			explicit UpdateRunDates( db::Connection & connection )
				: stmt{ connection.createStatement( "UPDATE TestRun SET CastorDate=?, SceneDate=? WHERE Id=?;" ) }
				, engineData{ stmt->createParameter( "CastorDate", db::FieldType::eDatetime ) }
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunDates UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * engineData{};
			db::Parameter * sceneDate{};
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
				, sceneDate{ stmt->createParameter( "SceneDate", db::FieldType::eDatetime ) }
				, id{ stmt->createParameter( "Id", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create UpdateRunSceneDate UPDATE statement." };
				}
			}

			db::StatementPtr stmt;
			db::Parameter * sceneDate{};
			db::Parameter * id{};
		};

		struct CheckTableExists
		{
			CheckTableExists( CheckTableExists const & ) = default;
			CheckTableExists & operator=( CheckTableExists const & ) = default;
			CheckTableExists( CheckTableExists && ) = default;
			CheckTableExists & operator=( CheckTableExists && ) = default;
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

		struct ListTests
		{
			ListTests( ListTests const & ) = default;
			ListTests & operator=( ListTests const & ) = default;
			ListTests( ListTests && ) = default;
			ListTests & operator=( ListTests && ) = default;
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
			db::Parameter * testId{};
		};

		struct ListLatestTestRun
		{
			ListLatestTestRun( ListLatestTestRun const & ) = default;
			ListLatestTestRun & operator=( ListLatestTestRun const & ) = default;
			ListLatestTestRun( ListLatestTestRun && ) = default;
			ListLatestTestRun & operator=( ListLatestTestRun && ) = default;
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
			ListLatestRendererTests( ListLatestRendererTests const & ) = default;
			ListLatestRendererTests & operator=( ListLatestRendererTests const & ) = default;
			ListLatestRendererTests( ListLatestRendererTests && ) = default;
			ListLatestRendererTests & operator=( ListLatestRendererTests && ) = default;
			ListLatestRendererTests() = default;
			explicit ListLatestRendererTests( TestDatabase * database )
				: database{ database }
				, stmt{ database->m_database.createStatement( "SELECT CategoryId, TestId, TestRun.Id, MAX(RunDate) AS RunDate, Status, CastorDate, SceneDate, TotalTime, AvgFrameTime, LastFrameTime FROM Test, TestRun WHERE Test.Id=TestRun.TestId AND RendererId=? GROUP BY CategoryId, TestId ORDER BY CategoryId, TestId; " ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListLatestRendererTests SELECT statement." };
				}
			}

			RendererTestRuns listTests( TestMap const & tests
				, CategoryMap & categories
				, Renderer renderer
				, wxProgressDialog & progress
				, int & index );
			void listTests( TestMap const & tests
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
				, name{ stmt->createParameter( "Name", db::FieldType::eVARCHAR, 1024 ) }
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

		struct ListAllTimes
		{
			ListAllTimes() = default;
			explicit ListAllTimes( db::Connection & connection )
				: stmt{ connection.createStatement( "SELECT RunDate, TotalTime, AvgFrameTime, LastFrameTime FROM TestRun WHERE TestId=? AND RendererId=? AND TotalTime > 0 ORDER BY RunDate;" ) }
				, testId{ stmt->createParameter( "TestId", db::FieldType::eSint32 ) }
				, rendererId{ stmt->createParameter( "RendererId", db::FieldType::eSint32 ) }
			{
				if ( !stmt->initialise() )
				{
					throw std::runtime_error{ "Couldn't create ListAllTimes SELECT statement." };
				}
			}

			std::map< wxDateTime, TestTimes > listTimes( Test const & test
				, Renderer const & renderer );

			db::StatementPtr stmt;

		private:
			db::Parameter * testId;
			db::Parameter * rendererId;
		};

	private:
		void insertRun( TestRun & run
			, bool moveFiles = true );
		void updateTestIgnoreResult( Test const & test
			, bool ignore );
		void updateRunStatus( TestRun const & run );
		void updateRunCastorDate( TestRun const & run );
		void updateRunSceneDate( TestRun const & run );
		void doCreateV1( wxProgressDialog & progress, int & index );
		void doCreateV2( wxProgressDialog & progress, int & index );
		void doCreateV3( wxProgressDialog & progress, int & index );
		void doCreateV4( wxProgressDialog & progress, int & index );
		void doUpdateCategories();
		void doUpdateRenderers();
		void doListCategories( wxProgressDialog & progress, int & index );
		void doFillDatabase( wxProgressDialog & progress, int & index );

	private:
		Config m_config;
		FileSystem & m_fileSystem;
		db::Connection m_database;
		InsertRenderer m_insertRenderer;
		InsertCategory m_insertCategory;
		InsertKeyword m_insertKeyword;
		InsertCategoryKeyword m_insertCategoryKeyword;
		InsertTestKeyword m_insertTestKeyword;
		RendererMap m_renderers;
		CategoryMap m_categories;
		KeywordMap m_keywords;
		InsertTest m_insertTest;
		InsertRunV2 m_insertRunV2;
		InsertRun m_insertRun;
		CheckTableExists m_checkTableExists;
		UpdateRunStatus m_updateRunStatus;
		UpdateTestIgnoreResult m_updateTestIgnoreResult;
		UpdateRunDates m_updateRunDates;
		UpdateRunCastorDate m_updateRunCastorDate;
		UpdateRunSceneDate m_updateRunSceneDate;
		ListCategories m_listCategories;
		ListTests m_listTests;
		ListLatestTestRun m_listLatestRun;
		ListLatestRendererTests m_listLatestRendererRuns;
		UpdateRunsCastorDate m_updateRunsCastorDate;
		UpdateTestCategory m_updateTestCategory;
		UpdateTestName m_updateTestName;
		GetDatabaseVersion m_getDatabaseVersion;
		ListAllTimes m_listAllTimes;
	};
}

#endif
