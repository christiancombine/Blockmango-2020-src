#pragma once
#include "LuaRegister/Template/Public.h"
#include "Core.h"
#include "Log/Log.h"

using namespace LORD;
struct AutoRunner
{
	AutoRunner(std::function<void()> fuc);
};


