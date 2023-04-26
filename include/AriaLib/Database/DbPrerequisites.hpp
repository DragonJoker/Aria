/*
See LICENSE file in root folder
*/
#ifndef ___Aria_DbPrerequisites_HPP___
#define ___Aria_DbPrerequisites_HPP___

#include "AriaLib/BeginExternHeaderGuard.hpp"
#include <cstdint>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include <wx/filename.h>
#include <wx/log.h>
#include "AriaLib/EndExternHeaderGuard.hpp"

#include "DbFieldType.hpp"
#include "DbParameterType.hpp"

#pragma warning( push )
#pragma warning( disable: 5264 )

#if !defined( _WIN32 )
#	define AriaLib_API
#else
#	if defined( AriaLib_EXPORTS )
#		define AriaLib_API __declspec( dllexport )
#	else
#		define AriaLib_API __declspec( dllimport )
#	endif
#endif

namespace aria::db
{
	using ByteArray = std::vector< uint8_t >;
	using StringArray = std::vector< std::string >;

	//YYYY-mm-dd HH:MM:SS
	static constexpr unsigned long SQLITE_STMT_DATETIME_SIZE = 19;

	static const std::string NULL_VALUE = "NULL";

	typedef wxDateTime DateTime;

	class Connection;
	class Field;
	class Parameter;
	class ParameteredObject;
	class Result;
	class Row;
	class Statement;
	class Transaction;
	class ValueBase;
	class ValuedObject;
	class ValuedObjectInfos;

	template< FieldType FieldTypeT >
	struct FieldTypeNeedsLimitsT : std::false_type{};
	template< FieldType FieldTypeT >
	struct FieldTypeDataTyperT;
	template< FieldType FieldTypeT >
	struct ValuePolicyT;
	template< FieldType FieldTypeT, typename PolicyT = ValuePolicyT< FieldTypeT > >
	class ValueT;

	using FieldPtr = std::unique_ptr< Field >;
	using ResultPtr = std::unique_ptr< Result >;
	using ValueBasePtr = std::unique_ptr< ValueBase >;
	using ParameterPtr = std::unique_ptr< Parameter >;
	using StatementPtr = std::unique_ptr< Statement >;

	using FieldArray = std::vector< FieldPtr >;
	using ParameterArray = std::vector< ParameterPtr >;
	using ValuedObjectInfosArray = std::vector< ValuedObjectInfos >;
	using RowArray = std::vector< Row >;
}

#pragma warning( pop )

#endif
