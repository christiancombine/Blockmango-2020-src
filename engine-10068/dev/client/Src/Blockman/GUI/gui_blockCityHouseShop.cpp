#include "gui_blockCityHouseShop.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"
#include "UI/GUICommonDef.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Setting/BlockCitySetting.h"
#include "game.h"
#include "ShellInterface.h"
#include "Util/TipType.h"
#include "rapidjson/writer.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIButton.h"
#include "GUI/GuiActorWindow.h"

using namespace rapidjson;

namespace BLOCKMAN
{
	i32 gui_blockCityHouseShop::replaceIndex = -1;
	gui_blockCityHouseShop::gui_blockCityHouseShop() :
		gui_layout("BlockCityHouseShop.json")
	{
	}

	gui_blockCityHouseShop::~gui_blockCityHouseShop()
	{
	}

	void gui_blockCityHouseShop::onLoad()
	{
		setText("BlockCityHouseShop-Title-Text", "gui_blockcity_house_shop");
		setText("BlockCityHouseShop-Info-Detail-Text", "gui_blockcity_house_shop_detail");
		setText("BlockCityHouseShop-Info-Classify-Text", "gui_blockcity_house_shop_classify");

		m_name = getWindow<GUIStaticText>("BlockCityHouseShop-Item-Name");
		m_actor = getWindow<GuiActorWindow>("BlockCityHouseShop-Actor");
		m_desc = getWindow<GUIStaticText>("BlockCityHouseShop-Info-Detail-Content");
		m_classify = getWindow<GUIStaticText>("BlockCityHouseShop-Info-Classify-Content");
		m_price = getWindow<GUIStaticText>("BlockCityHouseShop-Buy-Num");
		m_buy = getWindow<GUIButton>("BlockCityHouseShop-Buy");
		m_currencyIcon = getWindow<GUIStaticImage>("BlockCityHouseShop-Buy-Icon");

		m_subscriptionGuard.add(UpdateBlockCityHouseShopEvent::subscribe(std::bind(&gui_blockCityHouseShop::onShopUpdate, this)));
		m_subscriptionGuard.add(ShowBlockCityHouseShopByNpcEvent::subscribe(std::bind(&gui_blockCityHouseShop::openShopByNpc, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowBlockCityHouseShopEvent::subscribe(std::bind(&gui_blockCityHouseShop::openShop, this, std::placeholders::_1, std::placeholders::_2, false)));
		getWindow("BlockCityHouseShop-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityHouseShop::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow<GUIButton>("BlockCityHouseShop-Up")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityHouseShop::onClick, this, std::placeholders::_1, ViewId::UP));
		getWindow<GUIButton>("BlockCityHouseShop-Next")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityHouseShop::onClick, this, std::placeholders::_1, ViewId::NEXT));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityHouseShop::onClick, this, std::placeholders::_1, ViewId::BUY));

	}

	bool gui_blockCityHouseShop::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityHouseShop::onClick(const EventArgs & args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::UP:
		{
			auto items = m_shopItemInfos.items;
			String shopClassifyName = m_shopItemInfos.shopClassifyIdName;

			if (items.size() > 0)
			{
				m_itemIndex -= 1;
				if (m_itemIndex < 0)
				{
					m_itemIndex = items.size() - 1;
				}
				changeUI(items[m_itemIndex], shopClassifyName, m_itemIndex);
			}
		}
		break;
		case ViewId::NEXT:
		{
			auto items = m_shopItemInfos.items;
			String shopClassifyName = m_shopItemInfos.shopClassifyIdName;

			if (items.size() > 0)
			{
				m_itemIndex += 1;
				if (m_itemIndex >= (i32)items.size())
				{
					m_itemIndex = 0;
				}
				changeUI(items[m_itemIndex], shopClassifyName, m_itemIndex);
			}
		}
		break;
		case ViewId::BUY:
		{
			auto item = BlockCitySetting::getItem(m_shopItem.itemId);
			if (item)
			{
				if (m_shopItem.itemStatus >= 2 && m_shopItemInfos.shopClassifyId != i32(ShopClassifyId::Flower))
				{
					i32 index = findFreeInventoryIndex(m_shopItem.itemId);
					ClientNetwork::Instance()->getCommonSender()->sendBlockCityChangeToolStatus(m_shopItemInfos.shopClassifyId, m_shopItem.itemId, index);
				}
				else if (m_shopItem.moneyType == i32(CurrencyType::PLATFORM_MONEY))
				{	
					UICommon::checkMoney(item->cube, m_shopItem.moneyType, [=](bool isSuccess) {
						if (isSuccess)
						{
							int index = findFreeInventoryIndex(m_shopItem.itemId);
							ClientNetwork::Instance()->getCommonSender()->sendBlockCityHouseShopBuyGoods(m_shopClassifyId, m_shopItem.itemId, m_shopItem.moneyType, index);
						}
						else
						{
							GameClient::CGame::Instance()->getShellInterface()->onRecharge(1);
						}
					});
				}
				else
				{
					UICommon::checkMoney(item->price, m_shopItem.moneyType, [=](bool isSuccess) {
						if (isSuccess)
						{
							int index = findFreeInventoryIndex(m_shopItem.itemId);
							ClientNetwork::Instance()->getCommonSender()->sendBlockCityHouseShopBuyGoods(m_shopClassifyId, m_shopItem.itemId, m_shopItem.moneyType, index);
						}
						else
						{
							vector<BlockCityPayment>::type pays;
							BlockCityPayment pay;
							pay.num = 1;
							pay.price = (i32)Math::Ceil(item->price / 100.f);
							pay.moneyType = 1;
							pay.tabId = m_shopClassifyId;
							pay.itemId = m_shopItem.itemId;
							pay.index = findFreeInventoryIndex(m_shopItem.itemId);
							pays.push_back(pay);
							ShowBlockCityShowCommonTipEvent::emit(pays, (i32)BlockCityTipType::BLOCKCITY_LACK_MONEY_HOUSE_SHOP, "gui_blockcity_lock_money_tip");
						}
					});
				}
			}
		}
		break;
		}
		return false;
	}

	bool gui_blockCityHouseShop::onShopUpdate()
	{
		if (isShown())
		{
			openShop(m_areaId, m_shopItem.itemId, true);
		}
		return true;
	}

	bool gui_blockCityHouseShop::openShop(i32 areaId, i32 itemId, bool isUpdate)
	{
		if (!UICommon::checkBlockCityParam())
		{
			return false;
		}

		m_shopItemInfos = Blockman::Instance()->m_pPlayer->m_blockCity->getShopItemInfoByAreaId(areaId);
		m_shopClassifyId = m_shopItemInfos.shopClassifyId;
		String shopClassifyName = m_shopItemInfos.shopClassifyIdName;
		auto items = m_shopItemInfos.items;
		m_areaId = areaId;

		for (size_t i = 0; i < items.size(); i++)
 		{
 			if (itemId == 0 && i == 0 || items[i].itemId == itemId)
 			{
 				changeUI(items[i], shopClassifyName, (i32)i);
 				if (!isUpdate)
  				{
 					RootGuiLayout::Instance()->showBlockCityHouseShop();
 				}
				return true;
 			}
 		}
		return false;
	}

	bool gui_blockCityHouseShop::openShopByNpc(const String & msg)
	{
		Document doc = Document();
		doc.Parse(msg.c_str());
		if (doc.HasParseError())
		{
			return false;
		}

		if (doc.HasMember("areaId") && doc.HasMember("itemId"))
		{
			openShop(doc.FindMember("areaId")->value.GetInt(), doc.FindMember("itemId")->value.GetInt(), false);
		}
		return true;
	}

	void gui_blockCityHouseShop::changeUI(BlockCityHouseShopItem shopItem, String shopClassify, i32 index)
	{
		m_itemIndex = index;
		m_shopItem = shopItem;

		auto item = BlockCitySetting::getItem(shopItem.itemId);
		if (item)
		{
			setText(m_name, item->name);
			setText(m_desc, item->desc);
			setText(m_classify, shopClassify);

			auto items = m_shopItemInfos.items;
			m_actor->SetActor((items[m_itemIndex].itemActorName).c_str());
			m_actor->SetArea(UDim(-0.1f, 0), UDim(-0.35f, 0), UDim(1.f, 0), UDim(1.f, 0));
			float itemScale = items[m_itemIndex].itemScale;
			m_actor->SetActorScale(itemScale);
			m_currencyIcon->SetImage(UICommon::getCurrencyIconByType(shopItem.moneyType).c_str());
			if (items[m_itemIndex].itemStatus == 1 || m_shopItemInfos.shopClassifyId == 7)
			{
				m_price->SetText(shopItem.moneyType != i32(CurrencyType::PLATFORM_MONEY) ? UICommon::numberToString(item->price).c_str() : UICommon::numberToString(item->cube).c_str());
				m_price->SetHorizontalAlignment(HA_RIGHT);
				m_buy->SetNormalImage("set:blockcity_house_shop.json image:buy");
				m_buy->SetPushedImage("set:blockcity_house_shop.json image:buy");
				m_currencyIcon->SetVisible(true);
			}
			else
			{
				m_buy->SetNormalImage(items[m_itemIndex].itemStatus > 2 ? "set:blockcity_pack.json image:demount" : "set:blockcity_pack.json image:use");
				m_buy->SetPushedImage(items[m_itemIndex].itemStatus > 2 ? "set:blockcity_pack.json image:demount" : "set:blockcity_pack.json image:use");
				setText(m_price, items[m_itemIndex].itemStatus > 2 ? "gui_blockcity_pack_demount" : "gui_blockcity_pack_use");
				m_price->SetHorizontalAlignment(HA_CENTRE);
				m_currencyIcon->SetVisible(false);
			}
			m_buy->SetVisible(items[m_itemIndex].isCanBuy > 0);
		}
	}

	i32 gui_blockCityHouseShop::findFreeInventoryIndex(i32 itemId)
	{
		InventoryPlayer* inventory = Blockman::Instance()->m_pPlayer->inventory;
		i32 index = inventory->getInventorySlotContainItem(itemId);
		if (index == -1)
		{
			index = (m_shopClassifyId == i32(ShopClassifyId::Wings)) ? inventory->getInventoryIndexOfCurrentItem() : inventory->getFirstEmptyStackInHotbar();
			if (index == -1)
			{
				replaceIndex = ++replaceIndex < 9 ? replaceIndex : 0;
				index = replaceIndex;
			}
			else
			{
				replaceIndex = -1;
			}
		}
		inventory->currentItemIndex = index;
		inventory->onInventoryChanged();
		return index;
	}

}
