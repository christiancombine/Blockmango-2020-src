#include "gui_pixelChickenAward.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "game.h"
#include "UI/GUIGridView.h"
#include "ShellInterface.h"

namespace BLOCKMAN
{
	gui_pixelChickenAward::gui_pixelChickenAward() :
		gui_layout("PixelChickenAward.json")
	{
	}

	gui_pixelChickenAward::~gui_pixelChickenAward()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_pixelChickenAward::onLoad()
	{
		GUIStaticText* Title = getWindow<GUIStaticText>("PixelChickenAward-Title-Text");
		GUIStaticText* RewardTitle = getWindow<GUIStaticText>("PixelChickenAward-Reward-Title-Text");
		GUIStaticText* RankText = getWindow<GUIStaticText>("PixelChickenAward-Rank-Text");
		GUIStaticText* KillText = getWindow<GUIStaticText>("PixelChickenAward-Kill-Text");
		GUIStaticText* NameText = getWindow<GUIStaticText>("PixelChickenAward-Name-Text");
		GUIStaticText* HonorText = getWindow<GUIStaticText>("PixelChickenAward-Honor-Text");
		GUIStaticText* RankTotalText = getWindow<GUIStaticText>("PixelChickenAward-Rank-Total-Text");
		GUIStaticText* LoginText = getWindow<GUIStaticText>("PixelChickenAward-Login-Text");
		GUIStaticText* BlockmanText = getWindow<GUIStaticText>("PixelChickenAward-Blockman-Text");
		GUIStaticText* AdText = getWindow<GUIStaticText>("PixelChickenAward-Ad-Text");

		m_honor = getWindow<GUIStaticImage>("PixelChickenAward-Hornor");
		m_rank = getWindow<GUIStaticText>("PixelChickenAward-Rank");
		m_name = getWindow<GUIStaticText>("PixelChickenAward-Name");
		m_rank_total = getWindow<GUIStaticText>("PixelChickenAward-Rank-Total");
		m_kill = getWindow<GUIStaticText>("PixelChickenAward-Kill-Num");
		m_player_title = getWindow<GUIStaticText>("PixelChickenAward-Over-Text");

		m_exit = getWindow<GUIButton>("PixelChickenAward-Exit");
		m_watch = getWindow<GUIButton>("PixelChickenAward-Watch");
		m_next = getWindow<GUIButton>("PixelChickenAward-Next");
		m_ad = getWindow<GUIButton>("PixelChickenAward-Ad");

		m_gold_num = getWindow<GUIStaticText>("PixelChickenAward-Gold-Num");
		m_key_num = getWindow<GUIStaticText>("PixelChickenAward-Key-Num");
		m_honor_num = getWindow<GUIStaticText>("PixelChickenAward-Honor-Num");
		m_exp_num = getWindow<GUIStaticText>("PixelChickenAward-Exp-Num");
		m_login_num = getWindow<GUIStaticText>("PixelChickenAward-Login-Num");

		m_effect = getWindow<GUIStaticImage>("PixelChickenAward-Effect");

		Title->SetText(LanguageManager::Instance()->getString("pixel_chicken_award_title").c_str());
		RewardTitle->SetText(LanguageManager::Instance()->getString("pixel_chicken_reward_title").c_str());
		RankText->SetText(LanguageManager::Instance()->getString("pixel_chicken_rank_text").c_str());
		KillText->SetText(LanguageManager::Instance()->getString("pixel_chicken_kill_text").c_str());
		LoginText->SetText(LanguageManager::Instance()->getString("pixel_chicken_login_text").c_str());
		BlockmanText->SetText(LanguageManager::Instance()->getString("pixel_chicken_blockman_text").c_str());
		NameText->SetText(LanguageManager::Instance()->getString("pixel_chicken_name_text").c_str());
		HonorText->SetText(LanguageManager::Instance()->getString("pixel_chicken_honor_text").c_str());
		RankTotalText->SetText(LanguageManager::Instance()->getString("pixel_chicken_rank_total_text").c_str());

		m_watch->SetText(LanguageManager::Instance()->getString("pixel_chicken_btn_watch").c_str());
		m_next->SetText(LanguageManager::Instance()->getString("gui_pixel_gun_result_btn_next").c_str());
		m_exit->SetText(LanguageManager::Instance()->getString("gui_pixel_gun_result_btn_back").c_str());
		m_ad->SetText(LanguageManager::Instance()->getString("gui_pixel_double_reward").c_str());

		m_exit->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenAward::onButtonClick, this, std::placeholders::_1, ButtonAction::EXIT));
		m_watch->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenAward::onButtonClick, this, std::placeholders::_1, ButtonAction::WATCH));
		m_next->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenAward::onButtonClick, this, std::placeholders::_1, ButtonAction::NEXT));
		m_ad->subscribeEvent(EventButtonClick, std::bind(&gui_pixelChickenAward::onButtonClick, this, std::placeholders::_1, ButtonAction::AD));

		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_pixelChickenAward::onVideoAdVisible, this, std::placeholders::_1)));
	}


	void gui_pixelChickenAward::onUpdate(ui32 nTimeElapse)
	{
		if (!isShown())
		{
			return;
		}
		if (m_show_effect_time > 0)
		{
			m_show_effect_time -= nTimeElapse;
			if (m_show_effect_time <= 0)
			{
				m_effect->SetVisible(false);
				getWindow<GUILayout>("PixelChickenAward-Bg")->SetVisible(true);
				m_has_show = true;
			}
		}
	}

	bool gui_pixelChickenAward::showChickenAward(const String & data)
	{
		if (data.length() == 0)
			return false;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return false;
		}

		int rank = doc->FindMember("Rank")->value.GetInt();
		int total = doc->FindMember("Total")->value.GetInt();

		m_rank->SetText(StringUtil::Format("%d", rank).c_str());
		m_name->SetText(doc->FindMember("Name")->value.GetString());
		m_rank_total->SetText(StringUtil::Format("%d/%d", rank, total).c_str());
		m_kill->SetText(doc->FindMember("Kill")->value.GetString());
		auto isWin = doc->FindMember("Win")->value.GetBool();
		if (isWin)
			m_player_title->SetText(LanguageManager::Instance()->getString("pixel_chicken_player_win").c_str());
		else
			m_player_title->SetText(LanguageManager::Instance()->getString("pixel_chicken_player_lose").c_str());
		m_honor->SetImage(doc->FindMember("Honor")->value.GetString());
		m_gold_num->SetText(doc->FindMember("Gold")->value.GetString());
		m_key_num->SetText(doc->FindMember("Key")->value.GetString());
		m_exp_num->SetText(doc->FindMember("Exp")->value.GetString());
		m_honor_num->SetText(doc->FindMember("HonorNum")->value.GetString());
		m_login_num->SetText(doc->FindMember("LoginNum")->value.GetString());
		auto show_ad = doc->HasMember("Show_Ad") ? doc->FindMember("Show_Ad")->value.GetBool() : false;
		m_ad->SetVisible(show_ad && m_have_ad && GameClient::CGame::Instance()->isEnableIndie());
		showResultCup(isWin);
		return true;
	}

	bool gui_pixelChickenAward::onButtonClick(const EventArgs & events, ButtonAction action)
	{
		playSoundByType(ST_Click);

		switch (action)
		{
		case ButtonAction::EXIT:
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunBackHall();
			break;
		case ButtonAction::WATCH:
			getParent()->showMainControl();
			ShowChickenWatchEvent::emit();
			break;
		case ButtonAction::NEXT:
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunPlayAgain();
			break;
		case ButtonAction::AD:
			GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::GAME);
			m_ad->SetVisible(false);
			break;
		default:
			break;
		}
		return false;
	}

	void gui_pixelChickenAward::onGetCommonData(String key, String data)
	{
		if (key == "ChickenResultData")
		{
			getParent()->showPixelChickenResult();
			showChickenAward(data);
			return;
		}
		if (key == "ShowChickenResult")
		{
			if (m_has_show)
				getParent()->showPixelChickenResult();
			return;
		}
	}

	bool gui_pixelChickenAward::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunChicken);
	}

	bool gui_pixelChickenAward::onVideoAdVisible(bool visible)
	{
		m_have_ad = visible;
		return true;
	}

	void gui_pixelChickenAward::showResultCup(bool isWin)
	{
		if (isWin)
		{
			playSoundByType(ST_GameCompleteWin);
			m_effect->SetEffectName("129_victory.effect");
		}
		else
		{
			playSoundByType(ST_GameCompleteLose);
			m_effect->SetEffectName("128_defeat.effect");
		}
		m_effect->SetVisible(true);
		getWindow<GUILayout>("PixelChickenAward-Bg")->SetVisible(false);
		m_show_effect_time = 4000;
	}
}