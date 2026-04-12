#include "gui_blockCityPortal.h"
#include "GUI/GuiBlockCityPortalItem.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "cWorld/Blockman.h"
#include "UI/GUILayout.h"
#include "Network/ClientNetwork.h"
#include "Util/ClientEvents.h"
#include "GuiDef.h"

namespace BLOCKMAN
{
	gui_blockCityPortal::gui_blockCityPortal()
		: gui_layout("BlockCityPortal.json")
	{
	}

	gui_blockCityPortal::~gui_blockCityPortal()
	{
	}

	void gui_blockCityPortal::onLoad()
	{
		setText("BlockCityPortal-Title", "gui_portal_title");
		setText("BlockCityPortal-Transfer", "gui_blockcity_transfer");
		setText("BlockCityPortal-Not-Open-Tip", "gui_portal_transfer_fail");

		m_items = getWindow<GUILayout>("BlockCityPortal-Detail");
		m_subscriptionGuard.add(BlockCityPortalItemClickEvent::subscribe(std::bind(&gui_blockCityPortal::onItemClick, this, std::placeholders::_1)));
		getWindow("BlockCityPortal-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityPortal::onClick, this, std::placeholders::_1, ViewId::CLOSE));
		getWindow("BlockCityPortal-Transfer")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityPortal::onClick, this, std::placeholders::_1, ViewId::TRANSFER));
	}

	void gui_blockCityPortal::onShow()
	{
	}

	void gui_blockCityPortal::onHide()
	{
	}

	bool gui_blockCityPortal::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	void gui_blockCityPortal::onUpdate(ui32 nTimeElapse)
	{
		Vector3 position = Blockman::Instance()->m_pPlayer->position;
		i32 portalId = BlockCityPortalSetting::getPortalActorByPos(position);
		if (portalId != 0 && !m_isShowUI && !BLOCKMAN::Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			showPortal(portalId);
			m_isShowUI = true;
		}
		if (portalId == 0)
		{
			RootGuiLayout::Instance()->hideBlockCityPortal();
			m_isShowUI = false;
		}
	}

	void gui_blockCityPortal::showPortal(i32 portalId)
	{
		m_portalId = portalId;
		RootGuiLayout::Instance()->showBlockCityPortal();

		auto portalItems = BlockCityPortalSetting::getPortalItemsById(m_portalId);
		static int count = 0;
		float itemY = (m_items->GetPixelSize().y - 3 * 3.f) / 4;
		if (portalItems.size() != m_items->GetChildCount())
		{
			m_items->CleanupChildren();
			for (size_t i = 0; i < portalItems.size(); i++)
			{
				GUIString strItemName = StringUtil::Format("BlockCityPortal-Detail-Items-%d", count++).c_str();
				GuiBlockCityPortalItem* itemView = (GuiBlockCityPortalItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_PORTAL_ITEM, strItemName.c_str());
				itemView->SetArea(UDim(0, 0), UDim(0, i * itemY), UDim(1.0f, 0.f), UDim(0.0f, itemY));
				itemView->onSetItem(portalItems[i]);
				m_items->AddChildWindow(itemView);
			}

		}
		else
		{
			for (size_t i = 0; i < m_items->GetChildCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockCityPortalItem*>(m_items->GetChildByIndex(i));
				itemView->onSetItem(portalItems[i]);
			}
		}

		BlockCityPortalItem defaultItem;
		for (size_t i = 0; i < portalItems.size(); i++)
		{
			if (i == 0)
			{
				defaultItem = portalItems[0];
			}
			if (portalItems[i].id == m_curItem.id && portalItems[i].id != 0)
			{
				defaultItem = portalItems[i];
				break;
			}
		}
		if (defaultItem.id != 0)
		{
			m_curItem = defaultItem;
			BlockCityPortalItemClickEvent::emit(m_curItem);
		}
	}

	bool gui_blockCityPortal::onItemClick(const BlockCityPortalItem & item)
	{
		m_curItem = item;
		for (size_t i = 0; i < m_items->GetChildCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCityPortalItem*>(m_items->GetChildByIndex(i));
			itemView->setSelect(item.id);
		}
		return true;
	}

	bool gui_blockCityPortal::onClick(const EventArgs& args, ViewId viewId)
	{
		switch (viewId)
		{
		case CLOSE:
			RootGuiLayout::Instance()->showMainControl();
			break;
		case TRANSFER:
			if (!m_curItem.isOpen)
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_portal_transfer_fail");
				return false;
			}
			ClientNetwork::Instance()->getCommonSender()->sendBlockCityPortal(m_curItem.portalId, m_curItem.id);
			RootGuiLayout::Instance()->showMainControl();
			break;
		}
		return true;
	}

}