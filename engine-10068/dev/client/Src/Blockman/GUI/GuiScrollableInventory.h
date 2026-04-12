#ifndef __GUI_SCROLLABLE_INVENTORY_HEADER__
#define __GUI_SCROLLABLE_INVENTORY_HEADER__

#include <memory>
#include "UI/GUIWindow.h"
#include "UI/GUIGridView.h"
#include "UI/GUIScrollablePane.h"
#include "GUI/GuiSlotTable.h"
#include "Inventory/IInventory.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiScrollableInventory : public CustomGuiWindow
	{
	public:
		using SlotSize = Vector2;
		class ItemStackTouchEventArgs : public EventArgs
		{
		public:
			ItemStackTouchEventArgs(size_t index) : index(index) {}
			size_t index;
		};
		static const String ITEM_STACK_TOUCH_DOWN;
		static const String ITEM_STACK_TOUCH_UP;
		static const String ITEM_STACK_LONG_TOUCH_START;
		static const String ITEM_STACK_LONG_TOUCH_END;

	private:
		GuiItemSlot::Style m_slotStyle = GuiItemSlot::Style::METAL;
		using BaseClass = CustomGuiWindow;
		GUIGridView* m_gridView = nullptr;
		IInventory* m_inventory = nullptr;
		SlotSize m_slotSize{ 0, 0 };
		size_t m_columnCount = 0;
		bool m_isAutoSize = false;
		bool m_isAutoColumnCount = false;
		int m_showCount = 0;
		int m_rowSize = 0;
		int m_selectionIndex = -1;
		int m_oldSelectionIndex = -2;

		float m_horizontalInterval = 0.0;
		float m_verticalInterval = 0.0;
		Color m_itemStackBackgroundColor = Color::TRANS;
		GuiItemStack::NumberVisibility m_itemStackNumberVisibility = GuiItemStack::NumberVisibility::AUTO;

		bool onItemStackTouchDown(const EventArgs & events);
		bool onItemStackTouchUp(const EventArgs & events);
		bool onItemStackLongTouchStart(const EventArgs & events);
		bool onItemStackLongTouchEnd(const EventArgs & events);
		bool onItemSlotTouchDown(const EventArgs & events);
		bool onItemSlotTouchUp(const EventArgs & events);
		bool onItemSlotTouchMove(const EventArgs & events);
		bool onItemSlotMotionRelease(const EventArgs & events);
		void updateGridView();
		void updateSelectionFrame();
		void resize();

	protected:
		vector<GUIWindow*>::type getPrivateChildren() override;

	public:
		GuiScrollableInventory(GUIWindowType nType, const GUIString& strName);
		~GuiScrollableInventory() = default;

		void selectItemSlot(size_t index)
		{
			m_selectionIndex = index;
		}
		void unselectItemSlot()
		{
			m_selectionIndex = -1;
		}
		IInventory* getInventory() const
		{
			return m_inventory;
		}
		ItemStackPtr getItemStack(size_t index) const
		{
			return getInventory()->getStackInSlot(index);
		}
		void setInventory(IInventory* inventory)
		{
			m_inventory = inventory;
			m_showCount = inventory->getSizeInventory();
		}
		void setGrayedOut(size_t index, bool grayedOut, int type = 0)
		{
			dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(index))->setItemStackGrayedOut(grayedOut, type);
		}
		void setStyle(int style) { m_slotStyle = GuiItemSlot::Style(style); }
		void setShowCount(int size) { m_showCount = size; }
		void setRowSize(int size) { m_rowSize = size; }
		void setAutoColumnCount(bool isAutoColumnCount) { this->m_isAutoColumnCount = isAutoColumnCount; }
		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;
		void resetPos();
		GUIGridView* getGridView() { return m_gridView; }

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		// used to create animation of moving item stack
		UVector2 getGuiItemStackPosition(size_t index)
		{
			return dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(index))->getGuiItemStackPosition();
		}

		GuiItemStack* cloneGuiItemStack(size_t index, const GUIString& newName)
		{
			return dynamic_cast<GuiItemSlot*>(m_gridView->GetItem(index))->cloneGuiItemStack(newName);
		}

	};
}

#endif // !__GUI_SCROLLABLE_INVENTORY_HEADER__

