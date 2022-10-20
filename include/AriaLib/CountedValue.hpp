/*
See LICENSE file in root folder
*/
#ifndef ___Aria_CountedValue_HPP___
#define ___Aria_CountedValue_HPP___

#include "Prerequisites.hpp"

namespace aria
{
	template< typename ValueT >
	struct CountedValueT
	{
		CountedValueT()
			: m_value{}
		{
		}

		explicit CountedValueT( ValueT value )
			: m_value{ std::move( value ) }
		{
		}

		CountedValueT & operator++()
		{
			++m_value;
			return *this;
		}

		CountedValueT & operator--()
		{
			assert( m_value > 0 );
			--m_value;
			return *this;
		}

		CountedValueT & operator+=( ValueT rhs )
		{
			m_value += rhs;
			return *this;
		}

		CountedValueT & operator-=( ValueT rhs )
		{
			assert( m_value > 0 );
			m_value -= std::min( m_value, rhs );
			return *this;
		}

		operator ValueT ()const
		{
			return m_value;
		}

	private:
		ValueT m_value;
	};
}

#endif
