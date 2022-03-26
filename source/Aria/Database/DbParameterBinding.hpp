/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbParameterBinding_HPP___
#define ___CTP_DbParameterBinding_HPP___

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wcast-align"
#pragma clang diagnostic ignored "-Wdisabled-macro-expansion"
#pragma clang diagnostic ignored "-Wextra-semi-stmt"
#pragma clang diagnostic ignored "-Wimplicit-fallthrough"
#pragma clang diagnostic ignored "-Wnewline-eof"
#pragma clang diagnostic ignored "-Wold-style-cast"
#pragma clang diagnostic ignored "-Woverloaded-virtual"
#pragma clang diagnostic ignored "-Wsource-uses-openmp"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Walloc-zero"
#pragma GCC diagnostic ignored "-Wcast-qual"
#pragma GCC diagnostic ignored "-Wclass-memaccess"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wuseless-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"

#include "DbPrerequisites.hpp"

#include "DbStatementParameter.hpp"

#include <sstream>

namespace aria::db
{
	static const std::string ERROR_SQLITE_PARAMETER_VALUE = "Can't set parameter value";
	static const std::string ERROR_SQLITE_UPDATE_UNIMPLEMENTED = "updateValue not implemented for this data type";

	static const std::string INFO_SQLITE_SET_PARAMETER_VALUE = "Set parameter value: ";
	static const std::string INFO_SQLITE_SET_PARAMETER_NULL = "Set parameter NULL";

	static const std::string SQLITE_FORMAT_STMT_SDATE = "%Y-%m-%d";
	static const std::string SQLITE_FORMAT_STMT_STIME = "%H:%M:%S";
	static const std::string SQLITE_FORMAT_STMT_SDATETIME = "%Y-%m-%d %H:%M:%S";

	template< FieldType FieldTypeT >
	struct ParameterBindingT
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement
			, sqlite3 * connection
			, uint16_t index
			, ValueT< FieldTypeT > const & value )
			: ParameterBinding{ statement, connection, index }
			, mm_value{ value }
		{
		}

		void updateValue() override
		{
			throw std::runtime_error{ ERROR_SQLITE_UPDATE_UNIMPLEMENTED };
		}

		ValueT< FieldTypeT > mm_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eBit >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eBit > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int( statement, index, m_value.getValue() ? 1 : 0 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eBit > const & m_value;
	};

	struct IntegerParameterBinding
		: public ParameterBinding
	{
		IntegerParameterBinding( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index )
			: ParameterBinding( statement, connection, index )
		{
		}

		void updateValue( bool null, int value )
		{
			if ( null )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int( statement, index, value ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << value, connection );
			}
		}
	};

	template<>
	struct ParameterBindingT< FieldType::eSint32 >
		: public IntegerParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eSint32 > const & value )
			: IntegerParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			IntegerParameterBinding::updateValue( m_value.isNull(), int( m_value.getValue() ) );
		}

		ValueT< FieldType::eSint32 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eUint32 >
		: public IntegerParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eUint32 > const & value )
			: IntegerParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			IntegerParameterBinding::updateValue( m_value.isNull(), int( m_value.getValue() ) );
		}

		ValueT< FieldType::eUint32 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eSint64 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eSint64 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int64( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eSint64 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eUint64 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eUint64 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_int64( statement, index, int64_t( m_value.getValue() ) ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eUint64 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eFloat32 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eFloat32 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_double( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eFloat32 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eFloat64 >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eFloat64 > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_double( statement, index, m_value.getValue() ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getValue(), connection );
			}
		}

		ValueT< FieldType::eFloat64 > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eChar >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eChar > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_text64( statement, index, reinterpret_cast< const char * >( m_value.getPtrValue() ), m_value.getPtrSize(), SQLITE_STATIC, SQLITE_UTF8 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << "[" <<  m_value.getValue() << "]", connection );
			}
		}

		ValueT< FieldType::eChar > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eVarchar >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eVarchar > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_text64( statement, index, reinterpret_cast< const char * >( m_value.getPtrValue() ), m_value.getPtrSize(), SQLITE_STATIC, SQLITE_UTF8 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << "[" <<  m_value.getValue() << "]", connection );
			}
		}

		ValueT< FieldType::eVarchar > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eText >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eText > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_text64( statement, index, reinterpret_cast< const char * >( m_value.getPtrValue() ), m_value.getPtrSize(), SQLITE_STATIC, SQLITE_UTF8 ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << "[" <<  m_value.getValue() << "]", connection );
			}
		}

		ValueT< FieldType::eText > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eDatetime >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eDatetime > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull()
				|| !m_value.getValue().IsValid() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				m_holder = m_value.getValue().FormatISODate() + " " + m_value.getValue().FormatISOTime();
				sqliteCheck( sqlite3_bind_text( statement, index, m_holder.c_str(), int( m_holder.size() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_holder, connection );
			}
		}

		ValueT< FieldType::eDatetime > const & m_value;
		std::string m_holder;
	};

	template<>
	struct ParameterBindingT< FieldType::eBinary >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eBinary > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_blob( statement, index, m_value.getPtrValue(), int( m_value.getPtrSize() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getPtrValue(), connection );
			}
		}

		ValueT< FieldType::eBinary > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eVarbinary >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eVarbinary > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_blob( statement, index, m_value.getPtrValue(), int( m_value.getPtrSize() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getPtrValue(), connection );
			}
		}

		ValueT< FieldType::eVarbinary > const & m_value;
	};

	template<>
	struct ParameterBindingT< FieldType::eBlob >
		: public ParameterBinding
	{
		ParameterBindingT( sqlite3_stmt * statement, sqlite3 * connection, uint16_t index, ValueT< FieldType::eBlob > const & value )
			: ParameterBinding( statement, connection, index )
			, m_value{ value }
		{
		}

		void updateValue() override
		{
			if ( m_value.isNull() )
			{
				sqliteCheck( sqlite3_bind_null( statement, index ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_NULL, connection );
			}
			else
			{
				sqliteCheck( sqlite3_bind_blob( statement, index, m_value.getPtrValue(), int( m_value.getPtrSize() ), SQLITE_STATIC ), std::stringstream{} << INFO_SQLITE_SET_PARAMETER_VALUE << m_value.getPtrValue(), connection );
			}
		}

		ValueT< FieldType::eBlob > const & m_value;
	};

	template< FieldType FieldTypeT >
	std::unique_ptr< ParameterBinding > makeSqliteBind( sqlite3_stmt * statement
		, Connection & connection
		, uint16_t index
		, ValueBase const & value )
	{
		return std::make_unique< ParameterBindingT< FieldTypeT > >( statement
			, connection.getConnection()
			, index
			, static_cast< ValueT< FieldTypeT > const & >( value ) );
	}
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop

#endif
