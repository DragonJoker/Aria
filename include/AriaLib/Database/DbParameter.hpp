/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbParameter_HPP___
#define ___Aria_DbParameter_HPP___

#include "DbValuedObject.hpp"
#include "DbValue.hpp"

namespace aria::db
{
	class Parameter
		: public ValuedObject
	{
	public:
		struct ValueUpdater
		{
			virtual ~ValueUpdater() = default;
			virtual void update( const Parameter & parameter ) = 0;
		};

	public:
		Parameter( Connection & connection
			, ValuedObjectInfos infos
			, unsigned short index
			, ParameterType parameterType
			, std::unique_ptr< ValueUpdater > updater );

		virtual void setNull();
		void setValue( const ValuedObject & object );
		void setValue( FieldType type, ValueBase const & value );

		const unsigned short & getIndex() const
		{
			return m_index;
		}

		const ParameterType & getParamType() const
		{
			return m_parameterType;
		}

		template< typename T >
		void setValue( const T & value )
		{
			doSetValue( value );
			m_updater->update( *this );
		}

		template< typename T >
		inline T getValue() const
		{
			T val;
			getValue( val );
			return val;
		}

		template< typename T >
		inline void getValue( T & value ) const
		{
			doGetValue( value );
		}

	private:
		std::string m_name;
		ParameterType m_parameterType;
		unsigned short m_index;
		std::unique_ptr< ValueUpdater > m_updater;
	};
}

#endif
