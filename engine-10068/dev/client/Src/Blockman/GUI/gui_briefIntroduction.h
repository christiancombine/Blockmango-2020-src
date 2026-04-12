#ifndef __GUI_BRIEF_INTRODUCTION_HEADER__
#define __GUI_BRIEF_INTRODUCTION_HEADER__

#include "gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "Setting/BriefIntroductionSetting.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_briefIntroduction : public gui_layout
	{
	private:
		GUILayout* m_content = nullptr;
		GUIStaticImage* m_border = nullptr;
		BI_group* m_group = nullptr;
		int max_order = 1;
		int m_order = 0;

	public:
		gui_briefIntroduction();
		~gui_briefIntroduction();

		void onLoad() override;
		bool isNeedLoad() override;

	private:
		bool changeGroup(String GroupKey);
		void changePicture();
		bool onShowWindow();
		bool onNextClick(const EventArgs & events);
		bool onPreviousClick(const EventArgs & events);
		bool onCloseClick(const EventArgs & events);

		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif