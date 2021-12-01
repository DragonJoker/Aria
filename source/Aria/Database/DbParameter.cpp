#include "Database/DbParameter.hpp"

namespace aria::db
{
	namespace
	{
		bool areTypesCompatibleSet( FieldType typeFrom, FieldType typeTo )
		{
			bool result = typeFrom == typeTo;

			if ( !result )
			{
				switch ( typeFrom )
				{
				case FieldType::eNull:
					result = false;
					break;

				case FieldType::eBit:
					result = ( typeTo == FieldType::eSint32 ) || ( typeTo == FieldType::eSint64 )
						|| ( typeTo == FieldType::eUint32 ) || ( typeTo == FieldType::eUint64 )
						|| ( typeTo == FieldType::eVarchar ) || ( typeTo == FieldType::eText );
					break;

				case FieldType::eSint32:
					result = ( typeTo == FieldType::eBit ) || ( typeTo == FieldType::eSint64 )
						|| ( typeTo == FieldType::eUint32 ) || ( typeTo == FieldType::eUint64 )
						|| ( typeTo == FieldType::eFloat32 ) || ( typeTo == FieldType::eFloat64 );
					break;

				case FieldType::eSint64:
					result = ( typeTo == FieldType::eBit ) || ( typeTo == FieldType::eUint64 );
					break;

				case FieldType::eUint32:
					result = ( typeTo == FieldType::eSint32 ) || ( typeTo == FieldType::eSint64 )
						|| ( typeTo == FieldType::eBit ) || ( typeTo == FieldType::eUint64 )
						|| ( typeTo == FieldType::eFloat32 ) || ( typeTo == FieldType::eFloat64 );
					break;

				case FieldType::eUint64:
					result = ( typeTo == FieldType::eSint64 ) || ( typeTo == FieldType::eBit );
					break;

				case FieldType::eFloat32:
					result = ( typeTo == FieldType::eSint32 ) || ( typeTo == FieldType::eSint64 )
						|| ( typeTo == FieldType::eUint32 ) || ( typeTo == FieldType::eUint64 )
						|| ( typeTo == FieldType::eFloat32 ) || ( typeTo == FieldType::eFloat64 );
					break;

				case FieldType::eFloat64:
					result = ( typeTo == FieldType::eSint32 ) || ( typeTo == FieldType::eSint64 )
						|| ( typeTo == FieldType::eUint32 ) || ( typeTo == FieldType::eUint64 )
						|| ( typeTo == FieldType::eFloat32 ) || ( typeTo == FieldType::eFloat64 );
					break;

				case FieldType::eChar:
					result = ( typeTo == FieldType::eVarchar ) || ( typeTo == FieldType::eText );
					break;

				case FieldType::eVarchar:
					result = ( typeTo == FieldType::eChar ) || ( typeTo == FieldType::eText );
					break;

				case FieldType::eText:
					result = ( typeTo == FieldType::eChar ) || ( typeTo == FieldType::eVarchar );
					break;

				case FieldType::eDatetime:
					result = ( typeTo == FieldType::eDatetime );
					break;

				case FieldType::eBinary:
					result = ( typeTo == FieldType::eBinary ) || ( typeTo == FieldType::eVarbinary ) || ( typeTo == FieldType::eBlob );
					break;

				case FieldType::eVarbinary:
					result = ( typeTo == FieldType::eBinary ) || ( typeTo == FieldType::eVarbinary ) || ( typeTo == FieldType::eBlob );
					break;

				case FieldType::eBlob:
					result = ( typeTo == FieldType::eBinary ) || ( typeTo == FieldType::eVarbinary ) || ( typeTo == FieldType::eBlob );
					break;

				default:
					result = false;
					break;
				}
			}

			return result;
		}
	}

	static const std::string ERROR_DB_INCOMPATIBLE_TYPES = "Incompatible types between values, parameter: ";
	static const std::string ERROR_DB_PARAMETER_SETVALUE_TYPE = "Type error while setting value for the parameter: ";

	Parameter::Parameter( Connection & connection
		, ValuedObjectInfos infos
		, unsigned short index
		, ParameterType parameterType
		, std::unique_ptr< ValueUpdater > updater )
		: ValuedObject{ connection, std::move( infos ) }
		, m_parameterType{ parameterType }
		, m_index{ index }
		, m_updater{ std::move( updater ) }
	{
		doCreateValue();
	}

	Parameter::~Parameter()
	{
	}

	void Parameter::setNull()
	{
		getObjectValue().setNull();
		m_updater->update( *this );
	}

	void Parameter::setValue( const ValuedObject & object )
	{
		if ( !object.getObjectValue().isNull() && !areTypesCompatibleSet( object.getType(), getType() ) )
		{
			std::string errMsg = ERROR_DB_INCOMPATIBLE_TYPES + this->getName();
			throw std::runtime_error{ errMsg };
		}

		setValue( object.getType(), object.getObjectValue() );
	}

	void Parameter::setValue( FieldType type
		, ValueBase const & value )
	{
		if ( value.isNull() )
		{
			setNull();
		}
		else
		{
			switch ( type )
			{
			case FieldType::eBit:
				doSetValue( static_cast< ValueT< FieldType::eBit > const & >( value ).getValue() );
				break;

			case FieldType::eSint32:
				doSetValue( static_cast< ValueT< FieldType::eSint32 > const & >( value ).getValue() );
				break;

			case FieldType::eSint64:
				doSetValue( static_cast< ValueT< FieldType::eSint64 > const & >( value ).getValue() );
				break;

			case FieldType::eUint32:
				doSetValue( static_cast< ValueT< FieldType::eUint32 > const & >( value ).getValue() );
				break;

			case FieldType::eUint64:
				doSetValue( static_cast< ValueT< FieldType::eUint64 > const & >( value ).getValue() );
				break;

			case FieldType::eFloat32:
				doSetValue( static_cast< ValueT< FieldType::eFloat32 > const & >( value ).getValue() );
				break;

			case FieldType::eFloat64:
				doSetValue( static_cast< ValueT< FieldType::eFloat64 > const & >( value ).getValue() );
				break;

			case FieldType::eChar:
				doSetValue( static_cast< ValueT< FieldType::eChar > const & >( value ).getValue() );
				break;

			case FieldType::eVarchar:
				doSetValue( static_cast< ValueT< FieldType::eVarchar > const & >( value ).getValue() );
				break;

			case FieldType::eText:
				doSetValue( static_cast< ValueT< FieldType::eText > const & >( value ).getValue() );
				break;

			case FieldType::eDatetime:
				doSetValue( static_cast< ValueT< FieldType::eDatetime > const & >( value ).getValue() );
				break;

			case FieldType::eBinary:
				doSetValue( static_cast< ValueT< FieldType::eBinary > const & >( value ).getValue() );
				break;

			case FieldType::eVarbinary:
				doSetValue( static_cast< ValueT< FieldType::eVarbinary > const & >( value ).getValue() );
				break;

			case FieldType::eBlob:
				doSetValue( static_cast< ValueT< FieldType::eBlob > const & >( value ).getValue() );
				break;

			default:
				throw std::runtime_error{ ERROR_DB_PARAMETER_SETVALUE_TYPE + this->getName() };
			}

			m_updater->update( *this );
		}
	}
}

