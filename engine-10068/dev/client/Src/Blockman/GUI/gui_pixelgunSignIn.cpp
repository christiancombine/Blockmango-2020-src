#include "gui_pixelgunSignIn.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "GUI/GUIPixelGunSignInItem.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{
	gui_pixelgunSignIn::gui_pixelgunSignIn() :
		gui_layout("PixelGunSignIn.json")
	{
	}

	gui_pixelgunSignIn::~gui_pixelgunSignIn()
	{
		m_subscriptionGuard.unsubscribeAll();
		SignInSetting::unloadSetting();
	}

	void gui_pixelgunSignIn::onLoad()
	{
		SignInSetting::loadSetting();

		getWindow("PixelGunSignIn-Title")->SetText(getString("pixel_gun_sign_in"));
		m_items = getWindow<GUILayout>("PixelGunSignIn-Items");

		auto Button = getWindow<GUIButton>("PixelGunSignIn-Button");
		Button->subscribeEvent(EventButtonClick, std::bind(&gui_pixelgunSignIn::onReceivedClick, this, std::placeholders::_1));
		auto BtnText = getWindow<GUIStaticText>("PixelGunSignIn-BtnText");
		BtnText->SetText(getString("task_receive"));

	}

	bool gui_pixelgunSignIn::isNeedLoad()
	{
		return checkGameType(ClientGameType::PixelGunHall);
	}

	void gui_pixelgunSignIn::onGetCommonData(String key, String data)
	{
		if (key == "SignInData")
		{
			updateData(data);
		}
	}

	void gui_pixelgunSignIn::onShow(int startOrder, int nowOrder, ItemStatus status)
	{
		map<int, SignInItem*>::type data = SignInSetting::getShowItems(startOrder, nowOrder, status);
		m_items->CleanupChildren();

		SignInItem* data_item = NULL;
		auto side = m_items->GetPixelSize();
		float DistanceX = 25.0f;
		float DistanceY = 15.0f;
		float itemWidth = (side.x - (DistanceX * 5.0f)) / 4;
		float itemHeight = (side.y - DistanceY * 3.0f) / 2;
		int Room = 0;
		for (auto iter = data.begin(); iter != data.end();)
		{
			int Room = iter->first;
			data_item = iter->second;
			if (nowOrder == data_item->order)
			{
				current_item = data_item;
			}
			String ItemName = StringUtil::Format("PixelGunSignIn-Items-%d", iter->first).c_str();
			GUIPixelGunSignInItem* item = (GUIPixelGunSignInItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_PIXEL_GUN_SIGN_IN_ITEM, ItemName.c_str());

			item->SetSize({ {0.0f, itemWidth + (itemWidth + DistanceX) * (data_item->side - 1)}, {0.0f, itemHeight} });
			item->SetXPosition({ 0.0f, DistanceX + (DistanceX + itemWidth) * (Room % 4) });
			item->SetYPosition({ 0.0f, DistanceY + (DistanceY + itemHeight) * (Room / 4) });
			item->setSignInItem(iter->second);
			m_items->AddChildWindow(item);
			iter++;
		}
		RootGuiLayout::Instance()->showPixelGunSignIn();
	}


	bool gui_pixelgunSignIn::updateData(const String & data)
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
		auto StartOrder = doc->FindMember("StartOrder")->value.GetInt();
		auto NowOrder = doc->FindMember("NowOrder")->value.GetInt();
		auto Status = doc->FindMember("Status")->value.GetInt();

		LordDelete(doc);
		onShow(StartOrder, NowOrder, (ItemStatus)Status);
		return true;
	}

	bool gui_pixelgunSignIn::onShowWindow()
	{
		RootGuiLayout::Instance()->showPixelGunSignIn();
		return true;
	}

	bool gui_pixelgunSignIn::onReceivedClick(const EventArgs & events)
	{
		if (current_item && current_item->status == ItemStatus::ABLE_RECEIVED)
		{
			playSoundByType(ST_Click);
			ClientNetwork::Instance()->getCommonSender()->sendPixelGunReceiveSignIn(current_item->id);
			ShowChestRewardResultEvent::emit(current_item->rewardId);
			return true;
		}
		getParent()->showMainControl();
		return true;
	}


}
