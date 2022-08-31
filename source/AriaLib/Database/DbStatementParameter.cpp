#include "Database/DbStatementParameter.hpp"

#include "Database/DbConnection.hpp"
#include "Database/DbParameterBinding.hpp"

#include "Database/DbValuedObjectInfos.hpp"

namespace aria::db
{
	int StatementParameter::SqliteDataTypes[size_t( FieldType::eCount )] =
	{
		SQLITE_NULL,     //!< FieldType::eNull
		SQLITE_INTEGER,  //!< FieldType::eBit
		SQLITE_INTEGER,  //!< FieldType::eSint32
		SQLITE_INTEGER,  //!< FieldType::eSint64
		SQLITE_INTEGER,  //!< FieldType::eUint32
		SQLITE_INTEGER,  //!< FieldType::eUint64
		SQLITE_FLOAT,    //!< FieldType::eFloat32
		SQLITE_FLOAT,    //!< FieldType::eFloat64
		SQLITE3_TEXT,    //!< FieldType::eChar
		SQLITE3_TEXT,    //!< FieldType::eVarchar
		SQLITE3_TEXT,    //!< FieldType::eText
		SQLITE_INTEGER,  //!< FieldType::eDatetime
		SQLITE_BLOB,     //!< FieldType::eBinary
		SQLITE_BLOB,     //!< FieldType::eVarbinary
		SQLITE_BLOB,     //!< FieldType::eBlob
	};

	static const std::string ERROR_SQLITE_PARAMETER_TYPE = "Undefined parameter type when trying to set its binding.";

	StatementParameter::StatementParameter( Connection & connection
		, ValuedObjectInfos infos
		, unsigned short index
		, ParameterType parameterType
		, std::unique_ptr< ValueUpdater > updater )
		: Parameter{ connection, infos, index, parameterType, std::move( updater ) }
		, m_dataType( SqliteDataTypes[size_t( getType() )] )
	{
	}

	const int & StatementParameter::getDataType() const
	{
		return m_dataType;
	}

	void StatementParameter::setNull()
	{
		Parameter::setNull();
		m_binding->updateValue();
	}

	void StatementParameter::setStatement( sqlite3_stmt * statement )
	{
		m_statement = statement;

		switch ( getType() )
		{
		case FieldType::eBit:
			m_binding = makeSqliteBind< FieldType::eBit >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eSint32:
			m_binding = makeSqliteBind< FieldType::eSint32 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eSint64:
			m_binding = makeSqliteBind< FieldType::eSint64 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eUint32:
			m_binding = makeSqliteBind< FieldType::eUint32 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eUint64:
			m_binding = makeSqliteBind< FieldType::eUint64 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eFloat32:
			m_binding = makeSqliteBind< FieldType::eFloat32 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eFloat64:
			m_binding = makeSqliteBind< FieldType::eFloat64 >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eChar:
			m_binding = makeSqliteBind< FieldType::eChar >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eVarchar:
			m_binding = makeSqliteBind< FieldType::eVarchar >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eText:
			m_binding = makeSqliteBind< FieldType::eText >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eDatetime:
			m_binding = makeSqliteBind< FieldType::eDatetime >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eBinary:
			m_binding = makeSqliteBind< FieldType::eBinary >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eVarbinary:
			m_binding = makeSqliteBind< FieldType::eVarbinary >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		case FieldType::eBlob:
			m_binding = makeSqliteBind< FieldType::eBlob >( m_statement, getConnection(), getIndex(), getObjectValue() );
			break;

		default:
			throw std::runtime_error{ ERROR_SQLITE_PARAMETER_TYPE };
		}
	}
}
