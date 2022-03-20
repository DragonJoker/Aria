/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestsModelPrerequisites_HPP___
#define ___CTP_TestsModelPrerequisites_HPP___

#include "Model/ModelPrerequisites.hpp"

namespace aria
{
	class TestTreeModelNode;
	class TestTreeModel;

	typedef std::vector< TestTreeModelNode * > TestTreeModelNodePtrArray;

	struct Config
	{
		wxFileName test;
		wxFileName work;
		bool init{};
	};

	struct Test
	{
		uint32_t id;
		std::string name;
		db::DateTime runDate;
		TestStatus status;
		std::string renderer;
		std::string category;
	};

	using TestArray = std::vector< Test >;
	using TestMap = std::map< std::string, TestArray >;
}

#endif
