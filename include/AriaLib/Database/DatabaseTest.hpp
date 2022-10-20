/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DatabaseTest_HPP___
#define ___Aria_DatabaseTest_HPP___

#include "Prerequisites.hpp"

namespace aria
{
	class DatabaseTest
	{
		friend class TestDatabase;
		friend struct TestsCounts;

	public:
		AriaLib_API DatabaseTest( DatabaseTest const & ) = delete;
		AriaLib_API DatabaseTest & operator=( DatabaseTest const & ) = delete;
		AriaLib_API DatabaseTest( DatabaseTest && ) = default;
		AriaLib_API DatabaseTest & operator=( DatabaseTest && ) = default;
		AriaLib_API DatabaseTest( TestDatabase & database
			, TestRun test );

		AriaLib_API void updateIgnoreResult( bool ignore
			, db::DateTime engineDate
			, bool useAsReference );
		AriaLib_API void updateEngineDateNW( db::DateTime const & engineDate );
		AriaLib_API void updateEngineDate( db::DateTime const & engineDate );
		AriaLib_API void updateEngineDate();
		AriaLib_API void updateTestDate( db::DateTime const & engineDate );
		AriaLib_API void updateTestDate();
		AriaLib_API void updateStatusNW( TestStatus newStatus );
		AriaLib_API void updateStatus( TestStatus newStatus
			, bool useAsReference );
		AriaLib_API void createNewRun( TestStatus status
			, db::DateTime const & runDate
			, TestTimes const & times );
		AriaLib_API void createNewRun( wxFileName const & match
			, TestTimes const & times );
		AriaLib_API void changeCategory( Category dstCategory
			, TestsCounts & dstCounts );
		AriaLib_API std::string getPrefixedName( uint32_t index )const;
		AriaLib_API std::string getUnprefixedName()const;
		AriaLib_API bool hasNumPrefix()const;

		bool checkOutOfEngineDate()const
		{
			updateOutOfDate();
			return m_outOfEngineDate;
		}

		bool checkOutOfTestDate()const
		{
			updateOutOfDate();
			return m_outOfTestDate;
		}

		int getTestId()const
		{
			return m_test.test->id;
		}

		int getRunId()const
		{
			return m_test.id;
		}

		Renderer getRenderer()const
		{
			return m_test.renderer;
		}

		Category getCategory()const
		{
			return m_test.test->category;
		}

		bool getIgnoreResult()const
		{
			return m_test.test->ignoreResult;
		}

		TestStatus getStatus()const
		{
			return m_test.status;
		}

		std::string const & getName()const
		{
			return m_test.test->name;
		}

		db::DateTime const & getRunDate()const
		{
			return m_test.runDate;
		}

		db::DateTime const & getEngineDate()const
		{
			return m_test.engineDate;
		}

		db::DateTime const & getTestDate()const
		{
			return m_test.testDate;
		}

		TestsCounts & getCounts()const
		{
			return *m_counts;
		}

		TestRun const * operator->()const
		{
			return &m_test;
		}

		TestRun const & operator*()const
		{
			return m_test;
		}

	private:
		void update( int id );
		void update( int id
			, db::DateTime runDate
			, TestStatus status
			, db::DateTime engineDate
			, db::DateTime testDate
			, TestTimes times );
		void updateReference( TestStatus status );
		AriaLib_API void updateOutOfDate( bool remove = true )const;

	private:
		TestDatabase * m_database;
		TestsCounts * m_counts{};
		TestRun m_test;
		mutable bool m_outOfEngineDate;
		mutable bool m_outOfTestDate;
		mutable bool m_outOfDate;
	};

	class RendererTestRuns
	{
	private:
		using Cont = std::vector< DatabaseTest >;

	public:
		AriaLib_API RendererTestRuns( RendererTestRuns const & ) = delete;
		AriaLib_API RendererTestRuns & operator=( RendererTestRuns const & ) = delete;
		AriaLib_API RendererTestRuns( RendererTestRuns && ) = default;
		AriaLib_API RendererTestRuns & operator=( RendererTestRuns && ) = delete;
		AriaLib_API explicit RendererTestRuns( TestDatabase & database );

		AriaLib_API DatabaseTest & addTest( TestRun run );
		AriaLib_API DatabaseTest & addTest( DatabaseTest test );
		AriaLib_API void removeTest( DatabaseTest const & test );
		AriaLib_API DatabaseTest & getTest( int32_t testId );
		AriaLib_API void listTests( FilterFunc filter
			, DatabaseTestArray & result );
		AriaLib_API void changeCategory( DatabaseTest const & test
			, Category oldCategory
			, Category newCategory )const;

		size_t size()
		{
			return m_runs.size();
		}

		Cont::iterator begin()
		{
			return m_runs.begin();
		}

		Cont::iterator end()
		{
			return m_runs.end();
		}

		Cont::const_iterator begin()const
		{
			return m_runs.begin();
		}

		Cont::const_iterator end()const
		{
			return m_runs.end();
		}

	private:
		TestDatabase & m_database;
		Cont m_runs;
	};

	class AllTestRuns
	{
	private:
		using Cont = std::map< Renderer, RendererTestRuns, LessIdValue >;

	public:
		AriaLib_API AllTestRuns( AllTestRuns const & ) = delete;
		AriaLib_API AllTestRuns & operator=( AllTestRuns const & ) = delete;
		AriaLib_API AllTestRuns( AllTestRuns && ) = default;
		AriaLib_API AllTestRuns & operator=( AllTestRuns && ) = delete;
		AriaLib_API explicit AllTestRuns( TestDatabase & database );

		AriaLib_API RendererTestRuns & addRenderer( Renderer renderer );
		AriaLib_API RendererTestRuns & getRenderer( Renderer renderer );
		AriaLib_API void listTests( FilterFunc filter
			, DatabaseTestArray & result );

		Cont::iterator begin()
		{
			return m_runs.begin();
		}

		Cont::iterator end()
		{
			return m_runs.end();
		}

		Cont::const_iterator begin()const
		{
			return m_runs.begin();
		}

		Cont::const_iterator end()const
		{
			return m_runs.end();
		}

	private:
		TestDatabase & m_database;
		Cont m_runs;
	};
}

#endif
