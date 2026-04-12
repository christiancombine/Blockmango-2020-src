#include "GuiCheckinItem.h"
#include "UI/GUIWindowManager.h"
#include "Network/ClientNetwork.h"
#include "Audio/SoundSystem.h"
#include "Audio/SoundDef.h"

namespace BLOCKMAN
{
	
	void GuiCheckinItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	
	vector<GUIWindow*>::type GuiCheckinItem::getPrivateChildren()
	{
		return { m_window };
	}

	GuiCheckinItem::GuiCheckinItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		init("BirdCheckinItem.json");
	}

	void GuiCheckinItem::init(const String & jsonName)
	{
		removeComponents();
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON(jsonName.c_str()));
		renameComponents(m_window);
		m_tvTitle = dynamic_cast<GUIButton*>(m_window->GetChildByIndex(0));

		m_ivIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(0));
		m_tvNum = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1)->GetChildByIndex(1));
		m_ivDone = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1)->GetChildByIndex(2));
		m_tvName = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(2));
		m_ivBackDone = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		m_window->subscribeEvent(EventWindowTouchUp, std::bind(&GuiCheckinItem::onCheckin, this, std::placeholders::_1));
		AddChildWindow(m_window);
	}

	void GuiCheckinItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	
	void GuiCheckinItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GuiCheckinItem::setBirdCheckin(const CheckinItem & item)
	{
		m_ivIcon->SetImage(item.icon.c_str());
		m_ivDone->SetVisible(item.state == 3);
		m_tvName->SetText(getString(item.name));
		m_ivBackDone->SetVisible(item.state == 3);
		m_tvNum->SetText(StringUtil::Format("x%d", item.num).c_str());
		m_tvTitle->SetText(StringUtil::Format(getString("gui_bird_checkin_day").c_str(), item.id).c_str());
		m_ivBackDone->SetImage(item.id == 7 ? "set:bird_checkin.json image:item_big_pre" : "set:bird_checkin.json image:item_pre");
		m_window->SetBackImage(item.id == 7 ? "set:bird_checkin.json image:item_big_nor" : "set:bird_checkin.json image:item_nor");
		m_window->SetTouchable(item.state == 2);
	}

	void GuiCheckinItem::setBlockCityCheckin(const CheckinItem & item)
	{
		m_ivIcon->SetImage(item.icon.c_str());
		m_ivDone->SetVisible(item.state == 3);
		m_tvName->SetText(getString(item.name));
		m_ivBackDone->SetVisible(item.state == 3);
		m_tvNum->SetText(StringUtil::Format("x%d", item.num).c_str());
		m_tvTitle->SetText(StringUtil::Format(getString("gui_checkin_day").c_str(), item.id).c_str());
		m_ivBackDone->SetImage(item.id == 7 ? "set:blockcity_checkin.json image:item_big_pre" : "set:blockcity_checkin.json image:item_pre");
		m_window->SetBackImage(item.id == 7 ? "set:blockcity_checkin.json image:item_big_nor" : "set:blockcity_checkin.json image:item_nor");
		m_window->SetTouchable(item.state == 2);
		m_tvTitle->SetNormalImage(item.id == 7 ? "set:blockcity_checkin.json image:item_big_title" : "set:blockcity_checkin.json image:item_title");
		m_tvTitle->SetPushedImage(item.id == 7 ? "set:blockcity_checkin.json image:item_big_title" : "set:blockcity_checkin.json image:item_title");
		m_tvTitle->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(0.0f, item.id == 7 ? 152.f : 98.f), UDim(0.0f, 41.f));
	}

	bool GuiCheckinItem::onCheckin(const EventArgs &)
	{
		ClientNetwork::Instance()->getSender()->sendCheckin(1);
		SoundSystem::Instance()->playEffectByType(ST_Click);
		return true;
	}
}