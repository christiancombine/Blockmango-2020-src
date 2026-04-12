#include "GuiBlockCityTigerLotteryItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/UICommon.h"
#include "GUI/GuiItemStack.h"
#include "Util/ClientEvents.h"
#include "Setting/BlockCitySetting.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityTigerLotteryItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityTigerLotteryItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityTigerLotteryItem::GuiBlockCityTigerLotteryItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityTigerLotteryItem.json"));
		renameComponents(m_window);

		m_image = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_lock = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		m_text = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_new = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_hot = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(4));
		m_clock = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));

		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBlockCityTigerLotteryItem::onClick, this, std::placeholders::_1));

		AddChildWindow(m_window);
	}

	void GuiBlockCityTigerLotteryItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityTigerLotteryItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCityTigerLotteryItem::setLotteryItem(const int& itemId)
	{
		m_window->SetBackImage("set:blockcity_tiger_lottery.json image:image_back");
		m_image->SetRenderAsBlock(false);
		m_image->SetImage(BlockCitySetting::getItemIcon(itemId).c_str());
		m_lock->SetVisible(false);
		m_text->SetVisible(false);
		m_new->SetVisible(false);
		m_hot->SetVisible(false);
		m_clock->SetVisible(false);
	}

	void GuiBlockCityTigerLotteryItem::setBlockLotteryItem(const int& itemId, const int& itemIdMate)
	{
		m_window->SetBackImage("set:blockcity_tiger_lottery.json image:image_back");
		m_lock->SetVisible(false);
		m_text->SetVisible(false);
		m_new->SetVisible(false);
		m_hot->SetVisible(false);
		m_clock->SetVisible(false);
		GuiItemStack::showItemIcon(m_image, itemId, itemIdMate);
	}

	void GuiBlockCityTigerLotteryItem::setStoreItem(const BlockCityStoreItem& item)
	{
		m_storeItem = item;
		m_text->SetVisible(true);
		m_text->SetTextHorzAlign(HA_RIGHT);
		m_lock->SetVisible(item.limit != 0);
		m_new->SetVisible(item.isNew == 1);
		m_hot->SetVisible(item.isHot == 1);
		m_clock->SetVisible(false);
		if (item.limit == 0)
		{
			m_window->SetBackImage("set:blockcity_store.json image:item_have_back");
			m_text->SetText(item.num == 0 ?  getString(LanguageKey::GUI_BLOCKCITY_STORE_NOHAVE) : UICommon::numberToString(item.num).c_str());
		}
		else
		{
			m_window->SetBackImage("set:blockcity_store.json image:item_lock_back");
			m_text->SetText(StringUtil::Format("Lv %d", item.limit).c_str());
		}
		if (item.itemMate >= 0)
		{
			float x = GetPixelSize().x > 100.f ? 100.f : GetPixelSize().x;
			m_image->SetSize(UVector2(UDim(0, x), UDim(0, x)));
			GuiItemStack::showItemIcon(m_image, item.itemId, item.itemMate);
		}
		else
		{
			m_image->SetImage(BlockCitySetting::getItemIcon(item.id).c_str());
			m_image->SetSize(UVector2(UDim(1.f, 0), UDim(1.f, 0)));
		}
	}

	void GuiBlockCityTigerLotteryItem::setStoreMaterialItem(const BlockCityMaterial& item)
	{
		m_window->SetBackImage("set:blockcity_store.json image:material_item_back");
		m_lock->SetVisible(false);
		m_text->SetVisible(true);
		m_new->SetVisible(false);
		m_hot->SetVisible(false);
		m_clock->SetVisible(false);
		m_text->SetTextHorzAlign(HA_CENTRE);
		m_text->SetText(item.haveNum >= item.needNum ? StringUtil::Format("▢FF00FF00%d▢FF000000/%d", item.haveNum, item.needNum).c_str() : StringUtil::Format("▢FFFF0000%d▢FF000000/%d", item.haveNum, item.needNum).c_str());
		if (item.itemMate >= 0)
		{
			m_image->SetSize(UVector2(UDim(0, 70.f), UDim(0, 70.f)));
			GuiItemStack::showItemIcon(m_image, item.itemId, item.itemMate);
		}
		else
		{
			m_image->SetImage(BlockCitySetting::getItemIcon(item.id).c_str());
			m_image->SetSize(UVector2(UDim(1.f, 0), UDim(1.f, 0)));
		}
		m_window->SetTouchable(false);
	}

	void GuiBlockCityTigerLotteryItem::setLackItem(const BlockCityLackItem& item)
	{
		m_window->SetBackImage("");
		m_lock->SetVisible(false);
		m_text->SetVisible(true);
		m_new->SetVisible(false);
		m_hot->SetVisible(false);
		m_clock->SetVisible(false);
		m_text->SetTextHorzAlign(HA_CENTRE);
		m_text->SetText(StringUtil::Format("▢FFFF0000%d▢FF000000/%d", item.haveNum, item.needNum).c_str());
		if (item.itemMeta >= 0)
		{
			m_image->SetSize(UVector2(UDim(0, 65.f), UDim(0, 65.f)));
			GuiItemStack::showItemIcon(m_image, item.itemId, item.itemMeta);
		}
		else
		{
			m_image->SetImage(BlockCitySetting::getItemIcon(item.id).c_str());
			m_image->SetSize(UVector2(UDim(1.f, 0), UDim(1.f, 0)));
		}
		m_window->SetTouchable(false);
	}

	void GuiBlockCityTigerLotteryItem::setPackItem(const BlockCityHouseShopItem& item)
	{
		m_packItem = item;
		m_lock->SetVisible(false);
		m_new->SetVisible(false);
		m_hot->SetVisible(false);
		m_text->SetText(StringUtil::ToString(item.haveNum).c_str());
		m_text->SetVisible(item.limit == 0);
		m_window->SetBackImage("set:blockcity_pack.json image:item_have_back");
		m_image->SetImage( BlockCitySetting::getItemIcon(item.itemId).c_str());
		m_clock->SetVisible(item.limitTime > 0);
	}

	bool GuiBlockCityTigerLotteryItem::onClick(const EventArgs& args)
	{
		switch (m_showType)
		{
		case 1:
			break;
		case 2:
			if (m_storeItem.id <= 0)
			{
				return false;
			}
			SoundSystem::Instance()->playEffectByType(ST_Click);
			BlockCityStoreItemClickEvent::emit(m_storeItem);
			break;
		case 3:
			if (m_packItem.itemId <= 0)
			{
				return false;
			}
			SoundSystem::Instance()->playEffectByType(ST_Click);
			BlockCityPackItemClickEvent::emit(m_packItem);
			break;
		default:
			break;
		}
		return true;
	}

	void GuiBlockCityTigerLotteryItem::setSelectId(i32 id)
	{
		switch (m_showType)
		{
		case 1:
			break;
		case 2:
		{
			m_isSelect = (id > 0 && id == m_storeItem.id);

			if (m_storeItem.limit > 0)
			{
				m_window->SetBackImage(m_isSelect ? "set:blockcity_store.json image:item_choose" : "set:blockcity_store.json image:item_lock_back");
			}
			else
			{
				m_window->SetBackImage(m_isSelect ? "set:blockcity_store.json image:item_choose" : "set:blockcity_store.json image:item_have_back");
			}
		}
			break;
		case 3:
		{
			m_isSelect = (id > 0 && id == m_packItem.itemId);
			m_window->SetBackImage(m_isSelect ? "set:blockcity_pack.json image:item_choose_back" : "set:blockcity_pack.json image:item_have_back");
		}
		default:
			break;
		}
		
		
	}

}