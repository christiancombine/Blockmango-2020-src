#include "gui_birdPackAndFuse.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiBirdPack.h"
#include "GUI/GuiBirdFuse.h"
#include "GUI/GuiBirdPackAndFuseItem.h"
#include "Game/BirdSimulator.h"
#include "Network/ClientNetwork.h"
#include "Util/UICommon.h"
#include "GUI/GuiBirdTool.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_birdPackAndFuse::gui_birdPackAndFuse()
		: gui_layout("BirdPackAndFuse.json")
	{
	}

	gui_birdPackAndFuse::~gui_birdPackAndFuse()
	{

	}

	void gui_birdPackAndFuse::onHide()
	{
		m_birdPackView->hideView();
	}

	void gui_birdPackAndFuse::onShow()
	{
		if (m_isPackUpdate)
		{
			changeSortBirdItem(false);
			changeToolItem();
			m_isPackUpdate = false;
		}
	}

	void gui_birdPackAndFuse::onLoad()
	{

		setText("BirdPackAndFuse-Sort-Tab-Text", LanguageKey::GUI_BIRD_PACK_SORT);
		m_radioMap[ViewId::PACK] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Nest");
		m_radioMap[ViewId::FUSE] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Fuse");	
		m_radioMap[ViewId::TOOL] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Tool");
		

		setText("BirdPackAndFuse-Nest-Name", "gui_bird_nest");
		setText("BirdPackAndFuse-Tool-Name", "gui_bird_pack");
		setText("BirdPackAndFuse-Fuse-Name", "gui_bird_fuse");

		m_pack = getWindow<GUILayout>("BirdPackAndFuse-Detil-Nest");
		m_fuse = getWindow<GUILayout>("BirdPackAndFuse-Detil-Fuse");
		m_tool = getWindow<GUILayout>("BirdPackAndFuse-Detil-Tool");

		m_radioContentMap[ViewId::PACK] = getWindow<GUILayout>("BirdPackAndFuse-Detil-Nest");
		m_radioContentMap[ViewId::FUSE] = getWindow<GUILayout>("BirdPackAndFuse-Detil-Fuse");
		m_radioContentMap[ViewId::TOOL] = getWindow<GUILayout>("BirdPackAndFuse-Detil-Tool");

		m_radioSortMap[ViewId::QUALITY] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Quality");
		m_radioSortMap[ViewId::LEVEL] = getWindow<GUIRadioButton>("BirdPackAndFuse-Choose-Level");
		

		setText(m_radioSortMap[ViewId::QUALITY], "gui_bird_pack_quality");
		setText(m_radioSortMap[ViewId::LEVEL], "gui_bird_pack_level");

		m_gvSort = getWindow<GUIGridView>("BirdPackAndFuse-Sort-GridView");
		m_gvSort->InitConfig(13.f, 13.f, 5);
		m_carryTab = getWindow<GUILayout>("BirdPackAndFuse-Carry-Tab");
		m_icon = getWindow<GUIStaticImage>("BirdPackAndFuse-Icon");
		m_name = getWindow<GUIStaticText>("BirdPackAndFuse-Name");

		m_gvTool = getWindow<GUIGridView>("BirdPackAndFuse-Tool-GridView");
		m_gvTool->InitConfig(13.f, 13.f, 5);

		initPackItem();
		initFuseItem();
		initToolItem();

		for (auto item : m_radioMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_birdPackAndFuse::onRadioChange, this, std::placeholders::_1, item.first));
		}

		for (auto item : m_radioSortMap)
		{
			item.second->subscribeEvent(EventRadioStateChanged, std::bind(&gui_birdPackAndFuse::onRadioSortChange, this, std::placeholders::_1, item.first));
		}

		getWindow("BirdPackAndFuse")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::MAIN));
		getWindow("BirdPackAndFuse-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BirdPackAndFuse-Carry-Tab-Button")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::CARRAY));
		getWindow("BirdPackAndFuse-Capacity-Tab-Button")->subscribeEvent(EventButtonClick, std::bind(&gui_birdPackAndFuse::onClick, this, std::placeholders::_1, ViewId::CAPACITY));
		
		m_subscriptionGuard.add(UpdateBirdPackEvent::subscribe(std::bind(&gui_birdPackAndFuse::onPackUpdate, this)));
		m_subscriptionGuard.add(ShowBirdInfoClickEvent::subscribe(std::bind(&gui_birdPackAndFuse::onShowBirdInfo, this, std::placeholders::_1, false)));
		m_subscriptionGuard.add(BirdFuseRemoveItemEvent::subscribe(std::bind(&gui_birdPackAndFuse::onFuseRemoveItem, this, std::placeholders::_1)));
		m_subscriptionGuard.add(BirdFusePutItemEvent::subscribe(std::bind(&gui_birdPackAndFuse::onFusePutItem, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(BirdNestExpandEvent::subscribe(std::bind(&gui_birdPackAndFuse::openExpandCarry, this)));
		m_subscriptionGuard.add(BirdShowCarryGuideEvent::subscribe(std::bind(&gui_birdPackAndFuse::onShowBirdCarryGuide, this)));
		m_subscriptionGuard.add(ShowBirdToolInfoClickEvent::subscribe(std::bind(&gui_birdPackAndFuse::onShowToolInfo, this, std::placeholders::_1)));
		
		m_radioSortMap[ViewId::QUALITY]->SetSelected(true);
		m_radioMap[ViewId::PACK]->SetSelected(true);
	}


	void gui_birdPackAndFuse::initPackItem()
	{
		m_birdPackView = (GuiBirdPack*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PACK, "BirdPackAndFuse-BirdPackView");
		m_birdPackView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.0f), UDim(1.0f, 0.0f));
		m_birdPackView->SetTouchable(false);
		m_pack->AddChildWindow(m_birdPackView);
	}

	void gui_birdPackAndFuse::initFuseItem()
	{
		m_birdFuseView = (GuiBirdFuse*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_FUSE, "BirdPackAndFuse-BirdFuseView");
		m_birdFuseView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.0f), UDim(1.0f, 0.0f));
		m_birdFuseView->SetTouchable(false);
		m_fuse->AddChildWindow(m_birdFuseView);
	}

	void gui_birdPackAndFuse::initToolItem()
	{
		m_birdToolView = (GuiBirdTool*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_TOOL, "BirdPackAndFuse-BirdToolView");
		m_birdToolView->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, 0.0f), UDim(1.0f, 0.0f));
		m_birdToolView->SetTouchable(false);
		m_tool->AddChildWindow(m_birdToolView);
	}

	void gui_birdPackAndFuse::onUpdate(ui32 nTimeElapse)
	{
		if (m_gvSortXSize != m_gvSort->GetPixelSize().x)
		{
			m_gvSortXSize = m_gvSort->GetPixelSize().x;
			sortItemSizeChange();
		}
		if (m_gvToolXSize != m_gvTool->GetPixelSize().x)
		{
			m_gvToolXSize = m_gvTool->GetPixelSize().x;
		}
		getWindow("BirdPackAndFuse-Carry-Guide")->SetVisible(m_isShowCarryGuide);
	}

	bool gui_birdPackAndFuse::onPackUpdate()
	{
		m_isPackUpdate = true;
		if (isShown())
		{
			onShow();
		}
		return false;
	}

	i32 gui_birdPackAndFuse::getShowType()
	{
		if (m_radioMap[ViewId::PACK]->IsSelected())
		{
			return 1;
		}
		if (m_radioMap[ViewId::FUSE]->IsSelected())
		{
			return 2;
		}
		return 3;
	}

	bool gui_birdPackAndFuse::onShowBirdInfo(i64 birdId, bool isRefresh)
	{
		if (!UICommon::checkBirdBagParam())
		{
			return false;
		}
		bool isChange = false;
		auto birds = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag()->birds;
		for (auto bird : birds)
		{
			if (bird.id == birdId)
			{
				isChange = true;
				m_birdInfo = bird;
				m_birdPackView->setBird(bird);
				break;
			}
		}

		if (!isChange && birds.size() > 0)
		{
			m_birdInfo = birds[0];
			m_birdPackView->setBird(m_birdInfo);
		}

		for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
		{
			dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i))->setSelect(birdId);
		}
		if (isRefresh)
		{
			m_birdPackView->changeView();
		}
		else
		{
			m_birdPackView->hideView();
		}
		return true;
	}

	bool gui_birdPackAndFuse::onRadioChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioMap[viewId]->IsSelected())
		{
			m_carryTab->SetVisible(viewId == ViewId::PACK);
			setIconAndName(viewId);
			getWindow("BirdPackAndFuse-Sort-Tab")->SetVisible(getShowType() != 3);
			for (auto item : m_radioContentMap)
			{
				item.second->SetVisible(item.first == viewId);
			}

			if (getShowType() == 3)
			{
				changeToolItem();
				m_gvSort->SetVisible(false);
				m_gvTool->SetVisible(true);
			}
			else
			{
				changeSortBirdItem(true);
				m_gvSort->SetVisible(true);
				m_gvTool->SetVisible(false);
			}
			m_birdFuseView->removeAllItems();
			playSoundByType(ST_Click);
		}
		return true;
	}

	bool gui_birdPackAndFuse::onRadioSortChange(const EventArgs & events, ViewId viewId)
	{
		if (m_radioSortMap[viewId]->IsSelected())
		{
			playSoundByType(ST_Click);
			changeSortBirdItem(true);
			m_birdFuseView->removeAllItems();
		}
		return true;
	}

	bool gui_birdPackAndFuse::onClick(const EventArgs & events, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case ViewId::CARRAY:
			if (UICommon::checkBirdBagParam())
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (bag->maxCarry >= bag->maxCarryLevel)
				{
					ShowBirdCommonTipEvent::emit("bird_carry_already_max_level");
				}
				else
				{
					ShowBirdExpandCarryTipEvent::emit(bag->expandCarryPrice, bag->expandCurrencyType);
				}
				if (m_isShowCarryGuide)
				{
					m_isShowCarryGuide = false;
				}
			}
			break;
		case ViewId::CAPACITY:
			if (UICommon::checkBirdBagParam())
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (bag->maxCapacity >= bag->maxCapacityLevel)
				{
					ShowBirdCommonTipEvent::emit("bird_capacity_already_max_level");
				}
				else
				{
					ShowBirdExpandCapacityTipEvent::emit(bag->expandCapacityPrice, bag->expandCurrencyType);
				}
			}
			break;
		}
		m_birdPackView->hideView();
		return true;
	}

	bool gui_birdPackAndFuse::openExpandCarry()
	{
		RootGuiLayout::Instance()->showBirdPackAndFuse();
		onClick(EventArgs(), ViewId::CARRAY);
		return false;
	}

	void gui_birdPackAndFuse::changeSortBirdItem(bool isForceRefresh)
	{
		if (!UICommon::checkBirdBagParam())
		{
			return;
		}

		static int count = 0;
		auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
		getWindow("BirdPackAndFuse-Carry-Tab-Text")->SetText(StringUtil::Format("%d/%d", bag->curCarry, bag->maxCarry).c_str());
		getWindow("BirdPackAndFuse-Capacity-Tab-Text")->SetText(StringUtil::Format("%d/%d", bag->curCapacity, bag->maxCapacity).c_str());
		auto birds = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBirdsSort(m_radioSortMap[ViewId::LEVEL]->IsSelected());
		float oldOffset = m_gvSort->GetScrollOffset();
		if (m_gvSort->GetItemCount() != bag->maxCapacity || m_birdSize != birds.size())
		{
			m_birdSize = (i32) birds.size();
			m_gvSort->RemoveAllItems();
			m_birdFuseView->removeAllItems();
			for (auto item : birds)
			{
				String iterLayout = StringUtil::Format("BirdPackAndFuse-Sort-GridView-Item-%d", count++).c_str();
				GuiBirdPackAndFuseItem* itemView = (GuiBirdPackAndFuseItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PACK_AND_FUSE_ITEM, iterLayout.c_str());
				float w = (m_gvSort->GetPixelSize().x - 52) / 5;
				itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
				itemView->setBirdInfo(item);
				itemView->SetTouchable(true);
				itemView->setShowType(getShowType());
				m_gvSort->AddItem(itemView);
			}

			for (i32 i = 0; i < bag->maxCapacity - (i32)birds.size(); i++)
			{
				String iterLayout = StringUtil::Format("BirdPackAndFuse-Sort-GridView-Item-%d", count++).c_str();
				GuiBirdPackAndFuseItem* itemView = (GuiBirdPackAndFuseItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PACK_AND_FUSE_ITEM, iterLayout.c_str());
				float w = (m_gvSort->GetPixelSize().x - 52) / 5;
				itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
				itemView->SetTouchable(false);
				m_gvSort->AddItem(itemView);
			}
		}
		else
		{
			for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
			{
				auto view = dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i));
				view->setBirdInfo(i < birds.size() ? birds[i] : BirdInfo());
				view->setShowType(getShowType());
				if (getShowType() == 2 && isForceRefresh)
				{
					view->setSelect(false);
			    }
			}
		}
		m_gvSort->ResetPos();
		m_gvSort->SetScrollOffset(oldOffset);
		if (getShowType() == 1 && birds.size() > 0)
		{
			onShowBirdInfo(m_birdInfo.id > 0 ? m_birdInfo.id : birds[0].id);
		}

		if (birds.size() == 0)
		{
			m_birdPackView->setBird(BirdInfo());
		}
		
	}

	void gui_birdPackAndFuse::changeToolItem()
	{
		if (!UICommon::checkBirdBagParam())
		{
			return;
		}
		auto tools = Blockman::Instance()->m_pPlayer->m_birdSimulator->getStoreSort();
		float oldOffset = m_gvTool->GetScrollOffset();
		m_gvTool->RemoveAllItems();
		static int count = 0;
		for (auto item : tools)
		{
			String iterLayout = StringUtil::Format("BirdPackAndFuse-Tool-GridView-Item-%d", count++).c_str();
			GuiBirdPackAndFuseItem* itemView = (GuiBirdPackAndFuseItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_PACK_AND_FUSE_ITEM, iterLayout.c_str());
			float w = (m_gvTool->GetPixelSize().x - 52) / 5;
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
			itemView->setToolInfo(item);
			itemView->setShowType(getShowType());
			itemView->SetTouchable(true);
			m_gvTool->AddItem(itemView);
			if (item.status == 3)
			{
				m_birdToolView->setCurUse(item);
			}
		}
		m_gvTool->ResetPos();
		m_gvTool->SetScrollOffset(oldOffset);

		if (tools.size() > 0)
		{
			onShowToolInfo(m_toolInfo.id > 0 ? m_toolInfo.id : tools[0].id);
		}
	}

	bool gui_birdPackAndFuse::onShowToolInfo(i32 toolId)
	{
		if (!UICommon::checkBirdBagParam())
		{
			return false;
		}
		bool isChange = false;
		auto tools = Blockman::Instance()->m_pPlayer->m_birdSimulator->getStoreSort();
		for (auto tool : tools)
		{
			if (tool.id == toolId)
			{
				isChange = true;
				m_toolInfo = tool;
				m_birdToolView->setToolInfo(m_toolInfo);
				break;
			}
		}

		if (!isChange && tools.size() > 0)
		{
			m_toolInfo = tools[0];
			m_birdToolView->setToolInfo(m_toolInfo);
		}

		for (size_t i = 0; i < m_gvTool->GetItemCount(); i++)
		{
			dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvTool->GetItem(i))->setSelect((i64)m_toolInfo.id);
		}
		return true;
	}

	void gui_birdPackAndFuse::setShowPack()
	{
		m_radioMap[ViewId::PACK]->SetSelected(true);
	}

	bool gui_birdPackAndFuse::isNeedLoad()
	{
		return checkGameType(ClientGameType::BirdSimulator);
	}

	bool gui_birdPackAndFuse::onFuseRemoveItem(i64 birdId)
	{
		for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
		{
			dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i))->unSelect(birdId);
		}
		return true;
	}

	bool gui_birdPackAndFuse::onFusePutItem(i64 birdId, const String & image, const String& backImage)
	{
		if (m_birdFuseView->onPutBird(birdId, image, backImage))
		{
			for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
			{
				dynamic_cast<GuiBirdPackAndFuseItem*>(m_gvSort->GetItem(i))->setSelect(birdId);
			}
		}
		return false;
	}

	void gui_birdPackAndFuse::sortItemSizeChange()
	{
		for (size_t i = 0; i < m_gvSort->GetItemCount(); i++)
		{
			float w = (m_gvSort->GetPixelSize().x - 52) / 5;
			m_gvSort->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
		}
	}

	void gui_birdPackAndFuse::toolItemSizeChange()
	{
		for (size_t i = 0; i < m_gvTool->GetItemCount(); i++)
		{
			float w = (m_gvTool->GetPixelSize().x - 52) / 5;
			m_gvTool->GetItem(i)->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, w), UDim(0.0f, w + 4.f));
		}
	}

	bool gui_birdPackAndFuse::onShowBirdCarryGuide()
	{
		m_isShowCarryGuide = true;
		return true;
	}

	void gui_birdPackAndFuse::setIconAndName(ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::PACK:
		{
			m_icon->SetImage("set:bird_pack.json image:nest_icon");
			setText(m_name, "gui_bird_nest");
		}
		break;
		case ViewId::TOOL:
		{
			m_icon->SetImage("set:bird_pack.json image:backpack_icon");
			setText(m_name, "gui_bird_pack");
		}
		break;
		case ViewId::FUSE:
		{
			m_icon->SetImage("set:bird_pack.json image:fuse_icon");
			setText(m_name, "gui_bird_fuse");
		}
		break;
		}
	}

}
