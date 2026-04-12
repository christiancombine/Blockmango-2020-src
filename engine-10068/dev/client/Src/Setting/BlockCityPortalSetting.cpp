#include "BlockCityPortalSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	map<i32, BlockCityPortalActorItem*>::type BlockCityPortalSetting::m_portalActorItemMap = {};
	vector<BlockCityPortalItem>::type BlockCityPortalSetting::m_portalItems = {};

	BlockCityPortalSetting::BlockCityPortalSetting()
	{
		loadSetting();
		loadActorSetting();
	}

	BlockCityPortalSetting::~BlockCityPortalSetting()
	{
		unloadSetting();
		unloadActorSetting();
	}

	bool BlockCityPortalSetting::loadSetting()
	{
		m_portalItems.clear();

		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/portal.csv").c_str();
		
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			BlockCityPortalItem pSetting;
			pReader->GetInt("id", pSetting.id);
			pReader->GetInt("uId", pSetting.portalId);
			pReader->GetBool("isOpen", pSetting.isOpen);
			pReader->GetString("name", pSetting.name);

			m_portalItems.push_back(pSetting);
		}

		LordSafeDelete(pReader);
		return true;
	}

	void  BlockCityPortalSetting::unloadSetting()
	{
		m_portalItems.clear();
	}

	bool BlockCityPortalSetting::loadActorSetting()
	{
		BlockCityPortalActorItem* pPortalActorItem = NULL;
		m_portalActorItemMap.clear();
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting/portalActor.csv").c_str();

		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			pPortalActorItem = new BlockCityPortalActorItem();
			float x1 = 0.f;
			float y1 = 0.f;
			float z1 = 0.f;
			float x2 = 0.f;
			float y2 = 0.f;
			float z2 = 0.f;
			pReader->GetFloat("x1", x1);
			pReader->GetFloat("y1", y1);
			pReader->GetFloat("z1", z1);
			pReader->GetFloat("x2", x2);
			pReader->GetFloat("y2", y2);
			pReader->GetFloat("z2", z2);
			x1 = Math::Min(x1, x2);
			y1 = Math::Min(y1, y2);
			z1 = Math::Min(z1, z2);
			x2 = Math::Max(x1, x2);
			y2 = Math::Max(y1, y2);
			z2 = Math::Max(z1, z2);
			pPortalActorItem->startPos = Vector3(x1, y1, z1);
			pPortalActorItem->endPos = Vector3(x2, y2, z2);
			pReader->GetInt("uId", pPortalActorItem->id);
			m_portalActorItemMap[pPortalActorItem->id] = pPortalActorItem;
		}

		LordSafeDelete(pReader);
		return true;
	}

	void  BlockCityPortalSetting::unloadActorSetting()
	{
		for (auto iter = m_portalActorItemMap.begin(); iter != m_portalActorItemMap.end(); iter++)
		{
			if (iter->second)
			{
				delete iter->second;
				iter->second = nullptr;
			}
		}
		m_portalActorItemMap.clear();
	}

	i32 BlockCityPortalSetting::getPortalActorByPos(Vector3 pos)
	{
		for (const auto &item : m_portalActorItemMap)
		{
			if (pos.x >= item.second->startPos.x && pos.y >= item.second->startPos.y &&
				pos.z >= item.second->startPos.z && pos.x <= item.second->endPos.x &&
				pos.y <= item.second->endPos.y && pos.z <= item.second->endPos.z)
			{
				return item.first;
			}
		}
		return 0;
	}

	vector<BlockCityPortalItem>::type BlockCityPortalSetting::getPortalItemsById(i32 portalId)
	{
		vector<BlockCityPortalItem>::type items;
		for (const auto &item : m_portalItems)
		{
			if (item.portalId == portalId)
			{
				items.push_back(item);
			}
		}
		return items;
	}
}
