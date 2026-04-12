#pragma once
#ifndef __BT_TREE_CONTROLLER_HEADER__
#define __BT_TREE_CONTROLLER_HEADER__

#include "Core.h"
#include <mutex>
#include "LuaRegister/Template/TableVisitor.h"
#include "behaviac/agent/agent.h"

using namespace LORD;

namespace BLOCKMAN
{
	class BtTreeRecord
	{
	public:
		float m_updateInterval = 0.5f; // second
		float m_recordUpdateTime = 0;
		String m_btTreeName{ "" };
		behaviac::Agent* m_agent = NULL;
	};
	
	class BtTreeController : public Singleton<BtTreeController>, public ObjectAlloc
	{
	public:
		BtTreeController();
		~BtTreeController();

		void onUpdate();

		int addBtTree(const String & btTreeName, TableVisitor tableVistor, float updateInterval = 0.5f);
		void removeBtTree(int treeId);

		int addBtTreeFromAgent(const String& btTreeName, behaviac::Agent* agent, float updateInterval = 0.5f);

		void clearBtTrees();

	private:
		map<int, BtTreeRecord>::type m_trees;
		int m_uniqueId = 0;
		std::mutex m_loadAgentMutex;
	};
	
}

#endif