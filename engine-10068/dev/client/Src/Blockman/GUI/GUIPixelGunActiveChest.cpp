#include "GUIPixelGunActiveChest.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "GUI/GuiActorWindow.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{
	GUIPixelGunActiveChest::GUIPixelGunActiveChest(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("PixelGunActiveChest.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
		auto open = (GUIButton*)m_window->GetChildByIndex(2);
		open->subscribeEvent(EventButtonClick, std::bind(&GUIPixelGunActiveChest::onOpenClick, this, std::placeholders::_1));

	}

	void GUIPixelGunActiveChest::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIPixelGunActiveChest::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
		if (m_data && m_data->HasChange)
		{
			SetTaskActive(m_data);
		}
	}

	void GUIPixelGunActiveChest::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIPixelGunActiveChest::SetTaskActive(TaskActive * active)
	{
		m_data = active;
		m_data->HasChange = false;
		auto value = m_window->GetChildByIndex(0);
		value->SetText(StringUtil::ToString(m_data->ActiveValue).c_str());
		auto actor = dynamic_cast<GuiActorWindow*>(m_window->GetChildByIndex(1));
		actor->SetActor(m_data->Actor.c_str(), "idle", 10.0f);
		actor->SetRotateX(20.0f);
		switch (m_data->Status)
		{
		case TaskActiveStatus::ACTIVE_STATUS_UNDONE:
			actor->SetSkillName("idle");
			break;
		case TaskActiveStatus::ACTIVE_STATUS_DONE:
			actor->SetSkillName("shake");
			break;
		case TaskActiveStatus::ACTIVE_STATUS_RECEIVE:
			actor->SetSkillName("idle_open");
			break;
		default:
			break;
		}
	}

	bool GUIPixelGunActiveChest::onOpenClick(const EventArgs & events)
	{
		if (m_data->Status != ACTIVE_STATUS_DONE)
		{
			return false;
		}
		m_data->Status = ACTIVE_STATUS_RECEIVE;
		m_data->HasChange = true;
		ReceiveTaskActiveRewardEvent::emit(m_data->Id);
		return true;
	}

}
