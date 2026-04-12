#ifndef __GUI_BLOCKCITY_MAIN_HEADER__
#define __GUI_BLOCKCITY_MAIN_HEADER__
#include "GUI/gui_layout.h"

namespace LORD
{
	class GUICheckBox;
	class GUIButton;
	class GUILayout;
	class GUIStaticImage;
	class GUIStaticText;
}

namespace BLOCKMAN
{
	class gui_blockCityMain : public gui_layout
	{
	private:
		enum class ViewId {
			FLY,
			RANK,
			CHECK_IN,
			FRIEND,
			MANOR,
			EDIT_MODE,
			BACK_HOME,
			MANOR_PRISE,
			GET_PRISE
		};

	public:
		gui_blockCityMain();
		~gui_blockCityMain();

		void onLoad() override;
		void onShow() override;
		void onUpdate(ui32 nTimeElapse) override;
		bool isNeedLoad() override;
		bool isForceUpdate() override { return checkGameType(ClientGameType::BlockCity); }

	private:
		GUIButton* m_fly = nullptr;
		GUILayout* m_hideBtnLayout = nullptr;
		GUICheckBox* m_cbHideBtn = nullptr;
		GUIButton* m_rank = nullptr;
		GUIButton* m_checkin = nullptr;
		GUIButton* m_friend = nullptr;
		GUIStaticText* m_hint = nullptr;
		GUIButton* m_editMode = nullptr;
		GUIButton* m_manorPrise = nullptr;
		GUILayout* m_getPrise = nullptr;
		GUILayout* m_cdReward = nullptr;
		GUIStaticText* m_tvCDReward = nullptr;
		GUIStaticImage* m_ivCDReward = nullptr;
		GUILayout* m_btnEditModeLayout = nullptr;
		GUIStaticImage* m_vace_vehicle = nullptr;
		GUIStaticImage* m_vace_vehicle_result = nullptr;
		GUILayout* m_raveVehicleMsgLayout = nullptr;

		i32 m_cdRewardTime = 0;
		i32 m_hideBtnLoadTime = 0;
		i32 m_showHintTime = 0;
		i32 m_showEditEffectTime = 0;
		i32 m_showHideBtnTime = 0;
		i32 m_showGetPraiseTimeEffect = 0;
		i32 m_showVaceVehicleCountdownTime = 0;
		i32 m_showVaceVehicleResult = 0;
		i32 m_showRaceVehicleMsgTime = 0;
		i32 m_intervalRaceVehicleMsgTime = 0;
		bool m_isOpenCapture = false;
		bool m_isShowHideBtn = false;		
		bool m_isOpenView = false;
		bool m_isShowGetPraise = false;

		bool m_isInManor = true;
		bool m_isEditMode = true;
		bool m_isOnVehicle = true;
		bool m_isOnDecoration = true;
		bool m_isLockVehicle = false;

		std::vector<String> m_broadcastMsg;

	private:
		bool onClickBtn(const EventArgs &args, ViewId viewId);
		void updateBtnShow(i32 nTimeElapse);
		bool updateCaptureStatus(bool isOpen);
		void hideBtnUpdate(i32 nTimeElapse);
		bool onHideBtnCheckChenge(const EventArgs &);
		void showBtnsCowntdown(i32 nTimeElapse);
		bool updateIsHaveViewShow(bool isHave);
		void showHintCountdown(i32 nTimeElapse);
		void checkEditMode();
		void checkShowEffect(ui32 nTimeElapse);
		bool showGetPraise();
		bool syncCDReward(const i32& times, const String& img);
		void updateCDRewardTime(i32 nTimeElapse);
		String timeFormat(i32 times);
		void toggleEditModeImg(bool isEditMode);
		void showVaceVehicleCowntdown(i32 nTimeElapse);
		bool setRaceVehicleCountdown(bool isBegin);
		bool setRaceVehicResult(bool isWin);
		void showVaceVehicleResult(i32 nTimeElapse);
		bool broadcastRaceVehicleResult(const String& winner, const String& loser);
		void showBroadcastVaceVehicleMsg(i32 nTimeElapse);

		void checkPlayerState();
		int getPersonView();
	};
}

#endif // !__GUI_PLAYER_INVENTORY_HEADER__
