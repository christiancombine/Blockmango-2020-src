#include "gui_skyBlockSignIn.h"
#include "UI/GUIButton.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiDef.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Inventory/InventoryPlayer.h"

namespace BLOCKMAN
{
	gui_skyblockSignIn::gui_skyblockSignIn() :
		gui_layout("SkyBlockSignIn.json")
	{
		LordNew SkyBlockSignInSetting();
	}

	gui_skyblockSignIn::~gui_skyblockSignIn()
	{
		m_subscriptionGuard.unsubscribeAll();
		SkyBlockSignInSetting::Instance()->unloadSetting();
		LordDelete SkyBlockSignInSetting::Instance();
	}

	void gui_skyblockSignIn::onLoad()
	{
		SkyBlockSignInSetting::Instance()->loadSetting();

		getWindow("SkyBlockSignIn-Title")->SetText(getString("skyblock_sign_in"));
		m_items = getWindow<GUILayout>("SkyBlockSignIn-Items");

		auto Button = getWindow<GUIButton>("SkyBlockSignIn-Button");
		Button->subscribeEvent(EventButtonClick, std::bind(&gui_skyblockSignIn::onReceivedClick, this, std::placeholders::_1));

		auto close_btn = getWindow<GUIButton>("SkyBlockSignIn-Close");
		close_btn->subscribeEvent(EventButtonClick, std::bind(&gui_skyblockSignIn::onCloseClick, this, std::placeholders::_1));

		auto BtnText = getWindow<GUIStaticText>("SkyBlockSignIn-BtnText");
		BtnText->SetText(getString("skyblock_signin_receive"));
	}

	bool gui_skyblockSignIn::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain);
	}

	void gui_skyblockSignIn::onShow(int nowOrder, int status, bool isShow)
	{
		vector<SkyBlockSignInInfo>::type data;
		if (!SkyBlockSignInSetting::Instance()->getShowItems(data, nowOrder, status))
		{
			return;
		}
		m_items->CleanupChildren();

		auto side = m_items->GetPixelSize();
		float DistanceX = 25.0f;
		float DistanceY = 15.0f;
		float itemWidth = (side.x - (DistanceX * 3.0f)) / 4;
		float itemHeight = (side.y - DistanceY * 1.0f) / 2;
		int Room = 0;
		for (auto cur_good : data)
		{
			int Room = cur_good.useRoom;
			//data_item = iter->second;
	
			String ItemName = StringUtil::Format("SkyBlockSignIn-Items-%d", cur_good.id).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(ItemName.c_str()))
			{
				continue;;
			}

			GUISkyBlockSignInItem* item = (GUISkyBlockSignInItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_SIGN_IN_ITEM, ItemName.c_str());

			item->SetSize({ {0.0f, itemWidth + (itemWidth + DistanceX) * (cur_good.side - 1)}, {0.0f, itemHeight} });
			item->SetXPosition({ 0.0f, DistanceX + (DistanceX + itemWidth) * (Room % 4) });
			item->SetYPosition({ 0.0f, DistanceY + (DistanceY + itemHeight) * (Room / 4) });
			item->setSignInItem(cur_good);
			m_items->AddChildWindow(item);
			if (nowOrder == cur_good.id)
			{
				current_item = cur_good;
				cur_sifnItem = item;
			}
		}
		if (isShow)
			getParent()->addOrderShowLayout(RootGuiLayout::LayoutId::SKYBLOCK_SIGN_IN);
	}

	void gui_skyblockSignIn::onGetCommonData(String key, String data)
	{
		if (key == "SignInData")
		{
			updateData(data);
			return;
		}
	}


	bool gui_skyblockSignIn::updateData(const String & data)
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
		auto isShow = doc->FindMember("IsShow")->value.GetBool();
		auto NowOrder = doc->FindMember("NowOrder")->value.GetInt();
		auto Status = doc->FindMember("Status")->value.GetInt();
		auto IconShow = doc->FindMember("IconShow")->value.GetBool();

		LordDelete(doc);
		onShow(NowOrder, Status, isShow);
		return true;
	}

	bool gui_skyblockSignIn::onShowWindow()
	{
		RootGuiLayout::Instance()->showSkyBlockSignIn();
		return true;
	}

	bool gui_skyblockSignIn::onReceivedClick(const EventArgs & events)
	{
		playSoundByType(ST_Click);
		if (current_item.status == SkyBlockSignInStatus::SKYBLOCK_ABLE_RECEIVED_SIGN)
		{
			if (current_item.rewardType == SKYBLOCK_SIGNIN_ITEM)
			{
				if (!Blockman::Instance()->m_pPlayer->inventory->isCanAddItem(current_item.rewardId, current_item.meta, current_item.num))
				{
					ShowResultTipDialogEvent::emit(ResultCode::COMMON,"gui_sign_in_iventor_is_full");
					return false;
				}
				ClientNetwork::Instance()->getCommonSender()->sendSkyBlockReceiveSignInReward(current_item.day);
				current_item.status = SkyBlockSignInStatus::SKYBLOCK_HAVE_RECEIVED_SIGN;
				if (cur_sifnItem)
					cur_sifnItem->setSignInItem(current_item);
			}
			else
			{
				ClientNetwork::Instance()->getCommonSender()->sendSkyBlockReceiveSignInReward(current_item.day);
				current_item.status = SkyBlockSignInStatus::SKYBLOCK_HAVE_RECEIVED_SIGN;
				if (cur_sifnItem)
					cur_sifnItem->setSignInItem(current_item);
			}
			return true;
		}

		ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_sign_in_today_is_recive");
		return true;
	}

	bool gui_skyblockSignIn::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		RootGuiLayout::Instance()->showMainControl();
		return true;
	}
}
