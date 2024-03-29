/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbRow_HPP___
#define ___Aria_DbRow_HPP___

#include "DbField.hpp"

namespace aria::db
{
	class Row
	{
	public:
		void addField( FieldPtr field );
		Field const & getField( const std::string & name ) const;
		Field const & getField( uint32_t index ) const;
		Field & getField( const std::string & name );
		Field & getField( uint32_t index );
		bool isNull( uint32_t index ) const;
		bool isNull( const std::string & name ) const;

		template< typename T >
		T get( uint32_t index )
		{
			T result;
			get( index, result );
			return result;
		}

		template< typename T >
		T get( const std::string & name )
		{
			T result;
			get( name, result );
			return result;
		}

		template< typename T >
		inline void get( uint32_t index, T & value )
		{
			getField( index ).getValue( value );
		}

		template< typename T >
		inline void get( const std::string & name, T & value )
		{
			getField( name ).getValue( value );
		}

	protected:
		FieldArray m_arrayFields;
	};
}

#endif
