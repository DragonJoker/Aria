/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbDateTimeHelpers_HPP___
#define ___CTP_DbDateTimeHelpers_HPP___

#include "DbPrerequisites.hpp"

namespace aria::db
{
	namespace time
	{
		std::string format( const Time & time, const std::string & format );
		std::string print( const Time & time, const std::string & format );
		bool isTime( const std::string & time, const std::string & format );
		bool isTime( const std::string & time, const std::string & format, Time & result );
		bool isValid( const Time & time );
	}


	namespace date
	{
		std::string format( const Date & time, const std::string & format );
		std::string print( const Date & date, const std::string & format );
		bool isDate( const std::string & date, const std::string & format );
		bool isDate( const std::string & date, const std::string & format, Date & result );
		bool isValid( const Date & date );
	}
}

#endif
