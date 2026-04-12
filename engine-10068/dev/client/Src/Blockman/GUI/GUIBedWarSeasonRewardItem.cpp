#include "GUIBedWarSeasonRewardItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUIStaticImage.h"
#include "UI/GUIStaticText.h"
#include "UI/GUILayout.h"
#include "UI/GUIFontManager.h"
#include "UI/GUIFont.h"

namespace BLOCKMAN
{
	GUIBedWarSeasonRewardItem::GUIBedWarSeasonRewardItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BedWarSeasonRewardItem.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIBedWarSeasonRewardItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIBedWarSeasonRewardItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIBedWarSeasonRewardItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIBedWarSeasonRewardItem::SetSeasonReward(Reward * reward, bool isBig)
	{
		auto Icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		Icon->SetImage(reward->Image.c_str());
		auto Count = m_window->GetChildByIndex(1);
		Count->SetText(StringUtil::ToString(reward->Count).c_str());
		if (isBig)
			Count->SetFont(GUIFontManager::Instance()->GetFont("HT20"));
		auto Hint = m_window->GetChildByIndex(2);
		if (reward->Type == RewardType::REWARD_HONOR_UP)
		{
			Icon->SetWidth({ 0.55f, 0.0f });
			Hint->SetVisible(true);
			Count->SetVisible(false);
		}
		else
		{
			Icon->SetWidth({ 0.55f, 0.0f });
			Hint->SetVisible(false);
			Count->SetVisible(true);
		}
	}

	void GUIBedWarSeasonRewardItem::SetSeasonRankReward(SeasonRankReward * rankReward, bool isBig)
	{
		auto reward = RewardSetting::findRewardById(rankReward->RewardId);
		if (!reward)
			return;

		auto Icon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		Icon->SetImage(reward->Image.c_str());
		auto Count = dynamic_cast<GUIStaticText*>(m_window->GetChildByIndex(1));
		Count->SetText(StringUtil::ToString(reward->Count).c_str());
		if (isBig)
			Count->SetFont(GUIFontManager::Instance()->GetFont("HT20"));

		auto Hint = m_window->GetChildByIndex(2);
		if (reward->Type == RewardType::REWARD_HONOR_UP)
		{
			Icon->SetWidth({ 0.55f, 0.0f });
			Hint->SetVisible(true);
			Count->SetVisible(false);
		}
		else
		{
			Icon->SetWidth({ 0.55f, 0.0f });
			Hint->SetVisible(false);
			Count->SetVisible(true);
		}
	}

}
