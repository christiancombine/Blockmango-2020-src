#ifndef __GUI_BIRD_ACTIVITY_HEADER__
#define __GUI_BIRD_ACTIVITY_HEADER__
#include "gui_layout.h"


using namespace LORD;
namespace LORD {
	class GUILayout;
	class GUIStaticText;
	class GUIStaticImage;
	class GUIListHorizontalBox;
}
namespace BLOCKMAN
{
	class RootGuiLayout;
	class gui_birdActivity : public  gui_layout
	{
	private:
		enum class ViewId {
			BUY,
			CLOSE
		};

	private:
		GUIStaticText* m_time = nullptr;
		GUIListHorizontalBox* m_gift = nullptr;
		GUIStaticImage* m_currencyIcon = nullptr;
		GUIStaticText* m_currencyNum = nullptr;
	public:
		gui_birdActivity();
		~gui_birdActivity();
		void onShow() override;
		void onLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool updateBirdActivity();
		void showBirdActivity();
		String timeFormat(i32 time);
		bool isNeedLoad() override;
	private:
		bool m_isActivityUpdate = false;
		i32 m_timeLeft = 0;
		i32 m_buyTime = 0;
	private:
		bool onClick(const EventArgs & events, ViewId viewId);
	};

}
#endif