/*
See LICENSE file in root folder
*/
#ifndef ___Aria_TestsCounts_HPP___
#define ___Aria_TestsCounts_HPP___

#include "CountedValue.hpp"
#include "Signal.hpp"

#include <array>

namespace aria
{
	struct CategoryTestsCounts
	{
	public:
		AriaLib_API CategoryTestsCounts( Plugin const & plugin
			, TestArray const & tests );
		AriaLib_API void addTest( DatabaseTest & test );
		AriaLib_API void removeTest( DatabaseTest & test );

		AriaLib_API void add( TestStatus status );
		AriaLib_API void remove( TestStatus status );
		AriaLib_API CountedUInt & getCount( TestsCountsType type );
		AriaLib_API CountedUInt const & getCount( TestsCountsType type )const;
		AriaLib_API uint32_t getValue( TestsCountsType type )const;
		AriaLib_API uint32_t getStatusValue( TestStatus status )const;
		AriaLib_API uint32_t getIgnoredValue()const;
		AriaLib_API uint32_t getOutdatedValue()const;
		AriaLib_API uint32_t getAllValue()const;
		AriaLib_API uint32_t getAllRunStatus()const;

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
		AriaLib_API explicit RendererTestsCounts( Plugin const & plugin );

		AriaLib_API CategoryTestsCounts & addCategory( Category category
			, TestArray const & tests );
		AriaLib_API CategoryTestsCounts & getCounts( Category category );
		AriaLib_API uint32_t getValue( TestsCountsType type )const;
		AriaLib_API uint32_t getAllValue()const;

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
		AriaLib_API explicit AllTestsCounts( Plugin const & plugin );

		AriaLib_API RendererTestsCounts & addRenderer( Renderer renderer );
		AriaLib_API RendererTestsCounts & getRenderer( Renderer renderer );

		AriaLib_API CategoryTestsCounts & addCategory( Renderer renderer
			, Category category
			, TestArray const & tests );
		AriaLib_API CategoryTestsCounts & getCategory( Renderer renderer
			, Category category );

		AriaLib_API uint32_t getValue( TestsCountsType type )const;
		AriaLib_API uint32_t getAllValue()const;

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
