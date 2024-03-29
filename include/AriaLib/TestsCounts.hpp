/*
See LICENSE file in root folder
*/
#ifndef ___Aria_TestsCounts_HPP___
#define ___Aria_TestsCounts_HPP___

#include "CountedValue.hpp"
#include "Signal.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <wx/dataview.h>

#include <array>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria
{
	struct TestsCounts
	{
	public:
		AriaLib_API TestsCounts( Plugin const & plugin );
		AriaLib_API void addTest( DatabaseTest & test );
		AriaLib_API void removeTest( DatabaseTest & test );

		AriaLib_API void add( TestStatus status );
		AriaLib_API void remove( TestStatus status );

		AriaLib_API void add( TestsCounts const & counts );
		AriaLib_API void remove( TestsCounts const & counts );
		AriaLib_API void clear();

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
			return getAllValue()
				? ( 100.0f * float( getValue( type ) ) ) / float( getAllValue() )
				: 0.0f;
		}

		float getIgnoredPercent()const
		{
			return getAllValue()
				? ( 100.0f * float( getIgnoredValue() ) ) / float( getAllValue() )
				: 0.0f;
		}

		float getOutdatedPercent()const
		{
			return getAllValue()
				? ( 100.0f * float( getOutdatedValue() ) ) / float( getAllValue() )
				: 0.0f;
		}

		float getAllPercent()const
		{
			return getAllValue()
				? ( 100.0f * float( getAllValue() ) ) / float( getAllValue() )
				: 0.0f;
		}

		float getStatusPercent( TestStatus status )const
		{
			return getAllValue()
				? ( 100.0f * float( getStatusValue( status ) ) ) / float( getAllValue() )
				: 0.0f;
		}

		float getNotRunPercent()const
		{
			return getAllValue()
				? ( 100.0f * float( getNotRunValue() ) ) / float( getAllValue() )
				: 0.0f;
		}

	private:
		void add( TestsCountsType type
			, uint32_t count );
		void remove( TestsCountsType type
			, uint32_t count );

	private:
		std::array< CountedUInt, TestsCountsType::eCount > m_values{};
		Plugin const & m_plugin;
	};

	struct RendererTestsCounts
	{
		AriaLib_API explicit RendererTestsCounts( Plugin const & plugin );

		AriaLib_API TestsCounts & addCategory( Category category
			, TestArray const & tests );
		AriaLib_API TestsCounts & getCounts( Category category );
		AriaLib_API uint32_t getValue( TestsCountsType type )const;
		AriaLib_API uint32_t getAllValue()const;

		float getPercent( TestsCountsType type )const
		{
			return getAllValue()
				? ( 100.0f * float( getValue( type ) ) ) / float( getAllValue() )
				: 0.0f;
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

		AriaLib_API TestsCounts & addCategory( Renderer renderer
			, Category category
			, TestArray const & tests );
		AriaLib_API TestsCounts & getCategory( Renderer renderer
			, Category category );

		AriaLib_API uint32_t getValue( TestsCountsType type )const;
		AriaLib_API uint32_t getAllValue()const;

		float getPercent( TestsCountsType type )const
		{
			return getAllValue()
				? ( 100.0f * float( getValue( type ) ) ) / float( getAllValue() )
				: 0.0f;
		}

	private:
		Plugin const & plugin;
		TestsCountsRendererMap renderers;
	};
}

#endif
