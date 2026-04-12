#pragma once
#ifndef __GUI_GUIDE_SYSTEM_HEADER__
#define __GUI_GUIDE_SYSTEM_HEADER__

#include "Core.h"
#include "gui_layout.h"
#include "GUI/RootGuiLayout.h"

using namespace LORD;

namespace BLOCKMAN
{
	class gui_guideSystem : public gui_layout
	{
	private:
		int m_skip = 0;
		int m_guide = 0;

	public:
		gui_guideSystem();
		virtual ~gui_guideSystem();

		void onLoad() override;

	private:
		bool onButtonClick(const EventArgs & events);
		bool onWindowClick(const EventArgs & events);
		bool onSkipClick(const EventArgs & events);
		bool showGuideInfo(bool isCompel, int skip, const UVector2& uvPos, int guideId, int buttonSize);
		bool showSereenEffect(String effect, StringArray pos);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}

	};
}
#endif 
