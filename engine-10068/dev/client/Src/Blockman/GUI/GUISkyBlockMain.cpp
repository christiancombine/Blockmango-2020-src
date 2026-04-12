#include "GUISkyBlockMain.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "game.h"
#include "ShellInterface.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Plugins/GameClientEvents.h"
#include "GameAnalytics/GameAnalyticsSetting.h"

namespace BLOCKMAN
{
	GUISkyBlockMain::GUISkyBlockMain() :
		gui_layout("SkyBlockMain.json")
	{
	}

	GUISkyBlockMain::~GUISkyBlockMain()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUISkyBlockMain::onLoad()
	{
		getWindow("SkyBlockMain-Island-Area-Info-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_ISLAND_AREA));
		getWindow("SkyBlockMain-SetHome-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_ISLAND_SETHOME));
		getWindow("SkyBlockMain-Home-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_ISLAND_HOME));
		getWindow("SkyBlockMain-Diggings-Img-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_DIGGINGS_TEXT));
		getWindow("SkyBlockMain-Resource-Island-Icon-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_RESOURCE_ISLAND));
		getWindow("SkyBlockMain-Resource-Appshop-Icon-Text")->SetText(getString(LanguageKey::GUI_SKY_BLOCK_APPSHOP_TITLE));
		setText("SkyBlockMain-Privilege-Text", "gui_skyblock_main_pri_title");
		

		m_player_level = getWindow<GUIStaticText>("SkyBlockMain-Island-Level-Value");
		m_player_exp = getWindow<GUIProgressBar>("SkyBlockMain-Island-Level-Exp");
		m_player_exp_text = getWindow<GUIStaticText>("SkyBlockMain-Island-Level-Exp-Text");
		m_player_area_text = getWindow<GUIStaticText>("SkyBlockMain-Area-Text");
		m_task_red = getWindow<GUIStaticImage>("SkyBlockMain-Task-Red-Img");
		m_mian_red = getWindow<GUIStaticImage>("SkyBlockMain-Red-All-Img");
		m_mian_red->SetVisible(false);
		m_task_red->SetVisible(false);
		m_effect_layout = getWindow<GUILayout>("SkyBlockMain-EffectLayout");
		m_effect_layout->SetVisible(false);

		for (int i = 0; i < SKY_BLOCK_LEVEL_MAX; ++i)
		{
			String imgName = StringUtil::Format("SkyBlockMain-LevelImg%d", i + 1).c_str();
			m_level_img[i] = getWindow<GUIStaticImage>(imgName.c_str());
		}
		
		m_listLayout = getWindow<GUILayout>("SkyBlockMain-List");
		m_option = getWindow<GUILayout>("SkyBlockMain-Details");
		m_option->SetVisible(true);

		m_option_bg = getWindow<GUIStaticImage>("SkyBlockMain-Option-Bg");
		m_option_bg->SetVisible(false);


		m_btn = getWindow<GUIButton>("SkyBlockMain-Option-Btn");
		m_btn->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickOption, this));
		m_btn->SetVisible(true);

	    m_jiantou = getWindow<GUIButton>("SkyBlockMain-Jiantou");
		m_jiantou->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickArrow, this, std::placeholders::_1));
		m_jiantou->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain);
		
		getWindow<GUIButton>("SkyBlockMain-Home-Button")->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickAddHome, this));
		getWindow<GUIButton>("SkyBlockMain-BackHome-Button")->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickBackHome, this));
		auto resourceIsland = getWindow<GUIButton>("SkyBlockMain-Resource-Island-Button");
		resourceIsland->SetVisible(checkGameType(ClientGameType::SkyBlockMain));
		resourceIsland->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickResourceIsland, this, std::placeholders::_1));

		auto appShop = getWindow<GUIButton>("SkyBlockMain-Resource-Appshop-Button");
		appShop->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnOpenAppShop, this));
		appShop->SetVisible(true);	

		auto diggings = getWindow<GUIButton>("SkyBlockMain-Diggings-Icon-Btn");
		diggings->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::onBackClick, this, std::placeholders::_1));
		diggings->SetVisible(checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockProduct));

		auto privilege_btn = getWindow<GUIButton>("SkyBlockMain-Privilege-Btn");
		privilege_btn->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnOpenPrivilegeShop, this));
		privilege_btn->SetVisible(true);



		m_listLayout->SetVisible(true);

		m_areaLayout = getWindow<GUILayout>("SkyBlockMain-Island-Area");
		m_areaLayout->SetVisible(false);

		m_homeLayout = getWindow<GUILayout>("SkyBlockMain-Island-Home");
		m_homeLayout->SetVisible(false);

		auto Island = getWindow<GUIButton>("SkyBlockMain-Island-Button");
		Island->SetTouchable(checkGameType(ClientGameType::SkyBlockMain));
		Island->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::onIslandClick, this, std::placeholders::_1));

		auto Area = getWindow<GUIButton>("SkyBlockMain-Area-Button");
		Area->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::onAreaClick, this, std::placeholders::_1));

		m_sky_task_btn = getWindow<GUIButton>("SkyBlockMain-Task-Icon-Btn");
		m_sky_task_btn->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickTaskIcon, this, std::placeholders::_1));
		m_sky_task_btn->SetVisible(false);

		m_sky_rank_btn = getWindow<GUIButton>("SkyBlockMain-Rank-Btn");
		m_sky_rank_btn->subscribeEvent(EventButtonClick, std::bind(&GUISkyBlockMain::OnClickRankIcon, this, std::placeholders::_1));
		m_sky_rank_btn->SetVisible(false);

		m_subscriptionGuard.add(ShowSkyBlockInfo::subscribe(std::bind(&GUISkyBlockMain::showSkyBlockInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7)));
		m_subscriptionGuard.add(ShowSkyBlockTaskIconEvent::subscribe(std::bind(&GUISkyBlockMain::onUpdateSkyBlockTaskCount, this)));

	}

	void GUISkyBlockMain::onUpdate(ui32 nTimeElapse)
 	{
		
	}

	bool GUISkyBlockMain::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct);
	}

	bool GUISkyBlockMain::onIslandClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_areaLayout->IsVisible())
		{ 
			m_areaLayout->SetVisible(false);
			m_jiantou->SetRotate(180);
		}
		else
		{
			m_areaLayout->SetVisible(true);
			m_jiantou->SetRotate(0);
		}
		m_homeLayout->SetVisible(!m_homeLayout->IsVisible());

		return true;
	}

	bool GUISkyBlockMain::OnClickArrow(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_areaLayout->IsVisible())
		{
			m_areaLayout->SetVisible(false);
			m_jiantou->SetRotate(180);
		}
		else
		{
			m_areaLayout->SetVisible(true);
			m_jiantou->SetRotate(0);
		}
		m_homeLayout->SetVisible(!m_homeLayout->IsVisible());

		return true;
	}

	bool GUISkyBlockMain::onAreaClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getNetwork()->getCommonSender()->sendClickExtendArea();
		return true;
	}
	
	bool GUISkyBlockMain::onUpdateSkyBlockTaskCount()
	{
		int count = SkyBlockTaskSystemSetting::Instance()->getWaitRewardCount();
		m_mian_red->SetVisible(count > 0);
		m_task_red->SetVisible(count > 0);
		return true;
	}	
	
	bool GUISkyBlockMain::OnClickOption()
	{
		playSoundByType(ST_Click);
		if (m_option_bg->IsVisible())
		{
			m_option_bg->SetVisible(false);
			m_sky_rank_btn->SetVisible(false);
			m_sky_task_btn->SetVisible(false);
			m_btn->SetRotate(180);
		}
		else
		{
			m_option_bg->SetVisible(true);
			m_sky_task_btn->SetVisible(true);
			m_sky_rank_btn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain);
			m_btn->SetRotate(0);
		}
		return true;
	}

	bool GUISkyBlockMain::onBackClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (checkGameType(ClientGameType::SkyBlockProduct))
		{
			ClientNetwork::Instance()->getCommonSender()->sendClickResourceIsland();
		}
		if (checkGameType(ClientGameType::SkyBlockMain))
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickBackHall();
		}
		return true;
	}

	bool GUISkyBlockMain::OnClickResourceIsland(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getCommonSender()->sendClickResourceIsland();
		return true;
	}

	bool GUISkyBlockMain::OnClickRankIcon(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		// RootGuiLayout::Instance()->showSkyBlockRank();
		SCRIPT_EVENT::SkyBlockOpenRankEvent::invoke(true);
		return true;
	}

	bool GUISkyBlockMain::showSkyBlockInfo(int lv, i64 cur_exp, i64 max_exp, int island_lv, String area, bool is_max, bool isMaster)
	{
		if (isMaster)
			m_player_exp->SetProgressImage("set:skyblock_main.json image:skyblock_progress");
		else
			m_player_exp->SetProgressImage("set:party.json image:exp_bg2");

		for (int i = 0; i < SKY_BLOCK_LEVEL_MAX; ++i)
		{
			m_level_img[i]->SetImage("set:skyblock_main.json image:skyblock_kongxin");
		}

		for (int i = 0; i < SKY_BLOCK_LEVEL_MAX; ++i)
		{
			if (i + 1 <= island_lv)
			{
				m_level_img[i]->SetImage("set:skyblock_main.json image:skyblock_shixin");
			}
		}

		m_player_level->SetText(StringUtil::Format("%d", lv).c_str());
		if (!is_max)
		{
			if (max_exp > 0)
			{
				String exp_txt = StringUtil::Format("%lld/%lld", cur_exp, max_exp);
				m_player_exp_text->SetText(exp_txt.c_str());
				m_player_exp_text->SetVisible(true);
				m_player_exp->SetProgress((float)cur_exp / (float)max_exp);
				m_player_exp_text->SetVisible(true);
			}
			else
			{
				m_player_exp_text->SetVisible(false);
				m_player_exp->SetProgress(0.f);
			}
		}
		else
		{
			m_player_exp->SetProgress(0.f);
			m_player_exp_text->SetText("MAX");
			m_player_exp_text->SetVisible(true);
		}

		m_player_area_text->SetText(area.c_str());
		return true;
	}

	bool GUISkyBlockMain::OnClickTaskIcon(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->showSkyBlockTaskSystem();
		return true;
	}

	bool GUISkyBlockMain::OnClickAddHome()
	{
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getCommonSender()->sendSkyBlockAddHome();
		return true;
	}
	bool GUISkyBlockMain::OnClickBackHome()
	{
		if (checkGameType(ClientGameType::SkyBlockMineCave)|| checkGameType(ClientGameType::SkyBlockProduct))
		{
			playSoundByType(ST_Click);
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickBackHall();
			return true;
		}
		ClientNetwork::Instance()->getCommonSender()->sendSkyBlockBackHome();
		return true;
	}

	bool GUISkyBlockMain::OnOpenAppShop()
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->showSkyBlockAppShop();
		if (GameClient::CGame::Instance()->isEnableIndie())
			GameAnalyticsSetting::Instance()->design(0, { "g1048common_click_indie", "7" });
		else
			GameAnalyticsSetting::Instance()->design(0, { "g1048common_click", "7" });
		return true;
	}

	bool GUISkyBlockMain::OnOpenPrivilegeShop()
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->showSkyBlockPrivilegeShop();
		if (GameClient::CGame::Instance()->isEnableIndie())
			GameAnalyticsSetting::Instance()->design(0, { "g1048common_click_indie", "8" });
		else
			GameAnalyticsSetting::Instance()->design(0, { "g1048common_click", "8" });
		return true;
	}

	void GUISkyBlockMain::onShowSkyBlockLoadingRewards(bool loading)
	{
		if (m_effect_layout)
		{
			if (loading)
			{
				m_effect_layout->SetVisible(true);
			}
			else
			{
				m_effect_layout->SetVisible(false);
			}
		}
	}
}
