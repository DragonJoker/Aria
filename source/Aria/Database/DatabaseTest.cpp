#include "Database/DatabaseTest.hpp"

#include "Plugin.hpp"
#include "TestsCounts.hpp"
#include "Database/TestDatabase.hpp"

#include <iomanip>
#include <sstream>

namespace aria
{
	//*********************************************************************************************

	DatabaseTest::DatabaseTest( TestDatabase & database
		, TestRun test )
		: m_database{ &database }
		, m_test{ std::move( test ) }
		, m_outOfEngineDate{ database.m_plugin->isOutOfEngineDate( m_test ) }
		, m_outOfSceneDate{ database.m_plugin->isOutOfSceneDate( m_test ) }
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
			m_database->updateRunEngineDate( m_test );
		}
	}

	void DatabaseTest::updateEngineDate()
	{
		m_database->m_plugin->updateEngineRefDate();
		updateEngineDate( m_database->m_plugin->getEngineRefDate() );
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
		auto sceneDate = m_database->getPlugin().getSceneDate( m_test );
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
		auto & plugin = *m_database->m_plugin;
		auto & config = plugin.config;
		TestStatus oldStatus = m_test.status;
		plugin.updateEngineRefDate();
		m_test.engineDate = plugin.getEngineRefDate();
		updateStatusNW( newStatus );
		m_database->updateRunStatus( m_test );
		m_database->moveResultFile( *this, oldStatus, newStatus, config.work );

		if ( useAsReference )
		{
			updateReference( newStatus );
		}
	}

	void DatabaseTest::createNewRun( TestStatus status
		, db::DateTime const & runDate
		, TestTimes const & times )
	{
		auto & plugin = *m_database->m_plugin;
		auto rawStatus = status;
		auto newStatus = rawStatus;

		if ( m_test.test->ignoreResult )
		{
			newStatus = TestStatus::eNegligible;
		}

		m_test.runDate = runDate;
		m_test.times = times;
		assert( m_test.runDate.IsValid() );
		plugin.updateEngineRefDate();
		m_test.engineDate = plugin.getEngineRefDate();
		assert( m_test.engineDate.IsValid() );
		m_test.sceneDate = m_database->getPlugin().getSceneDate( m_test );
		assert( m_test.sceneDate.IsValid() );
		updateStatusNW( newStatus );
		m_database->insertRun( m_test );

		if ( m_test.test->ignoreResult )
		{
			updateReference( rawStatus );
		}
	}

	void DatabaseTest::createNewRun( wxFileName const & match
		, TestTimes const & times )
	{
		auto path = match.GetPath();
		createNewRun( aria::getStatus( makeStdString( wxFileName{ path }.GetName() ) )
			, getFileDate( match )
			, times );
	}

	void DatabaseTest::changeCategory( Category dstCategory
		, CategoryTestsCounts & dstCounts )
	{
		wxASSERT( m_counts != nullptr && "Test counts not set" );
		m_counts->removeTest( *this );
		dstCounts.addTest( *this );
	}

	std::string DatabaseTest::getPrefixedName( uint32_t index )const
	{
		std::stringstream stream;
		stream << std::setw( 4u ) << std::setfill( '0' ) << std::right << index;
		return stream.str() + "-" + getUnprefixedName();
	}

	std::string DatabaseTest::getUnprefixedName()const
	{
		if ( !hasNumPrefix() )
		{
			return getName();
		}

		auto result = getName();
		auto it = result.find( "-" );
		return result.substr( it + 1 );
	}

	bool DatabaseTest::hasNumPrefix()const
	{
		auto pos = getName().find( "-" );

		if ( pos == std::string::npos )
		{
			return false;
		}

		auto end = std::next( getName().begin(), ptrdiff_t( pos ) );
		auto it = std::find_if( getName().begin()
			, end
			, []( char const & lookup )
			{
				return !std::isdigit( lookup );
			} );

		return it == end;
	}

	void DatabaseTest::update( int id )
	{
		m_test.id = id;
	}

	void DatabaseTest::update( int id
		, db::DateTime runDate
		, TestStatus status
		, db::DateTime engineDate
		, db::DateTime sceneDate
		, TestTimes times )
	{
		m_test.id = id;
		m_test.status = status;
		m_test.runDate = std::move( runDate );
		m_test.engineDate = std::move( engineDate );
		m_test.sceneDate = std::move( sceneDate );
		m_test.times = std::move( times );
		m_outOfEngineDate = m_database->getPlugin().isOutOfEngineDate( m_test );
		m_outOfSceneDate = m_database->getPlugin().isOutOfSceneDate( m_test );
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
		m_database->updateReferenceFile( *this
			, status );
	}

	void DatabaseTest::updateOutOfDate( bool remove )const
	{
		bool outOfCastorDate{ m_database->getPlugin().isOutOfEngineDate( m_test ) };
		bool outOfSceneDate{ m_database->getPlugin().isOutOfSceneDate( m_test ) };
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

	void RendererTestRuns::changeCategory( DatabaseTest const & test
		, Category oldCategory
		, Category newCategory )const
	{
		m_database.moveResultImage( test, oldCategory, newCategory );
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
