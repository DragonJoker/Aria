#include "Database/DbValuedObject.hpp"

#include "Database/DbValue.hpp"

namespace aria::db
{
	namespace
	{
		static const std::string ERROR_DB_FIELD_CREATION_TYPE = "Type error while creating the object: ";
	}

	//*********************************************************************************************

	ValuedObject::ValuedObject( Connection & connection, ValuedObjectInfos infos )
		: m_infos{ std::move( infos ) }
		, m_connection{ connection }
	{
	}

	void ValuedObject::doGetValue( bool & value ) const
	{
		assert( getType() == FieldType::eBit );
		value = static_cast< ValueT< FieldType::eBit > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( int32_t & value ) const
	{
		assert( getType() == FieldType::eSint32 || getType() == FieldType::eUint32 );
		value = static_cast< ValueT< FieldType::eSint32 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( uint32_t & value ) const
	{
		assert( getType() == FieldType::eSint32 || getType() == FieldType::eUint32 );
		value = static_cast< ValueT< FieldType::eUint32 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( int64_t & value ) const
	{
		assert( getType() == FieldType::eSint64 || getType() == FieldType::eUint64 );
		value = static_cast< ValueT< FieldType::eSint64 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( uint64_t & value ) const
	{
		assert( getType() == FieldType::eSint64 || getType() == FieldType::eUint64 );
		value = static_cast< ValueT< FieldType::eUint64 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( float & value ) const
	{
		assert( getType() == FieldType::eFloat32 );
		value = static_cast< ValueT< FieldType::eFloat32 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( double & value ) const
	{
		assert( getType() == FieldType::eFloat64 );
		value = static_cast< ValueT< FieldType::eFloat64 > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( std::string & value ) const
	{
		assert( getType() == FieldType::eText || getType() == FieldType::eVarchar || getType() == FieldType::eChar );

		if ( m_value->getPtrValue() )
		{
			if ( getType() == FieldType::eText )
			{
				value = static_cast< ValueT< FieldType::eText > & >( *m_value ).getValue();
			}
			else if ( getType() == FieldType::eVarchar )
			{
				value = static_cast< ValueT< FieldType::eVarchar > & >( *m_value ).getValue();
			}
			else
			{
				value = static_cast< ValueT< FieldType::eChar > & >( *m_value ).getValue();
			}
		}
		else
		{
			value.clear();
		}
	}

	void ValuedObject::doGetValue( DateTime & value ) const
	{
		assert( getType() == FieldType::eDatetime );
		value = static_cast< ValueT< FieldType::eDatetime > & >( *m_value ).getValue();
	}

	void ValuedObject::doGetValue( ByteArray & value ) const
	{
		assert( getType() == FieldType::eBinary || getType() == FieldType::eVarbinary || getType() == FieldType::eBlob );
		value = static_cast< ValueT< FieldType::eBinary > & >( *m_value ).getValue();
	}

	void ValuedObject::doSetValue( const bool & value )
	{
		assert( getType() == FieldType::eBit );
		static_cast< ValueT< FieldType::eBit > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const int32_t & value )
	{
		assert( getType() == FieldType::eSint32 || getType() == FieldType::eUint32 );
		static_cast< ValueT< FieldType::eSint32 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const uint32_t & value )
	{
		assert( getType() == FieldType::eSint32 || getType() == FieldType::eUint32 );
		static_cast< ValueT< FieldType::eUint32 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const int64_t & value )
	{
		assert( getType() == FieldType::eSint64 || getType() == FieldType::eUint64 );
		static_cast< ValueT< FieldType::eSint64 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const uint64_t & value )
	{
		assert( getType() == FieldType::eSint64 || getType() == FieldType::eUint64 );
		static_cast< ValueT< FieldType::eUint64 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const float & value )
	{
		assert( getType() == FieldType::eFloat32 );
		static_cast< ValueT< FieldType::eFloat32 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const double & value )
	{
		assert( getType() == FieldType::eFloat64 );
		static_cast< ValueT< FieldType::eFloat64 > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const std::string & value )
	{
		assert( getType() == FieldType::eText || getType() == FieldType::eVarchar || getType() == FieldType::eChar );

		if ( getType() == FieldType::eChar )
		{
			static_cast< ValueT< FieldType::eChar > & >( *m_value ).setValueLimits( value.c_str(), getLimits() );
		}
		else
		{
			static_cast< ValueT< FieldType::eText > & >( *m_value ).setValueLimits( value.c_str(), std::min( getLimits(), uint32_t( value.size() ) ) );
		}
	}

	void ValuedObject::doSetValue( const DateTime & value )
	{
		assert( getType() == FieldType::eDatetime );
		static_cast< ValueT< FieldType::eDatetime > & >( *m_value ).setValue( value );
	}

	void ValuedObject::doSetValue( const ByteArray & value )
	{
		assert( getType() == FieldType::eBinary || getType() == FieldType::eVarbinary || getType() == FieldType::eBlob );
		static_cast< ValueT< FieldType::eBinary > & >( *m_value ).setValueLimits( value.data(), std::min( getLimits(), uint32_t( value.size() ) ) );
	}

	void ValuedObject::doCreateValue()
	{
		switch ( getType() )
		{
		case FieldType::eBit:
			m_value = std::make_unique< ValueT< FieldType::eBit > >( m_connection );
			break;

		case FieldType::eSint32:
			m_value = std::make_unique< ValueT< FieldType::eSint32 > >( m_connection );
			break;

		case FieldType::eSint64:
			m_value = std::make_unique< ValueT< FieldType::eSint64 > >( m_connection );
			break;

		case FieldType::eUint32:
			m_value = std::make_unique< ValueT< FieldType::eUint32 > >( m_connection );
			break;

		case FieldType::eUint64:
			m_value = std::make_unique< ValueT< FieldType::eUint64 > >( m_connection );
			break;

		case FieldType::eFloat32:
			m_value = std::make_unique< ValueT< FieldType::eFloat32 > >( m_connection );
			break;

		case FieldType::eFloat64:
			m_value = std::make_unique< ValueT< FieldType::eFloat64 > >( m_connection );
			break;

		case FieldType::eChar:
			m_value = std::make_unique< ValueT< FieldType::eChar > >( m_connection );
			break;

		case FieldType::eVarchar:
			m_value = std::make_unique< ValueT< FieldType::eVarchar > >( m_connection );
			break;

		case FieldType::eText:
			m_value = std::make_unique< ValueT< FieldType::eText > >( m_connection );
			break;

		case FieldType::eDatetime:
			m_value = std::make_unique< ValueT< FieldType::eDatetime > >( m_connection );
			break;

		case FieldType::eBinary:
			m_value = std::make_unique< ValueT< FieldType::eBinary > >( m_connection );
			break;

		case FieldType::eVarbinary:
			m_value = std::make_unique< ValueT< FieldType::eVarbinary > >( m_connection );
			break;

		case FieldType::eBlob:
			m_value = std::make_unique< ValueT< FieldType::eBlob > >( m_connection );
			break;

		default:
			std::string errMsg = ERROR_DB_FIELD_CREATION_TYPE + getName();
			throw std::runtime_error{ errMsg };
		}
	}

	//*********************************************************************************************
}
