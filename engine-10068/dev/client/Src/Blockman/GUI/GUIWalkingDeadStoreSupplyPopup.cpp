#include "GUIWalkingDeadStoreSupplyPopup.h"
#include "GUIWalkingDeadSupplyDescItem.h"
#include "Setting/WalkingDeadStoreSetting.h"
#include "Setting/RewardSetting.h"
#include "GUI/GuiDef.h"
using namespace LORD;

namespace BLOCKMAN
{
	GUIWalkingDeadStoreSupplyPopup::GUIWalkingDeadStoreSupplyPopup()
		:gui_layout("WalkingDeadSupplyPopup.json")
	{
	}

	GUIWalkingDeadStoreSupplyPopup::~GUIWalkingDeadStoreSupplyPopup()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	bool GUIWalkingDeadStoreSupplyPopup::isNeedLoad()
	{
		return checkGameType(ClientGameType::WalkingDead);
	}

	void GUIWalkingDeadStoreSupplyPopup::onLoad()
	{
		initSupplyGoodsList();
		m_title = getWindow<GUIStaticText>("WalkingDeadSupplyPopup-Title-Text");
		m_close_button = getWindow<GUIButton>("WalkingDeadSupplyPopup-Close-Button");
		m_box_image = getWindow<GUIStaticImage>("WalkingDeadSupplyPopup-Box-Image");
		m_content = getWindow<GUILayout>("WalkingDeadSupplyPopup-Content");
		m_subscriptionGuard.add(WalkingDeadshowSupplyPopupEvent::subscribe(std::bind(&GUIWalkingDeadStoreSupplyPopup::showSupplyPopup, this, std::placeholders::_1)));
		m_close_button->subscribeEvent(EventButtonClick, std::bind(&GUIWalkingDeadStoreSupplyPopup::onCloseWalkingDeadStoreSupplyPopup, this, std::placeholders::_1));

	}

	void GUIWalkingDeadStoreSupplyPopup::initSupplyGoodsList()
	{
		auto SupplyGoodsList = getWindow("WalkingDeadSupplyPopup-Content");
		if (SupplyGoodsList == NULL)
		{
			return;
		}
		GUIString ChildsName = StringUtil::Format("GUIWalkingDeadStoreSupplyPopup-Goods-Container").c_str();
		m_supply_goods_list = (GUIGridView*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, ChildsName);
		m_supply_goods_list->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		m_supply_goods_list->InitConfig(0.0f, 0.0f, 5);
		SupplyGoodsList->AddChildWindow(m_supply_goods_list);
	}

	void GUIWalkingDeadStoreSupplyPopup::updateSupplyItemBySupplyId(int supplyId)
	{
		if (m_cur_supply_id == supplyId)
		{
			return;
		}
		m_cur_supply_id = supplyId;
		auto supply = WalkingDeadStoreSetting::findSupplyById(m_cur_supply_id);
		m_title->SetText(LanguageManager::Instance()->getString(supply->Name).c_str());
		m_box_image->SetImage(supply->Image.c_str());

		list<Reward*>::type rewardList;
		rewardList = RewardSetting::findRewardByIds(supply->RewardIds);
		m_supply_goods_list->RemoveAllItems();
		m_supply_goods_list->SetScrollOffset(0);
		float ContentSizeX = m_supply_goods_list->GetPixelSize().x;
		float ContentSizeY = m_supply_goods_list->GetPixelSize().y;
		static unsigned WalkingDeadChildCount = 0;
		float ChildWidth = ContentSizeX / 5;
		float ChildHeight = ContentSizeY;
		for (auto goods : rewardList)
		{
			WalkingDeadChildCount++;
			GUIString ChildName = StringUtil::Format("GUIWalkingDeadStoreSupplyPopup-Goods-%d", WalkingDeadChildCount).c_str();
			GUIWalkingDeadSupplyDescItem*  Item = (GUIWalkingDeadSupplyDescItem*)GUIWindowManager::Instance()->
				CreateGUIWindow(GWT_WALKING_DEAD_SUPPLY_ITEM, ChildName);
			Item->SetWidth({ 0, ChildWidth });
			Item->SetHeight({ 0, ChildHeight });
			Item->InitSupplyItems(goods);
			m_supply_goods_list->AddItem(Item);
		}
	}

	void GUIWalkingDeadStoreSupplyPopup::onUpdate(ui32 nTimeElapse)
	{
	}

	bool GUIWalkingDeadStoreSupplyPopup::showSupplyPopup(int supplyId)
	{
		getParent()->showSupplyPopup();
		updateSupplyItemBySupplyId(supplyId);
		return true;
	}

	bool GUIWalkingDeadStoreSupplyPopup::onCloseWalkingDeadStoreSupplyPopup(const EventArgs & events)
	{
		playSoundByType(ST_ClosePanel);
		getParent()->showWalkingDeadStore();
		return true;
	}
}