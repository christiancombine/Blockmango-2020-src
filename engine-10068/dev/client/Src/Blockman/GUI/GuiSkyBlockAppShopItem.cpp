#include "GuiSkyBlockAppShopItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageKey.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "game.h"
#include "Util/ClientEvents.h"
#include "Block/BlockUtil.h"
#include "GUI/GuiItemStack.h"
#include "GameAnalytics/GameAnalyticsSetting.h"
#include "Item/Item.h"

using namespace LORD;
namespace BLOCKMAN
{
	void GuiSkyBlockAppShopItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	GuiSkyBlockAppShopItem::GuiSkyBlockAppShopItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("SkyBlockAppShopItem.json"));
		renameComponents(m_window);
		m_item_icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_have_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_have_text = dynamic_cast<GUIStaticText*>(m_have_img->GetChildByIndex(0));
		m_select_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_lock_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_hot_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(4));
		m_hot_text = dynamic_cast<GUIStaticText*>(m_hot_img->GetChildByIndex(0));
		m_select_img->SetVisible(false);
		m_have_img->SetVisible(false);
		m_hot_img->SetVisible(false);
		m_have_text->SetText(getString(LanguageKey::GUI_SHOP_IS_HAVE));
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiSkyBlockAppShopItem::onButtonClick, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiSkyBlockAppShopItem::UpdateSelf(ui32 nTimeElapse)
	{

		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);

		int cur_blockId = m_goods.itemId;
		if (m_goods.goodType == SKYBLOCK_BLOCK)
		{
			if (BlockUtil::isValidBlockId(cur_blockId))
			{
				Block* pBlock = BlockManager::sBlocks[cur_blockId];
				if (pBlock != NULL)
				{
					if (!BlockUtil::isBasicBlock(cur_blockId))
					{
						cur_blockId = cur_blockId + Item::EC_BLOCK_TO_ITEM_OFFSET;
					}
				}
			}
		}
	
		GuiItemStack::showItemIcon(m_item_icon, cur_blockId, m_goods.meta, m_goods.icon);

		m_lock_img->SetVisible(m_goods.status == SKYBLOCK_GOOD_LOCK);
	}

	void GuiSkyBlockAppShopItem::Destroy()
	{
		m_subscriptionGuard.unsubscribeAll();
		removeComponents();
		BaseClass::Destroy();
	}

	vector<GUIWindow*>::type GuiSkyBlockAppShopItem::getPrivateChildren()
	{
		return { m_window };
	}

	bool GuiSkyBlockAppShopItem::onButtonClick(const EventArgs & events)
	{
		SkyBlockShopClickItemEvent::emit(m_goods, m_index);
		if (m_goods.goodType == SKYBLOCK_PRIVILIGE)
		{
			auto wallet = Blockman::Instance()->m_pPlayer->getWallet();
			if (GameClient::CGame::Instance()->isEnableIndie())
				GameAnalyticsSetting::Instance()->design(0, { "g1048click_privilege_indie",
					StringUtil::ToString(m_goods.id), StringUtil::ToString(wallet->getDiamondsGolds()), "1" });
			else
				GameAnalyticsSetting::Instance()->design(0, { "g1048click_privilege",
					StringUtil::ToString(m_goods.id), StringUtil::ToString(wallet->getDiamondsGolds()), "1" });
		}
		else
		{
			if (GameClient::CGame::Instance()->isEnableIndie())
				GameAnalyticsSetting::Instance()->design(0, { "g1048click_item_indie", StringUtil::ToString(m_goods.id) });
			else
				GameAnalyticsSetting::Instance()->design(0, { "g1048click_item", StringUtil::ToString(m_goods.id) });
		}

		return true;
	}

	void GuiSkyBlockAppShopItem::setGoods(SkyBlockGoods goods, int groupIndex)
	{

		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });

		m_goods = goods;
		m_groupIndex = groupIndex;

		int cur_blockId = m_goods.itemId;
		if (m_goods.goodType == SKYBLOCK_BLOCK)
		{
			if (BlockUtil::isValidBlockId(cur_blockId))
			{
				Block* pBlock = BlockManager::sBlocks[cur_blockId];
				if (pBlock != NULL)
				{
					if (!BlockUtil::isBasicBlock(cur_blockId))
					{
						cur_blockId = cur_blockId + Item::EC_BLOCK_TO_ITEM_OFFSET;
					}
				}
			}
		}

		GuiItemStack::showItemIcon(m_item_icon, m_goods.itemId, m_goods.meta, m_goods.icon);

		m_lock_img->SetVisible(m_goods.status == SKYBLOCK_GOOD_LOCK);

		m_hot_img->SetVisible(m_goods.hot == 1);
		m_have_img->SetVisible(m_goods.isHave);

		if (m_goods.hot == 1)
		{
			m_hot_img->SetVisible(true);
			m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
			m_hot_text->SetText(getString(LanguageKey::GUI_SHOP_HOT));
		}

		m_have_img->SetVisible(m_goods.isHave);

		if (m_goods.goodType == SKYBLOCK_PRIVILIGE || m_goods.goodType == SKYBLOCK_NPC_UNLOCK)
		{
			m_hot_img->SetVisible(true);
			m_hot_img->SetImage("set:skyblock_shop.json image:hot_greey");
			if (m_goods.goodType == SKYBLOCK_PRIVILIGE)
			{
				m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
			}

			if (m_goods.forever)
			{
				m_hot_text->SetText(getString(LanguageKey::GUI_SHOP_FOREVER));
			}
			else
			{
				if (m_goods.time > 0)
				{
					int  hour = (m_goods.time % 86400) / 3600;
					int  day = m_goods.time / 86400;

					if (day > 0)
					{
						m_hot_text->SetText(StringUtil::Format(getString(LanguageKey::GUI_SHOP_DAY).c_str(), day).c_str());
					}
				}
				else
				{
					m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
				}
			}
		}

		if (m_goods.goodType == SKYBLOCK_PRIVILIGE && m_goods.disCountTips.length() > 2)
		{
			m_hot_img->SetImage("set:skyblock_shop.json image:hot_red");
			m_hot_img->SetVisible(true);
			m_hot_text->SetText(getString(m_goods.disCountTips));
		}
	}
	
	void GuiSkyBlockAppShopItem::setIndex(int index)
	{
		m_index = index;
	}
}