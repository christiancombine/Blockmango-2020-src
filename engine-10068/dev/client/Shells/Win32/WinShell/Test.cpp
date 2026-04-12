#include "stdafx.h"
#include "Test.h"
#include "Any/VarList.h"
#include <iostream>

using namespace BLOCKMAN;

void Test::testAll()
{
	testVarList();
}

void Test::testVarList()
{
	VarList list;
	list << true << 11037 << std::string("Hello World!");
    VarList result;
    result << list << std::string("hh");
	std::cout << result.get<bool>(0) << " " << result.get<int>(1) << " " << result.get<std::string>(2) << " " << result.get<std::string>(3) << std::endl;

	/*std::vector<ANY> list{ true, 11037, std::string("Hello World!") };
	std::cout << any_cast<bool>(list[0]) << " " << any_cast<int>(list[1]) << " " << any_cast<std::string>(list[2]) << std::endl;*/
}
