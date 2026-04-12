#ifndef __GUI_BED_WAR_SEASON_HONOR_ITEM_HEADER__
#define __GUI_BED_WAR_SEASON_HONOR_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum BedWarHonorItemType
	{
		BED_WAR_LAST_SEASON,
		BED_WAR_CURRENT_SEASON
	};

	class GUIBedWarSeasonHonorItem : public CustomGuiWindow
	{
	private:
		GUIWindow* m_window = nullptr;

	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIBedWarSeasonHonorItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIBedWarSeasonHonorItem() = default;

		void UpdateSelf(ui32 nTimeElapse) override;
		void Destroy() override;

		void SetHonorInfo(BedWarHonorItemType type, int honorId, int rank, int honor);

	};
}

#endif