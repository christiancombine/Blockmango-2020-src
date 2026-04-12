#include "GuiScrollableInventory.h"
#include "Object/Root.h"
#include "UI/GUIWindowManager.h"
#include "GUI/GuiDef.h"
#include "Util/StringUtil.h"

namespace BLOCKMAN
{

	const String GuiScrollableInventory::ITEM_STACK_TOUCH_DOWN = "ItemStackTouchDown";
	const String GuiScrollableInventory::ITEM_STACK_TOUCH_UP = "ItemStackTouchUp";
	const String GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_START = "ItemStackLongTouchStart";
	const String GuiScrollableInventory::ITEM_STACK_LONG_TOUCH_END = "ItemStackLongTouchEnd";

	GuiScrollableInventory::GuiScrollableInventory(GUIWindowType nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_gridView = dynamic_cast<GUIGridView*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_GRID_VIEW, "GridView"));
		renameComponents(m_gridView);
		m_gridView->SetProperty(GWP_GRID_VIEW_ITEM_ALIGNMENT, "Centre");
		m_gridView->SetArea({0, 0 }, { 0, 0 }, { 1.0f, 0 }, { 1.0f, 0 });
		AddWindowToDrawList(*m_gridView);
		subscribeToBubbleUpMouseEvents();
	}

	bool GuiScrollableInventory::onItemStackTouchDown(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_TOUCH_DOWN.c_str(), newEvent);
		return true;
	}

	bool GuiScrollableInventory::onItemStackTouchUp(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_TOUCH_UP.c_str(), newEvent);
		return true;
	}

	bool GuiScrollableInventory::onItemStackLongTouchStart(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_LONG_TOUCH_START.c_str(), newEvent);
		return false;
	}

	bool GuiScrollableInventory::onItemStackLongTouchEnd(const EventArgs & events)
	{
		auto windowEventArgs = dynamic_cast<const WindowEventArgs&>(events);
		auto pWindow = windowEventArgs.window;
		auto index = pWindow->GetUserData();
		auto newEvent = ItemStackTouchEventArgs(index);
		fireEvent(ITEM_STACK_LONG_TOUCH_END.c_str(), newEvent);
		return true;
	}

	bool GuiScrollableInventory::onItemSlotTouchDown(const EventArgs & events)
	{
		if (m_gridView)
		{
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			m_gridView->TouchDown(mouseEventArgs.position);
		}
		return true;
	}

	bool GuiScrollableInventory::onItemSlotTouchUp(const EventArgs & events)
	{
		if (m_gridView)
		{
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			m_gridView->TouchUp(mouseEventArgs.position);
		}
		return true;
	}

	bool GuiScrollableInventory::onItemSlotTouchMove(const EventArgs & events)
	{
		if (m_gridView)
		{
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			m_gridView->TouchMove(mouseEventArgs.position);
		}
		return true;
	}

	bool GuiScrollableInventory::onItemSlotMotionRelease(const EventArgs & events)
	{
		if (m_gridView)
		{
			m_gridView->MotionRelease();
		}
		return true;
	}

	void GuiScrollableInventory::updateGridView()
	{
		m_columnCount = m_isAutoColumnCount ? 
			(int)((GetPixelSize().x + m_horizontalInterval) / (m_slotSize.x + m_horizontalInterval)) : m_columnCount;
		if (m_isAutoSize)
		{
			m_slotSize.x = (GetPixelSize().x - m_horizontalInterval * m_columnCount) / m_columnCount;
			m_slotSize.y = m_slotSize.x;
		}
		
		if (m_rowSize > 1)
		{
			float Interval = (GetPixelSize().x - m_slotSize.x * m_rowSize) / (m_rowSize - 1);
			m_gridView->InitConfig(Interval, Interval, m_rowSize);
		}
		else
		{
			m_gridView->InitConfig(m_horizontalInterval, m_verticalInterval, m_columnCount);
		}

		for (int i = 0; i < int(m_gridView->GetItemCount()); ++i)
		{
			auto pGuiItemSlot = dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(i));
			pGuiItemSlot->SetWidth({ 0, m_slotSize.x });
			pGuiItemSlot->SetHeight({ 0, m_slotSize.y });
		}
		/*m_gridView->ChangeContainerWidth();*/
	}

	void GuiScrollableInventory::resize()
	{
		m_gridView->RemoveAllItems();
		for (int i = 0; i < m_showCount; ++i)
		{
			GuiItemSlot* guiItemSlot = dynamic_cast<GuiItemSlot*>(GUIWindowManager::Instance()->CreateGUIWindow(GWT_ITEM_SLOT, m_strName + "-ItemSlot" + StringUtil::ToString(i).c_str()));
			guiItemSlot->setStyle(m_slotStyle);
			auto pItemStack = m_inventory->getStackInSlot(i);
			guiItemSlot->setItemStack(pItemStack);
			guiItemSlot->SetUserData(i);
			guiItemSlot->subscribeEvent(ITEM_STACK_TOUCH_DOWN.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackTouchDown, this));
			guiItemSlot->subscribeEvent(ITEM_STACK_TOUCH_UP.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackTouchUp, this));
			guiItemSlot->subscribeEvent(ITEM_STACK_LONG_TOUCH_START.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackLongTouchStart, this));
			guiItemSlot->subscribeEvent(ITEM_STACK_LONG_TOUCH_END.c_str(), SubscriberSlot(&GuiScrollableInventory::onItemStackLongTouchEnd, this));
			guiItemSlot->subscribeEvent(EventWindowTouchDown, SubscriberSlot(&GuiScrollableInventory::onItemSlotTouchDown, this));
			guiItemSlot->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&GuiScrollableInventory::onItemSlotTouchUp, this));
			guiItemSlot->subscribeEvent(EventWindowTouchMove, SubscriberSlot(&GuiScrollableInventory::onItemSlotTouchMove, this));
			guiItemSlot->subscribeEvent(EventMotionRelease, SubscriberSlot(&GuiScrollableInventory::onItemSlotMotionRelease, this));
			guiItemSlot->SetSize({ { 0, m_slotSize.x },{ 0, m_slotSize.y } });
			guiItemSlot->setItemStackBackgroundColor(m_itemStackBackgroundColor);
			guiItemSlot->setItemStackNumberVisibility(m_itemStackNumberVisibility);
			m_gridView->AddItem(guiItemSlot);
		}
		/*m_gridView->ChangeContainerWidth();*/
	}

	void GuiScrollableInventory::resetPos()
	{
		if (m_gridView)
			m_gridView->ResetPos();
	}
	
	vector<GUIWindow*>::type GuiScrollableInventory::getPrivateChildren()
	{
		return { m_gridView };
	}

	void GuiScrollableInventory::updateSelectionFrame()
	{
		if (m_selectionIndex == m_oldSelectionIndex)
		{
			return;
		}
		if (m_oldSelectionIndex >= 0)
		{
			if (m_gridView->GetItemCount() > m_oldSelectionIndex)
			{
				dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(m_oldSelectionIndex))->setSelected(false);
			}
		}
		if (m_selectionIndex >= 0)
		{
			auto pSelectedSlot = dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(m_selectionIndex));
			pSelectedSlot->setSelected(true);
			pSelectedSlot->ShowOnTop();
		}
		m_oldSelectionIndex = m_selectionIndex;
	}

	bool GuiScrollableInventory::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (BaseClass::SetProperty(strName, strValue))
		{
			return true;
		}

		if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			this->m_columnCount = StringUtil::ParseInt(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			this->m_slotSize.x = StringUtil::ParseReal(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			this->m_slotSize.y = StringUtil::ParseReal(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_ITEM_STACK_BACKGROUND_COLOR)
		{
			this->m_itemStackBackgroundColor = StringUtil::ParseColor(strValue.c_str());
			return true;
		}
		else if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			this->m_itemStackNumberVisibility = static_cast<GuiItemStack::NumberVisibility>(StringUtil::ParseInt(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_COLUMN_COUNT)
		{
			this->m_isAutoColumnCount = StringUtil::ParseBool(strValue.c_str());
			return true;
		}
		else if (strName == GWP_TABLE_HORIZONTAL_INTERVAL)
		{
			m_horizontalInterval = StringUtil::ParseFloat(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_TABLE_VERTICAL_INTERVAL)
		{
			m_verticalInterval = StringUtil::ParseFloat(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_SIZE)
		{
			this->m_isAutoSize = StringUtil::ParseBool(strValue.c_str());
			updateGridView();
			return true;
		}
		else if (strName == GWP_SLOT_STYLE)
		{
			this->m_slotStyle = static_cast<GuiItemSlot::Style>(StringUtil::ParseInt(strValue.c_str()));
			return true;
		}
		return false;
	}

	bool GuiScrollableInventory::GetProperty(const GUIString& strName, GUIString& strValue)
	{
		if (BaseClass::GetProperty(strName, strValue))
		{
			return true;
		}
		
		if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			strValue = StringUtil::ToString(this->m_columnCount).c_str();
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			strValue = StringUtil::ToString(this->m_slotSize.x).c_str();
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			strValue = StringUtil::ToString(this->m_slotSize.y).c_str();
			return true;
		}
		else if (strName == GWP_ITEM_STACK_BACKGROUND_COLOR)
		{
			strValue = StringUtil::ToString(this->m_itemStackBackgroundColor).c_str();
			return true;
		}
		else if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			strValue = StringUtil::ToString(static_cast<int>(m_itemStackNumberVisibility)).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_COLUMN_COUNT)
		{
			strValue = StringUtil::ToString(m_isAutoColumnCount).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_HORIZONTAL_INTERVAL)
		{
			strValue = StringUtil::ToString(m_horizontalInterval).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_VERTICAL_INTERVAL)
		{
			strValue = StringUtil::ToString(m_verticalInterval).c_str();
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_SIZE)
		{
			strValue = StringUtil::ToString(m_isAutoSize).c_str();
			updateGridView();
			return true;
		}
		else if (strName == GWP_SLOT_STYLE)
		{
			strValue = StringUtil::ToString(static_cast<int>(m_slotStyle)).c_str();
			return true;
		}
		return false;
	}

	void GuiScrollableInventory::GetPropertyList(GUIPropertyList& properlist)
	{
		BaseClass::GetPropertyList(properlist);
		properlist.push_back(GWP_TABLE_COLUMN_COUNT);
		properlist.push_back(GWP_SLOT_WIDTH);
		properlist.push_back(GWP_SLOT_HEIGHT);
		properlist.push_back(GWP_ITEM_STACK_BACKGROUND_COLOR);
		properlist.push_back(GWP_ITEM_STACK_NUMBER_VISIBILITY);
		properlist.push_back(GWP_TABLE_AUTO_COLUMN_COUNT);
		properlist.push_back(GWP_TABLE_HORIZONTAL_INTERVAL);
		properlist.push_back(GWP_TABLE_VERTICAL_INTERVAL);
		properlist.push_back(GWP_TABLE_AUTO_SIZE);
		properlist.push_back(GWP_SLOT_STYLE);
	}

	bool GuiScrollableInventory::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (BaseClass::GetPropertyType(strName, type))
		{
			return true;
		}

		if (strName == GWP_TABLE_COLUMN_COUNT)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_SLOT_WIDTH)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_SLOT_HEIGHT)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_ITEM_STACK_BACKGROUND_COLOR)
		{
			type = GPT_COLOR;
			return true;
		}
		else if (strName == GWP_ITEM_STACK_NUMBER_VISIBILITY)
		{
			type = GPT_INT;
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_COLUMN_COUNT)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_TABLE_HORIZONTAL_INTERVAL)
		{
			 type = GPT_FLOAT;;
			return true;
		}
		else if (strName == GWP_TABLE_VERTICAL_INTERVAL)
		{
			type = GPT_FLOAT;
			return true;
		}
		else if (strName == GWP_TABLE_AUTO_SIZE)
		{
			type = GPT_BOOL;
			return true;
		}
		else if (strName == GWP_SLOT_STYLE)
		{
			type = GPT_INT;
			return true;
		}

		return false;
	}

	void GuiScrollableInventory::UpdateSelf(ui32 nTimeElapse)
	{
		if (!m_gridView)
		{
			return;
		}
		if (m_isAutoColumnCount || m_isAutoSize)
		{
			updateGridView();
		}
		m_gridView->SetArea(getAbsolutePositionFromRoot(), { { 0, GetPixelSize().x },{ 0, GetPixelSize().y } });
		if (!m_inventory)
		{
			return;
		}
		if (m_gridView->GetItemCount() != m_showCount)
		{
			resize();
		}
		for (int i = 0; i < m_showCount; ++i)
		{
			if (auto guiItemSlot = dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(i)))
			{
				guiItemSlot->setItemStack(m_inventory->getStackInSlot(i));
			}
		}
		updateSelectionFrame();
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GuiScrollableInventory::Destroy()
	{
		if (m_gridView)
		{
			RemoveWindowFromDrawList(*m_gridView);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_gridView);
			m_gridView = nullptr;
		}
	}
}