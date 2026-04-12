#include "LuaRegister/Template/Util/AutoRunner.h"

AutoRunner::AutoRunner(std::function<void()> fuc)
{
	fuc();
}