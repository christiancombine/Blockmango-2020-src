#include "gui_skyBlockReciveNewPri.h"
#include "GUI/GuiDef.h"
#include "UI/GUIGridView.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "Util/ClientEvents.h"
#include "GUI/RootGuiLayout.h"
#include "Network/ClientNetwork.h"
#include "GUI/GuiSkyBlockPrivilegeRewardItem.h"
#include "UI/GUIButton.h"

namespace BLOCKMAN
{
	gui_skyBlockReciveNewPri::gui_skyBlockReciveNewPri() :
		gui_layout("SkyBlockReciveNewPri.json")
	{

	}

	gui_skyBlockReciveNewPri::~gui_skyBlockReciveNewPri()
	{
		m_subscriptionGuard.unsubscribeAll();


	}

	void gui_skyBlockReciveNewPri::onLoad()
	{
		auto itemListLayout = getWindow<GUILayout>("SkyBlockReciveNewPri-Items");
		m_itemGridView = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "SkyBlockReciveNewPri-GridView");
		m_itemGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_itemGridView->InitConfig(40.f, 15.f, 4);
		itemListLayout->AddChildWindow(m_itemGridView);

		getWindow("SkyBlockReciveNewPri-Title")->SetText(getString("gui_recive_new_pri_tip_title"));
		getWindow("SkyBlockReciveNewPri-Recive-Tips")->SetText(getString("gui_recive_new_pri_tip"));
		getWindow("SkyBlockReciveNewPri-BtnText")->SetText(getString("skyblock_signin_receive"));


		auto close_btn = getWindow<GUIButton>("SkyBlockReciveNewPri-Close");
		close_btn->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockReciveNewPri::onCloseClick, this, std::placeholders::_1));

		auto Button = getWindow<GUIButton>("SkyBlockReciveNewPri-Button");
		Button->subscribeEvent(EventButtonClick, std::bind(&gui_skyBlockReciveNewPri::onReciveClick, this, std::placeholders::_1));

		m_pri_name = getWindow<GUIStaticText>("SkyBlockReciveNewPri-Pri-Name");
		m_pri_des = getWindow<GUIStaticText>("SkyBlockReciveNewPri-Pri-Des");
		m_subscriptionGuard.add(SkyBlockReciveNewPriClickItemEvent::subscribe(std::bind(&gui_skyBlockReciveNewPri::updateShopItemDetail, this, std::placeholders::_1, std::placeholders::_2)));
		
	}

	bool gui_skyBlockReciveNewPri::isNeedLoad()
	{
		return checkGameType(ClientGameType::SkyBlockMain);
	}

	void gui_skyBlockReciveNewPri::onGetCommonData(String key, String data)
	{
		if (key == "ReviveNewPri")
		{
			parseUpdateData(data);
			return;
		}
	}

	void gui_skyBlockReciveNewPri::parseUpdateData(String data)
	{
		if (data.length() == 0)
			return;
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(data.c_str());
		if (doc->HasParseError())
		{
			LordDelete(doc);
			return;
		}
		if (!doc->HasMember("needRecivePriId") || !doc->HasMember("isShowIcon"))
		{
			LordLogError("gui_skyBlockReciveNewPri::parseUpdateData getItemInfo The game result content player_info missed some field");
			LordDelete(doc);
			return;
		}

		auto isShowIcon = doc->FindMember("isShowIcon")->value.GetBool();

		if (isShowIcon)
		{
			getParent()->addOrderShowLayout(RootGuiLayout::LayoutId::SKYBLOCK_REVIVE_NEW_PRI);
		}
		else
		{
			getParent()->hideSkyBlocReciveNewPri();	
		}

		rapidjson::Value priId = doc->FindMember("needRecivePriId")->value.GetArray();

		m_itemGridView->RemoveAllItems();
		m_itemGridView->SetScrollOffset(0);
		static unsigned reciveNewPriCount = 0;
		auto cur_count = 0;
		float halfWide = 37.5f;
		float Halfinterval = 20.0f;
		if (priId.Size() < 4)
			m_itemGridView->SetArea(UDim(0, (4 - priId.Size()) * (halfWide + Halfinterval)), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		else
			m_itemGridView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));

		for (size_t i = 0; i < priId.Size(); ++i)
		{
			int cur_priId = priId[i].GetInt();
			SkyBlockPrivilegeReward goods;
			if (SkyBlockAppShopSetting::Instance()->getPrivilegeRewardDataByPriId(goods, cur_priId))
			{
				reciveNewPriCount++;
				GUIString strTabName = StringUtil::Format("gui_skyBlockReciveNewPri-Content-RewardList-GridView-Item-%d", reciveNewPriCount).c_str();
				GuiSkyBlockPrivilegeRewardItem* goodsItem = (GuiSkyBlockPrivilegeRewardItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_SKYBLOCK_PRIVILEGE_REWARD_ITEM, strTabName);
				goodsItem->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, 75.f), UDim(0, 75.f));
				goodsItem->setGoods(goods, cur_count, SKYBLOCK_RECIVE_NEW_PRI);
				m_itemGridView->AddItem(goodsItem);
				if (cur_count == 0)
				{
					updateShopItemDetail(goods, cur_count);
				}
				cur_count++;
			}
		}
		LordDelete(doc);
		return;
	}

	bool gui_skyBlockReciveNewPri::updateShopItemDetail(const SkyBlockPrivilegeReward item, int index)
	{
		auto count = m_itemGridView->GetItemCount();
		for (size_t i = 0; i < count; i++)
		{
			GuiSkyBlockPrivilegeRewardItem* shopItem = (GuiSkyBlockPrivilegeRewardItem*)m_itemGridView->GetItem(i);
			if (shopItem)
			{
				shopItem->m_select_img->SetVisible(false);
				if (i == index)
				{
					m_pri_name->SetText(LanguageManager::Instance()->getString(item.name).c_str());
					m_pri_des->SetText(LanguageManager::Instance()->getString(item.detail).c_str());
					shopItem->m_select_img->SetVisible(true);
				}
			}
		}
		return true;
	}

	bool gui_skyBlockReciveNewPri::onCloseClick(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		RootGuiLayout::Instance()->showMainControl();
		return true;
	}

	bool gui_skyBlockReciveNewPri::onReciveClick(const EventArgs & events)

	{
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getCommonSender()->sendSkyBlocReciveNewPri();
		return true;
	}

}
