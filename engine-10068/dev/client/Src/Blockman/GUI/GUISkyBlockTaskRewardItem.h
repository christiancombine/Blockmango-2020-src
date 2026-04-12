#ifndef __GUI_SKYBLOCK_TASK_REWARD_ITEM_HEADER__
#define __GUI_SKYBLOCK_TASK_REWARD_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SkyBlockTaskSystemSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockTaskRewardItem : public CustomGuiWindow
	{

	private:

		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUISkyBlockTaskRewardItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISkyBlockTaskRewardItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	public:
		void Destroy() override;
		void InitReward(const SkyBlockTaskReward reward);
	};
}
#endif 
