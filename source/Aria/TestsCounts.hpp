/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestsCounts_HPP___
#define ___CTP_TestsCounts_HPP___

#include "CountedValue.hpp"
#include "Signal.hpp"

#include <array>

namespace aria
{
	struct CategoryTestsCounts
	{
	public:
		CategoryTestsCounts( Plugin const & plugin
			, TestArray const & tests );
		void addTest( DatabaseTest & test );
		void removeTest( DatabaseTest & test );

		void add( TestStatus status );
		void remove( TestStatus status );
		CountedUInt & getCount( TestsCountsType type );
		CountedUInt const & getCount( TestsCountsType type )const;
		uint32_t getValue( TestsCountsType type )const;
		uint32_t getStatusValue( TestStatus status )const;
		uint32_t getIgnoredValue()const;
		uint32_t getOutdatedValue()const;
		uint32_t getAllValue()const;
		uint32_t getAllRunStatus()const;

		void addIgnored()
		{
			++getCount( TestsCountsType::eIgnored );
		}

		void removeIgnored()
		{
			--getCount( TestsCountsType::eIgnored );
		}

		void addOutdated()
		{
			++getCount( TestsCountsType::eOutdated );
		}

		void removeOutdated()
		{
			--getCount( TestsCountsType::eOutdated );
		}

		uint32_t getNotRunValue()const
		{
			return getValue( TestsCountsType::eNotRun );
		}

		float getPercent( TestsCountsType type )const
		{
			return ( 100.0f * float( getValue( type ) ) ) / float( getAllValue() );
		}

		float getIgnoredPercent()const
		{
			return ( 100.0f * float( getIgnoredValue() ) ) / float( getAllValue() );
		}

		float getOutdatedPercent()const
		{
			return ( 100.0f * float( getOutdatedValue() ) ) / float( getAllValue() );
		}

		float getAllPercent()const
		{
			return ( 100.0f * float( getAllValue() ) ) / float( getAllValue() );
		}

		float getStatusPercent( TestStatus status )const
		{
			return ( 100.0f * float( getStatusValue( status ) ) ) / float( getAllValue() );
		}

		float getNotRunPercent()const
		{
			return ( 100.0f * float( getNotRunValue() ) ) / float( getAllValue() );
		}

	private:
		std::array< CountedUInt, TestsCountsType::eCount > m_values{};
		std::array< CountedUIntConnection, TestsCountsType::eCount > m_connections{};
		Plugin const & m_plugin;
	};

	struct RendererTestsCounts
	{
		explicit RendererTestsCounts( Plugin const & plugin );

		CategoryTestsCounts & addCategory( Category category
			, TestArray const & tests );
		CategoryTestsCounts & getCounts( Category category );
		uint32_t getValue( TestsCountsType type )const;
		uint32_t getAllValue()const;

		float getPercent( TestsCountsType type )const
		{
			return ( 100.0f * float( getValue( type ) ) ) / float( getAllValue() );
		}

		TestsCountsCategoryMap & getCategories()
		{
			return categories;
		}

	private:
		Plugin const & plugin;
		TestsCountsCategoryMap categories;
	};

	struct AllTestsCounts
	{
		explicit AllTestsCounts( Plugin const & plugin );

		RendererTestsCounts & addRenderer( Renderer renderer );
		RendererTestsCounts & getRenderer( Renderer renderer );

		CategoryTestsCounts & addCategory( Renderer renderer
			, Category category
			, TestArray const & tests );
		CategoryTestsCounts & getCategory( Renderer renderer
			, Category category );

		uint32_t getValue( TestsCountsType type )const;
		uint32_t getAllValue()const;

		float getPercent( TestsCountsType type )const
		{
			return ( 100.0f * float( getValue( type ) ) ) / float( getAllValue() );
		}

	private:
		Plugin const & plugin;
		TestsCountsRendererMap renderers;
	};
}

#endif
