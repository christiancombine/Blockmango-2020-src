#include "BtTreeController.h"
#include "Server.h"
#include "Behaviac/behaviac_generated/types/internal/behaviac_agent_headers.h"

using namespace LORD;

namespace BLOCKMAN
{
	BtTreeController::BtTreeController()
	{
	}

	BtTreeController::~BtTreeController()
	{
		clearBtTrees();
	}

	void BtTreeController::onUpdate()
	{
		float frameTime = Server::Instance()->getServerFramTime();
		for (auto& iter : m_trees)
		{
			iter.second.m_recordUpdateTime += frameTime;
			if (iter.second.m_recordUpdateTime < iter.second.m_updateInterval)
				continue;

			iter.second.m_recordUpdateTime -= iter.second.m_updateInterval;
			if (iter.second.m_agent != NULL && iter.second.m_agent->IsActive())
				iter.second.m_agent->btexec();
		}
	}

	int BtTreeController::addBtTree(const String& btTreeName, TableVisitor tableVistor, float updateInterval)
	{
		int uniqueId = 0;
		m_loadAgentMutex.lock();
		auto agent = behaviac::Agent::Create<ServerAgent>();
		if (agent->btload(btTreeName.c_str()))
		{
			m_uniqueId++;
			uniqueId = m_uniqueId;
			agent->Init(tableVistor, uniqueId);
			agent->btsetcurrent(btTreeName.c_str());
			agent->SetActive(true);
			BtTreeRecord tree;
			tree.m_btTreeName = btTreeName;
			tree.m_agent = agent;
			tree.m_updateInterval = updateInterval;
			m_trees.insert({ uniqueId, tree });
		}
		else
		{
			behaviac::Agent::Destroy(agent);
		}
		m_loadAgentMutex.unlock();
		return uniqueId;
	}

	void BtTreeController::removeBtTree(int treeId)
	{
		auto iter = m_trees.find(treeId);
		if (iter != m_trees.end())
		{
			if (iter->second.m_agent != NULL)
			{
				behaviac::Agent::Destroy(iter->second.m_agent);
				iter->second.m_agent = NULL;
			}
			m_trees.erase(treeId);
		}
	}

	int BtTreeController::addBtTreeFromAgent(const String& btTreeName, behaviac::Agent* agent, float updateInterval)
	{
		int uniqueId = 0;
		m_loadAgentMutex.lock();
		if (agent->btload(btTreeName.c_str()))
		{
			m_uniqueId++;
			uniqueId = m_uniqueId;
			agent->btsetcurrent(btTreeName.c_str());
			agent->SetActive(true);
			BtTreeRecord tree;
			tree.m_btTreeName = btTreeName;
			tree.m_agent = agent;
			tree.m_updateInterval = updateInterval;
			m_trees.insert({ uniqueId, tree });
		}
		else
		{
			behaviac::Agent::Destroy(agent);
		}
		m_loadAgentMutex.unlock();
		return uniqueId;
	}

	void BtTreeController::clearBtTrees()
	{
		m_trees.clear();
	}

}