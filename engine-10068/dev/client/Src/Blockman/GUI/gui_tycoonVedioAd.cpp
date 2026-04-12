#include "gui_tycoonVedioAd.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "ShellInterface.h"
#include "game.h"
#include "UI/GUIStaticImage.h"

namespace BLOCKMAN
{
	gui_tycoonVedioAd::gui_tycoonVedioAd() :
		gui_layout("TycoonVedioAd.json")
	{
	}

	gui_tycoonVedioAd::~gui_tycoonVedioAd()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_tycoonVedioAd::onLoad()
	{
		getWindow("TycoonVedioAd-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_tycoonVedioAd::onCloseClick, this, std::placeholders::_1));
		getWindow("TycoonVedioAd-Watch")->subscribeEvent(EventButtonClick, std::bind(&gui_tycoonVedioAd::onWatchClick, this, std::placeholders::_1));
	
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_tycoonVedioAd::onVideoAdVisible, this, std::placeholders::_1)));
	}

	bool gui_tycoonVedioAd::isNeedLoad()
	{
		return checkGameType(ClientGameType::Tycoon);
	}

	void gui_tycoonVedioAd::onGetCommonData(String key, String data)
	{
		auto builder = CommonDataBuilder().fromData(data);
		String title = builder.getStringParam("title");
		String hint = builder.getStringParam("hint");
		String icon = builder.getStringParam("icon");
		String count = builder.getStringParam("count");
		String btnText = builder.getStringParam("btnText");
		m_ad_type = builder.getIntParam("AdType");
		if (m_ad_type == (int)AdType::TYCOON_MONEY)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("moneyAD_apply", GameClient::CGame::Instance()->getGameType());
		}
		if (m_ad_type == (int)AdType::TYCOON_BUILDING)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("buildAD_apply", GameClient::CGame::Instance()->getGameType());
		}
		if (!m_have_ad)
			return;
		getWindow("TycoonVedioAd-Title")->SetText(getString(title));
		getWindow("TycoonVedioAd-Hint")->SetText(getString(hint));
		getWindow<GUIStaticImage>("TycoonVedioAd-Reward")->SetImage(icon.c_str());
		getWindow("TycoonVedioAd-Reward-Text")->SetText(count.c_str());
		getWindow("TycoonVedioAd-Watch")->SetText(getString(btnText));
		getParent()->showWindowByLayoutId(RootGuiLayout::LayoutId::TYCOON_VEDIO_AD);
		if (m_ad_type == (int)AdType::TYCOON_MONEY)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("moneyAD_ask", GameClient::CGame::Instance()->getGameType());
		}
		if (m_ad_type == (int)AdType::TYCOON_BUILDING)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("buildAD_ask", GameClient::CGame::Instance()->getGameType());
		}
	}

	bool gui_tycoonVedioAd::onVideoAdVisible(bool visible)
	{
		m_have_ad = visible;
		return true;
	}

	bool gui_tycoonVedioAd::onCloseClick(const EventArgs & events)
	{
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::TYCOON_VEDIO_AD);
		return true;
	}

	bool gui_tycoonVedioAd::onWatchClick(const EventArgs & events)
	{
		if (m_ad_type == 0)
			return false;
		GameClient::CGame::Instance()->getShellInterface()->onWatchAd(m_ad_type);
		getParent()->hideWindowByLayoutId(RootGuiLayout::LayoutId::TYCOON_VEDIO_AD);
		if (m_ad_type == (int)AdType::TYCOON_MONEY)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("click_moneyAD", GameClient::CGame::Instance()->getGameType());
		}
		if (m_ad_type == (int)AdType::TYCOON_BUILDING)
		{
			GameClient::CGame::Instance()->getShellInterface()->
				dataReport("click_buildAD", GameClient::CGame::Instance()->getGameType());
		}
		return true;
	}

}
