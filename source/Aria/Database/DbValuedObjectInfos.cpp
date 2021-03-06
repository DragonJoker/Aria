#include "Database/DbValuedObjectInfos.hpp"

namespace aria::db
{
	//*********************************************************************************************

	namespace valinfo
	{
		static const std::string ERROR_DB_MISSING_LIMITS = "Missing limits for field ";

		static bool needsLimits( FieldType type )
		{
			bool result = false;

			switch ( type )
			{
			case FieldType::eChar:
			case FieldType::eVarchar:
			case FieldType::eBinary:
			case FieldType::eVarbinary:
				result = true;
				break;

			default:
				result = false;
				break;
			}

			return result;
		}
	}

	//*********************************************************************************************

	ValuedObjectInfos::ValuedObjectInfos( const std::string & name )
		: ValuedObjectInfos( name, FieldType::eNull, uint32_t( -1 ) )
	{
	}

	ValuedObjectInfos::ValuedObjectInfos( const std::string & name
		, FieldType type )
		: ValuedObjectInfos( name, type, uint32_t( -1 ) )
	{
		if ( valinfo::needsLimits( type ) )
		{
			throw std::runtime_error{ valinfo::ERROR_DB_MISSING_LIMITS + getName() };
		}
	}

	ValuedObjectInfos::ValuedObjectInfos( const std::string & name
		, FieldType type
		, uint32_t limits )
		: m_name( name )
		, m_type( type )
		, m_limits( limits )
	{
	}

	void ValuedObjectInfos::setType( FieldType type )
	{
		if ( valinfo::needsLimits( type ) )
		{
			throw std::runtime_error{ valinfo::ERROR_DB_MISSING_LIMITS + getName() };
		}

		m_type = type;
	}

	void ValuedObjectInfos::setType( FieldType type, uint32_t limits )
	{
		m_type = type;
		m_limits = limits;
	}

	//*********************************************************************************************
}
