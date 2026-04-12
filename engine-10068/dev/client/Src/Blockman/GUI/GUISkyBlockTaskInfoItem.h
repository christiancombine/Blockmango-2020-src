#ifndef __GUI_SKYBLOCK_TASK_INFO_ITEM_HEADER__
#define __GUI_SKYBLOCK_TASK_INFO_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/SkyBlockTaskSystemSetting.h"
#include "GUISkyBlockTaskRewardItem.h"
#include "Util/SubscriptionGuard.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUISkyBlockTaskInfoItem : public CustomGuiWindow
	{

	private:
		SubscriptionGuard m_subscriptionGuard;
		SkyBlockTask m_data;

		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUISkyBlockTaskInfoItem(const GUIWindowType& nType, const GUIString& strName);
		~GUISkyBlockTaskInfoItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	public:
		void Destroy() override;
		void InitTask(const SkyBlockTask task);
		void InitTaskItem(const SkyBlockTask task, bool have_cube_ad);
		bool OnButtonClick(const EventArgs & events);
		void SetTaskStatus();
		void SetTaskQualityImg();
		void SetRewards(String rewardIds);
		bool onSetMaxTaskTimes(bool isMaxTaskTimes);

		bool m_isMaxTaskTimes = false;
		bool m_have_cube_ad = false;

	};
}
#endif 
