#include "GUIBedWarSeasonHonorItem.h"
#include "UI/GUIWindowManager.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "Util/LanguageManager.h"
#include "Util/LanguageKey.h"
#include "Setting/SeasonSetting.h"

namespace BLOCKMAN
{
	GUIBedWarSeasonHonorItem::GUIBedWarSeasonHonorItem(const GUIWindowType & nType, const GUIString & strName)
		: BaseClass(nType, strName)
	{
		m_window = dynamic_cast<GUILayout*>(GUIWindowManager::Instance()->LoadWindowFromJSON("BedWarSeasonHonorItem.json"));
		renameComponents(m_window);

		AddChildWindow(m_window);
		SetTouchable(false);
	}

	void GUIBedWarSeasonHonorItem::removeComponents()
	{
		if (m_window)
		{
			GUIWindowManager::Instance()->DestroyGUIWindow(m_window);
		}
		m_window = nullptr;
	}

	void GUIBedWarSeasonHonorItem::UpdateSelf(ui32 nTimeElapse)
	{
		const auto& windowSize = GetPixelSize();
		m_window->SetSize({ { 0, windowSize.x },{ 0, windowSize.y } });
		BaseClass::UpdateSelf(nTimeElapse);
	}

	void GUIBedWarSeasonHonorItem::Destroy()
	{
		removeComponents();
		BaseClass::Destroy();
	}

	void GUIBedWarSeasonHonorItem::SetHonorInfo(BedWarHonorItemType type, int honorId, int rank, int honor)
	{
		auto Title = m_window->GetChildByIndex(3);
		auto TitleIcon = m_window->GetChildByIndex(2);
		auto Flag = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(0));
		auto SmallHonorIcon = dynamic_cast<GUIStaticImage*>(Flag->GetChildByIndex(0)->GetChildByIndex(0));
		auto Rank = SmallHonorIcon->GetChildByIndex(0);
		auto HonorIcon = dynamic_cast<GUIStaticImage*>(m_window->GetChildByIndex(1));
		auto HonorName = HonorIcon->GetChildByIndex(0);
		auto HonorValue = HonorName->GetChildByIndex(0)->GetChildByIndex(0);
		Title->SetText(LanguageManager::Instance()->getString(type == BED_WAR_LAST_SEASON ? LanguageKey::GUI_SEASON_REWARD_LAST : LanguageKey::GUI_SEASON_REWARD_CURRENT).c_str());
		Rank->SetText(StringUtil::Format("NO:%d", rank).c_str());
		HonorValue->SetText(StringUtil::ToString(honor).c_str());
		auto season = SeasonSetting::findSeasonByHonorId((SeasonHonorId)honorId);
		if (season)
		{
			Flag->SetImage(season->HonorFlag.c_str());
			SmallHonorIcon->SetImage(season->HonorIcon.c_str());
			HonorIcon->SetImage(season->HonorIcon.c_str());
			HonorName->SetText(LanguageManager::Instance()->getString(season->HonorName).c_str());
		}
	}

}
