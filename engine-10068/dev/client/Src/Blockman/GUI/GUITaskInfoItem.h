#ifndef __GUI_TASK_INFO_ITEM_HEADER__
#define __GUI_TASK_INFO_ITEM_HEADER__

#include "UI/GUIWindow.h"
#include "GUI/CustomGuiWindow.h"
#include "Setting/TaskSystemSetting.h"
#include "GUITaskRewardItem.h"

using namespace LORD;

namespace BLOCKMAN
{
	class GUITaskInfoItem : public CustomGuiWindow
	{

	private:
		Task* m_data = nullptr;

		GUIWindow* m_window = nullptr;
		using BaseClass = CustomGuiWindow;
		void removeComponents();

	public:
		GUITaskInfoItem(const GUIWindowType& nType, const GUIString& strName);
		~GUITaskInfoItem() = default;
		void UpdateSelf(ui32 nTimeElapse) override;

	public:
		void Destroy() override;
		void InitTask(Task* task);
		void InitTaskItem(Task* task);
		bool OnButtonClick(const EventArgs & events);
		void SetTaskStatus();
		void SetRewards(String rewardIds);
		void OnSkipClick();

	};
}
#endif 
