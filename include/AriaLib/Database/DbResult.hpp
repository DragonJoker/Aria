/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbResult_HPP___
#define ___Aria_DbResult_HPP___

#include "DbRow.hpp"

namespace aria::db
{
	class Result
	{
	public:
		explicit Result( const ValuedObjectInfosArray & fieldInfos );

		uint32_t getFieldCount() const;
		ValuedObjectInfos const & getFieldInfos( uint32_t index ) const;
		void addRow( Row row );

		auto begin()const
		{
			return m_rows.begin();
		}

		auto end()const
		{
			return m_rows.end();
		}

		auto cbegin()const
		{
			return m_rows.cbegin();
		}

		auto cend()const
		{
			return m_rows.cend();
		}

		auto size()const
		{
			return m_rows.size();
		}

		auto empty()const
		{
			return m_rows.empty();
		}

	protected:
		RowArray m_rows;
		ValuedObjectInfosArray m_fieldInfos;
	};
}

#endif
