#pragma once
#include "Core.h"

#undef max
#undef min

namespace BLOCKMAN
{
	// New path finding utility
	class IPathPlanner
	{
	public:
		virtual ~IPathPlanner() = default;
		virtual bool findPath(const LORD::Vector3& from, const LORD::Vector3& to, int expandLimit) = 0;
		virtual LORD::Vector3 getPosInPath(int index) const = 0;
		virtual int getPathSize() const = 0;
		bool findPathNoLimit(const LORD::Vector3& from, const LORD::Vector3& to) { return findPath(from, to, 0); }
	};

	using PathPlannerCreator = IPathPlanner * (*)();

	template<typename Type>
	IPathPlanner* createPathPlanner()
	{
		return new Type;
	}

	class PathPlannerManager
	{
	public:
		static PathPlannerManager* Instance();
		bool registerPathPlanner(const LORD::String& type, PathPlannerCreator creator);
		IPathPlanner* newPathPlanner(const LORD::String& type);
		void deletePathPlanner(IPathPlanner* planner);

	private:
		LORD::map<LORD::String, PathPlannerCreator>::type creators;
	};

#define REGISTER_PATH_PLANNER(name) \
bool _pathPlanner##name##Registered = ([]{ return PathPlannerManager::Instance()->registerPathPlanner(#name, createPathPlanner<name>); })();

}