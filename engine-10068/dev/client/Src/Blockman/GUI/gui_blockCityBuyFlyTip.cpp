#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "gui_blockCityBuyFlyTip.h"
#include "GUI/GuiBlockCityBuyFlyItem.h"
#include "Blockman/GUI/RootGuiLayout.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiDef.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "game.h"
#include "ShellInterface.h"


using namespace LORD;

namespace BLOCKMAN
{
	gui_blockCityBuyFlyTip::gui_blockCityBuyFlyTip()
		: gui_layout("BlockCityBuyFlyTip.json")
	{
	}

	gui_blockCityBuyFlyTip::~gui_blockCityBuyFlyTip()
	{
	}

	void gui_blockCityBuyFlyTip::onLoad()
	{
		m_flyLayout = getWindow<GUILayout>("BlockCityBuyFlyTip-BuyFly");

		getWindow("BlockCityBuyFlyTip-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityBuyFlyTip::onClose, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowBlockCityBuyFlyTipEvent::subscribe(std::bind(&gui_blockCityBuyFlyTip::showFlyTip, this)));
		m_subscriptionGuard.add(VideoAdVisibleEvent::subscribe(std::bind(&gui_blockCityBuyFlyTip::onVideoAdVisible, this, std::placeholders::_1)));
	}

	bool gui_blockCityBuyFlyTip::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	void gui_blockCityBuyFlyTip::onUpdate(ui32 nTimeElapse)
	{
	}

	bool gui_blockCityBuyFlyTip::showFlyTip()
	{
		if (UICommon::checkBlockCityParam())
		{
			auto manor = Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo();
			if (manor)
			{
				setText("BlockCityBuyFlyTip-Title-Name", "gui_blockcity_buy_fly");

				static i32 count = 0;
				i32 itemNum = 0;
				m_flyLayout->CleanupChildren();
				float itemX = 0.f;
				getWindow<GUILayout>("BlockCityBuyFlyTip-Content")->SetWidth({ 0.f, 549.f });
				itemX = (m_flyLayout->GetPixelSize().x - 2 * 8.f) / 3;
				if (m_isAdReady && m_canShowAd)
				{
					itemNum++;
					getWindow<GUILayout>("BlockCityBuyFlyTip-Content")->SetWidth({ 0.f, 719.f });
					itemX = (m_flyLayout->GetPixelSize().x - 3 * 8.f) / 4;
					String iterLayout = StringUtil::Format("BlockCityTip-BuyFly-Content-Items-%d", count++).c_str();
					GuiBlockCityBuyFlyItem* itemView = (GuiBlockCityBuyFlyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_BUY_FLY_ITEM, iterLayout.c_str());
					itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.0f, itemX), UDim(1.0f, 0.f));
					itemView->setItem(BlockCityFlyingInfo());
					m_flyLayout->AddChildWindow(itemView);
				}
				for (size_t i = 0; i < manor->flyingInfo.size(); i++)
				{
					itemNum++;
					String iterLayout = StringUtil::Format("BlockCityTip-BuyFly-Content-Items-%d", count++).c_str();
					GuiBlockCityBuyFlyItem* itemView = (GuiBlockCityBuyFlyItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_BUY_FLY_ITEM, iterLayout.c_str());
					itemView->SetArea(UDim(0, (itemNum - 1) * (itemX + 8.f)), UDim(0, 0), UDim(0.0f, itemX), UDim(1.0f, 0.f));
					itemView->setItem(manor->flyingInfo[i]);
					m_flyLayout->AddChildWindow(itemView);
				}
				RootGuiLayout::Instance()->showWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_BUY_FLY);
				GameClient::CGame::Instance()->getShellInterface()->
					dataReport("realm_fly_win", GameClient::CGame::Instance()->getGameType());
				if (m_isAdReady && m_canShowAd)
				{
					GameClient::CGame::Instance()->getShellInterface()->
						dataReport("realm_fly_ad_show", GameClient::CGame::Instance()->getGameType());
				}
			}
		}
		return true;
	}

	bool gui_blockCityBuyFlyTip::onVideoAdVisible(bool visible)
	{
		m_isAdReady = visible;
		return true;
	}

	void gui_blockCityBuyFlyTip::onGetCommonData(String key, String data)
	{
		if (key == "FlyingAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			bool isShow = builder.getBoolParam("isShow");
			m_canShowAd = isShow;
			return;
		}
	}

	bool gui_blockCityBuyFlyTip::onClose(const EventArgs& args)
	{
		RootGuiLayout::Instance()->hideWindowByLayoutId(RootGuiLayout::LayoutId::BLOCK_CITY_BUY_FLY);
		return true;
	}

}
