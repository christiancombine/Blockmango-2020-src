#pragma once
#ifndef __GUI_COMMON_TAB_GRID_VIEW_HEADER__
#define __GUI_COMMON_TAB_GRID_VIEW_HEADER__
#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "UI/GUILayout.h"
#include "UI/GUIGridView.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GuiCommonTabGridView : public CustomGuiWindow
	{
	private:
		using BaseClass = CustomGuiWindow;
		GUILayout* m_window = nullptr;
		GUILayout* m_tabs = nullptr;
		GUIGridView* m_gvItems = nullptr;

	private:
		void removeComponents();

	public:
		GuiCommonTabGridView(const GUIWindowType& nType, const GUIString& strName);
		~GuiCommonTabGridView() = default;
		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void removeAllItems();
		void initGridView(float hInterval, float vInterval, i32 rowSize);
		void addItemView(GUIWindow* itemView, bool changeSize = true);
		void setGridViewArea(const URect& area);
		size_t getItemSize();
		GUIWindow* getItem(i32 index);

		void removeAllTabs();
		void addTabView(GUIWindow* tabView);
		void setTabsArea(const URect& area);

		bool SetProperty(const GUIString& strName, const GUIString& strValue) override;
		bool GetProperty(const GUIString& strName, GUIString& strValue) override;
		void GetPropertyList(GUIPropertyList& properlist) override;
		bool GetPropertyType(const GUIString& strName, GUIPropertyType& type) override;

	public:
		virtual UVector2 getItemPosition();

	protected:
		URect m_gvArea = URect(UDim(0,0), UDim(0, 0), UDim(1.f, 0), UDim(1.f, 0));
		URect m_tabsArea = URect(UDim(0,0), UDim(0, 0), UDim(1.f, -40.f), UDim(1.f, -40.f));
		Vector4 m_gridViewConfig = Vector4(0, 0, 3, 0);

		vector<GUIWindow*>::type getPrivateChildren() override;
	};

}
#endif
