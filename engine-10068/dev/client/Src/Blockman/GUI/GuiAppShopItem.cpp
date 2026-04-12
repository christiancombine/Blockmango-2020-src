#include "GuiAppShopItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"
#include "cItem/cItem.h"
#include "cBlock/cBlockManager.h"
#include "cBlock/cBlock.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Network/ClientNetwork.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Inventory/InventoryPlayer.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"
#include "game.h"
#include "ShellInterface.h"
#include "RootGuiLayout.h"
#include "gui_tipDialog.h"
#include "Util/PlayerPrefs.h"
#include "UI/GUILayout.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GuiAppShopItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GuiAppShopItem::GuiAppShopItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("AppShopItem.json"));
		renameComponents(m_window);
		m_itemName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));
		m_quota = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(1));
		m_quotaValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_itemIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_btnBuy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_currencyIcon = dynamic_cast<GUIStaticImage*>(m_btnBuy->GetChildByIndex(0));
		m_itemNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(4));
		m_btnVideo = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_btnBuy->subscribeEvent(EventButtonClick, std::bind(&GuiAppShopItem::onButtonClick, this, std::placeholders::_1));
		m_btnVideo->subscribeEvent(EventButtonClick, std::bind(&GuiAppShopItem::onVideoAdClick, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiAppShopItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);	
		if (m_goods.limit == -1)
		{
			m_quota->SetVisible(false);
		}
		else
		{
			m_quota->SetVisible(true);
			String num = StringUtil::Format(LanguageManager::Instance()->getString(LanguageKey::GUI_STR_APP_SHOP_LIMIT).c_str(), m_goods.limit);
			m_quotaValue->SetText(num.c_str());
		}
	}

	void GuiAppShopItem::updateIcon(int itemId, int damage, String icon)
	{
		if (itemId == m_oldItemId && damage == m_oldItemDamage)
		{
			return;
		}
		m_oldItemId = itemId;
		m_oldItemDamage = damage;

		if(m_itemIcon)
			GuiItemStack::showItemIcon(m_itemIcon, itemId, damage, icon);
	}

	void GuiAppShopItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GuiAppShopItem::getPrivateChildren()
	{
		return { m_window };
	}

	void GuiAppShopItem::setGoods(Goods goods, int groupIndex)
	{
		m_goods = goods;
		m_groupIndex = groupIndex;
		String name = LanguageManager::Instance()->getString(m_goods.desc);
		m_itemName->SetText(name.c_str());
		m_itemNum->SetText(StringUtil::ToString(m_goods.itemNum).c_str());
		m_btnBuy->SetText(StringUtil::ToString(m_goods.blockymodsPrice).c_str());
		m_currencyIcon->SetImage(UICommon::getCurrencyIconByType(m_goods.coinId).c_str());
		updateIcon(m_goods.itemId, m_goods.itemMeta, m_goods.icon);
	}

	void GuiAppShopItem::setVideoAdInfo(int adType, bool visible)
	{
		m_haveAd = visible;
		m_videoAdType = adType;
		m_btnVideo->SetVisible(m_haveAd && m_canShowAd);
	}

	void GuiAppShopItem::setCanShowAd(bool canShowAd)
	{
		m_canShowAd = canShowAd;
		m_btnVideo->SetVisible(m_haveAd && m_canShowAd);
	}

	bool GuiAppShopItem::onButtonClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_Click);

		EventArgs args;
		Item* pItem = Item::getItemById(m_goods.itemId);
		if (pItem)
		{
			if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(m_goods.itemId, m_goods.itemMeta, m_goods.itemNum))
			{
				args.handled = (int)BuyResult::FullInventory;
				fireEvent("LackOfMoney", args);
				return false;
			}
		}

		if (!m_goods.canBuy())
		{
			args.handled = (int)BuyResult::LackingGood;
			fireEvent("LackOfMoney", args);
			return false;
		}

		if (!Blockman::Instance()->m_pPlayer->canAddBulletItem(m_goods.itemId))
		{
			args.handled = (int)BuyResult::HasNoGun;
			fireEvent("LackOfMoney", args);
			return false;
		}

		if (m_goods.coinId <= 3)
		{
			UICommon::checkMoney(m_goods.blockymodsPrice, m_goods.coinId, [=](bool isSuccess) {
				if (isSuccess)
				{
					ClientNetwork::Instance()->getSender()->sendBuyGoods(m_groupIndex, m_goods.index, m_goods.uniqueId);
				}
				else
				{
					EventArgs args;
					args.handled = (int)BuyResult::LackOfMoney;
					fireEvent("LackOfMoney", args);
				}
			});
			return true;
		}
		ClientNetwork::Instance()->getSender()->sendBuyGoods(m_groupIndex, m_goods.index, m_goods.uniqueId);
		return true;
	}

	bool GuiAppShopItem::onVideoAdClick(const EventArgs & events)
	{
		if (m_videoAdType == 0)
			return false;
		String params = CommonDataBuilder()
			.addIntParam("ItemId", getGoodsItemId())
			.getData();
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd(m_videoAdType, params);
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("shop_click_AD", GameClient::CGame::Instance()->getGameType());
		return true;
	}
}