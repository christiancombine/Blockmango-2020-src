#pragma once

#include "../Blockman/World/ServerWorld.h"

class ServerDynamicCast
{
public:
	static ServerWorld* dynamicCastServerWorld(World* world)
	{
		return dynamic_cast<ServerWorld*>(world);
	}
};