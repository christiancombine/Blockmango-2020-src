#include "SignInSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"

namespace BLOCKMAN
{
	map<int, ItemSetting*>::type SignInSetting::m_item_setting_map = {};
	map<int, SignInItem*>::type SignInSetting::m_cur_sign_in_items = {};

	void SignInSetting::loadSetting()
	{
		ItemSetting* item = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "SignInSetting.csv").c_str();
		bool bRet = pReader->LoadFile(path);
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read();
		pReader->Read();
		while (pReader->Read())
		{
			item = new ItemSetting();
			pReader->GetInt("Id", item->id);
			pReader->GetInt("Order", item->order);
			pReader->GetString("Title", item->title);
			pReader->GetInt("Side", item->side);
			pReader->GetString("Background", item->background);
			pReader->GetString("Effect", item->effect);
			pReader->GetString("Image", item->image);
			pReader->GetInt("Num", item->num);
			pReader->GetString("NumText", item->numText);
			pReader->GetInt("RewardId", item->rewardId);
			m_item_setting_map[item->order] = item;
		}
		
		LordSafeDelete(pReader);
	}

	void SignInSetting::unloadSetting()
	{
		for (auto item : m_item_setting_map)
		{
			delete item.second;
		}
		m_item_setting_map.clear();
		for (auto item : m_cur_sign_in_items)
		{
			delete item.second;
		}
		m_cur_sign_in_items.clear();
	}

	map<int, SignInItem*>::type SignInSetting::getShowItems(int startOrder, int nowOrder, ItemStatus status)
	{
		for (auto item : m_cur_sign_in_items)
		{
			delete item.second;
		}
		m_cur_sign_in_items.clear();
		SignInItem* item = NULL;
		ItemSetting* setting = NULL;
		int UseRoom = 0;
		auto iter = m_item_setting_map.find(startOrder);
		while (iter != m_item_setting_map.end())
		{
			setting = iter->second;
			if (UseRoom + setting->side > 8)
			{
				break;
			}
			item = new SignInItem();
			item->order = setting->order;
			item->side = setting->side;
			item->id = setting->id;
			item->rewardId = setting->rewardId;
			if (startOrder < nowOrder)
			{
				item->status = ItemStatus::HAVE_RECEIVED;
			}
			else
			{
				if (startOrder == nowOrder)
				{
					item->status = status;
				}
				else
				{
					item->status = ItemStatus::UN_RECEIVED;
				}
			}
			if (UseRoom == 3 && item->side == 2)
			{
				UseRoom++;
			}
			m_cur_sign_in_items[UseRoom] = item;
			UseRoom = UseRoom + setting->side;
			iter++;
			startOrder++;
		}
		return m_cur_sign_in_items;
	}

	ItemSetting * SignInSetting::getSettingByOrder(int order)
	{
		auto iter = m_item_setting_map.find(order);
		if (iter != m_item_setting_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}
}