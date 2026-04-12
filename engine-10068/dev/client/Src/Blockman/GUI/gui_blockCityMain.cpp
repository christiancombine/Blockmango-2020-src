#include "gui_blockCityMain.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "GUI/RootGuiLayout.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "cWorld/Blockman.h"
#include "Network/ClientNetwork.h"
#include "game.h"
#include "Util/TipType.h"
#include "Util/RedDotManager.h"
#include "UI/GUIStaticText.h"
#include "UI/GUIButton.h"
#include "UI/GUILayout.h"
#include "UI/GUICheckBox.h"
#include "UI/GUIStaticImage.h"
#include "World/GameSettings.h"
#include "UI/GUICheckBox.h"

namespace BLOCKMAN
{
	gui_blockCityMain::gui_blockCityMain()
		: gui_layout("BlockCityMain.json")
	{
	}

	gui_blockCityMain::~gui_blockCityMain()
	{
	}

	void gui_blockCityMain::onLoad()
	{
		using namespace std::placeholders;
		m_fly = getWindow<GUIButton>("BlockCityMain-Fly");
		m_hideBtnLayout = getWindow<GUILayout>("BlockCityMain-HideBtn-Layout");
		m_cbHideBtn = getWindow<GUICheckBox>("BlockCityMain-Show-Hide-Btn");
		m_rank = getWindow<GUIButton>("BlockCityMain-Rank");
		m_checkin = getWindow<GUIButton>("BlockCityMain-Checkin");
		m_friend = getWindow<GUIButton>("BlockCityMain-Friend");
		m_hint = getWindow<GUIStaticText>("BlockCityMain-Hint");
		m_hint->SetVisible(false);
		m_editMode = getWindow<GUIButton>("BlockCityMain-EditMode");
		m_manorPrise = getWindow<GUIButton>("BlockCityMain-Manor-Praise");
		m_manorPrise->SetVisible(false);
		m_getPrise = getWindow<GUILayout>("BlockCityMain-Get-Praise-Layout");
		m_getPrise->SetVisible(false);
		m_cdReward = getWindow<GUILayout>("BlockCityMain-CDReward");
		m_tvCDReward = getWindow<GUIStaticText>("BlockCityMain-CDReward-CDText");
		m_ivCDReward = getWindow<GUIStaticImage>("BlockCityMain-CDReward-Img");
		m_cdReward->SetVisible(false);
		m_btnEditModeLayout = getWindow<GUILayout>("BlockCityMain-EditMode-Layout");
		m_btnEditModeLayout->SetVisible(false);
		m_vace_vehicle = getWindow<GUIStaticImage>("BlockCityMain-Race-Vehicle");
		m_vace_vehicle_result = getWindow<GUIStaticImage>("BlockCityMain-Race-Vehicle-Result");
		m_raveVehicleMsgLayout = getWindow<GUILayout>("BlockCityMain-Broadcast-Race-Vehicle-Msg");

		m_cbHideBtn->subscribeEvent(EventCheckStateChanged, std::bind(&gui_blockCityMain::onHideBtnCheckChenge, this, _1));
		m_fly->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::FLY));
		m_rank->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::RANK));
		m_checkin->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::CHECK_IN));
		m_friend->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::FRIEND));
		m_editMode->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::EDIT_MODE));
		m_manorPrise->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::MANOR_PRISE));
		getWindow("BlockCityMain-Back-Home")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::BACK_HOME));
		getWindow("BlockCityMain-Manor")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::MANOR));
		getWindow("BlockCityMain-Get-Praise")->subscribeEvent(EventButtonClick, std::bind(&gui_blockCityMain::onClickBtn, this, _1, ViewId::GET_PRISE));	
		m_subscriptionGuard.add(BlockCityCaptureStatusEvent::subscribe(std::bind(&gui_blockCityMain::updateCaptureStatus, this,_1)));
		m_subscriptionGuard.add(UpdateIsHaveViewShowEvent::subscribe(std::bind(&gui_blockCityMain::updateIsHaveViewShow, this,_1)));
		m_subscriptionGuard.add(BlockCityGetPraiseEvent::subscribe(std::bind(&gui_blockCityMain::showGetPraise, this)));
		m_subscriptionGuard.add(BlockCitySyncCDRewardEvent::subscribe(std::bind(&gui_blockCityMain::syncCDReward, this, _1, _2)));
		m_subscriptionGuard.add(setRaceVehileCountdownEvent::subscribe(std::bind(&gui_blockCityMain::setRaceVehicleCountdown, this, _1)));
		m_subscriptionGuard.add(setRaceVehileResultEvent::subscribe(std::bind(&gui_blockCityMain::setRaceVehicResult, this, _1)));
		m_subscriptionGuard.add(BroadcastRaceVehicleResultEvent::subscribe(std::bind(&gui_blockCityMain::broadcastRaceVehicleResult, this, _1, _2)));
	}

	void gui_blockCityMain::onShow()
	{
	
	}

	void gui_blockCityMain::onUpdate(ui32 nTimeElapse)
	{
		updateCDRewardTime(nTimeElapse);
		if (!isShown())
			return;
		updateBtnShow(nTimeElapse);
		hideBtnUpdate(nTimeElapse);
		showBtnsCowntdown(nTimeElapse);
		checkShowEffect(nTimeElapse);
		showVaceVehicleCowntdown(nTimeElapse);
		showVaceVehicleResult(nTimeElapse);
		showBroadcastVaceVehicleMsg(nTimeElapse);
		checkPlayerState();
		checkEditMode();
	}

	bool gui_blockCityMain::isNeedLoad()
	{
		return checkGameType(ClientGameType::BlockCity);
	}

	bool gui_blockCityMain::onClickBtn(const EventArgs &args, ViewId viewId)
	{
		switch (viewId)
		{
		case ViewId::FLY:
			if (!UICommon::checkBlockCityParam() || !Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo() || !Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo())
			{
				return false;
			}

			if (!Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo()->isGotFreeFly)
			{
				ClientNetwork::Instance()->getCommonSender()->sendBlockCityGetFreeFly();
			}
			else
			{
				ShowBlockCityBuyFlyTipEvent::emit();
			}
			break;
		case ViewId::RANK:
			RootGuiLayout::Instance()->showBlockCityRank();
			updateIsHaveViewShow(true);
			break;
		case ViewId::CHECK_IN:
			RootGuiLayout::Instance()->showCheckin();
			updateIsHaveViewShow(true);
			break;
		case ViewId::FRIEND:
			RootGuiLayout::Instance()->showBlockCityFriend();
			updateIsHaveViewShow(true);
			break;
		case ViewId::BACK_HOME:
			if (Blockman::Instance()->m_pPlayer->isOnDecoration())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_call_on_in_decoration");
			}
			else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isOnVehicle())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_manor_call_tp_in_vehicle");
			}
			else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isGlide())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_call_on_in_glide");
			}
			else if (BLOCKMAN::Blockman::Instance()->m_pPlayer->isUseElevator())
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_call_on_use_elevator");
			}
			else
			{
				ShowBlockCityShowCommonTipEvent::emit(vector<BlockCityPayment>::type(), (i32)BlockCityTipType::BLOCKCITY_GO_HOME, "gui_blockcity_if_transfer");
			}
			break;
		case ViewId::MANOR:
			if (Blockman::Instance()->m_pPlayer->m_blockCity->getManorManager())
			{
				RootGuiLayout::Instance()->showBlockCityManor();
			}
			else
			{
				ShowResultTipDialogEvent::emit(ResultCode::COMMON, "gui_block_city_manor_not_load");
			}
			break;
		case ViewId::EDIT_MODE:
		{
			if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo())
			{
				return false;
			}

 			bool isEditMode = !Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo()->isEditMode;
 			ClientNetwork::Instance()->getCommonSender()->sendBlockCityChangeEditMode(isEditMode);
			toggleEditModeImg(isEditMode);

			m_showEditEffectTime = isEditMode ? 2000 : 0;
		}
			break;
		case ViewId::MANOR_PRISE:
			ClientNetwork::Instance()->getSender()->sendGiveRose(UICommon::checkBlockCityManorUserId());
			break;
		case ViewId::GET_PRISE:
			RootGuiLayout::Instance()->setShowBlockCityReceiveFlower(true);
			m_isShowGetPraise = false;
			break;
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_blockCityMain::updateCaptureStatus(bool isOpen)
	{
		m_isOpenCapture = isOpen;
		return true;
	}

	bool gui_blockCityMain::showGetPraise()
	{
		m_isShowGetPraise = true;
		m_showGetPraiseTimeEffect = 10 * 1000;
		return true;
	}

	bool gui_blockCityMain::onHideBtnCheckChenge(const EventArgs &)
	{
		m_hideBtnLoadTime = 300;
		m_isShowHideBtn = !m_isShowHideBtn;
		if (m_isShowHideBtn)
		{
			m_showHideBtnTime = 5000;
		}
		return true;
	}

	bool gui_blockCityMain::updateIsHaveViewShow(bool isHave)
	{
		m_isOpenView = isHave;
		if (!m_isOpenView && m_isShowHideBtn)
		{
			m_showHideBtnTime = 5000;
		}
		return true;
	}

	void gui_blockCityMain::updateBtnShow(i32 nTimeElapse)
	{
		m_fly->SetVisible(UICommon::checkBlockCityFlyBtnShow() && !m_isOpenCapture);
		getWindow("BlockCityMain-Hide-RedDot")->SetVisible(RedDotManager::checkUserRequest() || RedDotManager::canCheckin);
		getWindow("BlockCityMain-Checkin-RedDot")->SetVisible(RedDotManager::canCheckin && m_cbHideBtn->GetChecked());
		getWindow("BlockCityMain-Friend-RedDot")->SetVisible(RedDotManager::checkUserRequest() && m_cbHideBtn->GetChecked());
		getWindow("BlockCityMain-BackHome-Layout")->SetVisible(!m_isOpenCapture);
		getWindow("BlockCityMain-Manor-Layout")->SetVisible(!m_isOpenCapture);
		//m_manorPrise->SetVisible(UICommon::checkBlockCityManorUserId() != 0 && !m_isOpenCapture);
		//getWindow("BlockCityMain-Get-Praise-Layout")->SetVisible(m_isShowGetPraise && !m_isOpenCapture);

		if (m_showGetPraiseTimeEffect > 0)
		{
			m_showGetPraiseTimeEffect -= nTimeElapse;
			m_showGetPraiseTimeEffect = m_showGetPraiseTimeEffect > 0 ? m_showGetPraiseTimeEffect : 0;
		}
		//getWindow("BlockCityMain-Get-Praise-Effect")->SetVisible(m_showGetPraiseTimeEffect > 0 && m_isShowGetPraise);
	}

	void gui_blockCityMain::checkEditMode()
	{
		m_btnEditModeLayout->SetVisible(m_isInManor && !m_isOpenCapture && !m_isOnVehicle);
		getWindow("BlockCityMain-EditMode-Effect")->SetVisible(!m_isEditMode);
		toggleEditModeImg(m_isEditMode);
	}

	void gui_blockCityMain::hideBtnUpdate(i32 nTimeElapse)
	{
		m_hideBtnLayout->SetVisible(!m_isOpenCapture);
		m_hideBtnLayout->SetHeight(UDim(0, 73.f));
		if (m_hideBtnLoadTime > 0)
		{
			m_hideBtnLoadTime -= nTimeElapse;
			m_hideBtnLoadTime = m_hideBtnLoadTime > 0 ? m_hideBtnLoadTime : 0;
		}

		if (m_isShowHideBtn)
		{
			float high = 73.f + 221.f * (1.f - m_hideBtnLoadTime / 300.f);
			m_hideBtnLayout->SetHeight(UDim(0, high));
			m_rank->SetEnabled(true);
			m_checkin->SetEnabled(true);
			m_friend->SetEnabled(true);
		}
		else
		{
			float high = 73.f + 221.f * m_hideBtnLoadTime / 300.f;
			m_hideBtnLayout->SetHeight(UDim(0, high));
			m_rank->SetEnabled(false);
			m_checkin->SetEnabled(false);
			m_friend->SetEnabled(false);
		}
	}

	void gui_blockCityMain::showBtnsCowntdown(i32 nTimeElapse)
	{
		if (m_showHideBtnTime > 0 && !m_isOpenView)
		{
			m_showHideBtnTime -= nTimeElapse;
			m_showHideBtnTime = m_showHideBtnTime > 0 ? m_showHideBtnTime : 0;
			if (m_showHideBtnTime <= 0 && m_isShowHideBtn)
			{
				m_cbHideBtn->SetChecked(false);
			}
		}
	}

	void gui_blockCityMain::showHintCountdown(i32 nTimeElapse)
	{
		if (m_showHintTime > 0)
		{
			m_hint->SetVisible(true);
			m_showHintTime -= nTimeElapse;
			m_showHintTime = m_showHintTime > 0 ? m_showHintTime : 0;
		}
		else
		{
			m_hint->SetVisible(false);
		}
	}

	void gui_blockCityMain::checkShowEffect(ui32 nTimeElapse)
	{
		if (m_showEditEffectTime > 0)
		{
			m_showEditEffectTime -= nTimeElapse;
			m_showEditEffectTime = m_showEditEffectTime > 0 ? m_showEditEffectTime : 0;
		}

		bool sign = Blockman::Instance()->m_pPlayer->capabilities.isFlying;
		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying() && !sign)
		{
			IsShowBlockCityEditEffectEvent::emit("Main-BlockCity-Fly-withEffect", m_showEditEffectTime > 0);
		}
		else
		{
			getWindow("BlockCityMain-Fly-Effect")->SetVisible(m_showEditEffectTime > 0);
		}
		IsShowBlockCityEditEffectEvent::emit("Main-BlockCity-BarEffect", m_showEditEffectTime > 0);
	}

	void gui_blockCityMain::updateCDRewardTime(i32 nTimeElapse)
	{
		m_cdRewardTime = (m_cdRewardTime - nTimeElapse) > 0 ? (m_cdRewardTime - nTimeElapse) : 0;
		if (m_cdRewardTime > 0)
		{
			m_tvCDReward->SetText(timeFormat(m_cdRewardTime).c_str());
		}
	}

	void gui_blockCityMain::showVaceVehicleCowntdown(i32 nTimeElapse)
	{
		float percent = 0.f;
		if (m_showVaceVehicleCountdownTime > 0)
		{
			m_showVaceVehicleCountdownTime = (m_showVaceVehicleCountdownTime - nTimeElapse) > 0 ? (m_showVaceVehicleCountdownTime - nTimeElapse) : 0;
			if (m_showVaceVehicleCountdownTime == 0)
			{
				m_isLockVehicle = false;
			}
		}
		if (m_showVaceVehicleCountdownTime > 3000)
		{
			percent = (m_showVaceVehicleCountdownTime - 3000) / 1000.f;
			m_vace_vehicle->SetImage("set:blockcity_race_vehicle.json image:three");
		}
		else if (m_showVaceVehicleCountdownTime > 2000)
		{
			percent = (m_showVaceVehicleCountdownTime - 2000) / 1000.f;
			m_vace_vehicle->SetImage("set:blockcity_race_vehicle.json image:two");
		}
		else if (m_showVaceVehicleCountdownTime > 1000)
		{
			percent = (m_showVaceVehicleCountdownTime - 1000) / 1000.f;
			m_vace_vehicle->SetImage("set:blockcity_race_vehicle.json image:one");			
		}
		else if (m_showVaceVehicleCountdownTime > 0)
		{
			if (!m_isLockVehicle)
			{
				setRaceVehileCountdownEvent::emit(false);
				m_isLockVehicle = true;
			}
			m_vace_vehicle->SetImage("set:blockcity_race_vehicle.json image:go");
			m_vace_vehicle->SetArea(UDim(0.f, 0.f), UDim(0.138889f, 0.f), UDim(2.13f, 0.f), UDim(0.86f, 0.f));
		}

		if (m_showVaceVehicleCountdownTime > 1000)
		{
			m_vace_vehicle->SetArea(UDim(0.f, 0.f), UDim(0.138889f, 0.f), UDim(percent, 0.f), UDim(percent, 0.f));
		}
		m_vace_vehicle->SetVisible(m_showVaceVehicleCountdownTime > 0);
	}

	void gui_blockCityMain::showVaceVehicleResult(i32 nTimeElapse)
	{
		if (m_showVaceVehicleResult > 0)
		{
			m_showVaceVehicleResult = (m_showVaceVehicleResult - nTimeElapse) > 0 ? (m_showVaceVehicleResult - nTimeElapse) : 0;
		}
		m_vace_vehicle_result->SetVisible(m_showVaceVehicleResult > 0);
		getWindow("BlockCityMain-Race-Vehicle-Result-Effect")->SetVisible(m_showVaceVehicleResult > 0);
	}

	void gui_blockCityMain::showBroadcastVaceVehicleMsg(i32 nTimeElapse)
	{
		if (m_broadcastMsg.size() > 0)
		{
			getWindow("BlockCityMain-Broadcast-Race-Vehicle-Result")->SetText(m_broadcastMsg[0].c_str());
			if (m_showRaceVehicleMsgTime <= 0 && m_intervalRaceVehicleMsgTime <= 0)
			{
				m_showRaceVehicleMsgTime = 7 * 1000;
				
				m_raveVehicleMsgLayout->SetXPosition({ 0, 384.f });
			}
			if (m_showRaceVehicleMsgTime > 0)
			{
				m_showRaceVehicleMsgTime = (m_showRaceVehicleMsgTime - nTimeElapse) > 0 ? (m_showRaceVehicleMsgTime - nTimeElapse) : 0;

				m_raveVehicleMsgLayout->SetXPosition({ 0, m_raveVehicleMsgLayout->GetXPosition().d_offset - 2.f });
				if (m_raveVehicleMsgLayout->GetXPosition().d_offset < -975)
				{
					m_raveVehicleMsgLayout->SetXPosition({ 0, 384.f });
				}
				if (m_showRaceVehicleMsgTime <= 0)
				{
					m_intervalRaceVehicleMsgTime = 1000;
				}
			}
			if (m_intervalRaceVehicleMsgTime > 0)
			{
				m_intervalRaceVehicleMsgTime = (m_intervalRaceVehicleMsgTime - nTimeElapse) > 0 ? (m_intervalRaceVehicleMsgTime - nTimeElapse) : 0;
				if (m_intervalRaceVehicleMsgTime <= 0)
				{
					auto iter = m_broadcastMsg.begin();
					m_broadcastMsg.erase(iter);
				}
			}
		}
		getWindow("BlockCityMain-Broadcast-Race-Vehicle-Msg")->SetVisible(m_showRaceVehicleMsgTime > 0);
		getWindow("BlockCityMain-Broadcast-Race-Vehicle-Msg-Layout")->SetVisible(m_broadcastMsg.size() > 0);
	}

	bool gui_blockCityMain::syncCDReward(const i32& times, const String& img)
	{
		m_cdRewardTime = times;
		m_cdReward->SetVisible(times > 0);
		m_ivCDReward->SetImage(getString(img).c_str());
		return true;
	}

	String gui_blockCityMain::timeFormat(i32 times)
	{
		i32 seconds = times / 1000;
		String min = StringUtil::ToString((seconds % 3600) / 60);
		min = (min.length() == 1 ? ("0" + min) : min);

		String sec = StringUtil::ToString(seconds % 60);
		sec = (sec.length() == 1 ? ("0" + sec) : sec);

		return StringUtil::Format("%s:%s", min.c_str(), sec.c_str());
	}

	void gui_blockCityMain::toggleEditModeImg(bool isEditMode)
	{
		static const GUIString normalMode = "set:blockcity_main_ui.json image:edit_quit";
		static const GUIString editMode = "set:blockcity_main_ui.json image:edit_mode";

		m_editMode->SetNormalImage(isEditMode ? normalMode : editMode);
		m_editMode->SetPushedImage(isEditMode ? normalMode : editMode);
		getWindow("BlockCityMain-EditMode-Effect")->SetVisible(!isEditMode);
	}

	void gui_blockCityMain::checkPlayerState()
	{
		if (!UICommon::checkBlockCityParam() || !Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo())
		{
			return;
		}

		auto pPlayer = Blockman::Instance()->m_pPlayer;
		bool isEditMode = pPlayer->m_blockCity->getPlayerInfo()->isEditMode;
		bool isOnVehicle = pPlayer->isOnVehicle();
		bool isOnDecoration = pPlayer->isOnDecoration();
		bool isInManor = UICommon::checkBlockCityIsInManor();

		if (isEditMode != m_isEditMode || isOnVehicle != m_isOnVehicle || isOnDecoration != m_isOnDecoration || isInManor != m_isInManor)
		{
			m_isInManor = isInManor;
			m_isEditMode = isEditMode;
			m_isOnDecoration = isOnDecoration;
			m_isOnVehicle = isOnVehicle;
			Blockman::Instance()->setPersonView(getPersonView());
		}
	}

	int gui_blockCityMain::getPersonView()
	{
		return !m_isInManor ? GameSettings::SPVT_THIRD : (m_isEditMode && !m_isOnDecoration && !m_isOnVehicle) ? GameSettings::SPVT_FIRST : GameSettings::SPVT_THIRD;
	}

	bool gui_blockCityMain::setRaceVehicleCountdown(bool isBegin)
	{
		playSoundByType(ST_race_ready);
		if (isBegin)
		{
			m_showVaceVehicleCountdownTime = 4000;
		}
		return true;
	}

	bool gui_blockCityMain::setRaceVehicResult(bool isWin)
	{
		if (isWin && m_showVaceVehicleCountdownTime > 0)
		{
			m_showVaceVehicleCountdownTime = 0;
		}
		playSoundByType(isWin ? ST_race_win : ST_race_fail);
		m_showVaceVehicleResult = 5000;
		m_vace_vehicle_result->SetImage(isWin ? "set:blockcity_race_vehicle.json image:victory" : "set:blockcity_race_vehicle.json image:lose");
		getWindow("BlockCityMain-Race-Vehicle-Result-Effect")->SetEffectName(isWin ? "103_duanwei_1.1.effect" : "g1048_death.effect");

		setRaceVehileStatusEvent::emit(false);
		return true;
	}

	bool gui_blockCityMain::broadcastRaceVehicleResult(const String& winner, const String& loser)
	{
		String msg = StringUtil::Format(getString("gui_broadcast_race_vehicle_msg").c_str(), winner.c_str(), loser.c_str()).c_str();
		m_broadcastMsg.push_back(msg);
		return true;
	}

}