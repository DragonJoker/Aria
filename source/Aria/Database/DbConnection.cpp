#include "Database/DbConnection.hpp"
#include "Database/DbResult.hpp"
#include "Database/DbStatement.hpp"
#include "Database/DbValue.hpp"
#include "StringUtils.hpp"

#include <thread>

#pragma GCC diagnostic ignored "-Wconversion"

namespace aria::db
{
	namespace
	{
		static const std::string SQLITE_SQL_SNULL = "NULL";

		static const std::string ERROR_DB_MISSING_LIMITS = "Missing limits for field ";
		static const std::string ERROR_SQLITE_UNSUPPORTED_TYPE = "Unsupported type, for column ";

		static const char * INFO_SQLITE_SELECTION = "Database selection";
		static const char * INFO_SQLITE_STATEMENT_FINALISATION = "Statement finalisation";
		static const char * INFO_SQLITE_PREPARATION = "Statement preparation: ";

		static const std::string SQLITE_FORMAT_DATE = "STRFTIME('%%Y-%%m-%%d','%04i-%02i-%02i')";
		static const std::string SQLITE_FORMAT_TIME = "STRFTIME('%%H:%%M:%%S','%02i:%02i:%02i')";
		static const std::string SQLITE_FORMAT_DATETIME = "STRFTIME('%%Y-%%m-%%d %%H:%%M:%%S','%04i-%02i-%02i %02i:%02i:%02i')";
		static const std::string SQLITE_FORMAT_DATETIME_DATE = "STRFTIME('%%Y-%%m-%%d 00:00:00','%04i-%02i-%02i')";
		static const std::string SQLITE_FORMAT_DATETIME_TIME = "STRFTIME('0000-00-00 %%H:%%M:%%S','%02i:%02i:%02i')";
		static const std::string SQLITE_FORMAT_STMT_DATE = "%Y-%m-%d";
		static const std::string SQLITE_FORMAT_STMT_TIME = "%H:%M:%S";
		static const std::string SQLITE_FORMAT_STMT_DATETIME = "%Y-%m-%d %H:%M:%S";

		uint32_t retrieveLimits( std::string const & type )
		{
			std::pair< int, int > result( -1, -1 );
			size_t index = type.find( "(" );

			if ( index != std::string::npos )
			{
				size_t dotIndex = type.find( ",", index );

				if ( dotIndex == std::string::npos )
				{
					std::string limit = type.substr( index + 1, type.find( ")" ) - index );
					result.first = std::stoi( trim( limit ) );
				}
				else
				{
					std::string limit1 = type.substr( index + 1, dotIndex - index );
					result.first = std::stoi( trim( limit1 ) );
					std::string limit2 = type.substr( dotIndex + 1, type.find( ")" ) - dotIndex );
					result.second = std::stoi( trim( limit2 ) );
				}
			}

			return uint32_t( result.first );
		}

		ValuedObjectInfos getIntegerFieldInfos( const std::string & type
			, const std::string & columnName
			, const std::string & lowerName )
		{

			if ( type.find( "BIGINT" ) != std::string::npos
				|| lowerName.find( "max(" ) != std::string::npos
				|| lowerName.find( "min(" ) != std::string::npos
				|| lowerName.find( "count(" ) != std::string::npos
				|| lowerName.find( "sum(" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eSint64 };
			}

			if ( type.find( "SMALLINT" ) != std::string::npos
				|| type.find( "TINYINT" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eSint32 };
			}

			if ( type.find( "bool" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eBit };
			}

			size_t index = type.find( "INT" );

			if ( index != std::string::npos && type.size() > 3 )
			{
				int prec = 0;
				std::stringstream stream( type.substr( index + 3 ) );
				stream >> prec;

				if ( prec > 4 )
				{
					return ValuedObjectInfos{ columnName, FieldType::eSint64 };
				}
			}

			return ValuedObjectInfos{ columnName, FieldType::eSint32 };
		}

		ValuedObjectInfos getFloatFieldInfos( const std::string & type
			, const std::string & columnName
			, const std::string & lowerName )
		{
			if ( type.find( "DOUB" ) != std::string::npos
				|| type.find( "REAL" ) != std::string::npos
				|| type.find( "DECIMAL" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eFloat64 };
			}

			return ValuedObjectInfos{ columnName, FieldType::eFloat32 };
		}

		ValuedObjectInfos getStringFieldInfos( const std::string & type
			, const std::string & columnName
			, const std::string & lowerName )
		{
			if ( type.find( "VARCHAR" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eVarchar, retrieveLimits( type ) };
			}

			if ( type.find( "CHAR" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eChar, retrieveLimits( type ) };
			}

			return ValuedObjectInfos{ columnName, FieldType::eText };
		}

		ValuedObjectInfos getBlobFieldInfos( const std::string & type
			, const std::string & columnName
			, const std::string & lowerName )
		{
			if ( type.find( "VARBINARY" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eVarbinary, retrieveLimits( type ) };
			}

			if ( type.find( "BINARY" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eBinary, retrieveLimits( type ) };
			}

			return ValuedObjectInfos{ columnName, FieldType::eBlob };
		}

		ValuedObjectInfos getNullFieldInfos( const std::string & type
			, const std::string & columnName
			, const std::string & lowerName )
		{
			std::string upperType = upperCase( type );
			size_t index;

			if ( upperType == "DATETIME" )
			{
				return ValuedObjectInfos{ columnName, FieldType::eDatetime };
			}

			if ( upperType.find( "BIGINT" ) != std::string::npos
				|| lowerName.find( "max(" ) != std::string::npos
				|| lowerName.find( "min(" ) != std::string::npos
				|| lowerName.find( "count(" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eSint64 };
			}

			if ( upperType.find( "FLOA" ) != std::string::npos
				|| lowerName.find( "sum(" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eFloat32 };
			}

			if ( upperType.find( "NUMERIC" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eFloat64 };
			}

			if ( upperType.find( "DECIMAL" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eFloat64 };
			}

			if ( upperType.find( "SMALLINT" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eSint32 };
			}

			if ( upperType.find( "BOOL" ) != std::string::npos
				|| upperType.find( "BIT" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eBit };
			}

			if ( upperType.find( "TINYINT" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eSint32 };
			}

			if ( ( index = upperType.find( "MEDIUMINT" ) ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eSint32 };
			}

			if ( ( index = upperType.find( "INT" ) ) != std::string::npos )
			{
				if ( index != std::string::npos && upperType.size() > 3 )
				{
					int prec = 0;
					std::stringstream stream( upperType.substr( index + 3 ) );
					stream >> prec;

					if ( prec > 4 )
					{
						return ValuedObjectInfos{ columnName, FieldType::eSint64 };
					}
				}

				return ValuedObjectInfos{ columnName, FieldType::eSint32 };
			}

			if ( upperType.find( "NCHAR" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eChar, retrieveLimits( upperType ) };
			}

			if ( upperType.find( "NVARCHAR" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eVarchar, retrieveLimits( upperType ) };
			}

			if ( upperType.find( "VARCHAR" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eVarchar, retrieveLimits( upperType ) };
			}

			if ( upperType.find( "CHAR" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eChar, retrieveLimits( upperType ) };
			}

			if ( upperType.find( "NTEXT" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eText };
			}

			if ( upperType.find( "CLOB" ) != std::string::npos
				|| upperType.find( "TEXT" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eText };
			}

			if ( upperType.find( "REAL" ) != std::string::npos
				|| upperType.find( "DOUB" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eFloat64 };
			}

			if ( upperType.find( "BLOB" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eBlob };
			}

			if ( upperType.find( "VARBINARY" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eVarbinary, retrieveLimits( upperType ) };
			}

			if ( upperType.find( "BINARY" ) != std::string::npos )
			{
				return ValuedObjectInfos{ columnName, FieldType::eBinary, retrieveLimits( upperType ) };
			}

			wxLogWarning( wxString() << ERROR_SQLITE_UNSUPPORTED_TYPE << columnName << "(" << type << ")" );
			return ValuedObjectInfos{ columnName, FieldType::eNull };
		}

		template< FieldType Type, typename Value >
		FieldPtr constructField( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos, Value value )
		{
			int size = sqlite3_column_bytes( statement, i );
			FieldPtr field = std::make_unique< Field >( connection, infos );

			if ( size > 0 )
			{
				static_cast< ValueT< Type > & >( field->getObjectValue() ).setValue( typename ValueT< Type >::value_type( value ) );
			}

			return field;
		}

		template< FieldType Type > FieldPtr getValue( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos );

		template<>
		FieldPtr getValue< FieldType::eBit >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			bool value = sqlite3_column_int( statement, i ) != 0;
			return constructField< FieldType::eBit >( statement, i, connection, infos, value );
		}

		template<>
		FieldPtr getValue< FieldType::eSint32 >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			int32_t value = sqlite3_column_int( statement, i );
			return constructField< FieldType::eSint32 >( statement, i, connection, infos, value );
		}

		template<>
		FieldPtr getValue< FieldType::eSint64 >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			int64_t value = sqlite3_column_int64( statement, i );
			return constructField< FieldType::eSint64 >( statement, i, connection, infos, value );
		}

		template<>
		FieldPtr getValue< FieldType::eUint32 >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			auto value = uint32_t( sqlite3_column_int( statement, i ) );
			return constructField< FieldType::eUint32 >( statement, i, connection, infos, value );
		}

		template<>
		FieldPtr getValue< FieldType::eUint64 >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			auto value = uint64_t( sqlite3_column_int64( statement, i ) );
			return constructField< FieldType::eUint64 >( statement, i, connection, infos, value );
		}

		template<>
		FieldPtr getValue< FieldType::eFloat32 >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			float value = float( sqlite3_column_double( statement, i ) );
			return constructField< FieldType::eFloat32 >( statement, i, connection, infos, value );
		}

		template<>
		FieldPtr getValue< FieldType::eFloat64 >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			double value = sqlite3_column_double( statement, i );
			return constructField< FieldType::eFloat64 >( statement, i, connection, infos, value );
		}

		std::string getFieldTextValue( sqlite3_stmt * statement, int i, Connection & connection )
		{
			const char * value = reinterpret_cast< const char * >( sqlite3_column_text( statement, i ) );
			int size = sqlite3_column_bytes( statement, i );

			if ( value && size > 0 )
			{
				return std::string( value, value + size );
			}

			return std::string{};
		}

		template<>
		FieldPtr getValue< FieldType::eChar >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			FieldPtr field = std::make_unique< Field >( connection, infos );
			static_cast< ValueT< FieldType::eChar > & >( field->getObjectValue() ).updateValue( getFieldTextValue( statement, i, connection ) );
			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eVarchar >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			FieldPtr field = std::make_unique< Field >( connection, infos );
			static_cast< ValueT< FieldType::eVarchar > & >( field->getObjectValue() ).updateValue( getFieldTextValue( statement, i, connection ) );
			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eText >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			FieldPtr field = std::make_unique< Field >( connection, infos );
			static_cast< ValueT< FieldType::eText > & >( field->getObjectValue() ).updateValue( getFieldTextValue( statement, i, connection ) );
			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eDatetime >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			FieldPtr field = std::make_unique< Field >( connection, infos );
			static_cast< ValueT< FieldType::eDatetime > & >( field->getObjectValue() ).setValue( connection.parseDateTime( getFieldTextValue( statement, i, connection ) ) );
			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eBinary >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			const uint8_t * value = reinterpret_cast< const uint8_t * >( sqlite3_column_blob( statement, i ) );
			int size = sqlite3_column_bytes( statement, i );
			FieldPtr field = std::make_unique< Field >( connection, infos );

			if ( value && size > 0 )
			{
				static_cast< ValueT< FieldType::eBinary > & >( field->getObjectValue() ).setValueLimits( value, std::min( uint32_t( size ), infos.getLimits() ) );
			}

			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eVarbinary >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			const uint8_t * value = reinterpret_cast< const uint8_t * >( sqlite3_column_blob( statement, i ) );
			int size = sqlite3_column_bytes( statement, i );
			FieldPtr field = std::make_unique< Field >( connection, infos );

			if ( value && size > 0 )
			{
				static_cast< ValueT< FieldType::eVarbinary > & >( field->getObjectValue() ).setValueLimits( value, std::min( uint32_t( size ), infos.getLimits() ) );
			}

			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eBlob >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			const uint8_t * value = reinterpret_cast< const uint8_t * >( sqlite3_column_blob( statement, i ) );
			int size = sqlite3_column_bytes( statement, i );
			FieldPtr field = std::make_unique< Field >( connection, infos );

			if ( value && size > 0 )
			{
				static_cast< ValueT< FieldType::eBlob > & >( field->getObjectValue() ).setValueLimits( value, std::min( uint32_t( size ), infos.getLimits() ) );
			}

			return field;
		}

		template<>
		FieldPtr getValue< FieldType::eNull >( sqlite3_stmt * statement, int i, Connection & connection, ValuedObjectInfos & infos )
		{
			sqlite3_value * value = sqlite3_column_value( statement, i );
			int type = sqlite3_value_type( value );

			switch ( type )
			{
			case SQLITE_INTEGER:
				infos.setType( FieldType::eUint64 );
				return getValue< FieldType::eUint64 >( statement, i, connection, infos );

			case SQLITE_FLOAT:
				infos.setType( FieldType::eFloat64 );
				return getValue< FieldType::eFloat64 >( statement, i, connection, infos );

			case SQLITE_TEXT:
				{
					auto textValue = getFieldTextValue( statement, i, connection );
					wxDateTime tmp;

					if ( tmp.ParseFormat( textValue, SQLITE_FORMAT_STMT_DATETIME )
						&& tmp.IsValid() )
					{
						infos.setType( FieldType::eDatetime );
						return getValue< FieldType::eDatetime >( statement, i, connection, infos );
					}

					infos.setType( FieldType::eText );
					return getValue< FieldType::eText >( statement, i, connection, infos );
				}

			case SQLITE_BLOB:
				infos.setType( FieldType::eVarbinary );
				return getValue< FieldType::eVarbinary >( statement, i, connection, infos );

			case SQLITE_NULL:
				{
					const char * textValue = reinterpret_cast< const char * >( sqlite3_column_text( statement, i ) );
					int size = sqlite3_column_bytes( statement, i );

					if ( textValue && size > 0 )
					{
						infos.setType( FieldType::eText );
						FieldPtr field = std::make_unique< Field >( connection, infos );
						static_cast< ValueT< FieldType::eText > & >( field->getObjectValue() ).updateValue( std::string( textValue, textValue + size ) );
						return field;
					}

					infos.setType( FieldType::eSint32 );
					auto ivalue = uint32_t( sqlite3_column_int( statement, i ) );
					return constructField< FieldType::eSint32 >( statement, i, connection, infos, ivalue );
				}

			default:
				wxLogWarning( "Unsupported field value type." );
				return nullptr;
			}
		}

		FieldPtr setFieldValue( sqlite3_stmt * statement
			, int index
			, Connection & connection
			, ValuedObjectInfos & infos )
		{
			switch ( infos.getType() )
			{
			case FieldType::eNull:
				return getValue< FieldType::eNull >( statement, index, connection, infos );
			case FieldType::eBit:
				return getValue< FieldType::eBit >( statement, index, connection, infos );
			case FieldType::eSint32:
				return getValue< FieldType::eSint32 >( statement, index, connection, infos );
			case FieldType::eSint64:
				return getValue< FieldType::eSint64 >( statement, index, connection, infos );
			case FieldType::eUint32:
				return getValue< FieldType::eUint32 >( statement, index, connection, infos );
			case FieldType::eUint64:
				return getValue< FieldType::eUint64 >( statement, index, connection, infos );
			case FieldType::eFloat32:
				return getValue< FieldType::eFloat32 >( statement, index, connection, infos );
			case FieldType::eFloat64:
				return getValue< FieldType::eFloat64 >( statement, index, connection, infos );
			case FieldType::eChar:
				return getValue< FieldType::eChar >( statement, index, connection, infos );
			case FieldType::eVarchar:
				return getValue< FieldType::eVarchar >( statement, index, connection, infos );
			case FieldType::eText:
				return getValue< FieldType::eText >( statement, index, connection, infos );
			case FieldType::eDatetime:
				return getValue< FieldType::eDatetime >( statement, index, connection, infos );
			case FieldType::eBinary:
				return getValue< FieldType::eBinary >( statement, index, connection, infos );
			case FieldType::eVarbinary:
				return getValue< FieldType::eVarbinary >( statement, index, connection, infos );
			case FieldType::eBlob:
				return getValue< FieldType::eBlob >( statement, index, connection, infos );
			default:
				throw std::runtime_error( ERROR_SQLITE_UNSUPPORTED_TYPE );
			}
		}

		sqlite3_stmt * sqlitePrepareStatement( const std::string & query, sqlite3 * connection )
		{
			sqlite3_stmt * statement = nullptr;
			int code = sqlite3_prepare_v2( connection, query.c_str(), int( query.size() ), &statement, nullptr );

			while ( code == SQLITE_BUSY || code == SQLITE_LOCKED )
			{
				// Retry as long as the database is locked or busy
				std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
				code = sqlite3_prepare_v2( connection, query.c_str(), int( query.size() ), &statement, nullptr );
			}

			sqliteCheck( code, ( INFO_SQLITE_PREPARATION + query ).c_str(), connection );
			return statement;
		}

		ValuedObjectInfosArray sqliteGetColumns( sqlite3_stmt * statement )
		{
			ValuedObjectInfosArray arrayReturn;
			int iColumnCount = sqlite3_column_count( statement );

			for ( int i = 0; i < iColumnCount; i++ )
			{
				char const * pszName = sqlite3_column_name( statement, i );

				if ( pszName )
				{
					char const * pszType = sqlite3_column_decltype( statement, i );
					std::string type( pszType ? pszType : "" );

					switch ( sqlite3_column_type( statement, i ) )
					{
					case SQLITE_INTEGER:
						arrayReturn.push_back( getIntegerFieldInfos( type, pszName, lowerCase( pszName ) ) );
						break;

					case SQLITE_FLOAT:
						arrayReturn.push_back( getFloatFieldInfos( type, pszName, lowerCase( pszName ) ) );
						break;

					case SQLITE_TEXT:
						arrayReturn.push_back( getStringFieldInfos( type, pszName, lowerCase( pszName ) ) );
						break;

					case SQLITE_BLOB:
						arrayReturn.push_back( getBlobFieldInfos( type, pszName, lowerCase( pszName ) ) );
						break;

					case SQLITE_NULL:
						arrayReturn.push_back( getNullFieldInfos( type, pszName, lowerCase( pszName ) ) );
						break;

					default:
						throw std::runtime_error( ERROR_SQLITE_UNSUPPORTED_TYPE );
					}
				}
			}

			return arrayReturn;
		}

		ResultPtr sqliteFetchResult( sqlite3_stmt * statement
			, ValuedObjectInfosArray const & columns
			, Connection & connection )
		{
			auto result = std::make_unique< Result >( columns );
			int ret;

			while ( ( ret = sqlite3_step( statement ) ) != SQLITE_DONE )
			{
				if ( ret == SQLITE_ERROR )
				{
					return nullptr;
				}

				if ( ret == SQLITE_ROW )
				{
					Row row;
					int i = 0;

					for ( auto infos : columns )
					{
						row.addField( setFieldValue( statement, i++, connection, infos ) );
					}

					result->addRow( std::move( row ) );
				}
			}

			return result;
		}
	}

	//*********************************************************************************************

	void sqliteCheck( int code, char const * msg, sqlite3 * connection )
	{
		if ( code != SQLITE_OK )
		{
			wxString error;
			error << wxT( "Error : " ) << msg << wxT( " - " ) << sqlite3_errmsg( connection );
			throw std::runtime_error{ error.char_str( wxConvUTF8 ) };
		}

#if !defined( _NDEBUG )

		else
		{
			wxLogDebug( wxString() << wxT( "Success : " ) << msg );
		}

#endif
	}

	void sqliteCheck( int code, std::string const & msg, sqlite3 * connection )
	{
		sqliteCheck( code, msg.c_str(), connection );
	}

	void sqliteCheck( int code, std::ostream const & stream, sqlite3 * connection )
	{
		std::stringstream ss;
		ss << stream.rdbuf();
		sqliteCheck( code, ss.str(), connection );
	}

	//*********************************************************************************************

	Connection::Connection( wxFileName databaseFile )
	{
		wxString fileName = databaseFile.GetFullPath();

		if ( !wxFileExists( fileName ) )
		{
			if ( auto file = fopen( fileName.char_str( wxConvUTF8 ), "w" ) )
			{
				fclose( file );
			}
		}

		sqliteCheck( sqlite3_open( fileName.char_str( wxConvUTF8 ), &m_database )
			, wxString() << INFO_SQLITE_SELECTION
			, m_database );
	}

	Connection::~Connection()
	{
		sqlite3_close( m_database );
	}

	bool Connection::executeUpdate( sqlite3_stmt * statement )
	{
		ValuedObjectInfosArray infos;
		ResultPtr result;

		try
		{
			result = sqliteFetchResult( statement, infos, *this );
		}
		catch ( std::exception & exc )
		{
			wxLogError( wxString( exc.what() ) );
		}

		return result != nullptr;
	}

	ResultPtr Connection::executeSelect( sqlite3_stmt * statement
		, ValuedObjectInfosArray & infos )
	{
		ResultPtr result;

		try
		{
			if ( infos.empty() )
			{
				infos = sqliteGetColumns( statement );
			}

			result = sqliteFetchResult( statement, infos, *this );
		}
		catch ( std::exception & exc )
		{
			wxLogError( wxString( exc.what() ) );
		}

		return result;
	}

	sqlite3_stmt * Connection::prepareStatement( std::string const & query )
	{
		return sqlitePrepareStatement( query, m_database );
	}

	Transaction Connection::beginTransaction( std::string const & name )
	{
		return Transaction{ *this, name };
	}

	std::string Connection::writeText( const std::string & text ) const
	{
		std::string result( text );

		if ( result != SQLITE_SQL_SNULL )
		{
			replace( result, "'", "''" );
			replace( result, "\\", "\\\\" );
			result = "'" + result + "'";
		}

		return result;
	}

	std::string Connection::writeBinary( const ByteArray & array ) const
	{
		std::stringstream stream;
		stream.setf( std::ios::hex, std::ios::basefield );

		for ( auto && it = array.begin(); it != array.end(); ++it )
		{
			stream.width( 2 );
			stream.fill( '0' );
			stream << int( *it );
		}

		return "X'" + stream.str() + "'";
	}

	std::string Connection::writeName( const std::string & text ) const
	{
		return "[" + text + "]";
	}

	std::string Connection::writeBool( bool value ) const
	{
		return ( value ? "1" : "0" );
	}

	std::string Connection::writeFloat( float value ) const
	{
		std::stringstream stream;
		stream.imbue( std::locale{ "C" } );
		stream.precision( getPrecision( FieldType::eFloat32 ) );
		stream << value;
		return stream.str();
	}

	std::string Connection::writeDouble( double value ) const
	{
		std::stringstream stream;
		stream.imbue( std::locale{ "C" } );
		stream.precision( getPrecision( FieldType::eFloat64 ) );
		stream << value;
		return stream.str();
	}

	std::string Connection::writeDateTime( const DateTime & dateTime ) const
	{
		std::string strReturn;

		if ( dateTime.IsValid() )
		{
			strReturn = dateTime.FormatISODate() + " " + dateTime.FormatISOTime();
		}
		else
		{
			strReturn = SQLITE_SQL_SNULL;
		}

		return strReturn;
	}

	uint32_t Connection::getPrecision( FieldType type ) const
	{
		uint32_t result = 0;

		switch ( type )
		{
		case FieldType::eFloat32:
			result = 7;
			break;
		case FieldType::eFloat64:
			result = 15;
			break;
		default:
			break;
		}

		return result;
	}

	unsigned long Connection::getStmtDateTimeSize() const
	{
		return SQLITE_STMT_DATETIME_SIZE;
	}

	DateTime Connection::parseDateTime( const std::string & dateTime ) const
	{
		DateTime dateTimeObj;
		dateTimeObj.ParseFormat( dateTime, SQLITE_FORMAT_STMT_DATETIME );
		return dateTimeObj;
	}

	bool Connection::executeUpdate( const std::string & query )
	{
		bool ret = false;

		try
		{
			sqlite3_stmt * statement = sqlitePrepareStatement( query, m_database );
			ret = executeUpdate( statement );
			sqliteCheck( sqlite3_finalize( statement ), INFO_SQLITE_STATEMENT_FINALISATION, m_database );
		}
		catch ( std::exception & exc )
		{
			wxLogError( exc.what() );
		}

		return ret;
	}

	ResultPtr Connection::executeSelect( const std::string & query
		, ValuedObjectInfosArray & infos )
	{
		ResultPtr ret;

		try
		{
			sqlite3_stmt * statement = sqlitePrepareStatement( query, m_database );
			ret = executeSelect( statement, infos );
			sqliteCheck( sqlite3_finalize( statement ), INFO_SQLITE_STATEMENT_FINALISATION, m_database );
		}
		catch ( std::exception & exc )
		{
			wxLogError( exc.what() );
		}

		return ret;
	}

	ResultPtr Connection::executeSelect( const std::string & query )
	{
		ValuedObjectInfosArray infos;
		return executeSelect( query, infos );
	}

	StatementPtr Connection::createStatement( const std::string & query )
	{
		return std::make_unique< Statement >( *this, query );
	}

	void Connection::check( int code, char const * msg )
	{
		sqliteCheck( code, msg, m_database );
	}

	void Connection::check( int code, std::string const & msg )
	{
		sqliteCheck( code, msg, m_database );
	}

	void Connection::check( int code, std::ostream const & msg )
	{
		sqliteCheck( code, msg, m_database );
	}

	//*********************************************************************************************
}
