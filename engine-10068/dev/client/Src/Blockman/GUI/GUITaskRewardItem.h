#ifndef __GUI_TASK_REWARD_ITEM_HEADER__
#define __GUI_TASK_REWARD_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/RewardSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUITaskRewardItem : public CustomGuiWindow
	{

	private:
		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUITaskRewardItem(const GUIWindowType& nType, const GUIString& strName);
		~GUITaskRewardItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	public:
		void Destroy() override;
		void InitReward(Reward* reward);
	};
}
#endif 
