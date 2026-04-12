#ifndef __GUI_BLOCKCITY_ELEVATOR_HEADER__
#define __GUI_BLOCKCITY_ELEVATOR_HEADER__

#include "GUI/gui_layout.h"


namespace LORD {
	class GUILayout;
}

namespace BLOCKMAN
{
	class gui_blockCityElevator : public gui_layout
	{
	public:
		gui_blockCityElevator();
		~gui_blockCityElevator();

		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool isForceUpdate() override { return true; }

		bool onClose(const EventArgs& events);
		bool openElevator(i32 floorId);
		bool isInviteRaceVehile(bool invite);

	private:
		GUILayout* m_firstGroup = nullptr;
		GUILayout* m_secondGroup = nullptr;
		bool m_isShowUI = false;
		bool m_inviteRaceVehicle = false;
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
