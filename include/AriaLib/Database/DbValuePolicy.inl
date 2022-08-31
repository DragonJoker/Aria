#include "DbConnection.hpp"

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

namespace aria::db
{
	//*********************************************************************************************

	/** FieldTypeNeedsLimitsT for FieldType::eChar
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eChar > : std::true_type {};

	/** FieldTypeNeedsLimitsT for FieldType::eVarchar
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eVarchar > : std::true_type {};

	/** FieldTypeNeedsLimitsT for FieldType::eBinary
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eBinary > : std::true_type {};

	/** FieldTypeNeedsLimitsT for FieldType::eVarbinary
	*/
	template<> struct FieldTypeNeedsLimitsT< FieldType::eVarbinary > : std::true_type {};

	//*********************************************************************************************

	/** FieldTypeDataTyperT specialization for FieldType::eBit
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eBit >
	{
		static const size_t size = 1;
		using value_type = bool;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eSint32
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eSint32 >
	{
		static const size_t size = 32;
		using value_type = int32_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eSint64
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eSint64 >
	{
		static const size_t size = 64;
		using value_type = int64_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eUint32
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eUint32 >
	{
		static const size_t size = 32;
		using value_type = uint32_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eUint64
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eUint64 >
	{
		static const size_t size = 64;
		using value_type = uint64_t;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eFloat32
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eFloat32 >
	{
		static const size_t size = 32;
		using value_type = float;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eFloat64
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eFloat64 >
	{
		static const size_t size = 64;
		using value_type = double;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eDatetime
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eDatetime >
	{
		static const size_t size = 0;
		using value_type = DateTime;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eChar
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eChar >
	{
		static const size_t size = 0;
		using value_type = std::string;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eVarchar
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eVarchar >
	{
		static const size_t size = 0;
		using value_type = std::string;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eText
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eText >
	{
		static const size_t size = 0;
		using value_type = std::string;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eBinary
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eBinary >
	{
		static const size_t size = 0;
		using value_type = ByteArray;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eVarbinary
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eVarbinary >
	{
		static const size_t size = 0;
		using value_type = ByteArray;
	};

	/** FieldTypeDataTyperT specialization for FieldType::eBlob
	*/
	template<> struct FieldTypeDataTyperT< FieldType::eBlob >
	{
		static const size_t size = 0;
		using value_type = ByteArray;
	};

	//*********************************************************************************************

	/** ValuePolicyT specialization for FieldType::eFloat32 type
	*/
	template<> struct ValuePolicyT< FieldType::eFloat32 >
	{
		using value_type = FieldTypeDataTyperT< FieldType::eFloat32 >::value_type;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out = in;
			size = sizeof( value_type );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value;
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return connection.writeFloat( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eFloat64 type
	*/
	template<> struct ValuePolicyT< FieldType::eFloat64 >
	{
		using value_type = FieldTypeDataTyperT< FieldType::eFloat64 >::value_type;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out = in;
			size = sizeof( value_type );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value;
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return connection.writeDouble( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eBit type
	*/
	template<> struct ValuePolicyT< FieldType::eBit >
	{
		using value_type = FieldTypeDataTyperT< FieldType::eBit >::value_type;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out = in;
			size = sizeof( value_type );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value;
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return connection.writeBool( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** Policy used for text, char and nvarchar fields
	*/
	struct TextValuePolicyT
	{
		typedef std::string value_type;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out = in;
			size = static_cast< unsigned long >( in.size() );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value[0];
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value[0];
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return connection.writeText( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eVarchar type
	*/
	template<> struct ValuePolicyT< FieldType::eChar >
		: public TextValuePolicyT
	{
		using value_type = TextValuePolicyT::value_type;
	};

	/** ValuePolicyT specialization for FieldType::eVarchar type
	*/
	template<> struct ValuePolicyT< FieldType::eVarchar >
		: public TextValuePolicyT
	{
		using value_type = TextValuePolicyT::value_type;
	};

	/** ValuePolicyT specialization for FieldType::eText type
	*/
	template<> struct ValuePolicyT< FieldType::eText >
		: public TextValuePolicyT
	{
		using value_type = TextValuePolicyT::value_type;
	};

	/** ByteArray value policy (shared amongst FieldType::eBinary, FieldType::eVarbinary, and FieldType::eBlob)
	*/
	struct ByteArrayValuePolicyT
	{
		using value_type = ByteArray;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out.clear();

			if ( !in.empty() )
			{
				out.reserve( in.size() );
				out.insert( out.end(), in.begin(), in.end() );
				size = static_cast< unsigned long >( in.size() );
			}

			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			void * result = nullptr;

			if ( !value.empty() )
			{
				result = value.data();
			}

			return result;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			void const * result = nullptr;

			if ( !value.empty() )
			{
				result = value.data();
			}

			return result;
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return connection.writeBinary( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	/** ValuePolicyT specialization for FieldType::eBinary type
	*/
	template<> struct ValuePolicyT< FieldType::eBinary >
		: public ByteArrayValuePolicyT
	{
		typedef ByteArrayValuePolicyT::value_type value_type;
	};

	/** ValuePolicyT specialization for FieldType::eVarbinary type
	*/
	template<> struct ValuePolicyT< FieldType::eVarbinary >
		: public ByteArrayValuePolicyT
	{
		typedef ByteArrayValuePolicyT::value_type value_type;
	};

	/** ValuePolicyT specialization for FieldType::eBlob type
	*/
	template<> struct ValuePolicyT< FieldType::eBlob >
		: public ByteArrayValuePolicyT
	{
		typedef ByteArrayValuePolicyT::value_type value_type;
	};

	/** ValuePolicyT specialization for FieldType::eDatetime type
	*/
	template<> struct ValuePolicyT< FieldType::eDatetime >
	{
		typedef FieldTypeDataTyperT< FieldType::eDatetime >::value_type value_type;

		/** Sets the value to the given one
		@param in
			The input value
		@param out
			The output value
		@param size
			Receives the new value size
		@param connection
			The connection used to format the value
		*/
		bool set( const value_type & in, value_type & out, unsigned long & size, const Connection & connection )
		{
			out = in;
			size = static_cast< unsigned long >( connection.getStmtDateTimeSize() );
			return true;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		void * ptr( value_type & value )
		{
			return &value;
		}

		/** Retrieves a pointer from the given value
		@param value
			The value
		*/
		const void * ptr( const value_type & value )const
		{
			return &value;
		}

		/** Puts the value into the given string
		@param value
			The value
		@param valSet
			Tells that the value is set
		@param connection
			The connection used to format the value
		@param result
			Receives the insertable value
		*/
		std::string toQueryValue( const value_type & value, bool valSet, const Connection & connection )const
		{
			if ( valSet )
			{
				return connection.writeDateTime( value );
			}
			else
			{
				return NULL_VALUE;
			}
		}
	};

	//*********************************************************************************************
}

#pragma GCC diagnostic pop
#pragma clang diagnostic pop
