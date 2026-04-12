#ifndef __GUI_DEATHSETTLEMENT_PANEL_CONTROL_HEADER__
#define __GUI_DEATHSETTLEMENT_PANEL_CONTROL_HEADER__
#include <memory>
#include "GUI/gui_layout.h"
#include "UI/GUILayout.h"
#include "UI/GUIButton.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIStaticImage.h"
#include "GUI/RootGuiLayout.h"
#include "GUI/GuiUrlImage.h"

namespace BLOCKMAN
{
	class ShellInterface;
	class gui_deathSettlementControl : public gui_layout
	{
	private:
		struct  deathStruct
		{
			String designation = "";
			String name = "";
			String grade = "";

			String lifetimeText = "";
			String lifetimeScore = "";

			String killDeadText = "";
			String killDeadScore = "";

			String killBossText = "";
			String killBossScore = "";

			String totalText = "";

		};
		deathStruct *m_deathStruct=new deathStruct();

		GuiUrlImage * head = nullptr;
		GUIStaticText * designation = nullptr;
		GUIStaticText * name = nullptr;

		GUIStaticImage * grade = nullptr;

		GUIStaticText * lifetimeTitle = nullptr;
		GUIStaticText * lifetimeText = nullptr;
		GUIStaticText * lifetimeScore = nullptr;

		GUIStaticText * killDeadTitle = nullptr;
		GUIStaticText * killDeadText = nullptr;
		GUIStaticText * killDeadScore = nullptr;

		GUIStaticText * killBossTitle = nullptr;
		GUIStaticText * killBossText = nullptr;
		GUIStaticText * killBossScore = nullptr;

		GUIStaticText * totalTitle = nullptr;
		GUIStaticText * totalText = nullptr;

		GUIButton* restart = nullptr;
		GUIStaticText * restartText = nullptr;
		GUIButton* exit = nullptr;
		GUIStaticText * exitText = nullptr;
	public:
		gui_deathSettlementControl();
		~gui_deathSettlementControl();
		void onLoad() override;
		void onHide() override;
		bool isNeedLoad() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool onExitClick(const EventArgs & events);
		bool onRestartClick(const EventArgs & events);
		bool onReceiveDeathSettlement(const String& result);
		bool onGetHeadPic(i64 userId);
		void parseUpdateDeathSettlementa(const String& str);
		void onGetCommonData(String key, String data);
		RootGuiLayout* getParent()
		{
			return dynamic_cast<RootGuiLayout*>(m_parentLayout);
		}
	};
}

#endif // 
