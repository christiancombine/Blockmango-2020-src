#include "gui_bedWarRune.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticImage.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "GUIBedWarRuneTypeItem.h"
#include "GUIBedWarRuneItem.h"
#include "GUIBedWarReclaimRuneItem.h"
#include "GUIBedWarRuneProItem.h"
#include "Setting/RuneSetting.h"
#include "Network/ClientNetwork.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIRadioButton.h"
#include "UI/GUIListHorizontalBox.h"
#include "UI/GUIGridView.h"
#include "UI/GUIListBox.h"

namespace BLOCKMAN
{
	gui_bedwar_rune::gui_bedwar_rune() :
		gui_layout("BedWarRune.json")
	{
		
	}

	gui_bedwar_rune::~gui_bedwar_rune()
	{
		m_subscriptionGuard.unsubscribeAll();
		RuneSetting::unloadSetting();
	}
	
	RootGuiLayout* gui_bedwar_rune::getParent()
	{
		return dynamic_cast<RootGuiLayout*>(m_parentLayout);
	}

	void gui_bedwar_rune::onLoad()
	{
		RuneSetting::loadSetting();
		getWindow("BedWarRune-Title-Text")->SetText(getString(LanguageKey::GUI_BED_WAR_STORE_NAME));

		m_runeListBox = getWindow<GUIListBox>("BedWarRune-RuneList");
		m_runeProListBox = getWindow<GUIListBox>("BedWarRune-ProPerList"); 
		m_runeTypeList = getWindow<GUIListHorizontalBox>("BedWarRune-RuneTypeList");
		m_rune_assemble_layout = getWindow<GUILayout>("BedWarRune-Assemble");
		m_rune_reclaim_layout = getWindow<GUILayout>("BedWarRune-Reclaim");
		m_rune_list_layout = getWindow<GUIStaticImage>("BedWarRune-RuneListLayout");
		m_pro_sum_layout = getWindow<GUIStaticImage>("BedWarRune-ProSumLayout");
		m_rune_trough_pro_sum_level_txt = getWindow<GUIStaticText>("BedWarRune-ProSumLevelTxt");
		m_rune_assemble_layout = getWindow<GUILayout>("BedWarRune-Assemble");
		m_effect_layout = getWindow<GUILayout>("BedWarRune-EffectLayout");
		m_effect_layout->SetVisible(false);
		m_effect_bg = getWindow<GUIStaticImage>("BedWarRune-EffectBg");
		m_effect_bg->SetVisible(false);
		
		auto BtnClose = getWindow<GUIButton>("BedWarRune-Close");
		BtnClose->subscribeEvent(EventButtonClick, std::bind(&gui_bedwar_rune::onCloseClick, this, std::placeholders::_1));

		m_reclaim_batch_btn = getWindow<GUIButton>("BedWarRune-Batch-Reclaim");
		m_reclaim_batch_btn->subscribeEvent(EventButtonClick, std::bind(&gui_bedwar_rune::onBatchReclaimClick, this, std::placeholders::_1));
		m_reclaim_batch_btn->SetText(getString(LanguageKey::GUI_BED_WAR_BATCH_RECLAIM_TXT));

		m_subscriptionGuard.add(BedWarRuneTroughSelectChangeEvent::subscribe(std::bind(&gui_bedwar_rune::troughSelectChange, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BedWarRuneClickEvent::subscribe(std::bind(&gui_bedwar_rune::runeClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BedWarRuneReclaimClickEvent::subscribe(std::bind(&gui_bedwar_rune::runeReclaimClick, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BedWarLeftEvent::subscribe(std::bind(&gui_bedwar_rune::onLeftImpl, this)));
		m_subscriptionGuard.add(BedWarRightEvent::subscribe(std::bind(&gui_bedwar_rune::onRightImpl, this)));
		m_subscriptionGuard.add(BedWarOneKeyUnloadEvent::subscribe(std::bind(&gui_bedwar_rune::onOneKeyUnloadImpl, this)));
		m_subscriptionGuard.add(BedWarOneKeyAssembleEvent::subscribe(std::bind(&gui_bedwar_rune::onOneKeyAssembleImpl, this)));
		m_subscriptionGuard.add(BedWarProSumChangeEvent::subscribe(std::bind(&gui_bedwar_rune::onShowProSumImpl, this)));
		m_subscriptionGuard.add(BedWarRuneBackEvent::subscribe(std::bind(&gui_bedwar_rune::onShowRuneListImpl, this)));

		for (int i = 0; i < MAX_RUNE_TAB_NUM; ++i)
		{
			String btnName = StringUtil::Format("BedWarRune-Tab%d", i + 1);
			m_tab_radio_btn[i] = getWindow<GUIRadioButton>(btnName.c_str());
			m_tab_radio_btn[i]->subscribeEvent(EventWindowTouchUp, std::bind(&gui_bedwar_rune::onRadioChange, this, std::placeholders::_1, i));
			m_tab_radio_btn[i]->SetNormalImage("set:rune.json image:tab_noselect");
			m_tab_radio_btn[i]->SetPushedImage("set:rune.json image:tab_select");
			String txtName = StringUtil::Format("BedWarRune-TabTitle%d", i + 1);
			m_tab_txt[i] = getWindow<GUIStaticText>(txtName.c_str());
			m_tab_txt[i]->SetProperty(GWP_TEXT_BORDER, "true");

			if (i == 0)
			{ 
				m_tab_radio_btn[i]->SetSelected(true);
			}
			else
			{
				m_tab_radio_btn[i]->SetSelected(false);
			}
		}
		m_tab_txt[0]->SetText(getString(LanguageKey::GUI_BED_WAR_TAB1_TITLE));
		m_tab_txt[1]->SetText(getString(LanguageKey::GUI_BED_WAR_TAB2_TITLE));

		getWindow<GUIStaticText>("BedWarRune-ProSumLevelTitle")->SetText(getString(LanguageKey::GUI_BED_WAR_PRO_LV_SUM_TXT).c_str());
		getWindow<GUIStaticText>("BedWarRune-ProSumTitle")->SetText(getString(LanguageKey::GUI_BED_WAR_PRO_TXT).c_str());

		for (int i = 0; i < MAX_RUNE_RECALIM_TAB_NUM; ++i)
		{
			String btnName = StringUtil::Format("BedWarRune-Reclaim-Tab%d", i + 1);
			m_reclaim_tab_radio_btn[i] = getWindow<GUIRadioButton>(btnName.c_str());
			m_reclaim_tab_radio_btn[i]->subscribeEvent(EventWindowTouchUp, std::bind(&gui_bedwar_rune::onReclaimRadioChange, this, std::placeholders::_1, i));
			m_reclaim_tab_radio_btn[i]->SetNormalImage("set:rune.json image:top_tab_no_select");
			m_reclaim_tab_radio_btn[i]->SetPushedImage("set:rune.json image:top_tab_select");
			String txtName = StringUtil::Format("BedWarRune-Reclaim-TabTitle%d", i + 1);
			m_reclaim_tab_txt[i] = getWindow<GUIStaticText>(txtName.c_str());
			m_reclaim_tab_txt[i]->SetProperty(GWP_TEXT_BORDER, "true");

			m_reclaim_tab_txt[i]->SetText(getString(StringUtil::Format("gui_bed_war_re_tab_title%d", i + 1).c_str()).c_str());

			if (i == 0)
			{
				m_reclaim_tab_radio_btn[i]->SetSelected(true);
			}
			else
			{
				m_reclaim_tab_radio_btn[i]->SetSelected(false);
			}
		}
		m_pro_sum_layout->SetVisible(true);
		m_rune_list_layout->SetVisible(false);

		m_reclaim_grid_layout = getWindow<GUILayout>("BedWarRune-ReclaimGridViewLayout");
		m_reclaim_grid_view = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "BedWar-Reclaim-GridView");
		m_reclaim_grid_view->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_reclaim_grid_view->InitConfig(5.f, 1.f, 3);
		m_reclaim_grid_layout->AddChildWindow(m_reclaim_grid_view);

		initRuneTrough();
	}

	void gui_bedwar_rune::onUpdate(ui32 nTimeElapse)
	{
		if (m_left_time_tick > 0 && (int)m_rune_trough_map.size() > 0)
		{
			float w = m_runeTypeList->GetPixelSize().x;
			float one_tick_offset = (w / TIME_TICK_ALL) * (TIME_TICK_ALL - m_left_time_tick);
			float offset = m_left_start_pos + one_tick_offset;
			if (offset >= 0) offset = 0;
			m_runeTypeList->SetScrollOffset(offset);

			m_left_time_tick -= nTimeElapse;
			if (m_left_time_tick <= 0)
			{
				m_runeTypeList->SetScrollOffset(m_left_start_pos + w);
				m_rune_trough_list_end_pos = m_left_start_pos + w;
				m_left_time_tick = 0;
				m_pro_sum_layout->SetVisible(true);
				m_rune_list_layout->SetVisible(false);
				m_current_select_rune_trough_id = 0;
				m_current_select_rune_trough_rune_id = 0;
				m_runeTypeList->RestChildVisible();
				moveRuneTroughType(true);
				clearRuneTroughSelect();
				setRuneTroughMove(false);
				refreshRuneProList();
				refreshRuneList();
			}
		}
		else if (m_right_time_tick > 0 && (int)m_rune_trough_map.size() > 0)
		{
			float w = m_runeTypeList->GetPixelSize().x;
			float one_tick_offset = (w / TIME_TICK_ALL) * (TIME_TICK_ALL - m_right_time_tick);
			float offset = m_right_start_pos - one_tick_offset;
			if (offset <= -(( (int)m_rune_trough_map.size() - 1) * w)) offset = -(((int)m_rune_trough_map.size() - 1) * w);
			m_runeTypeList->SetScrollOffset(offset);

			m_right_time_tick -= nTimeElapse;
			if (m_right_time_tick <= 0)
			{
				m_runeTypeList->SetScrollOffset(m_right_start_pos - w);
				m_rune_trough_list_end_pos = m_right_start_pos - w;
				m_right_time_tick = 0;
				m_pro_sum_layout->SetVisible(true);
				m_rune_list_layout->SetVisible(false);
				m_current_select_rune_trough_id = 0;
				m_current_select_rune_trough_rune_id = 0;
				m_runeTypeList->RestChildVisible();
				moveRuneTroughType(false);
				clearRuneTroughSelect();
				setRuneTroughMove(false);
				refreshRuneProList();
				refreshRuneList();
			}
		}
		else
		{

		}

		if (m_current_tab_index == RT_ASSEMBLE)
		{
			if (!m_moving_trough)
			{
				float now = m_runeTypeList->GetScrollOffset();
				float w = m_runeTypeList->GetPixelSize().x;
				float target = -(w * (m_rune_trough_select_type - 1));
				if (now != target)
				{
					m_runeTypeList->SetScrollOffset(target);
					m_rune_trough_list_end_pos = target;
				}	
			}

			if (m_refreshRuneTypeListTime > 0)
			{
				m_refreshRuneTypeListTime -= nTimeElapse;
				m_runeTypeList->RestChildVisible();
				if (m_refreshRuneTypeListTime <= 0)
				{
					m_refreshRuneTypeListTime = 0;
				}
			}
		}

		if (m_effect_start_time > 0)
		{
			m_effect_start_time -= nTimeElapse;
			if (m_effect_start_time <= 0)
			{
				m_effect_layout->SetVisible(true);
				m_effect_layout->SetEffectName("jiesuobig.effect");
				m_effect_time = EFFECT_TIME;
			}
		}

		if (m_effect_time > 0)
		{
			m_effect_time -= nTimeElapse;
			if (m_effect_time <= 0)
			{
				m_effect_layout->SetVisible(false);
				m_effect_bg->SetVisible(false);
			}
		}
	}

	void gui_bedwar_rune::onShow()
	{
		switch (m_current_tab_index)
		{
		case RT_ASSEMBLE:
			m_rune_assemble_layout->SetVisible(true);
			m_rune_reclaim_layout->SetVisible(false);
			m_pro_sum_layout->SetVisible(true);
			m_rune_list_layout->SetVisible(false);
			// refreshRuneTypeList();
			getWindow<GUIStaticImage>("BedWarRune-BedWarRune-Reclaim-TabLayoutBg")->SetVisible(false);
			m_runeTypeList->RestChildVisible();
			break;
		case RT_RECLAIM:
			m_rune_assemble_layout->SetVisible(false);
			m_rune_reclaim_layout->SetVisible(true);
			getWindow<GUIStaticImage>("BedWarRune-BedWarRune-Reclaim-TabLayoutBg")->SetVisible(true);
			break;
		default:
			break;
		}
	}

	bool gui_bedwar_rune::isNeedLoad()
	{
		return checkGameType(ClientGameType::BED_WAR_HALL);
	}

	void gui_bedwar_rune::onGetCommonData(String key, String data)
	{
		if (key == "RuneData")
		{
			OpenBedWarRuneEvent::emit();
			refreshAll(data);
			return;
		}
		if (key == "RuneNextTrough")
		{
			nextSelectTrough();
			return;
		}
	}

	bool gui_bedwar_rune::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_bedwar_rune::onLeftImpl()
	{
		if (m_runeTypeList)
		{
			float offset = m_runeTypeList->GetScrollOffset();
			if (m_right_time_tick == 0 && m_left_time_tick == 0 && offset < 0)
			{
				m_left_time_tick = TIME_TICK_ALL;
				m_left_start_pos = m_runeTypeList->GetScrollOffset();
				m_runeTypeList->RestChildVisible();
				m_rune_list_layout->SetVisible(false);
				m_pro_sum_layout->SetVisible(true);
				setRuneTroughMove(true);
			}
			else
			{
				setRuneTroughMove(false);
			}
		}
		return true;
	}

	bool gui_bedwar_rune::onRightImpl()
	{
		if (m_runeTypeList)
		{
			float offset = m_runeTypeList->GetScrollOffset();
			float w = m_runeTypeList->GetPixelSize().x;
			if (m_right_time_tick == 0 && m_left_time_tick == 0 && (int)m_rune_trough_map.size() > 0 && offset > -(((int)m_rune_trough_map.size() - 1) * w))
			{
				m_right_time_tick = TIME_TICK_ALL;
				m_right_start_pos = m_runeTypeList->GetScrollOffset();
				m_runeTypeList->RestChildVisible();
				m_rune_list_layout->SetVisible(false);
				m_pro_sum_layout->SetVisible(true);
				setRuneTroughMove(true);
			}
			else
			{
				setRuneTroughMove(false);
			}
		}
		return true;
	}

	void gui_bedwar_rune::moveRuneTroughType(bool left)
	{
		if ((int)m_rune_trough_map.size() <= 1) return;

		auto iter = m_rune_trough_map.find(m_rune_trough_select_type);

		if (iter == m_rune_trough_map.end()) return;

		auto begin = m_rune_trough_map.begin();
		auto end = m_rune_trough_map.end();
		end = --end;

		if (iter == end && !left) return;

		if (iter == begin && left) return;

		if (left)
		{
			iter--;
			m_rune_trough_select_type = iter->first;
		}
		else
		{
			iter++;
			m_rune_trough_select_type = iter->first;
		}
		calEquipRuneLevel();
	}

	bool gui_bedwar_rune::onBatchReclaimClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		int sum = 0;
		int sum_key = 0;
		for (auto rune : m_rune_list)
		{
			RuneSettingItem* rune_item = RuneSetting::findRuneItemById(rune.rune_id);
			if (!rune_item) continue;

			if (m_current_reclaim_tab_index == RT_INVALID || (m_current_reclaim_tab_index != RT_INVALID && rune_item->Type == m_current_reclaim_tab_index))
			{
				if (rune_item->Level == 1)
				{
					sum += rune.rune_num;
					sum_key += (rune.rune_num * rune_item->SellKey);
				}
			}
		}
		if (sum > 0)
		{
			ShowBedWarReclaimAllTipEvent::emit(sum, sum_key, m_current_reclaim_tab_index);
		}
		
		return true;
	}

	bool gui_bedwar_rune::onProSumChangeClick(const EventArgs & events)
	{
		onShowRuneListImpl();
		return true;
	}

	bool gui_bedwar_rune::onOneKeyUnloadImpl()
	{
		m_current_select_rune_trough_id = 0;
		m_current_select_rune_trough_rune_id = 0;
		ClientNetwork::Instance()->getCommonSender()->sendBedWarOneKeyUnloadRune(m_rune_trough_select_type);
		return false;
	}

	bool gui_bedwar_rune::onOneKeyAssembleImpl()
	{
		m_current_select_rune_trough_id = 0;
		m_current_select_rune_trough_rune_id = 0;
		ClientNetwork::Instance()->getCommonSender()->sendBedWarOneKeyEquipRune(m_rune_trough_select_type);
		return false;
	}

	bool gui_bedwar_rune::onRuneChangeClick(const EventArgs & events)
	{
		onShowProSumImpl();
		return true;
	}

	bool gui_bedwar_rune::refreshAll(const String & data)
	{
 		std::vector<RuneTroughItem> rune_trough_list;
 		std::vector<RuneItem> rune_list;

		if (getAllInfo(data, rune_trough_list, rune_list))
		{
			for (std::map<int, std::vector<RuneTroughItem>>::iterator map_iter = m_rune_trough_map.begin(); map_iter != m_rune_trough_map.end(); ++map_iter)
			{
				for (std::vector<RuneTroughItem>::iterator iter = map_iter->second.begin(); iter != map_iter->second.end(); ++iter)
				{
					RuneTroughSettingItem* rune_trough_item = RuneSetting::findRuneTroughItemById((*iter).rune_trough_id);
					if (!rune_trough_item) continue;

					//(*iter).rune_lock = m_player_level < rune_trough_item->Level;
					
					for (auto troughInfo : rune_trough_list)
					{
						if ((*iter).rune_trough_id == troughInfo.rune_trough_id)
						{
							(*iter).rune_id = troughInfo.rune_id;
							(*iter).rune_lock = troughInfo.rune_lock;
							(*iter).need_show_effect = troughInfo.need_show_effect;
							break;
						}
					}
				}
			}

			m_rune_list.clear();

			for (auto rune : rune_list)
			{
				RuneItem runeItem;
				runeItem.rune_id = rune.rune_id;
				runeItem.rune_num = rune.rune_num;

				m_rune_list.push_back(runeItem);
			}

			refreshRuneTypeList();
			refreshRuneList();
			refreshGridView(m_current_reclaim_tab_index);
			refreshRuneProList();

			if (m_current_tab_index == RT_ASSEMBLE)
			{
				m_tab_txt[0]->SetTextColor(Color::WHITE);
				m_tab_txt[1]->SetTextColor(Color(1.0f, 0.4f, 0.0f));
			}
			else if (m_current_tab_index == RT_RECLAIM)
			{
				m_tab_txt[0]->SetTextColor(Color(1.0f, 0.4f, 0.0f));
				m_tab_txt[1]->SetTextColor(Color::WHITE);
			}

			if (m_current_tab_index == RT_ASSEMBLE && m_unlock_effect)
			{
 				m_effect_bg->SetVisible(true);
				m_effect_start_time = EFFECT_START_TIME;

				if (m_first_unlock_type > RT_INVALID && m_first_unlock_type < RT_COUNT)
				{
					float w = m_runeTypeList->GetPixelSize().x;
					m_runeTypeList->SetScrollOffset(-(w * (m_first_unlock_type - 1)));
					m_rune_trough_list_end_pos = -(w * (m_first_unlock_type - 1));
					m_refreshRuneTypeListTime = REFRESH_RUNE_LIST_TIME;
					m_runeTypeList->RestChildVisible();
				}
			}
			
			if (m_current_tab_index == RT_ASSEMBLE)
			{
				if (m_assemble_open_type == AOT_PRO)
				{
					onShowProSumImpl();
				}
				else if (m_assemble_open_type == AOT_RUNE_LIST)
				{
					onShowRuneListImpl();
				}
				else
				{

				}
			}
		}
		return true;
	}

	void gui_bedwar_rune::refreshGridView(int type)
	{
		m_reclaim_grid_view->RemoveAllItems();

		for (int i = 0; i < (int)m_rune_list.size(); ++i)
		{
			GUIString ChildsName = StringUtil::Format("BedWarReclaimListItem-%d", i + 1).c_str();

			if (GUIWindowManager::Instance()->GetGUIWindow(ChildsName.c_str()))
			{
				break;
			}

			RuneSettingItem* rune_item = RuneSetting::findRuneItemById(m_rune_list[i].rune_id);
			if (!rune_item) continue;

			if (rune_item->CanSell == true && (type == RT_INVALID || (type != RT_INVALID && rune_item->Type == type)))
			{
				float ContentSizeX = m_reclaim_grid_view->GetPixelSize().x;
				float ContentSizeY = m_reclaim_grid_view->GetPixelSize().y;
				float ChildWidth = (float)((ContentSizeX - 5.0 * 3) / 3);
				float ChildHeight = (float)((ContentSizeY - 1.0) / 3);
				GuiBedWarReclaimRuneItem* pBedWarReclaimItem = dynamic_cast<GuiBedWarReclaimRuneItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_BED_WAR_RECLAIM_RUNE_ITEM, ChildsName.c_str()));
				pBedWarReclaimItem->RefreshItem(m_rune_list[i].rune_id, m_rune_list[i].rune_num);
				pBedWarReclaimItem->SetWidth({ 0.f, ChildWidth });
				pBedWarReclaimItem->SetHeight({ 0.f, ChildHeight });
				pBedWarReclaimItem->SetTouchable(false);
				pBedWarReclaimItem->SetClipChild(true);
				m_reclaim_grid_view->AddItem(pBedWarReclaimItem);
			}
		}

	}

	bool gui_bedwar_rune::getAllInfo(const String & data, std::vector<RuneTroughItem> &rune_trough_list, std::vector<RuneItem> &rune_list)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_bedwar_rune::getAllInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("assemble_type")
			|| !doc->HasMember("unlock_effect")
			|| !doc->HasMember("unlock_trough_list") 
			|| !doc->HasMember("player_level") 
			|| !doc->HasMember("unlock_price") 
			|| !doc->HasMember("rune_trough_list") 
			|| !doc->HasMember("rune_list"))
		{
			LordLogError("gui_bedwar_rune::getAllInfo The info mode content missed some field.");
			LordDelete(doc);
			return false;
		}

		m_assemble_open_type = doc->FindMember("assemble_type")->value.GetInt();
		m_unlock_effect = doc->FindMember("unlock_effect")->value.GetBool();
		m_player_level = doc->FindMember("player_level")->value.GetInt();
		m_unlock_price = doc->FindMember("unlock_price")->value.GetInt();
		m_first_unlock_type = doc->FindMember("unlock_first_type")->value.GetInt();

		m_unlock_id_list.clear();

		rapidjson::Value arr_unlock_trough_list = doc->FindMember("unlock_trough_list")->value.GetArray();
		for (size_t i = 0; i < arr_unlock_trough_list.Size(); ++i)
		{
			if (!arr_unlock_trough_list[i].HasMember("level"))
			{
				LordLogError("gui_bedwar_rune::getAllInfo The game result content rune_list missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int level = arr_unlock_trough_list[i].FindMember("level")->value.GetInt();
			m_unlock_id_list.push_back(level);
		}

		rapidjson::Value arr_rune_trough_info = doc->FindMember("rune_trough_list")->value.GetArray();
		for (size_t i = 0; i < arr_rune_trough_info.Size(); ++i)
		{
			if (!arr_rune_trough_info[i].HasMember("rune_trough_id")
				|| !arr_rune_trough_info[i].HasMember("rune_id")
				|| !arr_rune_trough_info[i].HasMember("rune_lock")
				|| !arr_rune_trough_info[i].HasMember("need_show_effect"))
			{
				LordLogError("gui_bedwar_rune::getAllInfo The game result content rune_list missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int rune_trough_id = arr_rune_trough_info[i].FindMember("rune_trough_id")->value.GetInt();
			int rune_id = arr_rune_trough_info[i].FindMember("rune_id")->value.GetInt();
			bool rune_lock = arr_rune_trough_info[i].FindMember("rune_lock")->value.GetBool();
			bool need_show_effect = arr_rune_trough_info[i].FindMember("need_show_effect")->value.GetBool();

			RuneTroughItem runeTroughItem;
			runeTroughItem.rune_trough_id = rune_trough_id;
			runeTroughItem.rune_id = rune_id;
			runeTroughItem.rune_lock = rune_lock;
			runeTroughItem.need_show_effect = need_show_effect;

			rune_trough_list.push_back(runeTroughItem);
		}

		rapidjson::Value arr_rune_list_info = doc->FindMember("rune_list")->value.GetArray();
		for (size_t i = 0; i < arr_rune_list_info.Size(); ++i)
		{
			if (!arr_rune_list_info[i].HasMember("rune_id")
				|| !arr_rune_list_info[i].HasMember("rune_num"))
			{
				LordLogError("gui_bedwar_rune::getAllInfo The game result content rune_list missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int rune_id = arr_rune_list_info[i].FindMember("rune_id")->value.GetInt();
			int rune_num = arr_rune_list_info[i].FindMember("rune_num")->value.GetInt();

			RuneItem runeItem;
			runeItem.rune_id = rune_id;
			runeItem.rune_num = rune_num;

			rune_list.push_back(runeItem);
		}

		LordDelete(doc);
		return true;
	}

	void gui_bedwar_rune::refreshRuneTypeList()
	{
		m_runeTypeList->ClearAllItem();

		unsigned int count = 0;
		for (auto iter : m_rune_trough_map)
		{
			String itemLayout = StringUtil::Format("BedWar-Rune-Type-Item-%d", ++count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				continue;
			}

			float w = m_runeTypeList->GetPixelSize().x;
			float h = m_runeTypeList->GetPixelSize().y;
			GuiBedWarRuneTypeItem* itemView = dynamic_cast<GuiBedWarRuneTypeItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_BED_WAR_RUNE_TYPE_ITEM, itemLayout.c_str()));
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, h));

			std::vector<GuiBedWarRuneTypeItem::RuneTroughItem> troughItemVec;
			for (auto trough : iter.second)
			{
				GuiBedWarRuneTypeItem::RuneTroughItem troughItem;
				troughItem.rune_trough_id = trough.rune_trough_id;
				troughItem.rune_id = trough.rune_id;
				troughItem.rune_lock = trough.rune_lock;
				troughItem.need_show_effect = trough.need_show_effect;
				troughItemVec.push_back(troughItem);
			}
			itemView->RefreshItem(troughItemVec, iter.first, m_player_level, m_unlock_price, m_unlock_id_list);

			m_runeTypeList->AddItem(itemView, false);
		}
		m_runeTypeList->SetMoveAble(false);
		m_runeTypeList->SetTouchable(false);

		m_runeTypeList->SetScrollOffset(m_rune_trough_list_end_pos);
		m_runeTypeList->RestChildVisible();
		calEquipRuneLevel();
	}

	void gui_bedwar_rune::refreshRuneList()
	{
		m_runeListBox->ClearAllItem();
		unsigned int count = 0;
		for (auto rune : m_rune_list)
		{
			String itemLayout = StringUtil::Format("BedWar-Rune-Item-%d", ++count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				continue;
			}

			RuneSettingItem* rune_item = RuneSetting::findRuneItemById(rune.rune_id);
			if (!rune_item) continue;

			if (m_rune_trough_select_type != rune_item->Type)
			{
				continue;
			}

			GuiBedWarRuneItem* itemView = dynamic_cast<GuiBedWarRuneItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_BED_WAR_RUNE_ITEM, itemLayout.c_str()));
			itemView->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(1.f, 0.f), UDim(0.f, 111.f));
			itemView->RefreshItem(rune.rune_id, rune.rune_num);
			m_runeListBox->AddItem(itemView, false);
		}
	}

	void gui_bedwar_rune::refreshRuneProList(int rune_trough_id)
	{
		m_runeProListBox->ClearAllItem();
		unsigned int count = 0;
		auto iter = m_rune_trough_map.find(m_rune_trough_select_type);
		if (iter != m_rune_trough_map.end())
		{
			for (auto vec_iter : iter->second)
			{
				if (vec_iter.rune_id > 0)
				{
					String itemLayout = StringUtil::Format("BedWar-Rune-Pro-Item-%d", ++count).c_str();
					if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
					{
						continue;
					}

					GuiBedWarRuneProItem* itemView = dynamic_cast<GuiBedWarRuneProItem*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_BED_WAR_RUNE_PRO_ITEM, itemLayout.c_str()));
					itemView->SetArea(UDim(0.f, 0.f), UDim(0.f, 0.f), UDim(1.f, 0.f), UDim(0.f, 54.f));
					itemView->RefreshItem(count - 1, vec_iter.rune_id, rune_trough_id == vec_iter.rune_trough_id && rune_trough_id > 0);
					m_runeProListBox->AddItem(itemView, false);
				}	
			}
		}
	}

	bool gui_bedwar_rune::onRadioChange(const EventArgs & events, int index)
	{
		m_current_tab_index = index;
		
		switch (m_current_tab_index)
		{
		case RT_ASSEMBLE:
			m_rune_assemble_layout->SetVisible(true);
			m_rune_reclaim_layout->SetVisible(false);
			getWindow<GUIStaticImage>("BedWarRune-BedWarRune-Reclaim-TabLayoutBg")->SetVisible(false);
			m_runeTypeList->RestChildVisible();
			m_tab_txt[0]->SetTextColor(Color::WHITE);
			m_tab_txt[1]->SetTextColor(Color(1.0f, 0.4f, 0.0f));
			break;
		case RT_RECLAIM:
			m_rune_assemble_layout->SetVisible(false);
			m_rune_reclaim_layout->SetVisible(true);
			getWindow<GUIStaticImage>("BedWarRune-BedWarRune-Reclaim-TabLayoutBg")->SetVisible(true);
			m_tab_txt[0]->SetTextColor(Color(1.0f, 0.4f, 0.0f));
			m_tab_txt[1]->SetTextColor(Color::WHITE);
			break;
		default:
			break;
		}
		return true;
	}

	bool gui_bedwar_rune::onReclaimRadioChange(const EventArgs & events, int index)
	{
		m_current_reclaim_tab_index = index;
		refreshGridView(m_current_reclaim_tab_index);
		return true;
	}

	void gui_bedwar_rune::initRuneTrough()
	{
		if (!m_rune_trough_map.empty()) return;

		m_rune_trough_map.clear();

		std::map<int, RuneTroughSettingItem*> rune_trough_map = RuneSetting::getAllRuneTrough();

		if (rune_trough_map.empty()) return;

		for (auto trough : rune_trough_map)
		{
			RuneTroughItem runeTroughItem;
			runeTroughItem.rune_trough_id = trough.second->Id;
			runeTroughItem.rune_id = 0;
			runeTroughItem.rune_lock = true;

			auto iter = m_rune_trough_map.find(trough.second->Type);
			if (iter == m_rune_trough_map.end())
			{
				std::vector<RuneTroughItem> rune_trough_item_vec;
				rune_trough_item_vec.push_back(runeTroughItem);

				m_rune_trough_map[trough.second->Type] = rune_trough_item_vec;
			}
			else
			{
				m_rune_trough_map[trough.second->Type].push_back(runeTroughItem);
			}
		}

		m_rune_trough_select_type = m_rune_trough_map.begin()->first;
		calEquipRuneLevel();
		refreshRuneProList();
		refreshRuneTypeList();
	}

	bool gui_bedwar_rune::troughSelectChange(int rune_trough_id, int rune_id)
	{
		playSoundByType(ST_Click);
		m_current_select_rune_trough_id = rune_trough_id;
		m_current_select_rune_trough_rune_id = rune_id;
		if (m_current_select_rune_trough_rune_id > 0)
		{
			m_pro_sum_layout->SetVisible(true);
			m_rune_list_layout->SetVisible(false);
			refreshRuneProList(rune_trough_id);
		}
		else
		{	
			m_pro_sum_layout->SetVisible(false);
			m_rune_list_layout->SetVisible(true);
			onShowRuneListImpl();
		}
		return true;
	}

	bool gui_bedwar_rune::runeClick(int rune_id)
	{
		playSoundByType(ST_Click);
		if (m_current_select_rune_trough_id > 0)
		{
			auto iter = m_rune_trough_map.find(m_rune_trough_select_type);
			if (iter != m_rune_trough_map.end())
			{
				for (auto iter_vec : iter->second)
				{
					if (m_current_select_rune_trough_id == iter_vec.rune_trough_id)
					{
						if (iter_vec.rune_lock == false)
						{
							ClientNetwork::Instance()->getCommonSender()->sendBedWarChangeRune(m_current_select_rune_trough_id, m_current_select_rune_trough_rune_id, rune_id);
						}
					}
				}
			}
		}
		return true;
	}

	bool gui_bedwar_rune::runeReclaimClick(int rune_id)
	{
		playSoundByType(ST_Click);
		RuneSettingItem* rune_item = RuneSetting::findRuneItemById(rune_id);
		if (!rune_item) return true;
		int left_num = 0;
		for (auto rune : m_rune_list)
		{
			if (rune.rune_id == rune_id)
			{
				left_num = rune.rune_num;
			}
		}
		ShowBedWarReclaimTipEvent::emit(left_num, rune_item->Name, rune_id);
		return true;
	}

	bool gui_bedwar_rune::onShowRuneListImpl()
	{
		playSoundByType(ST_Click);
		m_pro_sum_layout->SetVisible(false);
		m_rune_list_layout->SetVisible(true);
		return true;
	}

	bool gui_bedwar_rune::onShowProSumImpl()
	{
		playSoundByType(ST_Click);
		m_pro_sum_layout->SetVisible(true);
		m_rune_list_layout->SetVisible(false);
		return true;
	}

	void gui_bedwar_rune::clearRuneTroughSelect()
	{
		m_rune_trough_select_index = -1;

		if (!m_runeTypeList) return;

		int count = m_runeTypeList->getContainerWindow()->GetChildCount();

		for (int i = 0; i < count; ++i)
		{
			if (dynamic_cast<GuiBedWarRuneTypeItem*>(m_runeTypeList->getContainerWindow()->GetChildByIndex(i)))
			{
				dynamic_cast<GuiBedWarRuneTypeItem*>(m_runeTypeList->getContainerWindow()->GetChildByIndex(i))->clearSelect();
			}
		}
	}

	void gui_bedwar_rune::setRuneTroughMove(bool sign)
	{
		m_moving_trough = sign;
		if (!m_runeTypeList) return;

		int count = m_runeTypeList->getContainerWindow()->GetChildCount();

		for (int i = 0; i < count; ++i)
		{
			if (dynamic_cast<GuiBedWarRuneTypeItem*>(m_runeTypeList->getContainerWindow()->GetChildByIndex(i)))
			{
				dynamic_cast<GuiBedWarRuneTypeItem*>(m_runeTypeList->getContainerWindow()->GetChildByIndex(i))->setMoving(sign);
			}
		}
	}

	void gui_bedwar_rune::calEquipRuneLevel()
	{
		m_rune_trough_sum_level = 0;
		auto iter = m_rune_trough_map.find(m_rune_trough_select_type);
		if (iter != m_rune_trough_map.end())
		{
			for (auto vec_iter : iter->second)
			{
				if (vec_iter.rune_id > 0)
				{
					RuneSettingItem* rune_item = RuneSetting::findRuneItemById(vec_iter.rune_id);
					if (!rune_item) continue;

					m_rune_trough_sum_level += rune_item->Level;
				}
			}
		}
		m_rune_trough_pro_sum_level_txt->SetText(StringUtil::Format("%d", m_rune_trough_sum_level).c_str());
	}

	bool gui_bedwar_rune::nextSelectTrough()
	{
		bool change = false;

		bool no_this_type_rune = true;
		for (auto rune : m_rune_list)
		{
			RuneSettingItem* rune_item = RuneSetting::findRuneItemById(rune.rune_id);
			if (!rune_item) continue;

			if (m_rune_trough_select_type == rune_item->Type)
			{
				no_this_type_rune = false;
				break;
			}
		}

		if (m_current_select_rune_trough_id > 0 && !no_this_type_rune && m_runeTypeList)
		{
			auto iter = m_rune_trough_map.find(m_rune_trough_select_type);
			if (iter != m_rune_trough_map.end())
			{
				for (auto iter_vec : iter->second)
				{
					if (iter_vec.rune_id == 0)
					{
						m_current_select_rune_trough_id = iter_vec.rune_trough_id;
						m_current_select_rune_trough_rune_id = 0;

						if (iter_vec.rune_lock == true) continue;

						int count = m_runeTypeList->getContainerWindow()->GetChildCount();

						for (int i = 0; i < count; ++i)
						{
							if (dynamic_cast<GuiBedWarRuneTypeItem*>(m_runeTypeList->getContainerWindow()->GetChildByIndex(i))->getTroughType() == m_rune_trough_select_type)
							{
								dynamic_cast<GuiBedWarRuneTypeItem*>(m_runeTypeList->getContainerWindow()->GetChildByIndex(i))->refreshSelectFromOutSide(m_current_select_rune_trough_id);
								change = true;
								
								break;
							}
						}

						break;
					}
				}
			}
		}

		if (!change)
		{
			onShowProSumImpl();
		}
		
		return true;
	}

}
