/*
See LICENSE file in root folder
*/
#ifndef ___CTP_DbFieldType_HPP___
#define ___CTP_DbFieldType_HPP___

namespace aria::db
{
	enum class FieldType
	{
		eNull,
		eBit,
		eSint32,
		eSint64,
		eUint32,
		eUint64,
		eFloat32,
		eFloat64,
		eChar,
		eVarchar,
		eText,
		eDatetime,
		eBinary,
		eVarbinary,
		eBlob,
		eCount
	};
}

#endif
