#include "gui_ranchFactory.h"
#include "Util/LanguageKey.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Game/Ranch.h"
#include "GUI/GuiRanchBuildItem.h"
#include "GUI/GuiDef.h"
#include "Util/ClientEvents.h"

using namespace LORD;

namespace BLOCKMAN
{
	gui_ranchFactory::gui_ranchFactory()
		: gui_layout("RanchFactory.json")
	{
	}

	gui_ranchFactory::~gui_ranchFactory()
	{
		m_subscriptionGuard.unsubscribeAll();
	}

	void gui_ranchFactory::onLoad()
	{
		m_lvList = getWindow<GUIListHorizontalBox>("RanchFactory-List");
		m_subscriptionGuard.add(RanchFactoryBuildChangeEvent::subscribe(std::bind(&gui_ranchFactory::onRanchFactoryChange, this)));
	}

	void gui_ranchFactory::onHide()
	{
	}

	bool gui_ranchFactory::isNeedLoad()
	{
		return checkGameType(ClientGameType::Rancher);
	}

	void gui_ranchFactory::onShow()
	{
		initItem();
	}

	bool gui_ranchFactory::onClick(const EventArgs & events, ViewId viewId)
	{
		return false;
	}

	bool gui_ranchFactory::onRanchFactoryChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return false;
	}

	void gui_ranchFactory::initItem()
	{
		if (!Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_ranch || !m_dataChange)
		{
			return;
		}
		float offset = m_lvList->GetScrollOffset();
		m_lvList->ClearAllItem();
		auto items = Blockman::Instance()->m_pPlayer->m_ranch->getFactoryBuild();
		offset = items.size() == 0 ? 0.f : offset;
		for (auto item : items)
		{
			static unsigned count = 0;
			count++;
			String itemLayout = StringUtil::Format("RanchFactory-List-Item-%d", count).c_str();
			if (GUIWindowManager::Instance()->GetGUIWindow(itemLayout.c_str()))
			{
				return;
			}
			GuiRanchBuildItem* itemView = (GuiRanchBuildItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RANCH_BUILD_ITEM, itemLayout.c_str());
			itemView->SetTouchable(false);
			itemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, 218.f), UDim(1.0f, 0));
			itemView->setType(2);
			itemView->setBuild(item);
			itemView->SetTouchable(false);
			m_lvList->AddItem(itemView, false);
		}

		m_lvList->SetScrollOffset(offset);
		m_dataChange = false;
	}
}
