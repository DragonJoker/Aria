#include "DatabaseTest.hpp"

#include "TestDatabase.hpp"
#include "TestsCounts.hpp"

namespace aria
{
	//*********************************************************************************************

	namespace
	{
		void moveResultFile( TestRun const & test
			, TestStatus oldStatus
			, TestStatus newStatus
			, wxFileName work )
		{
			if ( oldStatus == TestStatus::eNotRun
				|| newStatus == TestStatus::eNotRun
				|| oldStatus == newStatus )
			{
				return;
			}

			auto resultFolder = work / getResultFolder( *test.test );
			moveFile( resultFolder / getFolderName( oldStatus )
				, resultFolder / getFolderName( newStatus )
				, getResultName( test ) );
		}

		void moveResultImage( Config const config
			, DatabaseTest const & test
			, wxString const & oldName
			, wxString const & newName )
		{
			auto folder = config.work / getResultFolder( *test );
			moveFile( folder
				, folder
				, getResultName( *test, oldName )
				, getResultName( *test, newName ) );
		}

		void moveResultImage( Config const config
			, DatabaseTest const & test
			, Category oldCategory
			, Category newCategory )
		{
			auto srcFolder = config.work / getResultFolder( *test, oldCategory );
			auto dstFolder = config.work / getResultFolder( *test, newCategory );
			moveFile( srcFolder
				, dstFolder
				, getResultName( *test ) );
		}
	}

	//*********************************************************************************************

	DatabaseTest::DatabaseTest( TestDatabase & database
		, TestRun test )
		: m_database{ &database }
		, m_test{ std::move( test ) }
		, m_outOfEngineDate{ isOutOfEngineDate( database.m_config, m_test ) }
		, m_outOfSceneDate{ isOutOfSceneDate( database.m_config, m_test ) }
		, m_outOfDate{ m_outOfEngineDate || m_outOfSceneDate }
	{
	}

	void DatabaseTest::updateCastorDateNW( db::DateTime const & engineDate )
	{
		m_test.engineDate = engineDate;
		updateOutOfDate();
	}

	void DatabaseTest::updateEngineDate( db::DateTime const & engineDate )
	{
		if ( m_test.engineDate < engineDate )
		{
			updateCastorDateNW( engineDate );
			m_database->updateRunCastorDate( m_test );
		}
	}

	void DatabaseTest::updateEngineDate()
	{
		updateEngineRefDate( m_database->m_config );
		updateEngineDate( m_database->m_config.engineRefDate );
	}

	void DatabaseTest::updateSceneDate( db::DateTime const & sceneDate )
	{
		if ( m_test.sceneDate < sceneDate )
		{
			m_test.sceneDate = sceneDate;
			m_database->updateRunSceneDate( m_test );
			updateOutOfDate();
		}
	}

	void DatabaseTest::updateSceneDate()
	{
		auto sceneDate = aria::getSceneDate( m_database->m_config, m_test );
		updateSceneDate( sceneDate );
	}

	void DatabaseTest::updateStatusNW( TestStatus newStatus )
	{
		m_counts->remove( m_test.status );
		m_counts->add( newStatus );
		updateOutOfDate();
		m_test.status = newStatus;
	}

	void DatabaseTest::updateStatus( TestStatus newStatus
		, bool useAsReference )
	{
		auto & config = m_database->m_config;
		TestStatus oldStatus = m_test.status;
		updateEngineRefDate( config );
		m_test.engineDate = config.engineRefDate;
		updateStatusNW( newStatus );
		m_database->updateRunStatus( m_test );
		moveResultFile( m_test, oldStatus, newStatus, config.work );

		if ( useAsReference )
		{
			updateReference( newStatus );
		}
	}

	void DatabaseTest::createNewRun( TestStatus status
		, db::DateTime const & runDate )
	{
		auto & config = m_database->m_config;
		auto rawStatus = status;
		auto newStatus = rawStatus;

		if ( m_test.test->ignoreResult )
		{
			newStatus = TestStatus::eNegligible;
		}

		m_test.runDate = runDate;
		assert( m_test.runDate.IsValid() );
		updateEngineRefDate( config );
		m_test.engineDate = config.engineRefDate;
		assert( m_test.engineDate.IsValid() );
		m_test.sceneDate = aria::getSceneDate( config, m_test );
		assert( m_test.sceneDate.IsValid() );
		updateStatusNW( newStatus );
		m_database->insertRun( m_test );

		if ( m_test.test->ignoreResult )
		{
			updateReference( rawStatus );
		}
	}

	void DatabaseTest::createNewRun( wxFileName const & match )
	{
		auto path = match.GetPath();
		createNewRun( aria::getStatus( makeStdString( wxFileName{ path }.GetName() ) )
			, getFileDate( match ) );
	}

	void DatabaseTest::changeCategory( Category dstCategory
		, CategoryTestsCounts & dstCounts )
	{
		wxASSERT( m_counts != nullptr && "Test counts not set" );
		m_counts->removeTest( *this );
		dstCounts.addTest( *this );
	}

	void DatabaseTest::update( int id )
	{
		m_test.id = id;
	}

	void DatabaseTest::update( int id
		, db::DateTime runDate
		, TestStatus status
		, db::DateTime engineDate
		, db::DateTime sceneDate )
	{
		m_test.id = id;
		m_test.status = status;
		m_test.runDate = std::move( runDate );
		m_test.engineDate = std::move( engineDate );
		m_test.sceneDate = std::move( sceneDate );
		m_outOfEngineDate = isOutOfEngineDate( m_database->m_config, m_test );
		m_outOfSceneDate = isOutOfSceneDate( m_database->m_config, m_test );
		m_outOfDate = m_outOfEngineDate || m_outOfSceneDate;
	}

	void DatabaseTest::updateIgnoreResult( bool ignore
		, db::DateTime engineDate
		, bool useAsReference )
	{
		if ( m_test.test->ignoreResult != ignore )
		{
			if ( ignore )
			{
				m_counts->addIgnored();
			}
			else
			{
				m_counts->removeIgnored();
			}
		}

		m_test.test->ignoreResult = ignore;
		m_test.engineDate = std::move( engineDate );
		wxASSERT( m_test.engineDate.IsValid() );
		m_database->updateTestIgnoreResult( *m_test.test, ignore );

		if ( ignore )
		{
			updateStatus( TestStatus::eNegligible, useAsReference );
		}
	}

	void DatabaseTest::updateReference( TestStatus status )
	{
		auto & config = m_database->m_config;
		wxCopyFile( ( config.work / getResultFolder( *m_test.test ) / getFolderName( status ) / getResultName( m_test ) ).GetFullPath()
			, ( config.test / getReferenceFolder( m_test ) / getReferenceName( m_test ) ).GetFullPath()
			, true );
	}

	void DatabaseTest::updateOutOfDate( bool remove )const
	{
		bool outOfCastorDate{ isOutOfEngineDate( m_database->m_config, m_test ) };
		bool outOfSceneDate{ isOutOfSceneDate( m_database->m_config, m_test ) };
		bool outOfDate{ outOfSceneDate || outOfCastorDate };
		std::swap( m_outOfEngineDate, outOfCastorDate );
		std::swap( m_outOfSceneDate, outOfSceneDate );
		std::swap( m_outOfDate, outOfDate );

		if ( outOfDate != m_outOfDate )
		{
			if ( outOfDate )
			{
				if ( remove )
				{
					m_counts->removeOutdated();
				}
			}
			else
			{
				m_counts->addOutdated();
			}
		}
	}

	//*********************************************************************************************

	RendererTestRuns::RendererTestRuns( TestDatabase & database )
		: m_database{ database }
	{
	}

	DatabaseTest & RendererTestRuns::addTest( TestRun run )
	{
		m_runs.emplace_back( m_database, std::move( run ) );
		return m_runs.back();
	}

	DatabaseTest & RendererTestRuns::addTest( DatabaseTest test )
	{
		m_runs.emplace_back( std::move( test ) );
		return m_runs.back();
	}

	void RendererTestRuns::removeTest( DatabaseTest const & test )
	{
		auto it = std::find_if( m_runs.begin()
			, m_runs.end()
			, [&test]( DatabaseTest & lookup )
			{
				return lookup.getTestId() == test.getTestId();
			} );
		wxASSERT( it != m_runs.end() && "Test not found in CategoryTestRuns" );
		m_runs.erase( it );
	}

	DatabaseTest & RendererTestRuns::getTest( int32_t testId )
	{
		auto it = std::find_if( m_runs.begin()
			, m_runs.end()
			, [&testId]( DatabaseTest const & test )
			{
				return test.getTestId() == testId;
			} );
		wxASSERT( it != m_runs.end() && "Test not found in CategoryTestRuns" );
		return *it;
	}

	void RendererTestRuns::listTests( FilterFunc filter
		, std::vector< DatabaseTest * > & result )
	{
		for ( auto & run : m_runs )
		{
			if ( filter( run ) )
			{
				result.push_back( &run );
			}
		}
	}

	void RendererTestRuns::changeName( DatabaseTest const & test
		, wxString const & oldName
		, wxString const & newName )const
	{
		moveResultImage( m_database.getConfig(), test, oldName, newName );
	}

	void RendererTestRuns::changeCategory( DatabaseTest const & test
		, Category oldCategory
		, Category newCategory )const
	{
		moveResultImage( m_database.getConfig(), test, oldCategory, newCategory );
	}

	//*********************************************************************************************

	AllTestRuns::AllTestRuns( TestDatabase & database )
		: m_database{ database }
	{
	}

	RendererTestRuns & AllTestRuns::addRenderer( Renderer renderer )
	{
		auto it = m_runs.emplace( renderer, RendererTestRuns{ m_database } ).first;
		return it->second;
	}

	RendererTestRuns & AllTestRuns::getRenderer( Renderer renderer )
	{
		auto it = m_runs.find( renderer );
		wxASSERT( it != m_runs.end() && "Renderer not found in AllTestRuns" );
		return it->second;
	}

	void AllTestRuns::listTests( FilterFunc filter
		, std::vector< DatabaseTest * > & result )
	{
		for ( auto & run : m_runs )
		{
			run.second.listTests( filter, result );
		}
	}

	//*********************************************************************************************
}
