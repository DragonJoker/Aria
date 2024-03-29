#include "Database/DbResult.hpp"

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <sstream>
#include "AriaLib/EndExternHeaderGuard.hpp"

namespace aria::db
{
	namespace result
	{
		static const std::string ERROR_DB_NO_FIELD = "No field at index: ";
	}

	Result::Result( const ValuedObjectInfosArray & arrayFieldInfos )
		: m_fieldInfos( arrayFieldInfos )
	{
	}

	uint32_t Result::getFieldCount() const
	{
		return uint32_t( m_fieldInfos.size() );
	}

	ValuedObjectInfos const & Result::getFieldInfos( uint32_t index ) const
	{
		if ( index >= m_fieldInfos.size() )
		{
			std::stringstream message;
			message << result::ERROR_DB_NO_FIELD << index;
			throw std::runtime_error{ message.str() };
		}

		return m_fieldInfos[index];
	}

	void Result::addRow( Row row )
	{
		m_rows.emplace_back( std::move( row ) );
	}
}
