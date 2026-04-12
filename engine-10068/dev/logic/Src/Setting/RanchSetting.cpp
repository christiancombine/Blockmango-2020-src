#include "RanchSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"

namespace BLOCKMAN
{
	std::map<int, RanchItem*>  RanchSetting::m_ranchItemMap = {};

	bool RanchSetting::loadSetting(bool bIsClient)
	{
		loadRanchItem(bIsClient);
		return true;
	}

	void  RanchSetting::unloadSetting()
	{
		for (auto iter = m_ranchItemMap.begin(); iter != m_ranchItemMap.end(); iter++)
		{
			RanchItem* pRanchItem = iter->second;
			delete pRanchItem;
		}

		m_ranchItemMap.clear();
	}

	bool  RanchSetting::loadRanchItem(bool bIsClient)
	{
		RanchItem* pRanchItem = NULL;
		m_ranchItemMap.clear();
		CsvReader* pReader = new CsvReader();

		std::string path;
		if (bIsClient)
		{
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/RanchItems.csv")).c_str();
		}
		else
		{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/RanchItems.csv")).c_str();
#else
			path = (PathUtil::ConcatPath(Root::Instance()->getRootPath(), "RanchItems.csv")).c_str();
#endif
		}
		
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordLogError("Can not open  Ranch Items file %s", path.c_str());
			LordSafeDelete(pReader);
			return false;
		}

		pReader->Read(); // discard the second line
		String recipes = "";
		StringArray recipesArr;
		StringArray recipeArr;
		
		
		while (pReader->Read())
		{
			pRanchItem = new RanchItem();
			pReader->GetInt("Id", pRanchItem-> id);
			pReader->GetString("Name", pRanchItem->name);
			pReader->GetString("Desc", pRanchItem->desc);
			String icon = "";
			String makeIcon = "";
			pReader->GetString("Icon", icon);
			pReader->GetString("MakeIcon", makeIcon);
			pRanchItem->icon = StringUtil::Format("set:ranch_items.json image:%s", icon.c_str());
			pRanchItem->makeIcon = StringUtil::Format("set:ranch_items.json image:%s", makeIcon.c_str());
			pReader->GetInt("Type", pRanchItem->type);
			pReader->GetInt("CubePrice", pRanchItem->cubePrice);
			m_ranchItemMap[pRanchItem->id] = pRanchItem;
		}

		LordSafeDelete(pReader);
		return true;
	}

	RanchItem *  RanchSetting::getRanchItem(int  ranchItemId)
	{
		return m_ranchItemMap[ranchItemId];
	}

	i32 RanchSetting::getRanchItemType(int ranchItemId)
	{
		auto item = getRanchItem(ranchItemId);
		return item ? item->type : 0;
	}

	String RanchSetting::getRanchItemIcon(int ranchItemId)
	{
		auto item = getRanchItem(ranchItemId);
		return item ? item->icon : "";
	}

	String RanchSetting::getRanchItemName(int ranchItemId)
	{
		auto item = getRanchItem(ranchItemId);
		return item ? item->name : "";
	}

	String RanchSetting::getRanchItemDesc(int ranchItemId)
	{
		auto item = getRanchItem(ranchItemId);
		return item ? item->desc : "";
	}

	String RanchSetting::getRanchItemMakeIcon(int ranchItemId)
	{
		auto item = getRanchItem(ranchItemId);
		return item ? item->makeIcon : "";
	}

	i32 RanchSetting::getRanchItemCubePrice(int ranchItemId)
	{
		auto item = getRanchItem(ranchItemId);
		return item ? item->cubePrice : 0;
	}
}


