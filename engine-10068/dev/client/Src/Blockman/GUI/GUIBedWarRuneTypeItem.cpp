#include "GUIBedWarRuneTypeItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Setting/RuneSetting.h"

namespace BLOCKMAN
{

	void GuiBedWarRuneTypeItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiBedWarRuneTypeItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBedWarRuneTypeItem::GuiBedWarRuneTypeItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = GUIWindowManager::Instance()->LoadWindowFromJSON("BedWarRuneTypeItem.json");
		renameComponents(m_window);

		m_trough_bg = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_left_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(1));
		m_right_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(2));
		m_one_key_unload = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(3));
		m_one_key_assemble = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(4));
		m_type_tab_img = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(5));
		m_type_tab_txt = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(5)->GetChildByIndex(0));
		m_change_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(6));
		m_back_btn = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(7));

		m_left_btn->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarRuneTypeItem::onLeftClick, this, std::placeholders::_1));
		m_right_btn->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarRuneTypeItem::onRightClick, this, std::placeholders::_1));
		m_one_key_unload->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarRuneTypeItem::onOneKeyUnloadClick, this, std::placeholders::_1));
		m_one_key_assemble->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarRuneTypeItem::onOneKeyAssembleClick, this, std::placeholders::_1));
		m_change_btn->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarRuneTypeItem::onChangeBtnClick, this, std::placeholders::_1));
		m_back_btn->subscribeEvent(EventButtonClick, std::bind(&GuiBedWarRuneTypeItem::onBackBtnClick, this, std::placeholders::_1));

		m_one_key_unload->SetText(getString(LanguageKey::GUI_BED_WAR_ONE_KEY_UNLOAD_TXT));
		m_one_key_assemble->SetText(getString(LanguageKey::GUI_BED_WAR_ONE_KEY_ASSEMBLE_TXT));
		m_change_btn->SetText(getString(LanguageKey::GUI_BED_WAR_CHANGE_BTN_TXT));
		m_back_btn->SetText(getString(LanguageKey::GUI_BED_WAR_BACK_BTN_TXT));

		for (int i = 0; i < MAX_RUNE_TROUGH_NUM; ++i)
		{
			auto troughLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(8));
			m_trough_kuang[i] = dynamic_cast<GUIStaticImage*>(troughLayout->GetChildByIndex(i));
			m_trough_kuang[i]->subscribeEvent(EventWindowTouchDown, std::bind(&GuiBedWarRuneTypeItem::onRuneClick, this, std::placeholders::_1, i));
			m_trough_select[i] = dynamic_cast<GUIStaticImage*>(troughLayout->GetChildByIndex(i)->GetChildByIndex(1));
			m_trough_lock_img[i] = dynamic_cast<GUIStaticImage*>(troughLayout->GetChildByIndex(i)->GetChildByIndex(2));
			m_trough_lock_txt[i] = dynamic_cast<GUIStaticText*>(troughLayout->GetChildByIndex(i)->GetChildByIndex(2)->GetChildByIndex(0));
			m_trough_img[i] = dynamic_cast<GUIStaticImage*>(troughLayout->GetChildByIndex(i)->GetChildByIndex(3));
			m_trough_img[i]->subscribeEvent(EventWindowTouchDown, std::bind(&GuiBedWarRuneTypeItem::onRuneClick, this, std::placeholders::_1, i));
			m_trough_big_lock[i] = dynamic_cast<GUIStaticImage*>(troughLayout->GetChildByIndex(i)->GetChildByIndex(4));
			m_trough_effect_layout[i] = dynamic_cast<GUILayout*>(troughLayout->GetChildByIndex(i)->GetChildByIndex(0));
			m_trough_effect_layout[i]->SetVisible(false);
		}

		for (int i = 0; i < MAX_RUNE_TYPE; ++i)
		{
			auto tabCircleLayout = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(9));
			m_rune_type_circle[i] = dynamic_cast<GUIStaticImage*>(tabCircleLayout->GetChildByIndex(i));
		}

		AddChildWindow(m_window);
		subscribeToBubbleUpMouseEvents();
	}

	void GuiBedWarRuneTypeItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiBedWarRuneTypeItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBedWarRuneTypeItem::RefreshItem(std::vector<RuneTroughItem>& rune_trough_vec, int trough_type, int player_lv, int unlock_price, std::vector<int>& unlock_list)
	{
		m_rune_trough_vec.clear();
		m_trough_type = trough_type;
		m_player_lv = player_lv;
		m_unlock_price = unlock_price;
		m_change_btn->SetVisible(false);
		m_back_btn->SetVisible(false);
		for (int i = 0; i < MAX_RUNE_TROUGH_NUM; ++i)
		{
			m_trough_kuang[i]->SetVisible(false);
			m_trough_select[i]->SetVisible(false);
			m_trough_lock_img[i]->SetVisible(false);
			m_trough_big_lock[i]->SetVisible(false);
		}

		for (int i = 0; i < MAX_RUNE_TYPE; ++i)
		{
			if (i + 1 == trough_type)
			{
				m_rune_type_circle[i]->SetImage("set:rune.json image:yuan_select");
			}
			else
			{
				m_rune_type_circle[i]->SetImage("set:rune.json image:yuan_noselect");
			}
		}



		RuneTypeSettingItem* rune_type_item = RuneSetting::findRuneTypeItemByType(trough_type);
		if (rune_type_item)
		{
			if (rune_type_item->FlagImg.length() > 0)
			{
				m_type_tab_img->SetImage(rune_type_item->FlagImg.c_str());
				m_type_tab_img->SetVisible(true);
			}
			else
			{
				m_type_tab_img->SetVisible(false);
			}
			
			if (rune_type_item->BgImg.length() > 0)
			{
				m_trough_bg->SetImage(rune_type_item->BgImg.c_str());
				m_trough_bg->SetVisible(true);
			}
			else
			{
				m_trough_bg->SetVisible(false);
			}

			m_type_tab_txt->SetText(getString(rune_type_item->Title.c_str()).c_str());
		}

		for (int i = 0; i < (int)rune_trough_vec.size() && i < MAX_RUNE_TROUGH_NUM; ++i)
		{
			RuneTroughItem runeTroughItem;
			runeTroughItem.rune_trough_id = rune_trough_vec[i].rune_trough_id;
			runeTroughItem.rune_id = rune_trough_vec[i].rune_id;
			runeTroughItem.rune_lock = rune_trough_vec[i].rune_lock;
			runeTroughItem.need_show_effect = rune_trough_vec[i].need_show_effect;
			m_rune_trough_vec.push_back(runeTroughItem);

			RuneTroughSettingItem* rune_trough_item = RuneSetting::findRuneTroughItemById(rune_trough_vec[i].rune_trough_id);
			if (!rune_trough_item)
				continue;

			m_trough_kuang[i]->SetVisible(true);

			if (rune_trough_vec[i].rune_lock)
			{
				m_trough_lock_img[i]->SetVisible(true);
				
				m_trough_lock_txt[i]->SetVisible(true);
				m_trough_lock_txt[i]->SetText(StringUtil::Format(getString(LanguageKey::GUI_BED_WAR_RUNE_TROUGH_LOCK_TXT).c_str(), rune_trough_item->Level).c_str());
				m_trough_img[i]->SetVisible(false);
				m_trough_select[i]->SetVisible(false);
			}
			else
			{
				if (rune_trough_vec[i].rune_id > 0)
				{
					m_trough_lock_img[i]->SetVisible(false);
					m_trough_lock_txt[i]->SetVisible(false);
					m_trough_select[i]->SetVisible(false);
					RuneSettingItem* rune_item = RuneSetting::findRuneItemById(rune_trough_vec[i].rune_id);
					if (rune_item && rune_item->Image.length() > 0)
					{
						m_trough_img[i]->SetImage(rune_item->Image.c_str());
						m_trough_img[i]->SetVisible(true);
					}
					else
					{
						m_trough_img[i]->SetVisible(false);
					}
				}
				else
				{
					m_trough_lock_img[i]->SetVisible(false);
					m_trough_lock_txt[i]->SetVisible(false);
					m_trough_select[i]->SetVisible(false);
					m_trough_img[i]->SetVisible(false);

					auto iter = std::find(unlock_list.begin(), unlock_list.end(), rune_trough_item->Level);
					if (iter != unlock_list.end() && rune_trough_vec[i].need_show_effect)
					{
						m_trough_effect_layout[i]->SetEffectName("jiesuosmall.effect");
						m_trough_effect_layout[i]->SetVisible(true);
					}
				}
			}
		}
	}

	bool GuiBedWarRuneTypeItem::onRuneClick(const EventArgs & events, int index)
	{
		if (m_moving) return true;
		refreshSelect(index);
		return true;
	}

	bool GuiBedWarRuneTypeItem::onLeftClick(const EventArgs & events)
	{
		m_moving = true;
		m_change_btn->SetVisible(false);
		m_back_btn->SetVisible(false);
		BedWarLeftEvent::emit();
		return false;
	}

	bool GuiBedWarRuneTypeItem::onRightClick(const EventArgs & events)
	{
		m_moving = true;
		m_change_btn->SetVisible(false);
		m_back_btn->SetVisible(false);
		BedWarRightEvent::emit();
		return false;
	}

	bool GuiBedWarRuneTypeItem::onOneKeyUnloadClick(const EventArgs & events)
	{
		BedWarOneKeyUnloadEvent::emit();
		return false;
	}

	bool GuiBedWarRuneTypeItem::onOneKeyAssembleClick(const EventArgs & events)
	{
		BedWarOneKeyAssembleEvent::emit();
		return false;
	}

	bool GuiBedWarRuneTypeItem::onChangeBtnClick(const EventArgs & events)
	{
		m_change_btn->SetVisible(false);
		m_back_btn->SetVisible(true);
		BedWarRuneBackEvent::emit();
		return false;
	}

	bool GuiBedWarRuneTypeItem::onBackBtnClick(const EventArgs & events)
	{
		m_change_btn->SetVisible(true);
		m_back_btn->SetVisible(false);
		BedWarProSumChangeEvent::emit();
		
		return false;
	}

	void GuiBedWarRuneTypeItem::refreshSelect(int index)
	{
		if (index < 0 || index >= (int)m_rune_trough_vec.size() || index >= MAX_RUNE_TROUGH_NUM) return;

		if (!m_rune_trough_vec[index].rune_lock)
		{
			for (int i = 0; i < (int)m_rune_trough_vec.size() && i < MAX_RUNE_TROUGH_NUM; ++i)
			{
				m_trough_select[i]->SetVisible(false);
			}

			m_trough_select[index]->SetVisible(true);
			
			m_current_select_index = index;
			if (m_rune_trough_vec[index].rune_id > 0)
			{
				m_change_btn->SetVisible(true);
				m_back_btn->SetVisible(false);
			}
			else
			{
				m_change_btn->SetVisible(false);
				m_back_btn->SetVisible(true);
			}

			BedWarRuneTroughSelectChangeEvent::emit(m_rune_trough_vec[index].rune_trough_id, m_rune_trough_vec[index].rune_id);
		}
		else
		{
			if (m_unlock_price > 0)
			{
				ShowBedWarRuneUnlockTipEvent::emit(m_unlock_price, m_trough_type);
			}
		}
		m_trough_effect_layout[index]->SetVisible(false);
	}
	void GuiBedWarRuneTypeItem::refreshSelectFromOutSide(int trough_id)
	{
		for (int i = 0; i < (int)m_rune_trough_vec.size() && i < MAX_RUNE_TROUGH_NUM; ++i)
		{
			m_trough_select[i]->SetVisible(false);
			if (m_rune_trough_vec[i].rune_trough_id == trough_id)
			{
				m_current_select_index = i;
				m_trough_select[i]->SetVisible(true);
			}
		}
	}

	void GuiBedWarRuneTypeItem::clearSelect()
	{
		m_current_select_index = -1;
		for (int i = 0; i < MAX_RUNE_TROUGH_NUM; ++i)
		{
			m_trough_select[i]->SetVisible(false);
		}
	}
	void GuiBedWarRuneTypeItem::setMoving(bool sign)
	{
		m_moving = sign;
	}
}