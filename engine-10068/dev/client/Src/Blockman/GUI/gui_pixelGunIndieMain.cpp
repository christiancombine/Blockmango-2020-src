#include "gui_pixelGunIndieMain.h"
#include "Util/ClientEvents.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cWorld/Blockman.h"
#include "Inventory/Wallet.h"
#include "Util/LanguageKey.h"
#include "Util/FriendManager.h"
#include "Setting/SeasonSetting.h"
#include "Setting/TaskSystemSetting.h"
#include "Network/ClientHttpRequest.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Enums/Enums.h"
#include "ShellInterface.h"
#include "game.h"
#include "GUI/RootGuiLayout.h"
#include "UI/GUIImagesetManager.h"
#include "UI/GUIButton.h"

namespace BLOCKMAN
{
	gui_pixelGunIndieMain::gui_pixelGunIndieMain()
		: gui_layout("PixelGunIndieMain.json")
	{
		m_pixel_ad_info = m_pixel_ad_info ? m_pixel_ad_info : LordNew GUIPixelAdInfo;
	}

	gui_pixelGunIndieMain::~gui_pixelGunIndieMain()
	{
		m_subscriptionGuard.unsubscribeAll();
		LordSafeDelete(m_pixel_ad_info);
	}

	void gui_pixelGunIndieMain::onLoad()
	{
		GUIImagesetManager::Instance()->CreateFromFile("pixel_gun_indie_main.json");

		m_player_level = getWindow<GUIStaticText>("PixelGunIndieMain-Player-Level-Text");
		m_player_exp = getWindow<GUIProgressBar>("PixelGunIndieMain-Player-Exp");
		m_player_exp_text = getWindow<GUIStaticText>("PixelGunIndieMain-Player-Exp-Text");
		m_player_hp_text = getWindow<GUIStaticText>("PixelGunIndieMain-Player-Hp-Text");
		m_player_defense_text = getWindow<GUIStaticText>("PixelGunIndieMain-Player-Defense-Text");
		m_vip_time = getWindow<GUIStaticImage>("PixelGunIndieMain-Vip-Time");
		m_vip_time_text = getWindow<GUIStaticText>("PixelGunIndieMain-Vip-Time-Text");
		m_currency_text = getWindow<GUIStaticText>("PixelGunIndieMain-Currency-Text");
		m_diamondGolds_text = getWindow<GUIStaticText>("PixelGunIndieMain-Golds-Cube-Text");
		m_key_text = getWindow<GUIStaticText>("PixelGunIndieMain-Key-Text");
		m_task_red_point = getWindow<GUIStaticImage>("PixelGunIndieMain-Task-Red-Point");
		m_task_finished_count = getWindow<GUIStaticText>("PixelGunIndieMain-Task-Finished-Count");
		m_rank_icon = getWindow<GUIStaticImage>("PixelGunIndieMain-Rank-Icon");
		m_rank_num = getWindow<GUIStaticText>("PixelGunIndieMain-Rank-Num");
		m_honor_text = getWindow<GUIStaticText>("PixelGunIndieMain-Honor-Text");
		m_player_model = getWindow<GuiActorWindow>("PixelGunIndieMain-Player-Model");
		m_gift_bag_text = getWindow<GUIStaticText>("PixelGunIndieMain-Time");
		m_store_red_point = getWindow<GUIStaticImage>("PixelGunIndieMain-Gun-Store-Red-Point");
		m_defense_red_point = getWindow<GUIStaticImage>("PixelGunIndieMain-Defense-Icon-Red-Point");

		GUILayout* IndieMain = getWindow<GUILayout>("PixelGunIndieMain");
		m_pixel_ad_info->attachTo(this, IndieMain, false);

		getWindow("PixelGunIndieMain-Player-Name-Text")->SetText(GameClient::CGame::Instance()->getPlayerName().c_str());
		getWindow("PixelGunIndieMain-Setting-Text")->SetText(getString("gui.setting").c_str());
		getWindow("PixelGunIndieMain-Email-Text")->SetText(getString("gui_ranch_community_tab_mail").c_str());
		getWindow("PixelGunIndieMain-Friend-Text")->SetText(getString("gui_ranch_community_tab_friend").c_str());
		getWindow("PixelGunIndieMain-Free-Cube-Text")->SetText(getString("gui.free.cube").c_str());
		getWindow("PixelGunIndieMain-Gun-Store-Text")->SetText(getString("gui.gun.store.name").c_str());
		getWindow("PixelGunIndieMain-App-Shop-Text")->SetText(getString("gui_str_app_shop_shop").c_str());
		getWindow("PixelGunIndieMain-Task-Text")->SetText(getString("gui_tasksystem_title").c_str());
		getWindow("PixelGunIndieMain-Rank-Text")->SetText(getString("gui_rank_default_title_name").c_str());
		getWindow("PixelGunIndieMain-Defense-Text")->SetText(getString("gui_hall_armor_title").c_str());
		getWindow("PixelGunIndieMain-Dress-Text")->SetText(getString("gui.dress.text").c_str());
		getWindow("PixelGunIndieMain-More-Game-Text")->SetText(getString("gui.more.game").c_str());
		getWindow("PixelGunIndieMain-Fighting-Text")->SetText(getString("gui_pixel_gunhall_btn_fight_txt").c_str());
		getWindow("PixelGunIndieMain-Free-Vip-Text")->SetText(getString("pixel_gun_free_vip_ad").c_str());
		getWindow("PixelGunIndieMain-Free-Chest-Text")->SetText(getString("pixel_gun_free_chest_ad").c_str());

		getWindow<GUIButton>("PixelGunIndieMain-Setting")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onSettingClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Email")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onEmailClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Friend")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onFriendClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Add-Cube")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onAddCubeClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Free-Cube")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onFreeCubeClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Gun-Store")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onGunStoreClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-App-Shop")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onAppShopClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Task")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onTaskClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Rank")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onRankClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Defense")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onDefenseClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Dress")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onDressClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Chest-Lottery")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onChestLotteryClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-More-Game")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onMoreGameClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Fighting")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onFightingClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Gift-Bag")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onGiftBagClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Free-Vip")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onFreeVipClick, this, std::placeholders::_1));
		getWindow<GUIButton>("PixelGunIndieMain-Free-Chest")->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunIndieMain::onFreeChestClick, this, std::placeholders::_1));

		getWindow<GUIButton>("PixelGunIndieMain-Free-Cube")->SetVisible(false);
		getWindow<GuiActorWindow>("PixelGunIndieMain-Chest-Lottery-Actor")->SetActor("g1042_indie_chest.actor", "idle", 25.0f);

		m_subscriptionGuard.add(ShowPixelGunHallInfo::subscribe(std::bind(&gui_pixelGunIndieMain::showPixelGunHallInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5)));
		m_subscriptionGuard.add(ChangeDefenseEvent::subscribe(std::bind(&gui_pixelGunIndieMain::showPlayerDefense, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateCurrentSeasonInfoEvent::subscribe(std::bind(&gui_pixelGunIndieMain::showPlayerHonorInfo, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowPixelGunTaskIconEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onUpdatePixekGunTaskCount, this)));
		m_subscriptionGuard.add(UserActorChangeEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onPlayerModelChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdatePixelGunVipTimeEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onUpdateVipTime, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdatePlayerDressEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onUpdatePlayerModel, this)));
		m_subscriptionGuard.add(UpdatePixelGunGiftRefreshTimeEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onUpdateGiftRefreshTime, this, std::placeholders::_1)));
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onCubeAdVisible, this, std::placeholders::_1)));
		m_subscriptionGuard.add(StoreUpgradeCompleteEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onStoreUpgradeComplete, this, std::placeholders::_1)));
		m_subscriptionGuard.add(HideStoreUpgradeRedPointEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onHideStoreUpgradeRedPoint, this)));
		m_subscriptionGuard.add(DefenseUpgradeCompleteEvent::subscribe(std::bind(&gui_pixelGunIndieMain::onDefenseUpgradeComplete, this)));

		onUpdatePlayerModel();
	}

	void gui_pixelGunIndieMain::onUpdate(ui32 nTimeElapse)
	{
		auto player = Blockman::Instance() ? Blockman::Instance()->m_pPlayer : nullptr;
		if (player)
		{
			Wallet * wallet = player->m_wallet;
			if (wallet)
			{
				m_currency_text->SetText(StringUtil::ToString(wallet->getCurrency()).c_str());
				m_diamondGolds_text->SetText(StringUtil::ToString(wallet->getDiamondsGolds()).c_str());
			}
			float maxHp = player->getMaxHealth();
			m_player_hp_text->SetText(StringUtil::Format("%d/%d", (int)Math::Floor(maxHp + 0.5f), (int)Math::Floor(maxHp + 0.5f)).c_str());
		}
		if (m_vip_time_mills > 0)
		{
			m_vip_time->SetVisible(true);
			m_vip_time_mills -= nTimeElapse;
			i64 seconds = m_vip_time_mills / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			i64 hour = seconds / 3600;
			if (hour >= 24)
			{
				m_vip_time_text->SetText("> 24h");
			}
			else
			{
				m_vip_time_text->SetText(StringUtil::Format("%s:%s:%s",
					hour < 10 ? StringUtil::Format("0%s", StringUtil::ToString(hour).c_str()).c_str() : StringUtil::ToString(hour).c_str(),
					minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
					second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
			}	
		}
		else
		{
			m_vip_time->SetVisible(false);
		}

		if (m_player_model && RootGuiLayout::Instance())
		{
			m_player_model->SetVisible(!RootGuiLayout::Instance()->isShowTaskSystem());
		}
		
		if (m_gift_bag_time > 0)
		{
			m_gift_bag_time -= nTimeElapse;
			if (m_gift_bag_time <= 0)
			{
				m_gift_bag_time = 0;
				RootGuiLayout::Instance()->showPixelGunGiftBagStore(true);
			}
			onUpdateGiftRefreshTime(m_gift_bag_time);
		}
	}

	bool gui_pixelGunIndieMain::isNeedLoad()
	{
		if (!GameClient::CGame::Instance()->isEnableIndie())
		{
			return false;
		}
		return checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_pixelGunIndieMain::onGetCommonData(String key, String data)
	{
		if (key == "HallInfo")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int level = builder.getIntParam("level");
			int curExp = builder.getIntParam("curExp");
			int maxExp = builder.getIntParam("maxExp");
			bool isMax = builder.getBoolParam("isMax");
			int key = builder.getIntParam("key");
			int vipTime = builder.getIntParam("vipTime");
			UpdatePixelGunVipTimeEvent::emit(vipTime);
			ShowPixelGunHallInfo::emit(level, curExp, maxExp, key, isMax);
		}
		if (key == "Chest")
        {
        	m_pixel_ad_info->resetChest(data);
        	return;
        }
        if (key == "Vip")
        {
        	m_pixel_ad_info->resetVip(data);
        }
	}

	bool gui_pixelGunIndieMain::onSettingClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_SETTING);
		return true;
	}

	bool gui_pixelGunIndieMain::onEmailClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_EMAIL);
		return true;
	}

	bool gui_pixelGunIndieMain::onFriendClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_FRIEND);
		return true;
	}

	bool gui_pixelGunIndieMain::onFreeCubeClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_FREE_CUBE);
		return true;
	}

	bool gui_pixelGunIndieMain::onAddCubeClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_ADD_CUBE);
		return true;
	}

	bool gui_pixelGunIndieMain::onGunStoreClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showGunStore();
		return true;
	}

	bool gui_pixelGunIndieMain::onAppShopClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showAppShop();
		return true;
	}

	bool gui_pixelGunIndieMain::onTaskClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showTaskSystem();
		return true;
	}

	bool gui_pixelGunIndieMain::onRankClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showSeasonRank();
		return true;
	}

	bool gui_pixelGunIndieMain::onDefenseClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showPixelGunHallArmorUpgrade();
		if (m_defense_red_point->IsVisible())
		{
			m_defense_red_point->SetVisible(false);
		}
		return true;
	}

	bool gui_pixelGunIndieMain::onDressClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_DRESS);
		return true;
	}

	bool gui_pixelGunIndieMain::onChestLotteryClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showChestLottery();
		return true;
	}

	bool gui_pixelGunIndieMain::onMoreGameClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		GameClient::CGame::Instance()->getShellInterface()->
			onAppActionTrigger((int)AppActionType::APP_ACTION_MORE_GAME);
		return true;
	}

	bool gui_pixelGunIndieMain::onFightingClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showPixelGunHallModeSelect();
		return true;
	}

	bool gui_pixelGunIndieMain::onGiftBagClick(const EventArgs & event)
	{
		playSoundByType(ST_Click);
		getParent()->showPixelGunGiftBagStore(true);
		return true;
	}

	bool gui_pixelGunIndieMain::showPixelGunHallInfo(int lv, int cur_exp, int max_exp, int yaoshi, bool is_max)
	{
		m_player_level->SetText(StringUtil::Format("%d", lv).c_str());
		if (!is_max)
		{
			if (max_exp > 0)
			{
				String exp_txt = StringUtil::Format("%d/%d", cur_exp, max_exp);
				m_player_exp_text->SetText(exp_txt.c_str());
				m_player_exp->SetProgress((float)cur_exp / (float)max_exp);
			}
			else
			{
				m_player_exp->SetProgress(0.f);
			}
		}
		else
		{
			m_player_exp->SetProgress(1.f);
			m_player_exp_text->SetText("MAX");
		}
		m_key_text->SetText(StringUtil::Format("%d", yaoshi).c_str());
		UpdateLotteryChestKeyEvent::emit(yaoshi);
		return true;
	}

	bool gui_pixelGunIndieMain::showPlayerDefense(int curDefense, int maxDefense)
	{
		m_player_defense_text->SetText(StringUtil::Format("%d/%d", curDefense, maxDefense).c_str());
		return true;
	}

	bool gui_pixelGunIndieMain::showPlayerHonorInfo(int honorId, int rank, int honor, int endTime)
	{
		m_honor_text->SetText(StringUtil::Format("%d", honor).c_str());
		m_rank_num->SetText(StringUtil::Format("%d", rank).c_str());
		m_rank_num->SetVisible(rank <= 99);
		Season* season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		if (season)
		{
			m_rank_icon->SetImage(season->HonorIcon.c_str());
		}
		return true;
	}

	bool gui_pixelGunIndieMain::onUpdatePixekGunTaskCount()
	{
		int count = TaskSystemSetting::getWaitRewardCount();
		m_task_red_point->SetVisible(count > 0);
		m_task_finished_count->SetVisible(count > 0);
		m_task_finished_count->SetText(StringUtil::ToString(count).c_str());
		return true;
	}

	bool gui_pixelGunIndieMain::onUpdatePlayerModel()
	{
		ClientHttpRequest::loadUserInfo(GameClient::CGame::Instance()->getPlatformUserId());
		return true;
	}

	bool gui_pixelGunIndieMain::onPlayerModelChange(i64 userId)
	{
		if (GameClient::CGame::Instance()->getPlatformUserId() != userId)
		{
			return false;
		}
		UserActor* user = FriendManager::findUserActor(userId);
		if (user)
		{
			m_player_model->RemoveActor();
			m_player_model->SetActor(user->sex == 1 ? "boy.actor" : "girl.actor", "idle");
			m_player_model->SetCustomColor(user->actorColor);
			for (const auto& item : user->items)
			{
				if (item.resName.length() > 0 && item.resId.length() > 0 && item.resId != "0")
				{
					m_player_model->UseBodyPart(item.resName, item.resId);
				}
			}
		}
		return true;
	}

	bool gui_pixelGunIndieMain::onUpdateVipTime(int vip_time)
	{
		i64 seconds = (i64)vip_time;
		m_vip_time_mills = seconds * 1000;
		return true;
	}

	bool gui_pixelGunIndieMain::onUpdateGiftRefreshTime(int time)
	{
		if (time > 0)
		{
			m_gift_bag_time = time;
			int seconds = m_gift_bag_time / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			m_gift_bag_text->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
			return true;
		}
		return false;
	}

	bool gui_pixelGunIndieMain::onCubeAdVisible(bool visible)
	{
		if (!isLoaded())
			return false;
		getWindow<GUIButton>("PixelGunIndieMain-Free-Cube")->SetVisible(visible);
		return true;
	}
}