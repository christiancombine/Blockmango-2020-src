#include "GuiCommonTabGridView.h"
#include "UI/GUIWindowManager.h"
#include "Object/Root.h"
#include "GUI/GuiDef.h"

namespace BLOCKMAN
{

	void GuiCommonTabGridView::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	vector<GUIWindow*>::type GuiCommonTabGridView::getPrivateChildren()
	{
		return { m_window };
	}

	GuiCommonTabGridView::GuiCommonTabGridView(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("CommonTabGridView.json"));
		renameComponents(m_window);

		m_tabs = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_gvItems = dynamic_cast<GUIGridView*>(m_window->GetChildByIndex(1));
		if (Root::Instance()->isEditorMode())
		{
			AddWindowToDrawList(*m_window);
		}
		else
		{
			AddChildWindow(m_window);
		}
	}

	void GuiCommonTabGridView::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		if (Root::Instance()->isEditorMode())
		{
			m_window->SetPosition(getAbsolutePositionFromRoot());
		}
		BaseClass::UpdateSelf(nTimeElapse);
	}


	void GuiCommonTabGridView::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiCommonTabGridView::removeAllItems()
	{
		m_gvItems->RemoveAllItems();
		m_gvItems->ResetPos();
	}

	void GuiCommonTabGridView::initGridView(float hInterval, float vInterval, i32 rowSize)
	{
		if (m_gvItems)
		{
			m_gridViewConfig = Vector4(hInterval, vInterval, (float)rowSize, 0.f);
			m_gvItems->InitConfig(hInterval, vInterval, rowSize);
		}
	}

	void GuiCommonTabGridView::addItemView(GUIWindow * itemView, bool changeSize)
	{
		if (changeSize)
		{
			float x = (m_gvItems->GetPixelSize().x - m_gvItems->GetHorizontalInterval() * (m_gvItems->GetRowSize() - 1)) / m_gvItems->GetRowSize();
			itemView->SetSize(UVector2(UDim(0, x), UDim(0, x)));
		}
		m_gvItems->AddItem(itemView);
	}

	void GuiCommonTabGridView::setGridViewArea(const URect & area)
	{
		if (m_gvItems)
		{
			m_gvArea = area;
			m_gvItems->SetArea(area);
		}
	}

	size_t GuiCommonTabGridView::getItemSize()
	{
		if (m_gvItems)
		{
			return m_gvItems->GetItemCount();
		}
		return 0;
	}

	GUIWindow * GuiCommonTabGridView::getItem(i32 index)
	{
		return m_gvItems->GetItem(index);
	}

	void GuiCommonTabGridView::removeAllTabs()
	{
		m_tabs->CleanupChildren();
	}

	void GuiCommonTabGridView::addTabView(GUIWindow * tabView)
	{
		m_tabs->AddChildWindow(tabView);
	}

	void GuiCommonTabGridView::setTabsArea(const URect & area)
	{
		if (m_tabs)
		{
			m_tabsArea = area;
			m_tabs->SetArea(area);
		}
	}

	bool GuiCommonTabGridView::SetProperty(const GUIString& strName, const GUIString& strValue)
	{
		if (GUIWindow::SetProperty(strName, strValue))
		{
			return true;
		}
		else if (strName == GWP_GRID_VIEW_CONFIG)
		{
			m_gridViewConfig = StringUtil::ParseVec4(strValue.c_str());
			initGridView(m_gridViewConfig.x, m_gridViewConfig.y, (i32)m_gridViewConfig.z);
			return true;
		}
		else if (strName == GWP_GRID_VIEW_AREA)
		{
			setGridViewArea(StringUtil::ParseURect(strValue.c_str()));
			return true;
		}
		else if (strName == GWP_TABS_AREA)
		{
			setTabsArea(StringUtil::ParseURect(strValue.c_str()));
			return true;
		}
		return false;
	}

	bool GuiCommonTabGridView::GetProperty(const GUIString& strName, GUIString& strValue)
	{

		if (GUIWindow::GetProperty(strName, strValue))
		{
			return true;
		}
		else if (strName == GWP_GRID_VIEW_CONFIG)
		{
			strValue = StringUtil::ToString(m_gridViewConfig).c_str();
			return true;
		}
		else if (strName == GWP_GRID_VIEW_AREA)
		{
			strValue = StringUtil::ToString(m_gvArea).c_str();
			return true;
		}
		else if (strName == GWP_TABS_AREA)
		{
			strValue = StringUtil::ToString(m_tabsArea).c_str();
			return true;
		}

		return false;
	}

	void GuiCommonTabGridView::GetPropertyList(GUIPropertyList& properlist)
	{
		GUIWindow::GetPropertyList(properlist);
		properlist.push_back(GWP_GRID_VIEW_CONFIG);
		properlist.push_back(GWP_GRID_VIEW_AREA);
		properlist.push_back(GWP_TABS_AREA);
	}

	bool GuiCommonTabGridView::GetPropertyType(const GUIString& strName, GUIPropertyType& type)
	{
		if (GUIWindow::GetPropertyType(strName, type))
		{
			return true;
		}
		else if (strName == GWP_GRID_VIEW_CONFIG)
		{
			type = GPT_STRETCH_OFFSET;
			return true;
		}
		else if (strName == GWP_GRID_VIEW_AREA)
		{
			type = GPT_AREA;
			return true;
		}
		else if (strName == GWP_TABS_AREA)
		{
			type = GPT_AREA;
			return true;
		}
		return false;
	}

	UVector2 GuiCommonTabGridView::getItemPosition()
	{
		for (size_t i = 0; i < m_gvItems->GetItemCount(); i++)
		{
			auto itemView = dynamic_cast<CustomGuiWindow*>(m_gvItems->GetItem(i));
			if (itemView->isSelected())
			{
				return UVector2(UDim(0, itemView->GetRenderArea().left + itemView->GetRenderArea().getWidth() / 2), UDim(0, itemView->GetRenderArea().top + itemView->GetRenderArea().getHeight() / 2));
			}
		}
		return UVector2(UDim(0.5f, 0), UDim(0.5f, 0));
	}

}