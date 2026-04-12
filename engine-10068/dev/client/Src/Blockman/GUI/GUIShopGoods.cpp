#include "GUIShopGoods.h"
#include "Render/TextureAtlas.h"
#include "Render/RenderBlocks.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "cItem/cItem.h"
#include "cBlock/cBlockManager.h"
#include "cBlock/cBlock.h"
#include "cEntity/EntityPlayerSP.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "ShellInterface.h"
#include "Inventory/CoinManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "Inventory/InventoryPlayer.h"
#include "Setting/GameTypeSetting.h"
#include "Network/protocol/CommonDataBuilder.h"

using namespace LORD;

namespace BLOCKMAN {

	void GUIShopGoods::removeComponents()
	{

		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GUIShopGoods::GUIShopGoods(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ShopGoogsItem.json"));
		renameComponents(m_window);
		m_goodsIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_goodsDetails = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_buy = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2));
		m_goodsNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));
		m_currencyIcon = dynamic_cast<GUIStaticImage*>(m_buy->GetChildByIndex(0));
		m_goodsPrice = dynamic_cast<GUIStaticText*>(m_buy->GetChildByIndex(1));
		m_tipBtn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_watchAd = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(5));
		m_buy->subscribeEvent(EventButtonClick, std::bind(&GUIShopGoods::onButtonClick, this, std::placeholders::_1));
		m_tipBtn->subscribeEvent(EventButtonClick, std::bind(&GUIShopGoods::onButtonItemTipClick, this, std::placeholders::_1));
		m_watchAd->subscribeEvent(EventButtonClick, std::bind(&GUIShopGoods::onWatchAdClick, this, std::placeholders::_1));
		m_buy->SetText("");
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	vector<GUIWindow*>::type GUIShopGoods::getPrivateChildren()
	{
		return { m_window };
	}

	void GUIShopGoods::updateData()
	{
		if (m_oldUniqueId == m_commodity.uniqueId)
			return;
		m_oldUniqueId = m_commodity.uniqueId;

		int itemId = CoinManager::Instance()->coinIdToItemId(m_commodity.coinId);
		int price = m_commodity.price;

		m_goodsNum->SetText(StringUtil::ToString(m_commodity.itemNum).c_str());

		if (price > 0)
		{
			m_goodsPrice->SetText(StringUtil::ToString(price).c_str());
			int digits = 0;
			while (price > 10)
			{
				digits++;
				price /= 10;
			}
			int left = 7 - digits > 0 ? 2 + (7 - digits) * 5 : 2;
			left = left > 30 ? 30 : left;
			m_currencyIcon->SetXPosition(UDim(0, (float)left));
			m_goodsPrice->SetXPosition(UDim(0, 25.0f + (7 - digits) + left));
		}
		else
		{
			m_currencyIcon->SetVisible(false);
			m_goodsPrice->SetVisible(false);
			if (price < 0)
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::GUI_SHOP_TAB_SELL).c_str());
		}
		
		String desc = LanguageManager::Instance()->getString(m_commodity.desc);
		if (desc.length() > 0)
		{
			m_goodsDetails->SetText(desc.c_str());
		}
		else
		{
			m_goodsDetails->SetText(m_commodity.desc.c_str());
		}
		m_tipBtn->SetTouchable(m_commodity.tipDesc.length() > 0);
		updateIcon(m_commodity.itemId, m_commodity.itemMeta, m_commodity.image, itemId);
	}

	void GUIShopGoods::updateIcon(int itemId, int damage, String image, int coinItemId)
	{
		if (itemId == m_oldItemId && damage == m_oldItemDamage)
		{
			return;
		}

		GuiItemStack::showItemIcon(m_currencyIcon, coinItemId, 1);

		m_oldItemId = itemId;
		m_oldItemDamage = damage;
		GuiItemStack::showItemIcon(m_goodsIcon, itemId, damage, image);
	}

	void GUIShopGoods::updateVideoAd()
	{
		bool showAd = m_commodity.videoAd && m_curHaveAd && m_canShowAd;
		m_watchAd->SetVisible(showAd);
	}

	bool GUIShopGoods::onButtonClick(const EventArgs & events)
	{
		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		if (!player) {
			return false;
		}

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War)
		{
			auto merchant = dynamic_cast<EntityMerchant*>(Blockman::Instance()->getWorld()->getEntity(m_merchantId));
			if (merchant)
			{
				if (!m_commodity.price || player->isUnlocked(m_commodity.itemId, m_commodity.itemMeta))
				{
					if (player->inventory->isContainItem(m_commodity.itemId, m_commodity.itemMeta))
					{
						BuyCommodityResultEvent::emit(LanguageManager::Instance()->getString(LanguageKey::GUI_HAVE_ALREADY_GOT_BLOCK).c_str());
						return true;
					}
					GameClient::CGame::Instance()->getNetwork()->getSender()->sendGetCommodity(m_merchantId, m_groupIndex, m_commodity.index, m_commodity.uniqueId);
					return true;
				}
				else
				{
					showUnlockCommodityTipDialogEvent::emit(m_merchantId, m_groupIndex, m_commodity.index, m_commodity.uniqueId, m_commodity.price, m_commodity.desc);
					return true;
				}
			}
		}

		BuyResult result = BuyResult::BuySuccess;
		if (player->canBuy(m_commodity, result))
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuyCommodity(m_merchantId, m_groupIndex, m_commodity.index, m_commodity.uniqueId);
		}
		else
		{
			EventArgs args;

			if (!player->inventory->isCanAddItem(m_commodity.itemId, m_commodity.itemMeta, m_commodity.itemNum))
			{
				args.handled = (int)BuyResult::FullInventory;
				fireEvent("LackOfMoney", args);
				return false;
			}

			if (m_commodity.notify == 1 && !player->capabilities.isWatchMode)
			{
				args.handled = (int)BuyResult::LackOfMoneyGoAppShop;
				fireEvent("LackOfMoney", args);
			}
			else
			{
				args.handled = (int)result;
				fireEvent("LackOfMoney", args);
			}
		}
		return true;
	}

	bool GUIShopGoods::onButtonItemTipClick(const EventArgs & events)
	{
		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		String Name = LanguageManager::Instance()->getString(m_commodity.desc);

		if (!player) {
			return false;
		}
		if (m_commodity.tipDesc.length() > 0)
		{
			int itemId = CoinManager::Instance()->coinIdToItemId(m_commodity.coinId);
			int price = m_commodity.price;

			ShowShopItemTipEvent::emit(m_commodity.itemId, m_commodity.itemMeta, m_commodity.image, itemId, m_commodity.tipDesc, Name, price);
		}
		return true;
	}

	bool GUIShopGoods::onWatchAdClick(const EventArgs & events)
	{
		if (m_videoAdType == 0)
			return false;
		String params = CommonDataBuilder()
			.addIntParam("ItemId", m_commodity.itemId)
			.getData();
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd(m_videoAdType, params);
		GameClient::CGame::Instance()->getShellInterface()->
			dataReport("shop_click_AD", GameClient::CGame::Instance()->getGameType());
		return true;
	}

	void GUIShopGoods::updateUnlockedState()
	{
		if (m_commodity.price && Blockman::Instance()->m_pPlayer->isUnlocked(m_commodity.itemId, m_commodity.itemMeta))
		{
			m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_SELECT).c_str());
			m_buy->SetTextColor(m_color);
		}
	}

	void GUIShopGoods::setVideoAdInfo(int adType, bool visible)
	{
		m_videoAdType = adType;
		m_curHaveAd = visible;
		updateVideoAd();
	}

	void GUIShopGoods::setCanShowAd(bool canShowAd)
	{
		m_canShowAd = canShowAd;
		updateVideoAd();
	}

	void GUIShopGoods::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War)
		{
			auto merchant = dynamic_cast<EntityMerchant*>(Blockman::Instance()->getWorld()->getEntity(m_merchantId));
			if (m_commodity.price && merchant && !Blockman::Instance()->m_pPlayer->isUnlocked(m_commodity.itemId, m_commodity.itemMeta))
			{
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_UNLOCK).c_str());
				m_buy->SetTextColor(Color(1.0f, 0.0f, 0.0f));
			}
			else
			{
				m_buy->SetText(LanguageManager::Instance()->getString(LanguageKey::SHOP_BTN_SELECT).c_str());
			}
		}
	}

	void GUIShopGoods::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIShopGoods::setCommodity(Commodity commodity, int merchantId, int groupIndex)
	{
		this->m_commodity = commodity;
		this->m_merchantId = merchantId;
		this->m_groupIndex = groupIndex;
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War)
		{
			m_color = m_buy->GetTextColor();
			m_currencyIcon->SetVisible(false);
			m_goodsPrice->SetVisible(false);
			m_goodsNum->SetVisible(false);
			m_buy->SetRenderOffset(Vector2(0, 0));
		}
		updateData();
		updateVideoAd();
	}
}