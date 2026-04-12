#include "gui_walkingDeadStore.h"
#include "Setting/WalkingDeadStoreSetting.h"
#include "Setting/RewardSetting.h"
#include "GUI/GuiDef.h"
#include "Network/protocol/CommonDataBuilder.h"
using namespace LORD;

namespace BLOCKMAN
{
	gui_walkingDeadStore::gui_walkingDeadStore()
		:gui_layout("WalkingDeadStore.json")
	{
		m_goods_panel = m_goods_panel ? m_goods_panel : LordNew GUIWalkingDeadStoreGoodsPanel;
		m_supply_panel = m_supply_panel ? m_supply_panel : LordNew GUIWalkingDeadStoreSupplyPanel;
	}
	
	gui_walkingDeadStore::~gui_walkingDeadStore()
	{
		LordDelete m_goods_panel;
		LordDelete m_supply_panel;
		m_subscriptionGuard.unsubscribeAll();
		WalkingDeadStoreSetting::unloadSetting();
	}

	bool gui_walkingDeadStore::isNeedLoad()
	{
		return checkGameType(ClientGameType::WalkingDead);
	}

	void gui_walkingDeadStore::onLoad()
	{
		WalkingDeadStoreSetting::loadSetting();

		m_tabs = getWindow<GUILayout>("WalkingDeadStore-Tabs");
		m_ren = getWindow<GUILayout>("WalkingDeadStore-Ren");
		m_store = getWindow<GUIStaticText>("WalkingDeadStore-Title-Text");
		m_store->SetText(getString("walking.dead.store"));
		auto m_close_button = getWindow<GUIButton>("WalkingDeadStore-Exit-Button");
		m_close_button->subscribeEvent(EventButtonClick, std::bind(&gui_walkingDeadStore::onCloseWalkingDeadStore, this, std::placeholders::_1));


		GUILayout* Panels = getWindow<GUILayout>("WalkingDeadStore-Panels");
		m_goods_panel->attachTo(this, Panels);
		m_supply_panel->attachTo(this, Panels);

		m_subscriptionGuard.add(UpdateWalkingDeadSupplyEvent::subscribe(std::bind(&gui_walkingDeadStore::onUpdateWalkingDeadSupply, this, std::placeholders::_1)));

		m_subscriptionGuard.add(WalkingDeadShowStoreEvent::subscribe(std::bind(&gui_walkingDeadStore::showStoreById, this, std::placeholders::_1)));
		
		m_subscriptionGuard.add(WalkingDeadStoreByTravellerEvent::subscribe(std::bind(&gui_walkingDeadStore::showStoreByTraveller, this, std::placeholders::_1)));
		m_subscriptionGuard.add(WalkingDeadStoreUpdateGoodsEvent::subscribe(std::bind(&gui_walkingDeadStore::onUpdateGoods, this, std::placeholders::_1, std::placeholders::_2)));
		
		m_subscriptionGuard.add(WalkingDeadStoreTabChangeEvent::subscribe(std::bind(&gui_walkingDeadStore::onTabChange, this, std::placeholders::_1)));
		m_subscriptionGuard.add(WalkingDeadShowVipInfoEvent::subscribe(std::bind(&gui_walkingDeadStore::onWalkingDeadStoreVip, this, std::placeholders::_1, std::placeholders::_2)));

	}

	void gui_walkingDeadStore::onGetCommonData(String key, String data)
	{
		if (key == "ShowStoreId")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int storeId = builder.getIntParam("storeId");
			WalkingDeadShowStoreEvent::emit(storeId);
		}
		if (key == "UpdateGoods")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int goodsId = builder.getIntParam("goodsId");
			int limit = builder.getIntParam("limit");
			WalkingDeadStoreUpdateGoodsEvent::emit(goodsId, limit);
		}
		if (key == "SupplyData")
		{
			UpdateWalkingDeadSupplyEvent::emit(data);
		}
		if (key == "ShowVipInfo")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int vip = builder.getIntParam("vip");
			int64_t vipTime = builder.getI64Param("vipTime");
			WalkingDeadShowVipInfoEvent::emit(vip, vipTime);
		}
		if (key == "ShowStoreByTraveller")
		{
			WalkingDeadStoreByTravellerEvent::emit(data);
		}
	}

	void gui_walkingDeadStore::onUpdate(ui32 nTimeElapse)
	{
		WalkingDeadStoreSetting::onUpdate(nTimeElapse);
	}

	bool gui_walkingDeadStore::showStoreById(int storeId)
	{
		if (storeId == 2)
		{
			getParent()->showWalkingDeadStore();
			WalkingDeadStoreSetting::changeStoreType((WalkingDeadStoreType)3);
			auto tabs = WalkingDeadStoreSetting::findTabsByStoreId(storeId);
			initTabsUI(tabs);
		}
		else
		{
			getParent()->showWalkingDeadStore();
			WalkingDeadStoreSetting::changeStoreType(GeneralShop);
			auto tabs = WalkingDeadStoreSetting::findTabsByStoreId(storeId);
			initTabsUI(tabs);
		}
		return true;
	}

	bool gui_walkingDeadStore::showStoreByTraveller(const String & data)
	{
		getParent()->showWalkingDeadStore();
		WalkingDeadStoreSetting::changeStoreType(TravellerShop);
		WalkingDeadStoreSetting::parseTravellerGoodsData(data);
		auto tabs = WalkingDeadStoreSetting::findTabsFromTraveller();
		initTabsUI(tabs);
		return true;
	}

	bool gui_walkingDeadStore::onUpdateGoods(int goodsId, int limit)
	{
		WalkingDeadStoreSetting::updateGoods(goodsId, limit);
		return true;
	}

	bool gui_walkingDeadStore::onWalkingDeadStoreVip(int vip, int64_t vipTime)
	{
		WalkingDeadStoreSetting::parseWalkingDeadStoreVipData(vip, vipTime);
		return true;
	}

	bool gui_walkingDeadStore::onTabChange(int tabId)
	{
		auto firstGoods = WalkingDeadStoreSetting::findFirstGoodsByTabId(tabId);
		if (firstGoods)
		{
			UpdateWalkingDeadStoreAppShopInfoEvent::emit(firstGoods->GoodsId);
		}
		
		for (auto tab : m_store_tabs)
		{
			if (tab->GetTabId() == tabId)
			{
				tab->SetTabSelected(true);
				if (tab->GetTabType() == GeneralTabType)
				{
					m_supply_panel->hide();
					m_ren->SetVisible(false);
					m_goods_panel->show();
					m_goods_panel->updateGoodsByTabId(tabId);
				}
				else
				{
					m_goods_panel->hide();
					m_supply_panel->show();
					m_ren->SetVisible(true);
					m_supply_panel->updateSupplyByTabId();
				}
				continue;
			}
			if (tab->IsTabSelected())
			{
				tab->SetTabSelected(false);
			}
		}
		return true;
	}

	void gui_walkingDeadStore::initTabsUI(list<WalkingDeadStoreTab*>::type tabs)
	{
		m_store_tabs.clear();
		m_tabs->CleanupChildren();
		if (tabs.empty())
			return;

// 		size_t MinCount = Math::Max(tabs.size(), size_t(7));
// 		float TabSize = (m_tabs->GetPixelSize().y - 1.0f - MinCount) / MinCount;
// 		float TabSizeX = m_tabs->GetPixelSize().x;
// 		m_tabs->SetWidth({ 0.0f, TabSize });

		//static unsigned index = 0;
		 int index = 0;
		//float height = 1.0f / MinCount;
		int cur_tab_count = 0;
		for (auto tab : tabs)
		{
			float positionY = 85.0f * index;
			index++;
			cur_tab_count++;
			GUIString TabName = StringUtil::Format("GUIWalkingDeadStoreTab-%d", index).c_str();
			GUIWalkingDeadStoreTab* pStoreTab = (GUIWalkingDeadStoreTab*)GUIWindowManager::Instance()->
				CreateGUIWindow(GWT_WALKING_DEAD_STORE_TAB, TabName);
			//pStoreTab->SetWidth({ 0.0f, 166*1.0f });
			//pStoreTab->SetHeight({ 0.0f, TabSize });
			//pStoreTab->SetYPosition({ 0.0f, 1.0f * cur_tab_count + (cur_tab_count - 1) * TabSize });
			//pStoreTab->SetArea({ 0, 0 }, { 0.16f, positionY }, { 0, 166 }, { 0, 63 });
			pStoreTab->SetArea({ 0, 0 }, { 0, positionY }, { 0, 166 }, { 0, 63 });
			pStoreTab->InitStoreTab(tab);
			m_tabs->AddChildWindow(pStoreTab);
			m_store_tabs.push_back(pStoreTab);
		}

		GUIWalkingDeadStoreTab* tab = *m_store_tabs.begin();
		onTabChange(tab->GetTabId());
	}

	bool gui_walkingDeadStore::onCloseWalkingDeadStore(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showMainControl();
		return true;
	}

	bool gui_walkingDeadStore::onUpdateWalkingDeadSupply(const String & data)
	{
		WalkingDeadStoreSetting::parseUpdateWalkingDeadSupplyData(data);
		return true;
	}
}