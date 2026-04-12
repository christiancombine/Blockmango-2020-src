#ifndef __GUI_DESIGNATION_PANEL_CONTROL_HEADER__
#define __GUI_DESIGNATION_PANEL_CONTROL_HEADER__

#include <memory>
#include "GUI/gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIRadioButton.h"
#include "Setting/DesignationSetting.h"

namespace LORD
{
	class GUIButton;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	class gui_designationPanelControl : public gui_layout
	{
	private:

		int m_currentId = 0;
		GUILayout* m_leftLayout=nullptr;
		GUILayout* m_rightLayout = nullptr;
		map<int, GUIRadioButton*>::type m_radioMap;
		std::vector<DesignationItem*> tools;
		DesignationItem* useDesignationItem = nullptr;
		struct ToolTypeSort
		{
			bool operator()(DesignationItem* first, DesignationItem* second)
			{
				return first->Weight < second->Weight;
			}
		};

	public:
		gui_designationPanelControl();
		~gui_designationPanelControl();
		void onLoad() override;
		void onHide() override;
		bool isNeedLoad() override;
		void onUpdate(ui32) override;
		bool onRadioChange(const EventArgs& events, int tabId);
		void updataDesignationType();
		void updateDesignation();
		bool onUpdateDesignation(const String & designationData, const String & designationTypeData);
		void sortDesignation();
		void setLeftTextColor();
		void onGetCommonData(String key, String data);
	};
}

#endif // !__GUI_MAIN_INVENTORY_PANEL_HEADER__
