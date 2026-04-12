#include "GUIBedWarSeasonRankItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "Setting/RewardSetting.h"

namespace BLOCKMAN
{
	GUIBedWarSeasonRankItem::GUIBedWarSeasonRankItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BedWarSeasonRankItem.json"));
		renameComponents(m_window);
		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIBedWarSeasonRankItem::removeComponents()
	{
		if (m_window)
		{
			RemoveChildWindow(m_window);
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIBedWarSeasonRankItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIBedWarSeasonRankItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIBedWarSeasonRankItem::SetSeasonRankItem(int seasonId, SeasonRankItem rank)
	{
		m_window->GetChildByIndex(0)->SetText(StringUtil::ToString(rank.Rank).c_str());
		m_window->GetChildByIndex(1)->SetText(rank.Name.c_str());
		m_window->GetChildByIndex(2)->SetText(StringUtil::ToString(rank.Honor).c_str());
		auto RewardIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(3));
		auto RewardCount = RewardIcon->GetChildByIndex(0);
		SeasonRankReward* rank_reward = SeasonSetting::findSeasonRankRewardBySeasonIdAndRank(seasonId, rank.Rank);
		if (rank_reward)
		{
			auto reward = RewardSetting::findRewardById(rank_reward->RewardId);
			if (reward && rank.Type == SeasonRankType::TYPE_LEFT)
			{
				RewardIcon->SetImage(reward->Image.c_str());
				RewardCount->SetText(StringUtil::Format("x%d", reward->Count).c_str());
			}
			else
			{
				RewardIcon->SetVisible(false);
			}
		}
		else
		{
			RewardIcon->SetVisible(false);
		}
	}

}
