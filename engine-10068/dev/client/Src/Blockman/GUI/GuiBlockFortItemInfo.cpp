#include "GuiBlockFortItemInfo.h"

namespace BLOCKMAN
{
	void GuiBlockFortItemInfo::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}


	vector<GUIWindow*>::type GuiBlockFortItemInfo::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockFortItemInfo::GuiBlockFortItemInfo(const GUIWindowType& nType, const GUIString& strName) : BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockFortItemInfo.json"));
		renameComponents(m_window);
		
		m_ivIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		m_tvNameValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_tvGainValue = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(3));

		AddChildWindow(m_window);
	}

	void GuiBlockFortItemInfo::setItemInfo(const BlockFortItemInfo& itemInfo, BFItemEntrance entrance, bool isNeedSplit)
	{
		m_ivIcon->SetImage(itemInfo.icon.c_str());
		m_tvName->SetText(itemInfo.name.c_str());

		switch (entrance)
		{
		case BFItemEntrance::Store:
		{
			m_tvGainValue->SetVisible(false);
			m_tvNameValue->SetText(itemInfo.value.c_str());
			m_tvNameValue->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.1f, 0.f), UDim(1.0f, 0.f));
		}
			break;
		case BFItemEntrance::AttackBuilding:
		{
			m_tvGainValue->SetVisible(true);
			m_tvGainValue->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.05f, 0.f), UDim(1.0f, 0.f));
			m_tvNameValue->SetArea(UDim(-0.3f, 0), UDim(0, 0), UDim(0.1f, 0.f), UDim(1.0f, 0.f));

			if (isNeedSplit && StringUtil::Find(itemInfo.value, "#") && StringUtil::Find(itemInfo.value, "-"))
			{
				StringArray valArr = StringUtil::Split(itemInfo.value, "#");
				StringArray val = StringUtil::Split(valArr[1], "-");//BlockFort Version 1: valArr[ONE] only for display, acturaly ONE is the building level
				m_tvNameValue->SetText(val[1].c_str());
				m_tvGainValue->SetText(String("+" + val[2]).c_str());
			}
		}
			break;
		default:
			break;
		}
	}
}