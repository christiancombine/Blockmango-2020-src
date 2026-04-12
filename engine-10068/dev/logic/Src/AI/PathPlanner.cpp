#include "PathPlanner.h"

using namespace LORD;

namespace BLOCKMAN
{
	PathPlannerManager* PathPlannerManager::Instance()
	{
		static PathPlannerManager instance;
		return &instance;
	}

	bool PathPlannerManager::registerPathPlanner(const LORD::String& type, PathPlannerCreator creator)
	{
		auto&& func = creators[type];
		if (func)
			return false;
		func = creator;;
		return true;
	}

	IPathPlanner* PathPlannerManager::newPathPlanner(const LORD::String& type)
	{
		auto it = creators.find(type);
		if (it == creators.end())
		{
			LordLogError("Cannot find PathPlannerGenerator for type %s", type.c_str());
			return nullptr;
		}
		return (it->second)();
	}

	void PathPlannerManager::deletePathPlanner(IPathPlanner* planner)
	{
		delete planner;
	}
}