#include "GuiItemSlot.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/PlayerPrefs.h"
#include "Audio/SoundSystem.h"

using namespace LORD;
namespace BLOCKMAN
{

	void GuiItemSlot::removeComponents()
	{
		if (m_window)
		{
			RemoveWindowFromDrawList(*m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
		m_guiItemStack = nullptr;
		m_dropItemProgress = nullptr;
		m_armorTypeImage = nullptr;
		m_skillCdProgress = nullptr;
		m_durationProgress = nullptr;
	}

	vector<GUIWindow*>::type GuiItemSlot::getPrivateChildren()
	{
		return { m_window };
	}

	GuiItemSlot::GuiItemSlot(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUIStaticImage*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ItemSlot.json"));
		renameComponents(m_window);
		m_dropItemProgress = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(0));
		m_enchantmentIndicator = m_window->GetChildByIndex(1);
		m_armorTypeImage = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(2));
		m_guiItemStack = dynamic_cast<GuiItemStack*>(m_window->GetChildByIndex(3));
		m_skillCdProgress = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(4));
		m_guiItemStack->setGunNum(dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)));
		m_durationProgress = dynamic_cast<GUIProgressBar*>(m_window->GetChildByIndex(6));
		m_window->subscribeEvent(EventWindowTouchDown, SubscriberSlot(&GuiItemSlot::onItemStackTouchDown, this));
		m_window->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&GuiItemSlot::onItemStackTouchUp, this));
		m_window->subscribeEvent(EventWindowLongTouchStart, SubscriberSlot(&GuiItemSlot::onItemStackLongTouchStart, this));
		m_window->subscribeEvent(EventWindowLongTouchEnd, SubscriberSlot(&GuiItemSlot::onItemStackLongTouchEnd, this));
		m_window->subscribeEvent(EventMotionRelease, SubscriberSlot(&GuiItemSlot::onItemStackLongTouchEnd, this));
		auto funcion = [](const EventArgs & events)->bool {return true;};
		m_window->subscribeEvent(EventWindowClick, SubscriberSlot(&GuiItemSlot::onItemStackClick, this));
		m_window->setEnableLongTouch(true);
		m_skillCdProgress->SetVisible(false);
		m_durationProgress->SetVisible(false);
		setSelected(false);
		AddWindowToDrawList(*m_window);
		subscribeToBubbleUpMouseEvents();
	}

	bool GuiItemSlot::onItemStackClick(const EventArgs & events)
	{
		SoundSystem::Instance()->playEffectByType(ST_select_item);
		return true;
	}

	void GuiItemSlot::updateStyle()
	{
		if (!is_modified_style) 
			return;
		m_window->SetProperty(GWP_WINDOW_STRETCH_TYPE, "NineGrid");
		switch (m_style)
		{
		case BLOCKMAN::GuiItemSlot::Style::ROCK_TRANS:
			m_window->SetImage(m_selected ? "set:new_gui_material.json image:wupinkuang_xuanzhong" : "set:new_gui_material.json image:wupinkuang");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "7 10 7 10");
			m_guiItemStack->SetArea({{ 0, 7},{ 0, 7}},{{ 1, -17},{ 1, -17}});
			m_guiItemStack->SetVerticalAlignment(VA_TOP);
			m_guiItemStack->SetHorizontalAlignment(HA_LEFT);
			break;
		case BLOCKMAN::GuiItemSlot::Style::ROCK_SOLID:
			m_window->SetImage(m_selected ? "set:new_gui_material.json image:wupinkuang_shixin_xuanzhong" : "set:new_gui_material.json image:wupinkuang_shixin");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ?   "7 8 7 8" : "6 6 6 6" );
			break;
		case BLOCKMAN::GuiItemSlot::Style::METAL:
			m_window->SetImage(m_selected ? "set:new_gui_material.json image:wupinkuang_shixin_xuanzhong" : "set:new_gui_material.json image:wupinkuang_jinshu");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;
		case BLOCKMAN::GuiItemSlot::Style::BLOCKCITY:
			m_window->SetImage(m_selected ? "set:blockcity_store.json image:shortcut_choose" : "set:blockcity_store.json image:shortcut_pack");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;
		case BLOCKMAN::GuiItemSlot::Style::SKYBLOCK:
			m_window->SetImage(m_selected ? "set:skyblock_main.json image:skyblock_grid_press" : "set:skyblock_main.json image:skyblock_grid_bg");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;
		case BLOCKMAN::GuiItemSlot::Style::SKYBLOCK2:
			m_window->SetImage(m_selected ? "set:skyblockbag.json image:slot_select" : "set:skyblockbag.json image:slot_normal");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;
        case BLOCKMAN::GuiItemSlot::Style::WALKING_DEAD:
            if (!m_selected)
            {
                m_window->SetImage("set:walkingdead_block.json image:bg_item");
            }
            else
            {
                m_window->SetImage("set:walkingdead_block.json image:bg_item_selected");
            }
            m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "5 5 5 5" : "4 4 4 4");
			break;
		case BLOCKMAN::GuiItemSlot::Style::BLOCK_FORT:
			m_window->SetImage(m_selected ? "set:blockfort_main.json image:shortcut_choose" : "set:blockfort_main.json image:shortcut_pack");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;		
		case BLOCKMAN::GuiItemSlot::Style::BLOCK_FORT_PACK:
			m_window->SetImage(m_selected ? "set:blockfort_store.json image:item_pre" : "set:blockfort_store.json image:item_nor");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;	
		case BLOCKMAN::GuiItemSlot::Style::WALKING_DEAD_ARMOR:
			if (!m_selected)
			{
				m_window->SetImage("set:walkingdead_block.json image:bg_armor_item");
			}
			else
			{
				m_window->SetImage("set:walkingdead_block.json image:bg_armor_item");
			}
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "10 15 15 10" : "10 15 15 10");
			break;
		case BLOCKMAN::GuiItemSlot::Style::SKY_WAR_LUCKY_BLOCK1:
			m_window->SetImage(m_selected ? "set:lucky_sky_backpack.json image:prop_sele" : "set:lucky_sky_backpack.json image:prop_item");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "7 8 7 8" : "6 6 6 6");
			break;
		case BLOCKMAN::GuiItemSlot::Style::SKY_WAR_LUCKY_BLOCK2:
			m_window->SetImage(m_selected ? "set:lucky_sky_backpack.json image:equip_has" : "set:lucky_sky_backpack.json image:equip_nor");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ? "20 20 20 20" : "15 15 15 15");
			break;
		case BLOCKMAN::GuiItemSlot::Style::SKY_WAR_LUCKY_BLOCK3:
			m_guiItemStack->SetSize({ {0.6f, 0.f}, {0.6f, 0.f} });
			m_window->SetImage("set:lucky_sky_backpack.json image:prop_info_frame");
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "30 30 30 30");
			break;
		default:
		{
			StringArray names = {"NorImage", "PreImage", "NorStretchOffset", "PreStretchOffset"};
			map<String, String>::type values;
			for (String name : names)
			{
				String key = StringUtil::Format("SlotItem-%s-%d", name.c_str(), (int)m_style);
				values[name] = PlayerPrefs::Instance()->getStringPrefs(key);
			}
			m_window->SetImage(m_selected ? values["PreImage"].c_str() : values["NorImage"].c_str());
			m_window->SetProperty(GWP_WINDOW_STRETCH_OFFSET, m_selected ?
				values["PreStretchOffset"].c_str() : values["NorStretchOffset"].c_str());	
		}
			break;
		}

		is_modified_style = false;
	}

	void GuiItemSlot::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		m_window->SetPosition(getAbsolutePositionFromRoot());
		updateStyle();
		NBTTagList* tagList = m_guiItemStack->getItemStack() ? m_guiItemStack->getItemStack()->getEnchantmentTagList() : nullptr;
		m_enchantmentIndicator->SetVisible(tagList && tagList->tagCount() > 0);
		m_armorTypeImage->SetVisible(m_armor_type != ArmorType::NONE && !m_guiItemStack->getItemStack());
		updateItemDurationInfo(nTimeElapse);
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiItemSlot::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiItemSlot::updateItemDurationInfo(ui32 timeElapse)
	{
		ItemStackPtr itemStack = m_guiItemStack->getItemStack();
		if (itemStack && itemStack->getMaxDamage() > 0)// && ItemStack->isItemSkill())
		{
			setItemDurationProgress(itemStack->itemDamage, itemStack->getMaxDamage());
		}
		else
		{
			setItemDurationProgress(0, 0);
		}
	}

	GuiItemStack * GuiItemSlot::cloneGuiItemStack(const GUIString & newName)
	{
		auto newItemStack = GUIWindowManager::Instance()->CreateGUIWindow(GWT_ITEM_STACK, newName);
		newItemStack->Clone(newName, m_guiItemStack);
		return static_cast<GuiItemStack*>(newItemStack);
	}

	void GuiItemSlot::setArmorType(ArmorType type)
	{
		m_armor_type = type;
		switch (m_armor_type)
		{
		case ArmorType::HELMET:
			m_armorTypeImage->SetImage("set:armor_type.json image:armor_type_1");
			break;
		case ArmorType::CHESTPLATE:
			m_armorTypeImage->SetImage("set:armor_type.json image:armor_type_2");
			break;
		case ArmorType::LEGGINGS:
			m_armorTypeImage->SetImage("set:armor_type.json image:armor_type_3");
			break;
		case ArmorType::BOOTS:
			m_armorTypeImage->SetImage("set:armor_type.json image:armor_type_4");
			break;
		default:
			break;
		}
	}

	void GuiItemSlot::setDropItemProgress(float progress)
	{
		if (m_dropItemProgress)
		{
			m_dropItemProgress->SetProgress(progress);
		}
	}

	void GuiItemSlot::setItemSkillCdProgress(int curValue, int maxValue)
	{
		if (curValue == 0 || maxValue == 0 || curValue == maxValue)
		{
			m_skillCdProgress->SetVisible(false);
			return;
		}
		m_skillCdProgress->SetVisible(true);
		if (m_skillCdProgress)
		{
			m_skillCdProgress->SetProgress((float)curValue/(float)maxValue);
		}
	}

	void GuiItemSlot::setItemDurationProgress(int curValue, int maxValue)
	{
		bool isShowDuration = PlayerPrefs::Instance()->getBoolPrefs("IsShowItemDurability");
		if (!isShowDuration)
		{
			return;
		}
		int curDuration = maxValue - curValue;
		int maxDuration = maxValue;
		if (maxDuration == 0 || curDuration <= 0 || curDuration >= maxDuration)
		{
			m_durationProgress->SetVisible(false);
			return;
		}
		float pross = (float)curDuration / (float)maxDuration;
		if (pross >= 0.5f)
		{
			m_durationProgress->SetProgressImage("set:item_duration_progress.json image:itemduration_grid_green");
		}
		else if (pross >= 0.2f && pross < 0.5f)
		{
			m_durationProgress->SetProgressImage("set:item_duration_progress.json image:itemduration_grid_yellow");
		}
		else
		{
			m_durationProgress->SetProgressImage("set:item_duration_progress.json image:itemduration_grid_red");
		}
		m_durationProgress->SetVisible(true);
		if (m_durationProgress)
		{
			m_durationProgress->SetProgress(pross);
		}
	}
}
