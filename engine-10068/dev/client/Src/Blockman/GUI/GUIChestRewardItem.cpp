#include "GUIChestRewardItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "Util/ClientEvents.h"
#include "Setting/PixelGunGiftBagSetting.h"

namespace BLOCKMAN
{
	GUIChestRewardItem::GUIChestRewardItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("ChestRewardItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIChestRewardItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	bool GUIChestRewardItem::onItemClick(const EventArgs & events)
	{
		ShowChestRewardDetailEvent::emit(m_id);
		return true;
	}

	void GUIChestRewardItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIChestRewardItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIChestRewardItem::SetLotteryChestReward(LotteryChestReward * chest_reward)
	{
		auto reward = RewardSetting::findRewardById(chest_reward->RewardId);
		if (!reward)
		{
			return;
		}
		m_id = reward->Id;
		GUIStaticImage* icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		icon->SetImage(reward->Image.c_str());
		GUIStaticText* count = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		count->SetText(StringUtil::ToString(reward->Count).c_str());
		m_window->GetChildByIndex(2)->subscribeEvent(EventButtonClick, std::bind(&GUIChestRewardItem::onItemClick, this, std::placeholders::_1));
	}

	void GUIChestRewardItem::SetGiftBagReward(Reward * reward)
	{
		m_id = reward->Id;
		GUIStaticImage* icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		icon->SetImage(reward->Image.c_str());
		GUIStaticText* count = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		count->SetText(StringUtil::ToString(reward->Count).c_str());
		m_window->GetChildByIndex(2)->subscribeEvent(EventButtonClick, std::bind(&GUIChestRewardItem::onItemClick, this, std::placeholders::_1));
	}
}
