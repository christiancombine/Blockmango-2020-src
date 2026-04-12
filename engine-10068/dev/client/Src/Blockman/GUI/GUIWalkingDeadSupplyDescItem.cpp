#include "GUIWalkingDeadSupplyDescItem.h"
#include "UI/GUIWindowManager.h"
#include "Util/LanguageManager.h"



namespace BLOCKMAN
{
	GUIWalkingDeadSupplyDescItem::GUIWalkingDeadSupplyDescItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("WalkingDeadSupplyDescItem.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
		findViews();
	}

	void GUIWalkingDeadSupplyDescItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIWalkingDeadSupplyDescItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIWalkingDeadSupplyDescItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIWalkingDeadSupplyDescItem::findViews()
	{
		auto m_content = dynamic_cast<GUILayout*>(m_window->GetChildByIndex(0));
		m_prop = dynamic_cast<GUILayout*>(m_content->GetChildByIndex(0));
		m_Icon = dynamic_cast<GUIStaticImage*>(m_prop->GetChildByIndex(0));
		m_Name = dynamic_cast<GUIStaticText*>(m_prop->GetChildByIndex(1));

	}

	void GUIWalkingDeadSupplyDescItem::InitSupplyItems(Reward * goods)
	{
		if (goods == nullptr)
		{
			return;
		}
		m_goods = goods;
		m_Icon->SetImage(m_goods->Image.c_str());
		m_Name->SetText(LanguageManager::Instance()->getString(m_goods->Name).c_str());
	}
}
