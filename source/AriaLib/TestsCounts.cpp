#include "TestsCounts.hpp"
#include "Plugin.hpp"

#include "Database/DatabaseTest.hpp"

namespace aria
{
	//*********************************************************************************************

	TestsCounts::TestsCounts( Plugin const & plugin )
		: m_plugin{ plugin }
	{
	}

	void TestsCounts::addTest( DatabaseTest & test )
	{
		test.m_counts = this;
		add( test.getStatus() );

		if ( test.getIgnoreResult() )
		{
			addIgnored();
		}

		if ( m_plugin.isOutOfDate( *test ) )
		{
			addOutdated();
		}
	}

	void TestsCounts::removeTest( DatabaseTest & test )
	{
		if ( m_plugin.isOutOfDate( *test ) )
		{
			removeOutdated();
		}

		if ( test.getIgnoreResult() )
		{
			removeIgnored();
		}

		remove( test.getStatus() );
		test.m_counts = nullptr;
	}

	void TestsCounts::add( TestStatus status )
	{
		add( getType( status ), 1u );
	}

	void TestsCounts::remove( TestStatus status )
	{
		remove( getType( status ), 1u );
	}

	void TestsCounts::add( TestsCounts const & counts )
	{
		for ( uint32_t i = 0u; i < uint32_t( TestsCountsType::eCount ); ++i )
		{
			m_values[i] += counts.m_values[i];
		}
	}

	void TestsCounts::remove( TestsCounts const & counts )
	{
		for ( uint32_t i = 0u; i < uint32_t( TestsCountsType::eCount ); ++i )
		{
			m_values[i] -= counts.m_values[i];
		}
	}

	void TestsCounts::clear()
	{
		for ( uint32_t i = 0u; i < uint32_t( TestsCountsType::eCount ); ++i )
		{
			m_values[i] = CountedUInt{};
		}
	}

	CountedUInt & TestsCounts::getCount( TestsCountsType type )
	{
		return m_values[type];
	}

	CountedUInt const & TestsCounts::getCount( TestsCountsType type )const
	{
		return m_values[type];
	}

	uint32_t TestsCounts::getValue( TestsCountsType type )const
	{
		if ( type == TestsCountsType::eNotRun )
		{
			auto iall = getAllValue();
			auto allRun = getAllRunStatus();
			assert( getAllValue() >= allRun );
			return iall - allRun;
		}

		return uint32_t( getCount( type ) );
	}

	uint32_t TestsCounts::getStatusValue( TestStatus status )const
	{
		return getValue( getType( status ) );
	}

	uint32_t TestsCounts::getIgnoredValue()const
	{
		return getValue( TestsCountsType::eIgnored );
	}

	uint32_t TestsCounts::getOutdatedValue()const
	{
		return getValue( TestsCountsType::eOutdated );
	}

	uint32_t TestsCounts::getAllValue()const
	{
		return getValue( TestsCountsType::eAll );
	}

	uint32_t TestsCounts::getAllRunStatus()const
	{
		uint32_t result{};

		for ( auto i = 1u; i < TestsCountsType::eCountedInAllEnd; ++i )
		{
			result += getValue( TestsCountsType( i ) );
		}

		return result;
	}

	void TestsCounts::add( TestsCountsType type
		, uint32_t count )
	{
		getCount( TestsCountsType::eAll ) += count;
		getCount( type ) += count;
	}

	void TestsCounts::remove( TestsCountsType type
		, uint32_t count )
	{
		getCount( type ) -= count;
		getCount( TestsCountsType::eAll ) -= count;
	}

	//*********************************************************************************************

	RendererTestsCounts::RendererTestsCounts( Plugin const & plugin )
		: plugin{ plugin }
	{
	}

	TestsCounts & RendererTestsCounts::addCategory( Category category
		, TestArray const & tests )
	{
		auto countsIt = categories.emplace( category, TestsCounts{ plugin } ).first;
		return countsIt->second;
	}

	TestsCounts & RendererTestsCounts::getCounts( Category category )
	{
		auto countsIt = categories.find( category );
		return countsIt->second;
	}

	uint32_t RendererTestsCounts::getValue( TestsCountsType type )const
	{
		uint32_t result{};

		for ( auto & category : categories )
		{
			result += category.second.getValue( type );
		}

		return result;
	}

	uint32_t RendererTestsCounts::getAllValue()const
	{
		uint32_t result{};

		for ( auto & category : categories )
		{
			result += category.second.getAllValue();
		}

		return result;
	}

	//*********************************************************************************************

	AllTestsCounts::AllTestsCounts( Plugin const & plugin )
		: plugin{ plugin }
	{
	}

	RendererTestsCounts & AllTestsCounts::addRenderer( Renderer renderer )
	{
		auto countsIt = renderers.emplace( renderer, RendererTestsCounts{ plugin } ).first;
		return countsIt->second;
	}

	RendererTestsCounts & AllTestsCounts::getRenderer( Renderer renderer )
	{
		auto countsIt = renderers.find( renderer );
		return countsIt->second;
	}

	TestsCounts & AllTestsCounts::addCategory( Renderer renderer
		, Category category
		, TestArray const & tests )
	{
		return getRenderer( renderer ).addCategory( category, tests );
	}

	TestsCounts & AllTestsCounts::getCategory( Renderer renderer
		, Category category )
	{
		return getRenderer( renderer ).getCounts( category );
	}

	uint32_t AllTestsCounts::getValue( TestsCountsType type )const
	{
		uint32_t result{};

		for ( auto & renderer : renderers )
		{
			result += renderer.second.getValue( type );
		}

		return result;
	}

	uint32_t AllTestsCounts::getAllValue()const
	{
		uint32_t result{};

		for ( auto & renderer : renderers )
		{
			result += renderer.second.getAllValue();
		}

		return result;
	}

	//*********************************************************************************************
}
