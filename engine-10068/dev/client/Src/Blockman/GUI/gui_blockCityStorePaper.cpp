#include "gui_blockCityStorePaper.h"
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
	gui_blockCityStorePaper::gui_blockCityStorePaper()
		: gui_layout("BlockCityStorePaper.json")
	{
	}

	gui_blockCityStorePaper::~gui_blockCityStorePaper()
	{

	}

	void gui_blockCityStorePaper::onLoad()
	{
		setText("BlockCityStorePaper-Material-Label", "gui_blockcity_store_material");
		m_tabs = getWindow<GUILayout>("BlockCityStorePaper-Tabs");
		m_itemsView = getWindow<GUIGridView>("BlockCityStorePaper-Items");
		m_materialItems = getWindow<GUIGridView>("BlockCityStorePaper-Material-Items");
		m_itemsView->InitConfig(3.f, 3.f, 3);
		m_materialItems->InitConfig(4.f, 4.f, 3);
		m_subscriptionGuard.add(UpdateBlockCityStoreEvent::subscribe(std::bind(&gui_blockCityStorePaper::onDataChange, this)));
		m_subscriptionGuard.add(BlockCityStoreItemClickEvent::subscribe(std::bind(&gui_blockCityStorePaper::onItemClick, this, std::placeholders::_1)));
	}

	void gui_blockCityStorePaper::onShow()
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

	bool gui_blockCityStorePaper::onDataChange()
	{
		m_dataChange = true;
		if (isShown())
		{
			onShow();
		}
		return true;
	}

	bool gui_blockCityStorePaper::isNeedLoad()
	{
		return checkGameType({ ClientGameType::BlockCity ,ClientGameType::BlockFort });
	}

	void gui_blockCityStorePaper::initData()
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}

		auto stores = Blockman::Instance()->m_pPlayer->m_blockCity->getStore();
		for (BlockCityStoreTab store : stores)
		{
			if (store.tabId == 1)
			{
				initTabs(store.groups);
				break;
			}
		}
	}

	void gui_blockCityStorePaper::initTabs(std::vector<BlockCityStoreGroup>& groups)
	{
		m_radioMap.clear();
		static int count = 0;
		if (groups.size() != m_tabs->GetChildCount())
		{
			m_tabs->CleanupChildren();
			for (size_t i = 0; i < groups.size(); i++)
			{
				GUIString tabName = StringUtil::Format("BlockCityStorePaper-Tabs-Item-%d", count++).c_str();
				GUIRadioButton* radioItem = (GUIRadioButton*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_RADIO, tabName);
				radioItem->SetArea(UDim(i * 0.18f, 0), UDim(0, 0), UDim(0.168333f, 0.f), UDim(0, 56.f));
				radioItem->SetNormalImage("set:blockcity_store.json image:row_lable_nor");
				radioItem->SetPushedImage("set:blockcity_store.json image:row_lable_pre");
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityStorePaper::onRadioChange, this, std::placeholders::_1, groups[i].groupId));
				radioItem->SetText(getString(groups[i].groupName.c_str()));
				radioItem->SetTextColor(Color::BLACK);

				GUIString tabIconName = StringUtil::Format("BlockCityStorePaper-Tabs-Icon-Item-%d", count).c_str();
				GUIStaticImage* radioItemIcon = (GUIStaticImage*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_IMAGE, tabIconName);
				radioItemIcon->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.f, 0.f), UDim(1.f, 0.f));
				radioItemIcon->SetImage(groups[i].groupIcon.c_str());
				radioItem->AddChildWindow(radioItemIcon);

				GUIString tabSignName = StringUtil::Format("BlockCityStorePaper-Tabs-Sign-%d", count).c_str();
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
				radioItem->subscribeEvent(EventRadioStateChanged, std::bind(&gui_blockCityStorePaper::onRadioChange, this, std::placeholders::_1, groups[i].groupId));
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

	void gui_blockCityStorePaper::updateItems(std::vector<BlockCityStoreItem>& items)
	{
		if (!UICommon::checkBlockCityParam())
		{
			return;
		}
		static int count = 0;
		float itemX = (m_itemsView->GetPixelSize().x - 2 * 3.f) / 3;
		float itemY = 125.f / 135.f * itemX;

		if (items.size() != m_itemsView->GetItemCount())
		{
			m_itemsView->RemoveAllItems();
			m_itemsView->SetScrollOffset(0);
			for (const auto& item : items)
			{
				String iterLayout = StringUtil::Format("BlockCityStorePaper-Items-%d", count++).c_str();
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
		for (size_t i = 0; i < items.size() ; i++)
		{
			if (i == 0)
			{
				defaultItem = items[0];
			}
			if ( items[i].id == m_item.id && items[i].id != 0)
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

	bool gui_blockCityStorePaper::onRadioChange(const EventArgs & args, i32 groupId)
	{
		if (m_radioMap[groupId] && m_radioMap[groupId]->IsSelected())
		{
			auto stores = Blockman::Instance()->m_pPlayer->m_blockCity->getStore();
			for (BlockCityStoreTab store : stores)
			{
				if (store.tabId == 1)
				{
					std::vector<BlockCityStoreItem> items = std::vector<BlockCityStoreItem>();
					for (const auto& item : store.items)
					{
						if (item.groupId == groupId)
						{
							items.push_back(item);
						}
					}
					updateItems(items);
					break;
				}
			}
		}
		return true;
	}

	bool gui_blockCityStorePaper::onItemClick(const BlockCityStoreItem & item)
	{
		if (!isShown())
		{
			return false;
		}

		m_item = item;
		static int count = 0;
		float materiaX = (m_materialItems->GetPixelSize().x - 2 * 3.f) / 3;
		float materiaY = 88.f / 85.f * materiaX;
		
		if (m_materialItems->GetItemCount()  != m_item.materials.size())
		{
			m_materialItems->RemoveAllItems();
			m_materialItems->SetScrollOffset(0);
			for (const auto& matItem : item.materials)
			{
				String matIterLayout = StringUtil::Format("BlockCityStorePaper-Material-Items-%d", count++).c_str();
				GuiBlockCityTigerLotteryItem* matItemView = (GuiBlockCityTigerLotteryItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BLOCKCITY_TIGER_LOTTERY_ITEM, matIterLayout.c_str());
				matItemView->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0, materiaX), UDim(0, materiaY));
				matItemView->setStoreMaterialItem(matItem);
				m_materialItems->AddItem(matItemView);
			}
		}
		else
		{
			for (size_t i = 0; i < m_materialItems->GetItemCount(); i++)
			{
				auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_materialItems->GetItem(i));
				itemView->SetArea(UDim(0, 0), UDim(0, 0), UDim(0, materiaX), UDim(0, materiaY));
				itemView->setStoreMaterialItem(item.materials[i]);
				itemView->setShowType(2);
			}
		}

		for (size_t i = 0; i < m_itemsView->GetItemCount(); i++)
		{
			auto itemView = dynamic_cast<GuiBlockCityTigerLotteryItem*>(m_itemsView->GetItem(i));
			itemView->setSelectId(item.id);
		}
		return true;
	}

	UVector2 gui_blockCityStorePaper::getItemPosition()
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
