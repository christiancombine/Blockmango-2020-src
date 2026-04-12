#include "gui_pixelGunGiftBagStore.h"
#include "GUI/GuiDef.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageKey.h"
#include "game.h"
#include "GUIPixelGunGiftBagItem.h"
#include "Setting/PixelGunGiftBagSetting.h"
#include "UI/GUIGridView.h"
#include "ShellInterface.h"
#include "gui_baseMessage.h"
#include "Network/protocol/CommonDataBuilder.h"

namespace BLOCKMAN
{
	gui_pixelGunGiftBagStore::gui_pixelGunGiftBagStore() :
		gui_layout("PixelGunGiftBagStore.json")
	{
	}

	gui_pixelGunGiftBagStore::~gui_pixelGunGiftBagStore()
	{
		m_subscriptionGuard.unsubscribeAll();
		PixelGunGiftBagSetting::unloadSetting();
	}

	void gui_pixelGunGiftBagStore::onLoad()
	{
		PixelGunGiftBagSetting::loadSetting();

		auto Close = getWindow<GUIButton>("PixelGunGiftBagStore-Close");
		Close->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunGiftBagStore::onCloseClick, this, std::placeholders::_1));
		auto ReFreshAd = getWindow<GUIButton>("PixelGunGiftBagStore-Free-Refresh-Ad");
		ReFreshAd->subscribeEvent(EventButtonClick, std::bind(&gui_pixelGunGiftBagStore::onRefreshClick, this, std::placeholders::_1));

		initGiftBags();
	}

	void gui_pixelGunGiftBagStore::onUpdate(ui32 nTimeElapse)
	{
		if (m_giftAdTime > 0)
		{
			m_giftAdTime -= nTimeElapse;
		}
		
		if (m_last_time <= 0)
		{
			return;
		}
		m_last_time -= nTimeElapse;
		
		if (m_last_time < 0)
		{
			m_last_time = 0;
		}
		setRefreshTime(m_last_time);
	}

	bool gui_pixelGunGiftBagStore::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		RootGuiLayout::Instance()->showPixelGunGiftBagStore(false);
		return true;
	}

	bool gui_pixelGunGiftBagStore::onRefreshClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (m_giftAdTimes > 0)
		{
			if (m_giftAdTime <= 0)
			{
				GameClient::CGame::Instance()->getShellInterface()->onWatchAd((int)AdType::GIFT);
			}
			else
			{
				ShowMessageEvent::emit((int)MessageId::MESSAGE_CENTER, getString("gui.ad.loading.hint").c_str(), 3, true);
			}
		}
		else
		{
			ShowMessageEvent::emit((int)MessageId::MESSAGE_CENTER, getString("gui.ad.used.up").c_str(), 3, true);
		}
		return false;
	}

	void gui_pixelGunGiftBagStore::initGiftBags()
	{
		GUILayout* GiftBagPanel = getWindow<GUILayout>("PixelGunGiftBagStore-Gifts");
		if (GiftBagPanel == NULL)
		{
			return;
		}

		GUIStaticText* Title = getWindow<GUIStaticText>("PixelGunGiftBagStore-Title");
		Title->SetText(LanguageManager::Instance()->getString("pixel_gun_gift_title").c_str());

		GUIStaticText* FreeRefreshText = getWindow<GUIStaticText>("PixelGunGiftBagStore-Free-Refresh-Text");
		FreeRefreshText->SetText(LanguageManager::Instance()->getString("pixel_gun_gift_free_refresh").c_str());
		GUIString ListName = StringUtil::Format("PixelGunGiftBagStore-Gifts-List").c_str();
		m_list = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ListName);
		m_list->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_list->InitConfig(6.f, 6.f, 4);
		GiftBagPanel->AddChildWindow(m_list);
	}

	void gui_pixelGunGiftBagStore::showGiftBagStore(list<GiftBag*>::type gifts)
	{
		if (gifts.empty())
		{
			return;
		}

		if (!m_gifts.empty())
		{
			updateGifts(gifts);
			return;
		}

		for (auto gift : gifts)
		{
			createGiftBagItem(gift);
		}
	}

	void gui_pixelGunGiftBagStore::updateGifts(list<GiftBag*>::type gifts)
	{
		size_t count = gifts.size();
		auto iter = gifts.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (iter == gifts.end())
				break;
			GUIPixelGunGiftBagItem* gift = getGiftByIndex(i);
			if (gift)
			{
				gift->InitGiftItem(*iter);
			}
			else
			{
				createGiftBagItem(*iter);
			}
			iter++;
		}
	}

	void gui_pixelGunGiftBagStore::createGiftBagItem(GiftBag * gift)
	{
		static unsigned gift_index = 0;
		gift_index++;
		GUIString GiftName = StringUtil::Format("PixelGunGiftBagStore-Gifts-%d", gift_index).c_str();
		GUIPixelGunGiftBagItem* pGift = (GUIPixelGunGiftBagItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_PIXEL_GUN_GIFT_BAG_ITEM, GiftName);
		pGift->SetSize({ {0.247f, 0.0f}, {1.0f, 0.0f} });
		m_list->AddItem(pGift);
		pGift->InitGift(gift);
		m_gifts.push_back(pGift);
	}

	bool gui_pixelGunGiftBagStore::onUpdateGiftBagData(const String & data, bool isRefresh, bool isShow)
	{
		list<GiftBag*>::type result = PixelGunGiftBagSetting::parseUpdateGiftBagData(data, isRefresh);
		if (result.empty())
		{
			return false;
		}
		showGiftBagStore(result);

		if (isShow && !isShown())
		{
			RootGuiLayout::Instance()->showPixelGunGiftBagStore(true);
		}
		return true;
	}

	bool gui_pixelGunGiftBagStore::onUpdateGiftRefreshTime(int time)
	{
		setRefreshTime(time);
		return true;
	}

	void gui_pixelGunGiftBagStore::setRefreshTime(int time)
	{
		auto RefreshText = getWindow<GUIStaticText>("PixelGunGiftBagStore-Refresh-Text");
		if (time > 0)
		{
			m_last_time = time;
			int seconds = m_last_time / 1000;
			int second = seconds % 60;
			int minute = (seconds % 3600) / 60;
			int hour = seconds / 3600;
			RefreshText->SetText(StringUtil::Format("%s : %s:%s:%s",
				LanguageManager::Instance()->getString("pixel_gun_gift_refresh").c_str(),
				hour < 10 ? StringUtil::Format("0%d", hour).c_str() : StringUtil::ToString(hour).c_str(),
				minute < 10 ? StringUtil::Format("0%d", minute).c_str() : StringUtil::ToString(minute).c_str(),
				second < 10 ? StringUtil::Format("0%d", second).c_str() : StringUtil::ToString(second).c_str()).c_str());
			return;
		}
		ClientNetwork::Instance()->getCommonSender()->sendPixelGunRefreshGiftBag();
	}

	GUIPixelGunGiftBagItem* gui_pixelGunGiftBagStore::getGiftByIndex(int index)
	{

		size_t count = m_gifts.size();
		auto iter = m_gifts.begin();
		for (size_t i = 0; i < count; i++)
		{
			if (i == index)
			{
				return *iter;
			}
			iter++;
		}
		return nullptr;
	}

	bool gui_pixelGunGiftBagStore::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_pixelGunGiftBagStore::onGetCommonData(String key, String data)
	{
		if (key == "GiftBagAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int times = builder.getIntParam("times");
			int seconds = builder.getIntParam("seconds");
			onSyncGiftAdInfo(times, seconds);
			return;
		}

		if (key == "GiftBagData")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int isShow = builder.getBoolParam("isShow");
			int isRefresh = builder.getBoolParam("isRefresh");
			String c_data = builder.getStringParam("data");
			onUpdateGiftBagData(c_data, isRefresh, isShow);
			return;
		}

		if (key == "GiftBagRefreshTime")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int time = builder.getIntParam("time");
			onUpdateGiftRefreshTime(time);
			UpdatePixelGunGiftRefreshTimeEvent::emit(time);
			return;
		}
	}

	bool gui_pixelGunGiftBagStore::onSyncGiftAdInfo(int times, int seconds)
	{
		m_giftAdTimes = times;
		m_giftAdTime = seconds * 1000;
		return true;
	}
}