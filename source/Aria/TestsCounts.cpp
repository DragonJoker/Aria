#include "TestsCounts.hpp"
#include "Plugin.hpp"

#include "Database/DatabaseTest.hpp"

namespace aria
{
	//*********************************************************************************************

	CategoryTestsCounts::CategoryTestsCounts( Plugin const & plugin
		, TestArray const & tests )
		: m_plugin{ plugin }
	{
	}

	void CategoryTestsCounts::addTest( DatabaseTest & test )
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

	void CategoryTestsCounts::removeTest( DatabaseTest & test )
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

	void CategoryTestsCounts::add( TestStatus status )
	{
		++getCount( TestsCountsType::eAll );
		++getCount( getType( status ) );
	}

	void CategoryTestsCounts::remove( TestStatus status )
	{
		--getCount( getType( status ) );
		--getCount( TestsCountsType::eAll );
	}

	CountedUInt & CategoryTestsCounts::getCount( TestsCountsType type )
	{
		return m_values[type];
	}

	CountedUInt const & CategoryTestsCounts::getCount( TestsCountsType type )const
	{
		return m_values[type];
	}

	uint32_t CategoryTestsCounts::getValue( TestsCountsType type )const
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

	uint32_t CategoryTestsCounts::getStatusValue( TestStatus status )const
	{
		return getValue( getType( status ) );
	}

	uint32_t CategoryTestsCounts::getIgnoredValue()const
	{
		return getValue( TestsCountsType::eIgnored );
	}

	uint32_t CategoryTestsCounts::getOutdatedValue()const
	{
		return getValue( TestsCountsType::eOutdated );
	}

	uint32_t CategoryTestsCounts::getAllValue()const
	{
		return getValue( TestsCountsType::eAll );
	}

	uint32_t CategoryTestsCounts::getAllRunStatus()const
	{
		uint32_t result{};

		for ( auto i = 1u; i < TestsCountsType::eCountedInAllEnd; ++i )
		{
			result += getValue( TestsCountsType( i ) );
		}

		return result;
	}

	//*********************************************************************************************

	RendererTestsCounts::RendererTestsCounts( Plugin const & plugin )
		: plugin{ plugin }
	{
	}

	CategoryTestsCounts & RendererTestsCounts::addCategory( Category category
		, TestArray const & tests )
	{
		auto countsIt = categories.emplace( category, CategoryTestsCounts{ plugin, tests } ).first;
		return countsIt->second;
	}

	CategoryTestsCounts & RendererTestsCounts::getCounts( Category category )
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

	CategoryTestsCounts & AllTestsCounts::addCategory( Renderer renderer
		, Category category
		, TestArray const & tests )
	{
		return getRenderer( renderer ).addCategory( category, tests );
	}

	CategoryTestsCounts & AllTestsCounts::getCategory( Renderer renderer
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
