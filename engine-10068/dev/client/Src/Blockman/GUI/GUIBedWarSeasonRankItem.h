#ifndef __GUI_BED_WAR_SEASON_RANK_ITEM_HEADER__
#define __GUI_BED_WAR_SEASON_RANK_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SeasonSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIBedWarSeasonRankItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIBedWarSeasonRankItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIBedWarSeasonRankItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetSeasonRankItem(int seasonId, SeasonRankItem rank);

	};
}

#endif