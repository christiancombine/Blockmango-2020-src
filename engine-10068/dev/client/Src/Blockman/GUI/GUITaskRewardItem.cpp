#include "GUI/CustomGuiWindow.h"
#include "UI/GUIWindow.h"
#include "Setting/TaskSystemSetting.h"
#include "GUITaskRewardItem.h"
#include "UI/GUILayout.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Util/LanguageManager.h"

namespace BLOCKMAN
{
	GUITaskRewardItem::GUITaskRewardItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("TaskRewardItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
	}

	void GUITaskRewardItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUITaskRewardItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUITaskRewardItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUITaskRewardItem::InitReward(Reward* reward)
	{
		auto icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		auto value = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		icon->SetImage(reward->Image.c_str());
		if (reward->CountText != "#")
		{
			value->SetText(("x" + LanguageManager::Instance()->getString(reward->CountText)).c_str());
		}
		else
		{
			value->SetText(StringUtil::Format("x%d", reward->Count).c_str());
		}
	}
}
