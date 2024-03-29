/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbField_HPP___
#define ___Aria_DbField_HPP___

#include "DbValuedObject.hpp"

namespace aria::db
{
	class Field
		: public ValuedObject
	{
	public:
		Field( Connection & connection, ValuedObjectInfos infos );

		bool isNull() const
		{
			return getObjectValue().isNull();
		}

		template< typename T >
		T getValue() const
		{
			T result;
			getValue( result );
			return result;
		}

		template< typename T >
		void getValue( T & value ) const
		{
			doGetValue( value );
		}
	};
}

#endif
