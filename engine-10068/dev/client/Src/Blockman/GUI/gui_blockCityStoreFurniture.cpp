#include "gui_blockCityStoreFurniture.h"
#include "GUI/GuiBlockCityTigerLotteryItem.h"
#include "GUI/GuiDef.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Util/ClientEvents.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"
#include "UI/GUIRadioButton.h"
using namespace LORD;

namespace BLOCKMAN
{
	gui_blockCityStoreFurniture::gui_blockCityStoreFurniture()
		: gui_layout("BlockCityStoreFurniture.json")
	{
	}

	gui_blockCityStoreFurniture::~gui_blockCityStoreFurniture()
	{

	}

	void gui_blockCityStoreFurniture::onLoad()
	{
		m_tabs = getWindow<GUILayout>("BlockCityStoreFurniture-Tabs");
		m_itemsView = getWindow<GUIGridView>("BlockCityStoreFurniture-Items");
		m_itemsView->InitConfig(3.f, 3.f, 5);
		m_subscriptionGuard.add(UpdateBlockCityStoreEvent::subscribe(std::bind(&gui_blockCityStoreFurniture::onDataChange, this)));
		m_subscriptionGuard.add(BlockCityStoreItemClickEvent::subscribe(std::bind(&gui_blockCityStoreFurniture::onItemClick, this, std::placeholders::_1)));
	}

	void gui_blockCityStoreFurniture::onShow()
	{
		if (m_dataChange)
		{
			initData();
			m_dataChange = false;
		}
		else
		{
			if (m_item.id > 0)
				BlockCityStoreItemClickEvent::emit(m_item);
		}
	}

	void gui_blockCityStoreFurniture::onUpdate(ui32 nTimeElapse)
	{

	}

	bool gui_blockCityStoreFurniture::onDataChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	bool gui_blockCityStoreFurniture::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BlockCity ,ClientGameType::BlockFort });
	}

	void gui_blockCityStoreFurniture::initData()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}

		auto stores = Blockman::Instance()->m_pPlayer->m_blockCity->getStore();
		for (BlockCityStoreTab store : stores)
		{
			if (store.tabId == 3)
			{
				initTabs(store.groups);
				break;
			}
		}
	}

	void gui_blockCityStoreFurniture::initTabs(std::vector<BlockCityStoreGroup>& groups)
	{
		m_radioMap.clear();
		static int count = 0;
		if (groups.size() != m_tabs->GetChildCount())
		{
			m_tabs->CleanupChildren();
			for (size_t i = 0; i < groups.size(); i++)
			{
				GUIString tabName = StringUtil::Format("BlockCityStoreFurniture-Tabs-Item-%d", count++).c_str();
				GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, tabName);
				radioItem->SetArea(UDim(i * 0.18f, 0), UDim(0, 0), UDim(0.168333f, 0.f), UDim(0, 56.f));
				radioItem->SetNormalImage("set:blockcity_store.json image:row_lable_nor");
				radioItem->SetPushedImage("set:blockcity_store.json image:row_lable_pre");
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityStoreFurniture::onRadioChange, this, std::placeholders::_1, groups[i].groupId));
				radioItem->SetText(getString(groups[i].groupName.c_str()));
				radioItem->SetTextColor(Color::BLACK);

				GUIString tabIconName = StringUtil::Format("BlockCityStoreFurniture-Tabs-Icon-Item-%d", count).c_str();
				GUIStaticImage* radioItemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, tabIconName);
				radioItemIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0.f), UDim(1.f, 0.f));
				radioItemIcon->SetImage(groups[i].groupIcon.c_str());
				radioItem->AddChildWindow(radioItemIcon);

				GUIString tabSignName = StringUtil::Format("BlockCityStoreFurniture-Tabs-Sign-%d", count).c_str();
				GUIStaticImage* radioItemSign = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, tabSignName);
				radioItemSign->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.f, 54.f), UDim(0.f, 17.f));
				radioItemSign->SetImage("set:blockcity_pack.json image:new2");
				radioItemSign->SetVerticalAlignment(VerticalAlignment::VA_TOP);
				radioItemSign->SetHorizontalAlignment(HorizontalAlignment::HA_RIGHT);
				radioItemSign->SetTouchable(false);
				radioItemSign->SetVisible(groups[i].isNew == 1);
				radioItem->AddChildWindow(radioItemSign);

				m_radioMap.insert({ groups[i].groupId, radioItem });
				m_tabs->AddChildWindow(radioItem);
			}
		}
		else
		{
			for (size_t i = 0; i < m_tabs->GetChildCount(); i++)
			{
				auto radioItem = dynamic_cast<GUIRadioButton*>(m_tabs->GetChildByIndex(i));
				radioItem->removeAllEvents();
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityStoreFurniture::onRadioChange, this, std::placeholders::_1, groups[i].groupId));
				radioItem->SetText(getString(groups[i].groupName.c_str()));
				radioItem->SetTextColor(Color::BLACK);
				m_radioMap.insert({ groups[i].groupId, radioItem });
			}
		}

		i32 groupId = m_item.groupId > 0 ? m_item.groupId : 1;
		if (m_radioMap[groupId])
		{
			if (m_radioMap[groupId]->IsSelected())
			{
				onRadioChange(EventArgs(), groupId);
			}
			else
			{
				m_radioMap[groupId]->SetSelected(true);
			}
		}
	}

	void gui_blockCityStoreFurniture::updateItems(std::vector<BlockCityStoreItem>& items)
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}
		static int count = 0;
		float itemX = (m_itemsView->GetPixelSize().x - 4 * 3.f) / 5;
		float itemY = 125.f / 135.f * itemX;

		if (items.size() != m_itemsView->GetItemCount())
		{
			m_itemsView->RemoveAllItems();
			m_itemsView->SetScrollOffset(0);
			for (const auto& item : items)
			{
				String iterLayout = StringUtil::Format("BlockCityStoreFurniture-Items-%d", count++).c_str();
				GuiBlockCityTigerLotteryItem* itemView = (GuiBlockCityTigerLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_TIGER_LOTTERY_ITEM, iterLayout.c_str());
				itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, itemX), UDim(0, itemY));
				itemView->setStoreItem(item);
				itemView->setShowType(2);
				m_itemsView->AddItem(itemView);
			}
		}
		else
		{
			for (size_t i = 0; i < m_itemsView->GetItemCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_itemsView->GetItem(i));
				itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, itemX), UDim(0, itemY));
				itemView->setStoreItem(items[i]);
				itemView->setShowType(2);
			}
		}


		BlockCityStoreItem  defaultItem;

		for (size_t i = 0; i < items.size(); i++)
		{
			if (i == 0)
			{
				defaultItem = items[0];
			}
			if (items[i].id == m_item.id && items[i].id != 0)
			{
				defaultItem = items[i];
				break;
			}
		}
		if (defaultItem.id != 0)
		{
			m_item = defaultItem;
			BlockCityStoreItemClickEvent::emit(m_item);
		}
	}

	bool gui_blockCityStoreFurniture::onRadioChange(const EventArgs & args, i32 groupId)
	{
		if (m_radioMap[groupId] && m_radioMap[groupId]->IsSelected())
		{
			auto stores = Blockman::Instance()->m_pPlayer->m_blockCity->getStore();
			for (BlockCityStoreTab store : stores)
			{
				if (store.tabId == 3)
				{
					std::vector<BlockCityStoreItem> items = std::vector<BlockCityStoreItem>();
					for (const auto& item : store.items)
					{
						if (item.groupId == groupId)
						{
							items.push_back(item);
						}
					}

					if (items.size() > 2)
					{
						sort(items.begin(), items.end(), [](const BlockCityStoreItem &a, const BlockCityStoreItem &b) {
							return a.weight < b.weight;
						});
					}

					updateItems(items);
					break;
				}
			}
		}
		return true;
	}

	bool gui_blockCityStoreFurniture::onItemClick(const BlockCityStoreItem & item)
	{
		if (!isShown())
		{
			return false;
		}
		m_item = item;
		for (size_t i = 0; i < m_itemsView->GetItemCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_itemsView->GetItem(i));
			itemView->setSelectId(item.id);
		}
		return true;
	}

	UVector2 gui_blockCityStoreFurniture::getItemPosition()
	{
		for (size_t i = 0; i < m_itemsView->GetItemCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_itemsView->GetItem(i));
			if (itemView->isSelected())
			{
				return UVector2(UDim(0, itemView->GetRenderArea().left + itemView->GetRenderArea().getWidth() / 2), UDim(0, itemView->GetRenderArea().top + itemView->GetRenderArea().getHeight() / 2));
			}
		}
		return UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
	}

}
