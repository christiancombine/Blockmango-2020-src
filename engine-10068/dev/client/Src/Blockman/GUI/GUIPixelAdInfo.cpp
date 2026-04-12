#include "GUIPixelAdInfo.h"
#include "GUI/GuiDef.h"
#include "Util/LanguageKey.h"
#include "Util/ClientEvents.h"
#include "ShellInterface.h"
#include "game.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Setting/RewardSetting.h"

namespace BLOCKMAN
{
	GUIPixelAdInfo::GUIPixelAdInfo() :
		gui_layout("PixelAdInfo.json")
	{
	}

	GUIPixelAdInfo::~GUIPixelAdInfo()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void GUIPixelAdInfo::onLoad()
	{
		m_title = getWindow<GUIStaticText>("PixelAdInfo-Title");

		m_free_chest = getWindow<GUILayout>("PixelAdInfo-Free-Chest");
		m_chest_icon = getWindow<GUIStaticImage>("PixelAdInfo-Chest-Icon");

		m_free_vip = getWindow<GUILayout>("PixelAdInfo-Free-Vip");
		m_vip_title = getWindow<GUIStaticText>("PixelAdInfo-Vip-Title");
		m_vip_icon = getWindow<GUIStaticImage>("PixelAdInfo-Vip-Icon");
		m_vip_name = getWindow<GUIStaticText>("PixelAdInfo-Vip-Name");
		m_vip_desc = getWindow<GUIStaticText>("PixelAdInfo-Vip-Desc");
		m_vip_time_text = getWindow<GUIStaticText>("PixelAdInfo-Vip-Time");
		m_vip_times_text = getWindow<GUIStaticText>("PixelAdInfo-Vip-Times");

		auto ChestTip = getWindow<GUIStaticText>("PixelAdInfo-Chest-Tip");
		auto VipTip = getWindow<GUIStaticText>("PixelAdInfo-Vip-Tip");

		ChestTip->SetText(getString("pixel_chest_tip").c_str());
		VipTip->SetText(getString("pixel_vip_tip").c_str());

		auto Confirm = getWindow<GUIButton>("PixelAdInfo-Chest-Confirm");
		auto Cancel = getWindow<GUIButton>("PixelAdInfo-Chest-Cancel");
		auto AdBtn = getWindow<GUIButton>("PixelAdInfo-Vip-Ad");
		auto Close = getWindow<GUIButton>("PixelAdInfo-Vip-Close");

		Confirm->SetText(getString("pixel_ad_text").c_str());
		Cancel->SetText(getString(LanguageKey::GUI_CANCEL));

		Confirm->subscribeEvent(EventButtonClick, std::bind(&GUIPixelAdInfo::onAdClick, this, std::placeholders::_1, PixelAdType::CHEST));
		Cancel->subscribeEvent(EventButtonClick, std::bind(&GUIPixelAdInfo::onCloseClick, this, std::placeholders::_1));
		AdBtn->subscribeEvent(EventButtonClick, std::bind(&GUIPixelAdInfo::onAdClick, this, std::placeholders::_1, PixelAdType::VIP));
		Close->subscribeEvent(EventButtonClick, std::bind(&GUIPixelAdInfo::onCloseClick, this, std::placeholders::_1));

		if (!m_vip_data.empty())
			resetVip(m_vip_data);

		if (!m_chest_data.empty())
			resetChest(m_chest_data);
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&GUIPixelAdInfo::onVideoAdVisible, this, std::placeholders::_1)));
	}

	void GUIPixelAdInfo::onUpdate(ui32 nTimeElapse)
	{
		if (m_vip_time > 0)
		{
			m_vip_time -= nTimeElapse;
			if (m_vip_time <= 0)
			{
				m_vip_time = 0;
				setButtonStatus();
			}
			setVipTimeText(m_vip_time);
		}

		if (m_chest_time > 0)
		{
			m_chest_time -= nTimeElapse;
			if (m_chest_time <= 0)
			{
				m_chest_time = 0;
				setButtonStatus();
			}
			setChestTimeText(m_chest_time);
		}
	}

	void GUIPixelAdInfo::showAdInfo(int type)
	{
		show();
		switch ((PixelAdType)type)
		{
		case PixelAdType::CHEST:
			m_title->SetText(getString("pixel_chest_title").c_str());
			m_free_chest->SetVisible(true);
			m_free_vip->SetVisible(false);
			break;
		case PixelAdType::VIP:
			m_title->SetText(getString("pixel_vip_title").c_str());
			m_free_chest->SetVisible(false);
			m_free_vip->SetVisible(true);
			break;
		default:
			break;
		}
		setButtonStatus();
	}

	bool GUIPixelAdInfo::onCloseClick(const EventArgs & events)
	{
		hide();
		return true;
	}

	bool GUIPixelAdInfo::onAdClick(const EventArgs & events, PixelAdType type)
	{
		switch (type)
		{
		case PixelAdType::CHEST:
			GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::PIXEL_FREE_CHEST);
			break;
		case PixelAdType::VIP:
			GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::PIXEL_FREE_VIP);
			break;
		default:
			break;
		}
		hide();
		return true;
	}

	void GUIPixelAdInfo::setVipTimeText(int time)
	{
		if (time >= 0)
		{
			auto timeString = getTimeString(time);
			m_vip_time_text->SetText(StringUtil::Format("%s%s",
				getString("pixel_free_vip_time_text").c_str(),
				timeString.c_str()).c_str());
		}
	}

	void GUIPixelAdInfo::setChestTimeText(int time)
	{
		auto Confirm = getWindow<GUIButton>("PixelAdInfo-Chest-Confirm");
		if (time > 0)
		{
			auto timeString = getTimeString(time);
			Confirm->SetText(timeString.c_str());
		}
		else
			Confirm->SetText(getString("pixel_ad_text").c_str());
	}

	void GUIPixelAdInfo::resetChest(String data)
	{
		auto builder = CommonDataBuilder().fromData(data);
		if (!isLoaded())
		{
			m_chest_data = data;
			return;
		}
		m_chest_time = builder.getIntParam("time") * 1000;
		m_chest_times = builder.getIntParam("times");
		int rewardId = builder.getIntParam("rewardId");
		auto reward = RewardSetting::findRewardById(rewardId);
		setButtonStatus();
		if (!reward)
			return;
		m_chest_icon->SetImage(reward->Image.c_str());
	}

	void GUIPixelAdInfo::resetVip(String data)
	{
		auto builder = CommonDataBuilder().fromData(data);
		if (!isLoaded())
		{
			m_vip_data = data;
			return;
		}

		m_vip_time = builder.getIntParam("time") * 1000;
		m_vip_times = builder.getIntParam("times");
		String total = builder.getStringParam("total");
		int rewardId = builder.getIntParam("rewardId");
		auto reward = RewardSetting::findRewardById(rewardId);
		if (m_vip_time == 0)
			setVipTimeText(m_vip_time);
		m_vip_times_text->SetText(StringUtil::Format("%s%s", getString("pixel_free_vip_times_text").c_str(), total.c_str()).c_str());
		setButtonStatus();
		if (!reward)
			return;
		m_vip_icon->SetImage(reward->Image.c_str());
		m_vip_name->SetText(getString(reward->Name).c_str());
		m_vip_desc->SetText(getString(reward->Desc).c_str());
		m_vip_title->SetText(getString(reward->Title).c_str());
	}

	bool GUIPixelAdInfo::onVideoAdVisible(bool visible)
	{
		m_have_ad = visible;
		return false;
	}

	void GUIPixelAdInfo::setButtonStatus()
	{
		auto Confirm = getWindow<GUIButton>("PixelAdInfo-Chest-Confirm");
		Confirm->SetEnabled(m_have_ad && m_chest_times > 0 && m_chest_time <= 0);
		auto AdBtn = getWindow<GUIButton>("PixelAdInfo-Vip-Ad");
		AdBtn->SetEnabled(m_have_ad && m_vip_times > 0 && m_vip_time <= 0);
	}

	String GUIPixelAdInfo::getTimeString(int time)
	{
		if (time < 0)
			return "";
		int seconds = time / 1000;
		int second = seconds % 60;
		int minute = (seconds % 3600) / 60;
		int hour = seconds / 3600;
		auto result = StringUtil::Format("%s:%s:%s",
			hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
			minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
			second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str());
		return result;
	}
}
