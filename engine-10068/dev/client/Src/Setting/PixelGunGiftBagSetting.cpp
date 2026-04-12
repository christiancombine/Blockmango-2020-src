#include "PixelGunGiftBagSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "rapidjson/document.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/Wallet.h"

namespace BLOCKMAN
{
	map<int, GiftBag*>::type PixelGunGiftBagSetting::m_gift_bag_map = {};
	list<GiftBag*>::type PixelGunGiftBagSetting::m_shown_gift_bag_list = {};

	void PixelGunGiftBagSetting::loadSetting()
	{
		loadGiftBagSetting();
	}

	void PixelGunGiftBagSetting::unloadSetting()
	{
		for (auto iter : m_gift_bag_map)
		{
			delete iter.second;
		}
		m_gift_bag_map.clear();

		m_shown_gift_bag_list.clear();
	}

	void PixelGunGiftBagSetting::loadGiftBagSetting()
	{
		GiftBag* pGiftBag = NULL;
		CsvReader* pReader = new CsvReader();
		std::string path = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName(), "setting", "GiftSetting.csv").c_str();
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
			pGiftBag = new GiftBag();
			pReader->GetInt("Id", pGiftBag->Id);
			pReader->GetString("Name", pGiftBag->Name);
			pReader->GetString("Describe", pGiftBag->Describe);
			pReader->GetString("Image", pGiftBag->Image);
			pReader->GetInt("Price", pGiftBag->Price);
			pReader->GetInt("Discount", pGiftBag->Discount);
			pReader->GetString("Items", pGiftBag->Items);
			pReader->GetInt("Weight", pGiftBag->Weight);
			pReader->GetInt("Sort", pGiftBag->Sort);
			pReader->GetString("BgImage", pGiftBag->BgImage);

			String TagIcon = "";
			pReader->GetString("TagIcon", TagIcon);
			pGiftBag->TagIcon = TagIcon == "#" ? "" : TagIcon;

			int Type = 0;
			pReader->GetInt("Type", Type);
			pGiftBag->Type = (GiftBagType)Type;

			pReader->GetInt("MoneyType", pGiftBag->MoneyType);

			pReader->GetString("Items", pGiftBag->Items);

			m_gift_bag_map[pGiftBag->Id] = pGiftBag;
			
			m_shown_gift_bag_list.push_back(pGiftBag);
		}
		
		LordSafeDelete(pReader);
	}

	GiftBag * PixelGunGiftBagSetting::findGiftBagById(int id)
	{
		auto iter = m_gift_bag_map.find(id);
		if (iter != m_gift_bag_map.end())
		{
			return iter->second;
		}
		return nullptr;
	}

	GiftBag * PixelGunGiftBagSetting::findShownGiftBagById(int id)
	{
		for (auto gift_bag : m_shown_gift_bag_list)
		{
			if (gift_bag->Id == id)
			{
				return gift_bag;
			}
		}
		return nullptr;
	}

	list<Reward*>::type PixelGunGiftBagSetting::findGiftGoodsByGiftId(int id)
	{
		list<Reward*>::type result;
		GiftBag* gift = findGiftBagById(id);
		if (gift)
		{
			result = RewardSetting::findRewardByIds(gift->Items);
		}
		return result;
	}

	void PixelGunGiftBagSetting::removeShownGiftBagById(int id)
	{

		for (auto gift_bag : m_shown_gift_bag_list)
		{
			if (gift_bag->Id == id)
			{
				m_shown_gift_bag_list.remove(gift_bag);
				break;
			}
		}
	}

	list<GiftBag*>::type PixelGunGiftBagSetting::parseUpdateGiftBagData(const String & data, bool isRefresh)
 	{
		if (data.length() == 0)
			return m_shown_gift_bag_list;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return m_shown_gift_bag_list;
		}
		if (!doc->IsArray())
		{
			LordDelete(doc);
			return m_shown_gift_bag_list;
		}
		auto items = doc->GetArray();

		if (isRefresh)
		{
			m_shown_gift_bag_list.clear();
		}
		
		for (size_t i = 0; i < items.Size(); ++i)
		{
			auto obj = items[i].GetObject();
			if (!obj.HasMember("Id"))
				continue;
			int Id = obj.FindMember("Id")->value.GetInt();
			GiftBag* pGiftBag = findGiftBagById(Id);
			if (!pGiftBag)
			{
				continue;
			}
			int status = obj.FindMember("Status")->value.GetInt();
			pGiftBag->Status = (GiftBagStatus)status ? (GiftBagStatus)status : pGiftBag->Status;
			pGiftBag->HasChange = true;

			GiftBag* gift_bag = findShownGiftBagById(Id);
			if (gift_bag)
			{
				removeShownGiftBagById(Id);
			}
			m_shown_gift_bag_list.push_back(pGiftBag);
		}
		return sortGifts(m_shown_gift_bag_list);
		LordDelete(doc);
	}
	
	bool PixelGunGiftBagSetting::sortGiftFunc(GiftBag* gift1, GiftBag* gift2)
	{
		if (gift1->Status != gift2->Status)
		{
			return (int)gift1->Status < (int)gift2->Status;
		}
		return gift1->Sort > gift2->Sort;
	}

	list<GiftBag*>::type PixelGunGiftBagSetting::sortGifts(list<GiftBag*>::type gifts)
	{
		gifts.sort(sortGiftFunc);
		return gifts;
	}

	void PixelGunGiftBagSetting::onGiftBought(int giftBagId)
	{
		auto gift = findGiftBagById(giftBagId);
		if (!gift)
		{
			return;
		} 
		gift->Status = GIFT_STATUS_BOUGHT;
	}

	int PixelGunGiftBagSetting::checkEnoughMoney(GiftBag* Gift)
	{
		i64 money = Blockman::Instance()->m_pPlayer->m_wallet->getMoneyByCoinId(Gift->MoneyType);
		if (money >= (Gift->Price * Gift->Discount / 100))
		{
			return 0;
		}
		return (Gift->Price * Gift->Discount / 100) - money;
	}
}