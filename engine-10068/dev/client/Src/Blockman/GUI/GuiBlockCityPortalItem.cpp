#include "GuiBlockCityPortalItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/ClientEvents.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	
	void GuiBlockCityPortalItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiBlockCityPortalItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiBlockCityPortalItem::GuiBlockCityPortalItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BlockCityPortalItem.json"));
		renameComponents(m_window);

		m_name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(0));

 		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiBlockCityPortalItem::onClick, this, std::placeholders::_1));

		AddChildWindow(m_window);
	}

	void GuiBlockCityPortalItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiBlockCityPortalItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiBlockCityPortalItem::onSetItem(BlockCityPortalItem &item)
	{
		m_item = item;
		m_name->SetText(LanguageManager::Instance()->getString(item.name).c_str());
		dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1))->SetVisible(!m_item.isOpen);
	}

	void GuiBlockCityPortalItem::setSelect(i32 index)
	{
		m_window->SetBackImage(index == m_item.id ? "set:blockcity_portal.json image:choose" : m_item.isOpen ? "set:blockcity_portal.json image:clickable" : "set:blockcity_portal.json image:unclickable");
	}

	bool GuiBlockCityPortalItem::onClick(const EventArgs& args)
	{
		if (m_item.id > 0)
		{
			BlockCityPortalItemClickEvent::emit(m_item);
		}
		return true;
	}
}