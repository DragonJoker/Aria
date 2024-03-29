/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbConnection_HPP___
#define ___Aria_DbConnection_HPP___

#include "DbTransaction.hpp"

#include <AriaLib/BeginExternHeaderGuard.hpp>
#include <sqlite3.h>
#include <AriaLib/EndExternHeaderGuard.hpp>

namespace aria::db
{
	class Connection
	{
	public:
		explicit Connection( wxFileName databaseFile );
		~Connection();

		bool executeUpdate( sqlite3_stmt * statement );
		ResultPtr executeSelect( sqlite3_stmt * statement, ValuedObjectInfosArray & infos );
		Transaction beginTransaction( std::string const & name );
		sqlite3_stmt * prepareStatement( std::string const & query );

		std::string writeText( const std::string & text ) const;
		std::string writeBinary( const ByteArray & array ) const;
		std::string writeName( const std::string & text ) const;
		std::string writeBool( bool value ) const;
		std::string writeFloat( float value ) const;
		std::string writeDouble( double value ) const;
		std::string writeDateTime( const DateTime & dateTime ) const;

		uint32_t getPrecision( FieldType type ) const;
		unsigned long getStmtDateTimeSize() const;

		DateTime parseDateTime( const std::string & dateTime ) const;

		bool executeUpdate( const std::string & query );
		ResultPtr executeSelect( const std::string & query, ValuedObjectInfosArray & infos );
		ResultPtr executeSelect( const std::string & query );

		StatementPtr createStatement( const std::string & query );

		void check( int code, char const * msg );
		void check( int code, std::string const & msg );
		void check( int code, std::ostream const & msg );

		sqlite3 * getConnection() const
		{
			return m_database;
		}

	private:
		sqlite3 * m_database{};
	};
}

#endif
