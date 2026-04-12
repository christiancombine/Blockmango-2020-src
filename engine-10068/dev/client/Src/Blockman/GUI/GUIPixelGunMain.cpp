#include "GUIPixelGunMain.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/TaskSystemSetting.h"
#include "game.h"
#include "ShellInterface.h"

namespace BLOCKMAN
{
	GUIPixelGunMain::GUIPixelGunMain() :
		gui_layout("PixelGunMain.json")
	{
	}

	GUIPixelGunMain::~GUIPixelGunMain()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIPixelGunMain::onLoad()
	{
		auto FreeVip = getWindow<GUIButton>("PixelGunMain-Free-Vip");
		FreeVip->SetVisible(false);
		FreeVip->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunMain::onFreeVipClick, this, std::placeholders::_1));
		getWindow("PixelGunMain-Free-Vip-Text")->SetText(getString("gui.ad.free.vip"));

		auto FreeGold = getWindow<GUIButton>("PixelGunMain-Free-Gold");
		FreeGold->SetVisible(false);
		FreeGold->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunMain::onFreeGoldClick, this, std::placeholders::_1));
		getWindow("PixelGunMain-Free-Gold-Text")->SetText(getString("gui.ad.free.gold"));
		
		auto Reward = getWindow("PixelGunMain-Ad-Reward");
		Reward->subscribeEvent(EventWindowTouchUp, std::bind(&GUIPixelGunMain::onHideRewardResult, this, std::placeholders::_1));
		Reward->SetVisible(false);
		
		auto Task = getWindow<GUIButton>("PixelGunMain-Task");
		Task->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunMain::onOpenPixelGunTask, this, std::placeholders::_1));

		auto GiftBag = getWindow<GUIButton>("PixelGunMain-Gift-Bag");
		GiftBag->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunMain::onOpenPixelGunGiftBag, this, std::placeholders::_1));

		m_task_red_point = getWindow<GUIStaticImage>("PixelGunMain-Task-Red-Point");
		m_task_count = getWindow<GUIStaticText>("PixelGunMain-Task-Count");

		m_menu = getWindow<GUILayout>("PixelGunMain-Menu");
		m_menu->SetVisible(false);

		m_vip_time = getWindow<GUIStaticImage>("PixelGunMain-Vip-Time");
		m_vip_time_text = getWindow<GUIStaticText>("PixelGunMain-Vip-Time-Text");

		m_subscriptionGuard.add(ShowPixelGunTaskIconEvent::subscribe([this]() -> bool {
			m_menu->SetVisible(true);
			onUpdatePixekGunTaskCount();
			return true;
		}));
		m_subscriptionGuard.add(UpdateGoldAdInfoEvent::subscribe(std::bind(&GUIPixelGunMain::onSyncGoldAdInfo, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowGoldAdRewardEvent::subscribe(std::bind(&GUIPixelGunMain::onShowGoldReward, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(UpdatePixelGunGiftRefreshTimeEvent::subscribe(std::bind(&GUIPixelGunMain::onUpdateGiftRefreshTime, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdatePixelGunVipTimeEvent::subscribe(std::bind(&GUIPixelGunMain::onUpdateVipTime, this, std::placeholders::_1)));
	}

	void GUIPixelGunMain::onUpdate(ui32 nTimeElapse)
	{
		if (m_goldAdTime > 0)
		{
			m_goldAdTime -= nTimeElapse;
		}
		if (m_adRewardTime > 0)
		{
			m_adRewardTime -= nTimeElapse;
			if (m_adRewardTime <= 0)
			{
				m_adRewardTime = 0;
				onHideRewardResult(EventArgs());
			}
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
	}

	bool GUIPixelGunMain::onFreeVipClick(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showAppShop(0);
		return true;
	}

	bool GUIPixelGunMain::onFreeGoldClick(const EventArgs & events)
	{
		if (m_goldAdTimes > 0)
		{
			if (m_goldAdTime <= 0)
			{
				GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::GOLD);
			}
			else
			{
				ChatMessageReceiveEvent::emit(getString("gui.ad.loading.hint").c_str(),false);
			}
		}
		else
		{
			ChatMessageReceiveEvent::emit(getString("gui.ad.used.up").c_str(),false);
		}
		return true;
	}

	bool GUIPixelGunMain::onHideRewardResult(const EventArgs & events)
	{
		auto Reward = getWindow("PixelGunMain-Ad-Reward");
		Reward->SetVisible(false);
		return true;
	}

	bool GUIPixelGunMain::onShowGoldReward(int golds, int times, int maxTimes)
	{
		m_adRewardTime = 20000;
		auto Reward = getWindow("PixelGunMain-Ad-Reward");
		Reward->SetVisible(true);
		auto RewardIcon = getWindow<GUIStaticImage>("PixelGunMain-Ad-Reward-Icon");
		RewardIcon->SetImage("set:pixel_gun_ad.json image:ic_gold");
		auto RewardName = getWindow("PixelGunMain-Ad-Reward-Name");
		RewardName->SetText(StringUtil::Format(getString("gui.gold.ad.reward").c_str(), golds).c_str());
		auto RewardTimes = getWindow("PixelGunMain-Ad-Reward-Times");
		RewardTimes->SetText(StringUtil::Format(getString("gui.ad.reward.times").c_str(), times, maxTimes).c_str());
		return true;
	}

	bool GUIPixelGunMain::onOpenPixelGunTask(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->showTaskSystem();
		return true;
	}

	bool GUIPixelGunMain::onOpenPixelGunGiftBag(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		RootGuiLayout::Instance()->showPixelGunGiftBagStore(true);
		return true;
	}

	bool GUIPixelGunMain::onUpdatePixekGunTaskCount()
	{
		int count = TaskSystemSetting::getWaitRewardCount();
		m_task_red_point->SetVisible(count > 0);
		m_task_count->SetVisible(count > 0);
		m_task_count->SetText(StringUtil::ToString(count).c_str());
		return true;
	}

	bool GUIPixelGunMain::onSyncGoldAdInfo(int times, int seconds)
	{
		m_goldAdTimes = times;
		m_goldAdTime = seconds * 1000;
		return true;
	}

	bool GUIPixelGunMain::onUpdateGiftRefreshTime(int time)
	{
		auto TimeText = getWindow<GUIStaticText>("PixelGunMain-Gift-Bag-Time");
		if (time > 0)
		{
			m_gift_bag_time = time;
			int seconds = m_gift_bag_time / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			TimeText->SetText(StringUtil::Format("%s:%s:%s",
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
			return true;
		}
		return false;
	}

	bool GUIPixelGunMain::onUpdateVipTime(int vip_time)
	{
		i64 seconds = (i64)vip_time;
		m_vip_time_mills = seconds * 1000;
		m_vip_time->SetVisible(m_vip_time_mills > 0);
		return true;
	}
}
