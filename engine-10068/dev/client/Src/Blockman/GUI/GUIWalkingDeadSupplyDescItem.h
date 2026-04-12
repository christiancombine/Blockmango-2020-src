#ifndef __WALKINGDEAD_SUPPLY_DESC_ITEM_HEADER__
#define __WALKINGDEAD_SUPPLY_DESC_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/WalkingDeadStoreSetting.h"
#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "Setting/RewardSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUIWalkingDeadSupplyDescItem : public CustomGuiWindow
	{
	private:
		Reward* m_goods = nullptr;
		GUIWindow* m_window = nullptr;

		GUILayout* m_prop=nullptr;
		GUIStaticImage* m_Icon =nullptr;
		GUIStaticText* m_Name =nullptr;


	private:
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUIWalkingDeadSupplyDescItem(const GUIWindowType& nType, const GUIString& strName);
		~GUIWalkingDeadSupplyDescItem() = default;

		void Destroy() override;
		void UpdateSelf(ui32 nTimeElapse) override;
		void InitSupplyItems(Reward* goods);

	private:
		void findViews();

	};
}

#endif