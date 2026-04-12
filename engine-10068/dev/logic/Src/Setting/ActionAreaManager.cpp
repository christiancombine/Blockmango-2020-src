#include "ActionAreaManager.h"

namespace BLOCKMAN
{
	vector<ActionArea*>::type  ActionAreaManager::m_areas = {};

	void ActionAreaManager::unloadSetting()
	{
		for (auto item : m_areas )
		{
			delete item;
			item = nullptr;
		}
		m_areas.clear();
	}

	void ActionAreaManager::setActionAreas(const vector<ActionArea>::type& areas)
	{
		unloadSetting();
		for (const auto& item : areas)
		{
			ActionArea* area = new ActionArea;
			area->id = item.id;
			area->titleNor = item.titleNor;
			area->titlePre = item.titlePre;
			area->type = item.type;
			area->isVisible = item.isVisible;
			area->backNor = item.backNor;
			area->backPre = item.backPre;
			area->startPos = item.startPos;
			area->endPos = item.endPos;
			m_areas.push_back(area);
		}
	}

	ActionArea * ActionAreaManager::ActionAreaByPosition(const Vector3 & pos)
	{
		for (const auto  item : m_areas)
		{
			if (item && item->startPos.x <= pos.x && item->endPos.x >= pos.x && item->startPos.z <= pos.z && item->endPos.z >= pos.z && item->startPos.y <= pos.y &&  item->endPos.y >= pos.y)
			{
				return item;
			}
		}
		return nullptr;
	}
}