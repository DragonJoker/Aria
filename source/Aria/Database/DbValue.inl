#include "DbValuePolicy.hpp"

#include <sstream>

namespace aria::db
{
	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eChar, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		void updateValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		void setValueLimits( const char * tValue, uint32_t limits )
		{
			value_type value;

			if ( limits != 0 )
			{
				size_t length = strlen( tValue );

				if ( length < limits )
				{
					std::stringstream stream;
					stream.width( limits );
					stream.fill( ' ' );
					stream << std::left << tValue;
					value = stream.str();
				}
				else
				{
					value.assign( tValue, tValue + limits );
				}
			}

			updateValue( value );
		}

		void setValue( ValueBase const & value ) override;

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eVarchar, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		void updateValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		void setValueLimits( const char * tValue, uint32_t limits )
		{
			value_type value;

			if ( limits != 0 )
			{
				value.assign( tValue, tValue + std::min< uint32_t >( limits, uint32_t( strlen( tValue ) ) ) );
			}

			updateValue( value );
		}

		void setValue( ValueBase const & value ) override;

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eText, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		void updateValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		void setValueLimits( const char * tValue, uint32_t limits )
		{
			value_type value;

			if ( limits != 0 )
			{
				value.assign( tValue, tValue + std::min< uint32_t >( limits, uint32_t( strlen( tValue ) ) ) );
			}

			updateValue( value );
		}

		void setValue( ValueBase const & value ) override;

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eBinary, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		void updateValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		void setValueLimits( const uint8_t * tValue, uint32_t size )
		{
			value_type value;

			if ( tValue && size )
			{
				value.insert( value.end(), tValue, tValue + size );
			}

			updateValue( value );
		}

		void setValue( ValueBase const & value ) override;

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type( 0 );
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eVarbinary, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		void updateValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		void setValueLimits( const uint8_t * tValue, uint32_t size )
		{
			value_type value;

			if ( tValue && size )
			{
				value.insert( value.end(), tValue, tValue + size );
			}

			updateValue( value );
		}

		void setValue( ValueBase const & value ) override;

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{};
	};

	//*********************************************************************************************

	template< typename PolicyT >
	class ValueT< FieldType::eBlob, PolicyT >
		: public ValueBase
		, private PolicyT
	{
	public:
		typedef typename PolicyT::value_type value_type;

	public:
		ValueT( Connection & connection )
			: ValueBase( connection )
		{
		}

		void updateValue( const value_type & tValue )
		{
			setNull( !PolicyT::set( tValue, m_tValue, m_valueSize, m_connection ) );
		}

		void setValueLimits( const uint8_t * tValue, uint32_t size )
		{
			value_type value;

			if ( tValue && size )
			{
				value.insert( value.end(), tValue, tValue + size );
			}

			updateValue( value );
		}

		void setValue( ValueBase const & value ) override;

		std::string getQueryValue() const override
		{
			return PolicyT::toQueryValue( m_tValue, !isNull(), m_connection );
		}

		void * getPtrValue() override
		{
			return PolicyT::ptr( m_tValue );
		}

		const void * getPtrValue() const override
		{
			return PolicyT::ptr( m_tValue );
		}

		const value_type & getValue()const
		{
			return m_tValue;
		}

	private:
		void doSetNull() override
		{
			m_tValue = value_type();
			m_valueSize = 0;
		}

	private:
		value_type m_tValue{ 0 };
	};

	template< typename PolicyT >
	void ValueT< FieldType::eChar, PolicyT >::setValue( ValueBase const & value )
	{
		auto & val = static_cast< ValueT< FieldType::eChar, PolicyT > const & >( value );
		setValueLimits( val.m_tValue.c_str(), uint32_t( val.getPtrSize() ) );
	}

	template< typename PolicyT >
	void ValueT< FieldType::eVarchar, PolicyT >::setValue( ValueBase const & value )
	{
		updateValue( static_cast< ValueT< FieldType::eVarchar, PolicyT > const & >( value ).m_tValue );
	}

	template< typename PolicyT >
	void ValueT< FieldType::eText, PolicyT >::setValue( ValueBase const & value )
	{
		updateValue( static_cast< ValueT< FieldType::eText, PolicyT > const & >( value ).m_tValue );
	}

	template< typename PolicyT >
	void ValueT< FieldType::eBinary, PolicyT >::setValue( ValueBase const & value )
	{
		updateValue( static_cast< ValueT< FieldType::eBinary, PolicyT > const & >( value ).m_tValue );
	}

	template< typename PolicyT >
	void ValueT< FieldType::eVarbinary, PolicyT >::setValue( ValueBase const & value )
	{
		updateValue( static_cast< ValueT < FieldType::eVarbinary, PolicyT > const & > ( value ).m_tValue );
	}

	template< typename PolicyT >
	void ValueT< FieldType::eBlob, PolicyT >::setValue( ValueBase const & value )
	{
		updateValue( static_cast< ValueT< FieldType::eBlob, PolicyT > const & >( value ).m_tValue );
	}

	//*********************************************************************************************
}
