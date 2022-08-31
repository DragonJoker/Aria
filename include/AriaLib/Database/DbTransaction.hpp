/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbTransaction_HPP___
#define ___Aria_DbTransaction_HPP___

#include "AriaLib/Database/DbPrerequisites.hpp"

namespace aria::db
{
	class Transaction
	{
	public:
		Transaction( Connection & connection
			, std::string name );
		bool commit();
		bool rollback();

		operator bool() const
		{
			return m_valid;
		}

	private:
		Connection & m_connection;
		std::string m_name;
		bool m_valid{};
	};
}

#endif
