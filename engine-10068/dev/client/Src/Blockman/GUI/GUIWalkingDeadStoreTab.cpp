#include "GUIWalkingDeadStoreTab.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageManager.h"
#include "Util/ClientEvents.h"
#include "UI/GUIFontManager.h"
#include "Audio/SoundSystem.h"

namespace BLOCKMAN
{
	GUIWalkingDeadStoreTab::GUIWalkingDeadStoreTab(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("WalkingDeadStoreTab.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIWalkingDeadStoreTab::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIWalkingDeadStoreTab::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIWalkingDeadStoreTab::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIWalkingDeadStoreTab::InitStoreTab(WalkingDeadStoreTab * tab)
	{
		m_tab_id = tab->TabId;
		m_tab_type = tab->Type;
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		name = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		name->SetText(LanguageManager::Instance()->getString(tab->Name).c_str());
		name->SetTextColor((Color(198.f / 255.f, 179.f / 255.f, 143.f / 255.f)));

		name->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
		border->SetNormalImage("set:walkingdead_mainInventory.json image:bg_main_tab_normal");
		border->SetPushedImage("set:walkingdead_mainInventory.json image:bg_main_tab_selected");
		border->subscribeEvent(EventCheckStateChanged, std::bind(&GUIWalkingDeadStoreTab::OnTabSelected, this, std::placeholders::_1));
	}

	void GUIWalkingDeadStoreTab::SetTabSelected(bool selected)
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		border->SetChecked(selected);
		border->SetTouchable(!selected);
	}

	bool GUIWalkingDeadStoreTab::IsTabSelected()
	{
		auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
		return border->GetChecked();
	}

	bool GUIWalkingDeadStoreTab::OnTabSelected(const EventArgs & events)
	{
		if (IsTabSelected())
		{
			SoundSystem::Instance()->playEffectByType(ST_Click);
			auto border = dynamic_cast<GUICheckBox*>(m_window->GetChildByIndex(0));
			border->SetTouchable(false);
			name->SetTextColor((Color(124.f / 255.f, 70.f / 255.f, 23.f / 255.f)));
			WalkingDeadStoreTabChangeEvent::emit(m_tab_id);
		}
		else
		{
			name->SetTextColor((Color(198.f / 255.f, 179.f / 255.f, 143.f / 255.f)));
		}
		return true;
	}
}
