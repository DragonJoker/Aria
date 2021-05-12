#include "Database/DbDateTimeHelpers.hpp"

#include <sstream>

namespace aria::db
{
	namespace
	{
		static int MonthMaxDays[13] = { -1, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

		void str_formalize( std::string & formattedString
			, int maxSize
			, const char * format
			, va_list vaList )
		{
			static const std::string ERROR_DB_FORMALIZE = "Error while formatting: ";
			std::vector< char > strText( maxSize + 1, 0 );

			try
			{

				if ( format )
				{
					size_t written = vsnprintf( strText.data(), maxSize + 1, format, vaList );
					formattedString.assign( strText.data(), strText.data() + std::min( written, size_t( maxSize ) ) );
				}
			}
			catch ( ... )
			{
				std::stringstream message;
				message << ERROR_DB_FORMALIZE << formattedString.c_str();
				throw std::runtime_error{ message.str() };
			}
		}

		void formalize( std::string & formattedString, int maxSize, const char * format, ... )
		{
			formattedString.clear();

			if ( format )
			{
				va_list vaList;
				va_start( vaList, format );
				str_formalize( formattedString, maxSize, format, vaList );
				va_end( vaList );
			}
		}

		bool isLeap( int year )
		{
			return ( year % 4 == 0 ) && ( year % 100 != 0 || year % 400 == 0 );
		}

		template< typename CharType >
		int stoi( CharType const *& in, size_t count )
		{
			if ( *in == '-' )
			{
				++count;
			}

			int result = std::stoi( std::basic_string< CharType >( in, in + count ) );
			in += count;
			return result;
		}
	}

	//*********************************************************************************************

	namespace date_time
	{
		namespace utils
		{
			static const size_t DATE_TIME_MAX_SIZE = 100;

			template< typename Char >
			bool isDateTime( const  std::basic_string< Char > & date
				, const  std::basic_string< Char > & format
				, int & year
				, int & month
				, int & monthDay
				, int & hours
				, int & minutes
				, int & seconds )
			{
				bool bReturn = !format.empty() && !date.empty();

				monthDay = 0;
				month = 0;
				year = -1;
				hours = 0;
				minutes = 0;
				seconds = 0;

				if ( bReturn )
				{
					Char const * fc = format.data();
					Char const * dc = date.data();

					while ( bReturn && *fc )
					{
						if ( *fc == '%' )
						{
							bReturn = ++fc != NULL;

							if ( bReturn )
							{
								switch ( *fc++ )
								{
								case 'H':
									hours = stoi( dc, 2 );
									break;

								case 'M':
									minutes = stoi( dc, 2 );
									break;

								case 'S':
									seconds = stoi( dc, 2 );
									break;

								case 'Y':
									year = stoi( dc, 4 );
									break;

								case 'd':
									monthDay = stoi( dc, 2 );
									break;

								case 'm':
									month = stoi( dc, 2 );
									break;

								case 'y':
									year = stoi( dc, 2 ) + 1900;
									break;

								case '%':
									break;

								default:
									bReturn = false;
									break;
								}
							}
						}
						else if ( *fc == *dc )
						{
							++fc;
							++dc;
						}
						else
						{
							bReturn = false;
						}
					}
				}

				if ( bReturn )
				{
					bReturn = year >= wxDateTime::Inv_Year;
				}

				if ( bReturn )
				{
					bReturn = month >= wxDateTime::Jan && month <= wxDateTime::Dec;
				}

				if ( bReturn )
				{
					if ( month != wxDateTime::Feb )
					{
						bReturn = monthDay <= MonthMaxDays[month];
					}
					else
					{
						bReturn = monthDay <= ( MonthMaxDays[month] + isLeap( year ) );
					}
				}

				if ( bReturn )
				{
					bReturn = hours >= 0 && hours <= 23 && minutes >= 0 && minutes <= 59 && seconds >= 0 && seconds <= 59;
				}

				return bReturn;
			}
		}

		std::string format( const DateTime & dateTime, const std::string & format )
		{
			return std::string{ dateTime.Format( format ).char_str( wxConvUTF8 ) };
		}

		std::string print( const DateTime & dateTime, const std::string & format )
		{
			auto year = int( dateTime.GetYear() );
			auto month = int( dateTime.GetMonth() );
			auto day = int( dateTime.GetDay() );
			auto hours = dateTime.GetHour();
			auto minutes = dateTime.GetMinute();
			auto seconds = dateTime.GetSecond();
			std::string result;
			formalize( result, 1024, format.c_str(), year, month, day, hours, minutes, seconds );
			return result;
		}

		bool isDateTime( const std::string & date, const std::string & format )
		{
			int monthDay = 0;
			int month = 0;
			int year = -1;
			int hour = 0;
			int min = 0;
			int sec = 0;
			return utils::isDateTime( date, format, year, month, monthDay, hour, min, sec );
		}

		bool isDateTime( const std::string & date, const std::string & format, DateTime & result )
		{
			int monthDay = 0;
			int month = 0;
			int year = -1;
			int hour = 0;
			int min = 0;
			int sec = 0;
			bool bReturn = utils::isDateTime( date, format, year, month, monthDay, hour, min, sec );

			if ( bReturn )
			{
				result = DateTime{ wxDateTime::wxDateTime_t( monthDay )
					, wxDateTime::Month( month )
					, year
					, wxDateTime::wxDateTime_t( hour )
					, wxDateTime::wxDateTime_t( min )
					, wxDateTime::wxDateTime_t( sec ) };
			}
			else
			{
				result = DateTime();
			}

			return bReturn;
		}

		bool isValid( const DateTime & dateTime )
		{
			return dateTime.IsValid();
		}
	}

	//*********************************************************************************************
}
