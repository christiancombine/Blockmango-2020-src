#include "gui_mainControl.h"

#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/PlayerControlller.h"
#include "GUI/GuiSlotTable.h"
#include "Network/ClientNetwork.h"
#include "Network/ClientPacketSender.h"
#include "Network/protocol/CommonDataBuilder.h"

#include "World/GameSettings.h"
#include "Entity/Entity.h"
#include "Inventory/InventoryPlayer.h"
#include "GUI/GuiSlotTable.h"
#include "UI/GUIButton.h"
#include "UI/GUIListBox.h"
#include "UI/GUIStaticText.h"
#include "cEntity/PlayerControlller.h"
#include "Entity/EntityItem.h"
#include "Entity/EntityCreature.h"
#include "Inventory/InventoryPlayer.h"
#include "World/World.h"
#include "Util/Random.h"
#include "UI/GUIStaticImage.h"
#include "Math/MathLib.h"
#include <functional>
#include "game.h"
#include "Util/CommonEvents.h"
#include "Util/LanguageKey.h"
#include "Setting/TeamInfoSetting.h"
#include "Item/Items.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Util/LanguageManager.h"
#include "UI/GUICheckBox.h"
#include "Entity/EntityVehicle.h"
#include "Enums/Enums.h"
#include "Object/Root.h"
#include "Setting/MultiLanTipSetting.h"
#include "Render/RenderEntity.h"
#include "cEntity/EntityActorCannonClient.h"
#include "Util/UICommon.h"
#include "GUI/gui_birdPackAndFuse.h"
#include "Setting/LogicSetting.h"
#include "EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Game/BirdSimulator.h"
#include "Game/BlockCity.h"
#include "GuiBirdTaskItem.h"
#include "GuiDef.h"
#include "UI/GUIFontManager.h"
#include "Util/RedDotManager.h"
#include "GUI/RootGuiLayout.h"
#include "Util/TipType.h"
#include "Util/MacroUtil.h"
#include "Setting/ModuleBlockSetting.h"
#include "Util/PlayerPrefs.h"
#include "Util/StringUtil.h"
#include "ShellInterface.h"
#include "gui_driveControl.h"
#include "cWorld/WorldClient.h"
#include "Audio/SoundSystem.h"
#include "Plugins/GameClientEvents.h"

namespace BLOCKMAN
{
	static const int PixelGunSlotNum = 3;

	const gui_mainControl::ButtonId gui_mainControl::ALL_BUTTON_ID[] = {
		ButtonId::FORWARD,
		ButtonId::BACK,
		ButtonId::LEFT,
		ButtonId::RIGHT,
		ButtonId::JUMP,
		ButtonId::SNEAK,
		ButtonId::UP,
		ButtonId::DOWN,
		ButtonId::TOP_LEFT,
		ButtonId::TOP_LEFT_BG,
		ButtonId::TOP_RIGHT,
		ButtonId::TOP_RIGHT_BG
	};
	const Vector3 gui_mainControl::HOLD_SCALE = Vector3(1.05f, 1.05f, 1.05f);
	const Vector3 gui_mainControl::RELEASE_SCALE = Vector3(1.f, 1.f, 1.f);
	const map<gui_mainControl::ButtonId, std::reference_wrapper<const String>>::type gui_mainControl::BUTTON_TO_KEY_BINDINGS = {
		{ ButtonId::FORWARD,       KeyBinding::KEY_FORWORD },
		{ ButtonId::BACK,          KeyBinding::KEY_BACK },
		{ ButtonId::LEFT,          KeyBinding::KEY_LEFT },
		{ ButtonId::RIGHT,         KeyBinding::KEY_RIGHT },
		{ ButtonId::JUMP,          KeyBinding::KEY_JUMP },
		{ ButtonId::UP,            KeyBinding::KEY_JUMP },
		{ ButtonId::DOWN,          KeyBinding::KEY_SNEAK },
		{ ButtonId::TOP_LEFT,      KeyBinding::KEY_TOP_LEFT },
		{ ButtonId::TOP_LEFT_BG,   KeyBinding::KEY_TOP_LEFT },
		{ ButtonId::TOP_RIGHT,     KeyBinding::KEY_TOP_RIGHT },
		{ ButtonId::TOP_RIGHT_BG,  KeyBinding::KEY_TOP_RIGHT }
	};

	gui_mainControl::gui_mainControl()
		: gui_layout("Main.json")
	{
		m_subscriptionGuard.add(SyncClientPeerPropEvent::subscribe([this](const String& str)
		{
			std::map<std::string, std::string> res;
			StringUtil::SplitString(res, str.c_str());

			std::map<int, int> data;
			for (auto pair : res)
			{
				data.emplace(std::make_pair(
					StringUtil::ParseI32(pair.first.c_str()),
					StringUtil::ParseI32(pair.second.c_str())));
			}
			setSchematicData(data);

			return true;
		}));
	}

	gui_mainControl::~gui_mainControl()
	{
		for (auto& pair : m_buttons)
		{
			pair.second->removeAllEvents();
		}

		LordSafeDelete(m_guiPlayerOperation);
		LordSafeDelete(m_guiTeamResources);
		LordSafeDelete(m_guiPixelGunMain);
		LordSafeDelete(m_guiSkyBlockMain);
		LordSafeDelete(m_guiWalkingDeadMain);
		if (m_guiBlockFort)
		{
			LordSafeDelete(m_guiBlockFort);
		}
		if (m_guiBlockCity)
		{
			LordSafeDelete(m_guiBlockCity);
		}
		removeAllEventsOfWindow("MainControl-sneak");
		removeAllEventsOfWindow("Main-Jump");
		removeAllEventsOfWindow("Main-ToggleInventoryButton");
		removeAllEventsOfWindow("Main-VisibleBar-SlotTable");
		removeAllEventsOfWindow("Main-BtnChat");

		for (auto& item : m_birdScoreViews)
		{
			if (item.itemView)
			{
				m_rootWindow->RemoveChildWindow(item.itemView);
				GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
				item.itemView = nullptr;
			}
		}
		m_birdScoreViews.clear();

		m_subscriptionGuard.unsubscribeAll();
		m_ItemSkillCdTime.clear();
		ModuleBlockSetting::unloadSetting();
	}

	void gui_mainControl::onLoad()
	{
		//LordAssertX(!m_rootWindow, "gui_mainControl has initEvents already!");
		ModuleBlockSetting::loadSetting(true);
        MonsterSetting::loadMonsterSetting();
		m_rootWindow->SetTouchable(false);
		m_crossHairLayout = getWindow<GUILayout>("Main-Gun-CrossHairs");
		m_crossHairLayout->SetVisible(false);
		m_commonCrossHair = getWindow<GUIStaticImage>("Main-Common-CrossHair");
		m_moveTopCrossHair = getWindow<GUIStaticImage>("Main-MoveTop-CrossHair");
		m_moveLeftCrossHair = getWindow<GUIStaticImage>("Main-MoveLeft-CrossHair");
		m_moveBottomCrossHair = getWindow<GUIStaticImage>("Main-MoveBottom-CrossHair");
		m_moveRightCrossHair = getWindow<GUIStaticImage>("Main-MoveRight-CrossHair");
		m_moveTopCrossHairBeginYPos = m_moveTopCrossHair->GetYPosition().d_offset;
		m_moveLeftCrossHairBeginXPos = m_moveLeftCrossHair->GetXPosition().d_offset;
		m_moveBottomCrossHairBeginYPos = m_moveBottomCrossHair->GetYPosition().d_offset;
		m_moveRightCrossHairBeginXPos = m_moveRightCrossHair->GetXPosition().d_offset;
		m_shotgunMoveTopCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveTop-CrossHair");
		m_shotgunMoveLeftCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveLeft-CrossHair");
		m_shotgunMoveBottomCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveBottom-CrossHair");
		m_shotgunMoveRightCrossHair = getWindow<GUIStaticImage>("Main-Shotgun-MoveRight-CrossHair");
		m_shotgunMoveTopCrossHairBeginYPos = m_shotgunMoveTopCrossHair->GetYPosition().d_offset;
		m_shotgunMoveLeftCrossHairBeginXPos = m_shotgunMoveLeftCrossHair->GetXPosition().d_offset;
		m_shotgunMoveBottomCrossHairBeginYPos = m_shotgunMoveBottomCrossHair->GetYPosition().d_offset;
		m_shotgunMoveRightCrossHairBeginXPos = m_shotgunMoveRightCrossHair->GetXPosition().d_offset;

		m_bulletReload = getWindow<GUILayout>("Main-Bullet-Reload");
		m_bulletReload->SetVisible(false);

		m_feedbackHead = getWindow<GUIStaticImage>("Main-Feedback-Head");
		m_feedbackBody = getWindow<GUIStaticImage>("Main-Feedback-Body");
		m_feedbackHead->SetVisible(false);
		m_feedbackBody->SetVisible(false);

		//BedWar
		m_warningLayout = getWindow<GUILayout>("Main-Warning");
		m_warningLayout->SetVisible(false);
		m_shootWarning = getWindow<GUIStaticImage>("Main-Shoot-Warning");
		m_shootWarning->SetVisible(false);
		m_subscriptionGuard.add(ShootWarningEvent::subscribe([this](float rotateAngle, bool isScreenLeft) -> bool {
			m_warningLayout->SetVisible(true);
			m_showShootWarning = true;
			m_shootWarningAlpha = 1.0f;
			if (isScreenLeft)
			{
				m_shootWarningRotateAngle = 360.0f - rotateAngle * Math::RAD2DEG;
			}
			else
			{
				m_shootWarningRotateAngle = rotateAngle * Math::RAD2DEG;
			}
			return true;
		}));

		m_endingAnimationLayout = getWindow<GUILayout>("Main-Ending-Animation");
		m_endingAnimationLayout->SetVisible(false);
		m_endingStatus = getWindow<GUIStaticImage>("Main-Ending-Status");
		m_endingStatus->SetVisible(false);
		m_subscriptionGuard.add(PlayEndingAnimationEvent::subscribe([this](String resultStr, bool isNextServer, int result) -> bool {
			m_gameResult = resultStr;
			m_isNextServer = isNextServer;
			if (result == 0)
			{
				playSoundByType(ST_GameCompleteLose);
				m_endingStatus->SetImage("set:ending_status.json image:ending_fail");
			}
			else if(result == 1)
			{
				playSoundByType(ST_GameCompleteWin);
				m_endingStatus->SetImage("set:ending_status.json image:ending_victory");
			}
			else if (result == 2)
			{
				playSoundByType(ST_GameCompleteDraw);
				m_endingStatus->SetImage("set:ending_status.json image:ending_draw");
			}
			m_endingAnimationLayout->SetVisible(true);
			m_playEndingAnimation = true;
			m_isZoomInEndingImg = true;
			m_endingAnimationScale = 0.0f;
			m_endingAnimationRotatingTime = 0.0f;
			Blockman::Instance()->startCameraRotation(1000.0f, 4000.0f);
			Blockman::Instance()->m_gameSettings->setMoveState((int)(MoveState::RunState));
			refreshMoveState();
			return true;
		}));

		m_gunOperationWindow = getWindow("Main-Gun-Operate");
		m_gunSniperSight = getWindow("Main-Gun-Operate-SniperSight");
		m_subscriptionGuard.add(AimingStateChangeEvent::subscribe([this](bool aiming,int crossHairType) -> bool {
			m_crossHairLayout->SetVisible(aiming);
			checkHideGunUI();
			checkHideReleaseSkillBtnUI();
			if (aiming)
				switchCrossHair(crossHairType);
			return true;
		}));

		m_arrowControlWindow = getWindow("Main-Control");
		m_jumpControlWindow = getWindow("Main-Jump-Controls");
		m_moveStateWindow = getWindow("Main-MoveState");

		m_buttons[ButtonId::FORWARD] = getWindow("MainControl-forward");
		m_buttons[ButtonId::BACK] = getWindow("MainControl-back");
		m_buttons[ButtonId::LEFT] = getWindow("MainControl-left");
		m_buttons[ButtonId::RIGHT] = getWindow("MainControl-right");
		m_buttons[ButtonId::JUMP] = getWindow("Main-Jump");
		m_buttons[ButtonId::DROP] = getWindow("Main-Drop");
		m_buttons[ButtonId::UP] = getWindow("Main-Up");
		m_buttons[ButtonId::DOWN] = getWindow("Main-Down");
		m_buttons[ButtonId::TOP_LEFT] = getWindow("MainControl-top-left");
		m_buttons[ButtonId::TOP_RIGHT] = getWindow("MainControl-top-right");
		m_buttons[ButtonId::TOP_LEFT_BG] = getWindow("MainControl-top-left-bg");
		m_buttons[ButtonId::TOP_RIGHT_BG] = getWindow("MainControl-top-right-bg");
		m_buttons[ButtonId::SNEAK] = getWindow("MainControl-sneak");

		m_buttonImages[ButtonId::FORWARD] = { "set:arrow_key.json image:shang", "set:arrow_key.json image:shang_pre" };
		m_buttonImages[ButtonId::BACK] = { "set:arrow_key.json image:xia", "set:arrow_key.json image:xia_pre" };
		m_buttonImages[ButtonId::LEFT] = { "set:arrow_key.json image:zuo", "set:arrow_key.json image:zuo_pre" };
		m_buttonImages[ButtonId::RIGHT] = { "set:arrow_key.json image:you", "set:arrow_key.json image:you_pre" };
		m_buttonImages[ButtonId::JUMP] = { "set:jump_control.json image:qifei", "set:jump_control.json image:qifei" };
		m_buttonImages[ButtonId::DROP] = { "set:fly_control.json image:luodi", "set:fly_control.json image:luodi" };
		m_buttonImages[ButtonId::UP] = { "set:fly_control.json image:shenggao", "set:fly_control.json image:shenggao_pre" };
		m_buttonImages[ButtonId::DOWN] = { "set:fly_control.json image:jiangdi", "set:fly_control.json image:jiangdi_pre" };
		m_buttonImages[ButtonId::TOP_LEFT] = { "set:arrow_key.json image:zuoxieshang", "set:arrow_key.json image:zuoxieshang_pre" };
		m_buttonImages[ButtonId::TOP_RIGHT] = { "set:arrow_key.json image:youxieshang", "set:arrow_key.json image:youxieshang_pre" };
		m_buttonImages[ButtonId::TOP_LEFT_BG] = { "", "" };
		m_buttonImages[ButtonId::TOP_RIGHT_BG] = { "", "" };
		m_buttonImages[ButtonId::SNEAK] = { "set:move_state.json image:sneak", "set:move_state.json image:sneak_pre" };

		m_cbGlide = getWindow<GUICheckBox>("Main-Glide");
		m_cbGlide->subscribeEvent(EventCheckStateChanged, std::bind(&gui_mainControl::onGlideCheckChenge, this, std::placeholders::_1));
		m_buttonPressed[ButtonId::SNEAK] = false;

		m_breakBlockProgress = getWindow<GUIStaticImage>("Main-Break-Block-Progress-Nor");
		m_hitEntityProgress = getWindow<GUIProgressBar>("Main-Hit-Entity-Progress");
		m_guiSlotTable = getWindow<GuiSlotTable>("Main-VisibleBar-SlotTable");
		m_flyingControls = getWindow("Main-FlyingControls");
		m_btnPutFurnitureSure = getWindow<GUIButton>("Main-Put-Furniture-Sure");
		m_btnPutFurnitureCancel = getWindow<GUIButton>("Main-Put-Furniture-Cancel");
		m_btnManorPraise = getWindow<GUIButton>("Main-Manor-Praise");
		m_btnOpenFlying = getWindow<GUIButton>("Main-OpenFlying");
		setText(m_btnOpenFlying, "gui_flying");

		for (ButtonId id : ALL_BUTTON_ID)
		{
			subscribeTouchEvent(m_buttons[id], id);
		}
		m_buttons[ButtonId::SNEAK]->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onSneakDoubleClick, this, std::placeholders::_1));
		m_buttons[ButtonId::JUMP]->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onJumpDoubleClick, this, std::placeholders::_1));
		m_buttons[ButtonId::DROP]->subscribeEvent("WindowDoubleClick", std::bind(&gui_mainControl::onDropDoubleClick, this, std::placeholders::_1));
		getWindow("Main-ToggleInventoryButton")->subscribeEvent(EventWindowTouchUp, SubscriberSlot(&gui_mainControl::showPlayerInventory, this));
		getWindow("Main-GunPreviewCheckBox")->subscribeEvent(EventCheckStateChanged, SubscriberSlot(&gui_mainControl::onCheckChange, this));
		{
			for (int i = 1; i <= PixelGunSlotNum; ++i)
			{
				String slot_name = "Main-ItemSlot" + StringUtil::ToString(i);
				auto&& window = dynamic_cast<GuiItemSlot*>(getWindow(slot_name.c_str()));
				NULL_CONTINUE(window);
				window->SetUserData(i);
				window->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_mainControl::onPixelGunSlotTouchUp, this));
			}
			m_subscriptionGuard.add(SchemeticPlaceBlockFinishEvent::subscribe([this]()
			{
				updateSchematicData(false);
				return true;
			}));
			m_subscriptionGuard.add(PlaceBlocksEvent::subscribe([this]()
			{
				updateSchematicData(false);
				return true;
			}));
		}
		getWindow("Main-VisibleBar-SlotTable")->subscribeEvent("ItemStackTouchUp", SubscriberSlot(&gui_mainControl::onItemStackTouchUp, this));
		getWindow("Main-VisibleBar-SlotTable")->subscribeEvent("ItemStackLongTouchStart", SubscriberSlot(&gui_mainControl::onItemStackLongTouchStart, this));
		getWindow("Main-VisibleBar-SlotTable")->subscribeEvent("ItemStackLongTouchEnd", SubscriberSlot(&gui_mainControl::onItemStackLongTouchEnd, this));
		getWindow("Main-VisibleBar-SlotTable")->setEnableLongTouch(true);
		m_actorWindow = getWindow<GuiActorWindow>("Main-Gun-Actor");

		m_chatMessageListBox = getWindow<GUIListBox>("Main-Chat-Message");
		m_subscriptionGuard.add(ChatMessageReceiveEvent::subscribe(std::bind(&gui_mainControl::addChatMessage, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(TopSystemNotificationEvent::subscribe(std::bind(&gui_mainControl::showTopSystemNotification, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BottomSystemNotificationEvent::subscribe(std::bind(&gui_mainControl::showBottomSystemNotification, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(CenterSystemNotificationEvent::subscribe(std::bind(&gui_mainControl::showCenterSystemNotification, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BreakBlockProgressEvent::subscribe(std::bind(&gui_mainControl::onBreakBlockProgress, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BreakBlockUiManageEvent::subscribe(std::bind(&gui_mainControl::breakBlockUiManage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(PlayerChangeItemEvent::subscribe(std::bind(&gui_mainControl::onChangeCurrentItem, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(VehicleOnOffEvent::subscribe(std::bind(&gui_mainControl::onVehicleStateChanged, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ChangeSkillTypeEvent::subscribe(std::bind(&gui_mainControl::setSkillType, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)));
		m_subscriptionGuard.add(SetPutFurnitureIdEvent::subscribe(std::bind(&gui_mainControl::setPutFurnitureId, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowPlayerOperationEvent::subscribe(std::bind(&gui_mainControl::showPlayerOperation, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(SetManorBtnVisibleEvent::subscribe(std::bind(&gui_mainControl::setManorBtnVisible, this, std::placeholders::_1)));
		m_subscriptionGuard.add(RefreshManorUiEvent::subscribe(std::bind(&gui_mainControl::onPutFurnitureCancel, this)));
		m_subscriptionGuard.add(GameStatusEvent::subscribe(std::bind(&gui_mainControl::setGameStatus, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ServerSyncGameTypeEvent::subscribe(std::bind(&gui_mainControl::onGameTypeUpdate, this)));
		m_subscriptionGuard.add(BirdAddScoreEvent::subscribe(std::bind(&gui_mainControl::onBirdAddScore, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(BirdAddMessageEvent::subscribe(std::bind(&gui_mainControl::onBirdAddMessage, this, std::placeholders::_1)));
		m_subscriptionGuard.add(AllowUseItemStatusEvent::subscribe(std::bind(&gui_mainControl::setAllowUseItem, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ShowRotateViewEvent::subscribe(std::bind(&gui_mainControl::showRotateView, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(AttackScoreEvent::subscribe(std::bind(&gui_mainControl::onAttackScore, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(SetMoveStateEvent::subscribe(std::bind(&gui_mainControl::setMoveState, this, std::placeholders::_1)));
		m_subscriptionGuard.add(ChangeIsCanJumpEvent::subscribe(std::bind(&gui_mainControl::setJumpState, this, std::placeholders::_1)));
		m_chatMessageListBox->SetAllowScroll(true);

		m_watchBattleWindow = getWindow("Main-Watch-Battle-Tip");
		setWatchRespawnUI();

		getWindow("Main-Gun-Preview")->SetVisible(false);
		getWindow("Main-GunDetailsIcon")->SetVisible(false);
		getWindow("Main-GunPreviewCheckBox")->SetVisible(false);

		getWindow("Main-Gun-Harm-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_HARM));
		getWindow("Main-Gun-Bullet-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_BULLET));
		getWindow("Main-Gun-Range-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_RANGE));
		getWindow("Main-Gun-FiringRate-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_FIRING_RATE));
		getWindow("Main-Gun-Deviation-Title")->SetText(getString(LanguageKey::GUI_STR_GUN_DEVIATION));

		m_topSystemNotification = getWindow<GUIStaticText>("Main-Top-System-Notification");
		m_bottomSystemNotification = getWindow<GUIStaticText>("Main-Bottom-System-Notification");
		m_centerSystemNotification = getWindow<GUIStaticText>("Main-Center-System-Notification");
		m_playerOperationLayout = getWindow<GUILayout>("Main-PlayerOperation");
		m_playerOperationLayout->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::hidePlayerOperationView, this, std::placeholders::_1));

 		if (!m_guiBlockFort && checkGameType(ClientGameType::BlockFort))
 		{
 			m_guiBlockFort = LordNew gui_blockFortMain;
 			m_guiBlockFort->attachTo(this, getWindow("Main"));
 		}

		if (!m_guiBlockCity && checkGameType(ClientGameType::BlockCity))
		{
			m_guiBlockCity = LordNew gui_blockCityMain;
			m_guiBlockCity->attachTo(this, getWindow("Main"));
		}

		if (!m_guiPlayerOperation)
		{
			m_guiPlayerOperation = LordNew gui_playerOperation;
			m_guiPlayerOperation->attachTo(this, m_playerOperationLayout);
		}

		if (!m_guiTeamResources)
		{
			m_guiTeamResources = LordNew gui_teamResources;
			m_guiTeamResources->attachTo(this, getWindow("Main"));
		}

		if (checkGameType(ClientGameType::PixelGunHall) && !GameClient::CGame::Instance()->isEnableIndie())
		{
			if (!m_guiPixelGunMain)
			{
				m_guiPixelGunMain = LordNew GUIPixelGunMain;
				m_guiPixelGunMain->attachTo(this, getWindow("Main"));
			}
		}
		
		if (!m_guiWalkingDeadMain && checkGameType(ClientGameType::WalkingDead))
		{
			m_guiWalkingDeadMain = LordNew GUIWalkingDeadMain;
			m_guiWalkingDeadMain->attachTo(this, getWindow("Main"));
		}

		if (checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct))
		{
			if (!m_guiSkyBlockMain)
			{
				m_guiSkyBlockMain = LordNew GUISkyBlockMain;
				m_guiSkyBlockMain->attachTo(this, getWindow("Main"));
			}
		}

		m_leftShootBtn = getWindow<GUIButton>("Main-Gun-Operate-LeftShootBtn");
		m_leftShootBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onLeftShootBtnTouchUp, this, std::placeholders::_1));
		m_leftShootBtn->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onLeftShootBtnTouchDown, this, std::placeholders::_1));
		m_leftShootBtn->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onLeftShootBtnTouchUp, this, std::placeholders::_1));
		m_leftShootBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() != ClientGameType::BlockCity);
		m_rightShootBtn = getWindow<GUIButton>("Main-Gun-Operate-RightShootBtn");
		/*m_rightShootBtn->setMutexTouchMoveWithScene(true);*/
		m_rightShootBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onRightShootBtnTouchUp, this, std::placeholders::_1));
		m_rightShootBtn->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onRightShootBtnTouchDown, this, std::placeholders::_1));
		m_rightShootBtn->subscribeEvent(EventWindowTouchMove, std::bind(&gui_mainControl::onRightShootBtnMoved, this, std::placeholders::_1));
		m_rightShootBtn->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onRightShootBtnTouchUp, this, std::placeholders::_1));
		m_originButtonPos = m_rightShootBtn->GetPosition();
		m_reloadBulletBtn = getWindow<GUIButton>("Main-Gun-Operate-ReloadBtn");
		m_reloadBulletBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickReloadBulletBtn, this, std::placeholders::_1));
		m_aimBtn = getWindow<GUIButton>("Main-Gun-Operate-AimBtn");
		m_aimBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickAimBtn, this, std::placeholders::_1));
		m_cancelAimBtn = getWindow<GUIButton>("Main-Gun-Operate-CancelAimBtn");
		m_cancelAimBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickCancelAimBtn, this, std::placeholders::_1));
		m_throwGunBtn = getWindow<GUIButton>("Main-Gun-Operate-ThrowBtn");
		m_throwGunBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickThrowGunBtn, this, std::placeholders::_1));
		m_throwGunBtn->SetVisible(false);
		m_prevFovSetting = Blockman::Instance()->m_gameSettings->getFovSetting();
		m_prevMouseSensitive = Blockman::Instance()->m_gameSettings->getMouseSensitivity();
		m_prevCameraView = Blockman::Instance()->m_gameSettings->getPersonView();
		m_driveControl.reset(LordNew gui_driveControl);
		m_driveControl->attachTo(this, getWindow("Main"));
		m_driveControl->hide();
		m_showManorBtn = getWindow<GUIButton>("MainControl-Show-Manor");
		m_teleportBtn = getWindow<GUIButton>("MainControl-Teleport");
		m_btnPlaceBuilding = getWindow<GUIButton>("Main-Place-Building");
		m_teleportBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickTeleportBtn, this, std::placeholders::_1));
		m_showManorBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickShowManorBtn, this, std::placeholders::_1));
		m_btnPlaceBuilding->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onPlaceBuildingClick, this, std::placeholders::_1));
		m_teleportBtn->SetVisible(false);
		m_showManorBtn->SetVisible(false);

		m_parachuteBtn = getWindow<GUIButton>("Main-Parachute");
		if (m_parachuteBtn)
		{
			m_parachuteBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickParachuteBtn, this, std::placeholders::_1));
			m_parachuteBtn->SetVisible(false);
		}

		m_flyBtn = getWindow<GUIButton>("Main-Fly");
		if (m_flyBtn)
		{
			if (checkGameType(ClientGameType::BlockCity))
			{
				m_flyBtn->SetNormalImage("set:blockcity_main_ui.json image:fly");
				m_flyBtn->SetPushedImage("set:blockcity_main_ui.json image:fly");
			}
			else
			{
				m_flyBtn->SetNormalImage("set:fly_control.json image:luodi");
				m_flyBtn->SetPushedImage("set:fly_control.json image:luodi");
			}

			m_flyBtn->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onClickFlyBtnDown, this, std::placeholders::_1));
			m_flyBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickFlyBtnUp, this, std::placeholders::_1));
			m_flyBtn->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onClickFlyBtnUp, this, std::placeholders::_1));
		}

		m_buildWarBlockBtn = getWindow<GUIButton>("Main-BuildWar-Block");
		if (m_buildWarBlockBtn)
		{
			m_buildWarBlockBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBuildWarBlockBtn, this, std::placeholders::_1));
			m_buildWarBlockBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Build_War);
		}

		m_hideAndSeekControlWindow = getWindow("Main-HideAndSeek-Operate");
		if (m_hideAndSeekControlWindow)
		{
			m_hideAndSeekControlWindow->SetVisible(false);
		}

		m_changeActorBtn = getWindow<GUIButton>("Main-Change-Actor");
		if (m_changeActorBtn)
		{
			m_changeActorBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeActorBtn, this, std::placeholders::_1));
			m_changeActorBtn->SetVisible(true);
		}

		m_hideandseekChangeYaw = getWindow<GUILayout>("Main-HideAndSeek-ChangeYaw");
		m_hideandseekChangeYawLeftBtn = getWindow<GUIButton>("Main-HideAndSeek-ChangeYawLeftBtn");
		m_hideandseekChangeYawRightBtn = getWindow<GUIButton>("Main-HideAndSeek-ChangeYawRightBtn");
		if (m_hideandseekChangeYawLeftBtn)
		{
			m_hideandseekChangeYawLeftBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeActorYawBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->subscribeEvent("WindowLongTouchStart", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchStartBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->subscribeEvent("WindowLongTouchEnd", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->subscribeEvent("MotionRelease", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, true));
			m_hideandseekChangeYawLeftBtn->SetVisible(true);
		}
		if (m_hideandseekChangeYawRightBtn)
		{
			m_hideandseekChangeYawRightBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeActorYawBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->subscribeEvent("WindowLongTouchStart", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchStartBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->subscribeEvent("WindowLongTouchEnd", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->subscribeEvent("MotionRelease", std::bind(&gui_mainControl::onClickChangeActorYawLongTouchEndBtn, this, std::placeholders::_1, false));
			m_hideandseekChangeYawRightBtn->SetVisible(true);
		}


		m_cameraChangeStatusBtn = getWindow<GUIButton>("Main-Camera-Change-Status");
														
		if (m_cameraChangeStatusBtn)
		{
			m_cameraChangeStatusBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickChangeCameraStatusBtn, this, std::placeholders::_1));
			m_cameraChangeStatusBtn->SetVisible(true);
		}

		m_personalShopBtn = getWindow<GUIButton>("Main-Personal-Shop");
		if (m_personalShopBtn)
		{
			m_personalShopBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickPersonalShopBtn, this, std::placeholders::_1));
			m_personalShopBtn->SetVisible(false);
		}
		m_buyButtleBtn = getWindow<GUIButton>("Main-Buy-Bullet");
		{
			m_buyButtleBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBuyBulletBtn, this, std::placeholders::_1));
			m_buyButtleBtn->SetVisible(false);
		}

		m_subscriptionGuard.add(GameStatusEvent::subscribe(std::bind(&gui_mainControl::refreshHideAndSeekBtn, this, std::placeholders::_1)));

		m_cameraLockImage = getWindow<GUIStaticImage>("Main-Camera-Lock");
		m_cameraUnLockImage = getWindow<GUIStaticImage>("Main-Camera-Unlock");
		if (m_cameraUnLockImage && m_cameraLockImage)
		{
			m_cameraUnLockImage->SetVisible(true);
			m_cameraLockImage->SetVisible(false);
		}

		m_btnBirdCheckin = getWindow<GUIButton>("Main-Bird-Checkin");
		m_btnBirdPack = getWindow<GUIButton>("Main-Bird-Pack");
		m_btnBirdHelp = getWindow<GUIButton>("Main-Bird-Help");
		m_btnBirdAtlas = getWindow<GUIButton>("Main-Bird-Atlas");
		m_btnBirdPersonalShop = getWindow<GUIButton>("Main-Bird-Personal-Shop");
		m_btnBirdActivity = getWindow<GUIButton>("Main-Bird-Activity");

		m_btnBirdHelp->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdHelpBtn, this, std::placeholders::_1));
		m_btnBirdPack->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdPackBtn, this, std::placeholders::_1));
		m_btnBirdCheckin->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickCheckinBtn, this, std::placeholders::_1));
		m_btnBirdAtlas->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdAtlasBtn, this, std::placeholders::_1));
		m_btnBirdPersonalShop->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdPersonalShopBtn, this, std::placeholders::_1));
		m_btnBirdActivity->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBirdActivityBtn, this, std::placeholders::_1));
		
		m_poleControlWindow = getWindow("Main-PoleControl");
		m_poleBgWindow = getWindow("Main-PoleControl-BG");
		m_poleCenterWindow = getWindow("Main-PoleControl-Center");
		m_poleMoveWindow = getWindow("Main-PoleControl-Move");
		m_poleArrowWindow = getWindow("Main-PoleControl-Arrow");
		m_poleMoveWindow->subscribeEvent(EventWindowTouchMove, std::bind(&gui_mainControl::onPoleTouchMove, this, std::placeholders::_1));
		m_poleMoveWindow->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onPoleTouchUp, this, std::placeholders::_1));
		m_poleMoveWindow->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onPoleTouchDown, this, std::placeholders::_1));
		m_poleMoveWindow->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::onPoleTouchUp, this, std::placeholders::_1));
		m_poleBgWindow->SetAlpha(0.75f);
		m_originPolePos = m_poleCenterWindow->GetPosition();
		
		m_sneakStateWindow = getWindow("Main-MoveState-Sneak");
		m_runStateWindow = getWindow("Main-MoveState-Run");
		m_sprintStateWindow = getWindow("Main-MoveState-Sprint");
		m_skillBtn = getWindow<GUIButton>("Main-Skill");
		m_sneakStateWindow->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onClickSwitchMoveState, this, std::placeholders::_1));
		m_runStateWindow->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onClickSwitchMoveState, this, std::placeholders::_1));
		m_sprintStateWindow->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onClickSwitchMoveState, this, std::placeholders::_1));
		m_btnPutFurnitureSure->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickPutFurnitureSureBtn, this, std::placeholders::_1));
		m_btnPutFurnitureCancel->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickPutFurnitureCancelBtn, this, std::placeholders::_1, true));
		m_btnManorPraise->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickManorPraise, this, std::placeholders::_1));
		m_skillBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickSkillBtn, this, std::placeholders::_1));
		m_subscriptionGuard.add(UpdateBossBloodStripEvent::subscribe(std::bind(&gui_mainControl::refreshBossLife, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateBasementLifeEvent::subscribe(std::bind(&gui_mainControl::refreshBasementLife, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateBuildProgressEvent::subscribe(std::bind(&gui_mainControl::refreshBuildProgress, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchExTask::subscribe(std::bind(&gui_mainControl::showRanchExTask, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(ShowRanchExCurrentItemInfo::subscribe(std::bind(&gui_mainControl::showRanchExCurrentItemTip, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
		m_subscriptionGuard.add(ShowRanchExItem::subscribe(std::bind(&gui_mainControl::showRanchExItem, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(UpdateBulletReload::subscribe(std::bind(&gui_mainControl::updateBulletReload, this, std::placeholders::_1)));
		m_subscriptionGuard.add(UpdateShootingFeedback::subscribe(std::bind(&gui_mainControl::updateShootingFeedback, this, std::placeholders::_1)));

		refreshMoveState();
		
		//CreepyAliens boss life
		m_bossLifeProgress = getWindow<GUIProgressBar>("Main-Boss-Life");
		m_bossLifeProgress->SetVisible(false);
		m_bossLifeIcon = getWindow<GUIStaticImage>("Main-Boss-Life-Icon");

		// exp
		m_playerExpProgress = getWindow<GUIProgressBar>("Main-Player-Exp");

		//CreepyAliens basement life
		m_basementLifeLayout = getWindow<GUILayout>("Main-Basement-Life");
		m_basementLifeLayout->SetVisible(false);
		m_basementLifeInfo = getWindow<GUIStaticText>("Main-Basement-Life-Info");
		m_basementLifeProgress = getWindow<GUIProgressBar>("Main-Basement-Life-Value");

		m_subscriptionGuard.add(AutoCastSkillEvent::subscribe(std::bind(&gui_mainControl::onAutoCastSkill, this)));
		m_subscriptionGuard.add(CastSkillEvent::subscribe(std::bind(&gui_mainControl::castSkill, this)));

		CheckGunUI();
		checkAimUI();
		LoadSkillReleaseBtn();
		LoadThrowPotUi();
		LoadRanchExUI();
		
		m_hurtMask = getWindow<GUIStaticImage>("Main-Hurt-Mask");
		m_frozenMask = getWindow<GUIStaticImage>("Main-Frozen-Mask");

		m_btnFishing = getWindow<GUIButton>("Main-Fishing");
		m_btnFishing->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickFishingBtn, this, std::placeholders::_1));

		CheckFishingBtn();

		m_btnCannon = getWindow<GUIButton>("Main-Cannon");
		m_btnCannon->SetVisible(false);
		m_btnCannon->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickCannonBtn, this, std::placeholders::_1));
		m_btnOpenFlying->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickOpenFlying, this, std::placeholders::_1));
		m_subscriptionGuard.add(ShowCannonBtnEvent::subscribe(std::bind(&gui_mainControl::showCannonBtn, this, std::placeholders::_1, std::placeholders::_2)));
		m_subscriptionGuard.add(HideBirdActivityEvent::subscribe(std::bind(&gui_mainControl::hideBirdActivity, this)));
				
		m_birdTaskLayout = getWindow<GUILayout>("Main-Bird-Task");
		m_btnHideTask = getWindow<GUIButton>("Main-Bird-Task-Hide");
		m_btnShowTask = getWindow<GUIButton>("Main-Bird-BtnTask");
		m_birdTaskList = getWindow<GUIListBox>("Main-Bird-Task-List");
		m_btnHideTask->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onHideBirdTaskClick, this, std::placeholders::_1));
		m_btnShowTask->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onShowBirdTaskClick, this, std::placeholders::_1));
		m_subscriptionGuard.add(BirdTaskChangeEvent::subscribe(std::bind(&gui_mainControl::onBirdTaskChange, this)));
		m_subscriptionGuard.add(BirdAcceptTaskEvent::subscribe(std::bind(&gui_mainControl::onShowBirdTaskClick, this, EventArgs())));
		
		m_birdMsgList = getWindow<GUIListBox>("Main-Bird-Message");
		m_birdMsgList->SetTouchable(false);
		m_birdMsgList->SetVisible(false);
		m_birdTakeOutMsg = getWindow<GUILayout>("Main-Bird-Message-Takeout");
		m_birdTakeOutMsg->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::onShowBirdStoreMsg, this));

		m_subscriptionGuard.add(BirdShowPackGuideEvent::subscribe(std::bind(&gui_mainControl::onShowBirdPackGuide, this)));
		m_subscriptionGuard.add(ShowQuickBuildBlockEvent::subscribe(std::bind(&gui_mainControl::onClickQuicklyBuildBlockBtnImpl, this, std::placeholders::_1)));

		m_itemBarBgImage = getWindow<GUIStaticImage>("Main-ItemBarBg");
		m_btnInventory = getWindow<GUIStaticImage>("Main-ToggleInventoryButton");

		m_btnBlockCityDelete = getWindow<GUIButton>("Main-BlockCity-Del-Shortcut");
		m_btnBlockCityDelete->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBlockCityDelete, this, std::placeholders::_1));

		m_btnBlockCityPack = getWindow<GUIButton>("Main-BlockCity-Pack");
		m_btnBlockCityPack->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onShowBlockCityPack, this, std::placeholders::_1));

		m_cbCaptureOpen = getWindow<GUICheckBox>("Main-Capture-Open");
		m_cbCaptureOpen->subscribeEvent(EventCheckStateChanged, std::bind(&gui_mainControl::onCaptureChange, this, std::placeholders::_1));
		getWindow("Main-Capture-Close")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onCaptureClick, this, std::placeholders::_1, CaptureViewId::CAPTURE_CLOSE));
		getWindow("Main-Capture-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onCaptureClick, this, std::placeholders::_1, CaptureViewId::CAPTURE_SURE));

		m_subscriptionGuard.add(IsShowBlockCityEditEffectEvent::subscribe(std::bind(&gui_mainControl::updateBlockCityEditEffect, this, std::placeholders::_1, std::placeholders::_2)));

		m_txFloatTip = getWindow<GUIStaticText>("Main-Float-Tip");

		m_btnBlockFortDelete = getWindow<GUIButton>("Main-BlockFort-Del-Shortcut");
		m_btnBlockFortDelete->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickBlockFortDelete, this, std::placeholders::_1));
		setText("Main-BlockFort-Del-Shortcut-Name", "gui_blockcity_delete");

		m_subscriptionGuard.add(setRaceVehileCountdownEvent::subscribe(std::bind(&gui_mainControl::onCloseCaptureIfBeginRaceVehicle, this, std::placeholders::_1)));

		getWindow("Main-Rotate")->SetVisible(false);
		getWindow("Main-Rotate")->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::onClick, this, std::placeholders::_1, ViewId::BTN_ROTATE_MAIN));
		getWindow("Main-Rotate-Left")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClick, this, std::placeholders::_1, ViewId::BTN_ROTATE_LEFT));
		getWindow("Main-Rotate-Right")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClick, this, std::placeholders::_1, ViewId::BTN_ROTATE_RIGHT));
		getWindow("Main-Rotate-Sure")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClick, this, std::placeholders::_1, ViewId::BTN_ROTATE_SURE));
		m_subscriptionGuard.add(ShowFloatTipEvent::subscribe(std::bind(&gui_mainControl::showFloatTip, this, std::placeholders::_1)));
		//m_btnUseItem = getWindow<GUIButton>("Main-Use-Item");
		//m_btnUseItem->SetText(getString(LanguageKey::GUI_USEITEM));
		//m_btnUseItem->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickUseItemBtn, this, std::placeholders::_1));
		//CheckUseItemBtn();
		onGameTypeUpdate();
		setHotbar();

		m_subscriptionGuard.add(UpdateDisarmamentStatusEvent::subscribe([this](bool isDisarmament) -> bool {
			m_leftShootBtn->SetEnabled(!isDisarmament);
			m_rightShootBtn->SetEnabled(!isDisarmament);
			return true;
		}));

		showPixelGunSlot(false);

		m_btnQuicklyBuildBlock = getWindow<GUIButton>("Main-QuicklyBuildBlock");
		m_btnQuicklyBuildBlock->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickQuicklyBuildBlockBtn, this, std::placeholders::_1));
		m_btnQuicklyBuildBlock->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BED_WAR);

		m_sliderCameraDistance = getWindow<GUISlider>("Main-Camera-Distance");
		m_sliderCameraDistance->subscribeEvent(EventWindowTouchDown, std::bind(&gui_mainControl::onChangeCameraDistance, this, std::placeholders::_1));
		m_sliderCameraDistance->subscribeEvent(EventWindowTouchUp, std::bind(&gui_mainControl::onChangeCameraDistance, this, std::placeholders::_1));
		m_sliderCameraDistance->subscribeEvent(EventMotionRelease, std::bind(&gui_mainControl::onChangeCameraDistance, this, std::placeholders::_1));
		m_sliderCameraDistance->subscribeEvent(EventWindowTouchMove, std::bind(&gui_mainControl::onChangeCameraDistance, this, std::placeholders::_1));
	}

	void gui_mainControl::onShow()
	{
		checkAimUI();
	}

	bool gui_mainControl::onGameTypeUpdate()
	{
		m_btnBirdHelp->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdPack->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdCheckin->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdPersonalShop->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnShowTask->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdAtlas->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);
		m_btnBirdActivity->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator);

		m_btnBlockCityDelete->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity);

		m_btnBlockCityPack->SetVisible(checkGameType(ClientGameType::BlockCity));
		m_btnBlockFortDelete->SetVisible(checkGameType(ClientGameType::BlockFort));

		if (m_cbGlide)
		{
			if (checkGameType(ClientGameType::BlockCity))
			{
				m_cbGlide->SetNormalImage("set:blockcity_main_ui.json image:wing_open");
				m_cbGlide->SetPushedImage("set:blockcity_main_ui.json image:wing_close");
			}
			else
			{
				m_cbGlide->SetNormalImage("set:bird_main.json image:wing_open");
				m_cbGlide->SetPushedImage("set:bird_main.json image:wing_close");
			}
		}

		if (checkGameType(ClientGameType::BlockFort))
		{
			getWindow<GUIButton>("Main-Rotate-Left")->SetArea(UDim(-0.2f, 0), UDim(0, -137.f), UDim(0.0f, 83.f), UDim(0.0f, 81.f));
			getWindow<GUIButton>("Main-Rotate-Right")->SetArea(UDim(0.2f, 0), UDim(0, -137.f), UDim(0.0f, 83.f), UDim(0.0f, 81.f));
		}
		else
		{
			getWindow<GUIButton>("Main-Rotate-Left")->SetArea(UDim(-0.2f, 0), UDim(0, -100.f), UDim(0.0f, 83.f), UDim(0.0f, 81.f));
			getWindow<GUIButton>("Main-Rotate-Right")->SetArea(UDim(0.2f, 0), UDim(0, -100.f), UDim(0.0f, 83.f), UDim(0.0f, 81.f));
		}

		getWindow<GUIButton>("Main-Rotate-Left")->SetNormalImage(checkGameType(ClientGameType::BlockFort) ? "set:blockfort_main.json image:left" : "set:blockcity_main_ui.json image:left_rotate");
		getWindow<GUIButton>("Main-Rotate-Left")->SetPushedImage(checkGameType(ClientGameType::BlockFort) ? "set:blockfort_main.json image:left" : "set:blockcity_main_ui.json image:left_rotate");
		getWindow<GUIButton>("Main-Rotate-Right")->SetNormalImage(checkGameType(ClientGameType::BlockFort) ? "set:blockfort_main.json image:right" : "set:blockcity_main_ui.json image:right_rotate");
		getWindow<GUIButton>("Main-Rotate-Right")->SetPushedImage(checkGameType(ClientGameType::BlockFort) ? "set:blockfort_main.json image:right" : "set:blockcity_main_ui.json image:right_rotate");
		return true;
	}

	void gui_mainControl::setHotbar()
	{
		if (checkGameType(ClientGameType::BlockCity))
		{
			m_itemBarBgImage->SetImage("set:blockcity_store.json image:shortcut_panel");
			m_itemBarBgImage->SetArea(UDim(0, 0), UDim(0, 0.f), UDim(0.0f, 673.0f), UDim(0.0f, 77.0f));
			getWindow("Main-VisibleBar")->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.0f, 673.0f), UDim(0.0f, 77.0f));
			m_btnInventory->SetArea(UDim(1.0f, -80.f), UDim(0, 2.5f), UDim(0.0f, 72.0f), UDim(0.0f, 72.0f));
			m_playerExpProgress->SetBackImage("set:new_gui_material.json image:playerExpBg");
			m_playerExpProgress->SetProgressImage("set:new_gui_material.json image:playerExpValue");
			m_playerExpProgress->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "3 3 3 3");
			m_btnInventory->SetImage("set:blockcity_main_ui.json image:shortcut_icon");
			m_btnBlockCityPack->SetArea(UDim(1.0f, -80.f), UDim(0, 2.5f), UDim(0.0f, 72.0f), UDim(0.0f, 72.0f));
			GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
			slotTable->SetArea(UDim(0, 12.f), UDim(0, 0), UDim(0.0f, 583.0f), UDim(1.0f, 0));
			slotTable->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, "2");
			slotTable->SetProperty(GWP_SLOT_WIDTH, "63");
			slotTable->SetProperty(GWP_SLOT_HEIGHT, "66");
			slotTable->SetProperty(GWP_SLOT_STYLE, "3");
			m_itemBarBgImage->SetImage("set:blockcity_store.json image:shortcut_panel");
		}
		else if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMain
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockMineCave
			|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::SkyBlockProduct)
		{
			m_itemBarBgImage->SetArea(UDim(0, 0), UDim(0, 0.f), UDim(0.0f, 673.0f), UDim(0.0f, 77.0f));
			getWindow("Main-VisibleBar")->SetArea(UDim(0, 0), UDim(0, 0), UDim(0.0f, 673.0f), UDim(0.0f, 77.0f));
			m_btnInventory->SetArea(UDim(1.0f, -80.f), UDim(0, 3.f), UDim(0.0f, 63.0f), UDim(0.0f, 65.0f));
			m_btnInventory->SetImage("set:skyblock_main.json image:skyblock_bag_bg");
			GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
			slotTable->SetArea(UDim(0, 0), UDim(0, -3.0f), UDim(1.0f, -63.0f), UDim(1.0f, 0));
			//slotTable->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, "2");
			slotTable->SetProperty(GWP_SLOT_WIDTH, "63");
			slotTable->SetProperty(GWP_SLOT_HEIGHT, "65");
			slotTable->SetProperty(GWP_SLOT_STYLE, StringUtil::Format("%d", GuiItemSlot::SKYBLOCK).c_str());
			m_itemBarBgImage->SetImage("set:skyblock_main.json image:skyblock_backpack_bg");
		}
		else
		{
			m_itemBarBgImage->SetArea(UDim(0, 0), UDim(0, 0.f), UDim(0.0f, 674.0f), UDim(0.0f, 81.0f));
			m_itemBarBgImage->SetImage("set:new_gui_material.json image:itemTableBg");
			getWindow("Main-VisibleBar")->SetArea(UDim(0, 0), UDim(0, -10.f), UDim(0.0f, 630.0f), UDim(0.0f, 63.0f));
			m_btnInventory->SetArea(UDim(1.0f, -63.0f), UDim(0, 0), UDim(0.0f, 63.0f), UDim(0.0f, 63.0f));
			m_btnInventory->SetImage("set:new_gui_material.json image:diaochucaidananniu");
			m_playerExpProgress->SetBackImage("set:new_gui_material.json image:playerExpBg");
			m_playerExpProgress->SetProgressImage("set:new_gui_material.json image:playerExpValue");
			m_playerExpProgress->SetProperty(GWP_WINDOW_STRETCH_OFFSET, "3 3 3 3");
			GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
			slotTable->SetArea(UDim(0, 0), UDim(0, 0), UDim(1.0f, -63.0f), UDim(1.0f, 0));
			slotTable->SetProperty(GWP_TABLE_HORIZONTAL_INTERVAL, "0");
			slotTable->SetProperty(GWP_SLOT_WIDTH, "63");
			slotTable->SetProperty(GWP_SLOT_HEIGHT, "63");
			slotTable->SetProperty(GWP_SLOT_STYLE, "0");
			m_itemBarBgImage->SetImage("set:new_gui_material.json image:itemTableBg");
		}
	}

	bool gui_mainControl::onChangeCameraDistance(const EventArgs & args)
	{
		NULL_RETURN(m_sliderCameraDistance, true);

		if (m_sliderCameraDistance->GetProgress() < 0.05f)
		{
			m_sliderCameraDistance->SetProgress(0.05f);
		}
		else if (m_sliderCameraDistance->GetProgress() > 0.95f)
		{
			m_sliderCameraDistance->SetProgress(0.95f);
		}

		float distance = m_cameraDistanceMin + 
			(PlayerPrefs::Instance()->getFloatPrefs("CameraDistanceMax") - m_cameraDistanceMin) * m_sliderCameraDistance->GetProgress();
		if (LogicSetting::Instance()->isSeparateCameraControl())
			LogicSetting::Instance()->setThirdPersonDistance(distance);
		//LordLogDebug("LogicSetting::Instance()->setThirdPersonDistance %f", distance);
		
		return true;
	}

	void gui_mainControl::updateCameraDistance(bool bForce/* = false*/)
	{
		float currScale = RenderEntity::getRenderEntityActorScale(Blockman::Instance()->m_pPlayer);
		if (LogicSetting::Instance()->isSeparateCameraControl() && m_sliderCameraDistance && m_sliderCameraDistance->IsVisible() && (m_lastPlayerScale != currScale || bForce))
		{
			float cameraDistanceMax = PlayerPrefs::Instance()->getFloatPrefs("CameraDistanceMax");
			float optimumCameraDistance = currScale * m_cameraOptimumScale;
			optimumCameraDistance = Math::Clamp(optimumCameraDistance, m_cameraDistanceMin, cameraDistanceMax);
			float currCameraDistance = LogicSetting::Instance()->getThirdPersonDistance();
			currCameraDistance = Math::Clamp(currCameraDistance, optimumCameraDistance, cameraDistanceMax);
			float progress = 0.0f;
			if (Math::Abs(cameraDistanceMax - m_cameraDistanceMin) > 0.00001f)
			{
				progress = (currCameraDistance - m_cameraDistanceMin) / (cameraDistanceMax - m_cameraDistanceMin);
			}
			progress = Math::Clamp(progress, 0.05f, 0.95f);
			currCameraDistance = (cameraDistanceMax - m_cameraDistanceMin) * progress + m_cameraDistanceMin;
			m_sliderCameraDistance->SetProgress(progress);
			LogicSetting::Instance()->setThirdPersonDistance(currCameraDistance);
			m_lastPlayerScale = currScale;
		}
	}

	bool gui_mainControl::onSneakDoubleClick(const EventArgs &)
	{
		auto pClientMovementInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
		pClientMovementInput->sneakPressed = !pClientMovementInput->sneakPressed;
		if (pClientMovementInput->sneakPressed)
		{
			Blockman::Instance()->m_gameSettings->setMoveState((int)(MoveState::SneakState));
		}
		else
		{
			Blockman::Instance()->m_gameSettings->switchMoveState();
		}
		refreshMoveState();
		return true;
	}

	bool gui_mainControl::setMoveState(int state)
	{
		Blockman::Instance()->m_gameSettings->setMoveState(state);
		refreshMoveState();
		return true;
	}

	bool gui_mainControl::setJumpState(bool isCanJump)
	{
		m_isCanJump = isCanJump;
		return true;
	}

	bool gui_mainControl::onClickSkillBtn(const EventArgs &)
	{
		if (m_skillType == SkillType::NONE)
		{
			return false;
		}
		switch (m_skillType)
		{
		case SkillType::DEFENSE:
			Blockman::Instance()->m_pPlayer->playDefendProgress(m_skillDuration);
			break;
		case SkillType::SPRINT:
			break;
		case SkillType::RELEASE_TOXIC:
			Blockman::Instance()->m_pPlayer->setInsertUpperAction(ActionState::AS_SKILL_POISION, 633);
			break;
		}
		m_skillResidueCDTime = (int) m_skillCDTime * 1000;
		ClientNetwork::Instance()->getSender()->sendSkillType(m_skillType, true);
		return true;
	}

	bool gui_mainControl::onClickPutFurnitureSureBtn(const EventArgs &)
	{
		ManorFurniture* furniture = Blockman::Instance()->m_pPlayer->m_manor->findFurnitureById(m_putFurnitureId);
		Blockman::Instance()->setPersonView(m_currentCameraView);
		if (!furniture)
		{			
			m_putFurnitureId = -1;
			return false;
		}
		if (furniture->operationType == OperationType::RECYCLE &&  Blockman::Instance()->m_gameSettings->m_isEditFurniture)
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::EDIT_SURE);
		}
		else
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::SURE);
		}
		m_putFurnitureId = -1;
		return true;
	}

	bool gui_mainControl::onClickPutFurnitureCancelBtn(const EventArgs&, bool isBtnClick)
	{
		ManorFurniture* furniture = Blockman::Instance()->m_pPlayer->m_manor->findFurnitureById(m_putFurnitureId);
		Blockman::Instance()->setPersonView(m_currentCameraView);
		if (!furniture || !isBtnClick)
		{
			m_putFurnitureId = -1;
			return false;
		}
		if (furniture->operationType == OperationType::RECYCLE &&  Blockman::Instance()->m_gameSettings->m_isEditFurniture)
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::EDIT_CANCEL);
		}
		else
		{
			ClientNetwork::Instance()->getSender()->sendPutFurniture(m_putFurnitureId, PutFurnitureStatus::CANCEL);
		}
		m_putFurnitureId = -1;
		return true;
	}

	bool gui_mainControl::onPutFurnitureCancel()
	{
		onClickPutFurnitureCancelBtn(EventArgs(), false);
		return true;
	}

	bool gui_mainControl::onClickManorPraise(const EventArgs &)
	{
		ClientNetwork::Instance()->getSender()->sendGiveRose(manorCheckPlayerLocation());
		return true;
	}

	bool gui_mainControl::setSkillType(SkillType skillType, float duration, float cdTime)
	{
		this->m_skillType = skillType;
		this->m_skillDuration = duration;
		this->m_skillCDTime = cdTime;
		return true;
	}

	void gui_mainControl::onGetCommonData(String key, String data)
	{
		if (key == "PixelGunGoldAd")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int times = builder.getIntParam("times");
			int seconds = builder.getIntParam("seconds");
			UpdateGoldAdInfoEvent::emit(times, seconds);
			return;
		}

		if (key == "PixelGunGoldAdReward")
		{
			auto builder = CommonDataBuilder().fromData(data);
			int golds = builder.getIntParam("golds");
			int times = builder.getIntParam("times"); 
			int maxTimes = builder.getIntParam("maxTimes");
			ShowGoldAdRewardEvent::emit(golds, times, maxTimes);
			return;
		}
		if (key == "WalkingDeadIsCanJumpInfo")
		{
			auto builer = CommonDataBuilder().fromData(data);
			bool isCanJump = builer.getBoolParam("canJump");
			ChangeIsCanJumpEvent::emit(isCanJump);
			return;
		}

		if (key == "SkyBlockLoading")
		{
			if (!isLoaded())
				return;
			auto builder = CommonDataBuilder().fromData(data);
			bool loading = builder.getBoolParam("loading");
			if (m_guiSkyBlockMain)
				m_guiSkyBlockMain->onShowSkyBlockLoadingRewards(loading);
			return;
		}
	}

	bool gui_mainControl::updateBulletReload(float progress)
	{
		if (progress == 0.f)
		{
			m_bulletReload->SetVisible(false);
			return false;
		}
		int section = Math::Min((int)Math::Floor(progress * 30), 30);
		auto img = getWindow<GUIStaticImage>("Main-Bullet-Reload-Progress");
		m_bulletReload->SetVisible(section > 0);
		img->SetImage(StringUtil::Format("set:bullet_reload.json image:%d", section).c_str());
		if (progress == 1.f)
			m_bulletReload->SetVisible(false);
		return true;
	}

	bool gui_mainControl::updateShootingFeedback(bool isHeadshot)
	{
		if (isHeadshot)
		{
			m_shootingFeedbackHeadRemainingAlpha = 1.0f;
			m_shootingFeedbackHeadRemainingScale = 1.0f;
			m_feedbackHead->SetVisible(true);
			m_feedbackHead->SetAlpha(m_shootingFeedbackHeadRemainingAlpha);
			m_feedbackHead->SetScale(Vector3::ONE * Math::Clamp(m_shootingFeedbackHeadRemainingScale, 0.6f, 1.0f));
		}
		else
		{
			m_shootingFeedbackBodyRemainingAlpha = 1.0f;
			m_shootingFeedbackBodyRemainingScale = 1.0f;
			m_feedbackBody->SetVisible(true);
			m_feedbackBody->SetAlpha(m_shootingFeedbackBodyRemainingAlpha);
			m_feedbackBody->SetScale(Vector3::ONE * Math::Clamp(m_shootingFeedbackBodyRemainingScale, 0.6f, 1.0f));

		}
		return true;
	}

	bool gui_mainControl::showCameraDistanceSlider(bool isShow)
	{
		NULL_RETURN(m_sliderCameraDistance, true);

		m_sliderCameraDistance->SetVisible(isShow);
		if (isShow)
		{
			float cameraDistanceMax = PlayerPrefs::Instance()->getFloatPrefs("CameraDistanceMax");

			if (Math::Abs(cameraDistanceMax - m_cameraDistanceMin) < 0.00001f)
			{
				PlayerPrefs::Instance()->putFloatPrefs("CameraDistanceMax", m_cameraDistanceMin + 10.0f);
			}

			m_lastPlayerScale = RenderEntity::getRenderEntityActorScale(Blockman::Instance()->m_pPlayer);

			m_sliderCameraDistance->SetProgress(LogicSetting::Instance()->getThirdPersonDistance() / (cameraDistanceMax - m_cameraDistanceMin));
			if (m_sliderCameraDistance->GetProgress() < 0.05f)
			{
				m_sliderCameraDistance->SetProgress(0.05f);
			}
			else if (m_sliderCameraDistance->GetProgress() > 0.95f)
			{
				m_sliderCameraDistance->SetProgress(0.95f);
			}
		}

		return true;
	}

	void gui_mainControl::subscribeTouchEvent(GUIWindow* window, ButtonId id)
	{
		window->subscribeEvent("WindowTouchDown", std::bind(&gui_mainControl::onButtonPressed, this, std::placeholders::_1, id));
		window->subscribeEvent("WindowTouchMove", std::bind(&gui_mainControl::onButtonPressed, this, std::placeholders::_1, id));
		window->subscribeEvent("WindowTouchUp", std::bind(&gui_mainControl::onButtonReleased, this, std::placeholders::_1, id));
		window->subscribeEvent("MotionRelease", std::bind(&gui_mainControl::onButtonReleased, this, std::placeholders::_1, id));
	}

	bool gui_mainControl::onJumpDoubleClick(const EventArgs &)
	{
		// already add a new "Main-Fly" button to replace this function by single click
		return true;
// 		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying())
// 		{
// 			Blockman::Instance()->m_pPlayer->capabilities.isFlying = true;
// 			ClientNetwork::Instance()->getSender()->sendSetFlying(true);
// 			Blockman::Instance()->m_pPlayer->movementInput->down = false;
// 			Blockman::Instance()->m_pPlayer->movementInput->jump = false;
// 			m_buttonPressed[ButtonId::DROP] = false;
// 			m_buttonPressed[ButtonId::UP] = false;
// 			m_buttonPressed[ButtonId::DOWN] = false;
// 		}
// 		return true;
	}

	bool gui_mainControl::onDropDoubleClick(const EventArgs &)
	{
		auto pClientMovementInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
		Blockman::Instance()->m_pPlayer->capabilities.isFlying = false;
		ClientNetwork::Instance()->getSender()->sendSetFlying(false);
		pClientMovementInput->down = false;
		pClientMovementInput->jump = false;
		m_buttonPressed[ButtonId::JUMP] = false;
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(ButtonId::JUMP), false);
		return true;
	}

	bool gui_mainControl::onButtonPressed(const EventArgs &, ButtonId buttonId)
	{
		if (buttonId == ButtonId::SNEAK)
			return true;

		if ((Blockman::Instance()->m_pPlayer->m_isUseElevator || Blockman::Instance()->m_pPlayer->isOnDecoration() && !Blockman::Instance()->m_pPlayer->getCurrCameraStatus()) && (buttonId == ButtonId::FORWARD || buttonId == ButtonId::TOP_LEFT || buttonId == ButtonId::TOP_RIGHT ||
			buttonId == ButtonId::TOP_LEFT_BG || buttonId == ButtonId::TOP_RIGHT_BG || buttonId == ButtonId::LEFT || buttonId == ButtonId::RIGHT || buttonId == ButtonId::BACK))
		{
			return false;
		}

		m_buttonPressed[buttonId] = true;
		//LordLogInfo("onButtonPressed %d", (int)(buttonId));
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(buttonId), true);
		if (buttonId == ButtonId::FORWARD || buttonId == ButtonId::TOP_LEFT || buttonId == ButtonId::TOP_RIGHT ||
			buttonId == ButtonId::TOP_LEFT_BG || buttonId == ButtonId::TOP_RIGHT_BG)
		{
			m_isShowTopLeftAndTopRight = false;
			m_buttons[ButtonId::TOP_LEFT]->SetVisible(true); 
			m_buttons[ButtonId::TOP_RIGHT]->SetVisible(true);
			m_buttons[ButtonId::TOP_LEFT_BG]->SetVisible(true);
			m_buttons[ButtonId::TOP_RIGHT_BG]->SetVisible(true);
		}
		
		SCRIPT_EVENT::ClickMainControlEvent::invoke(true);

		return true;
	}

	bool gui_mainControl::onButtonReleased(const EventArgs &, ButtonId buttonId)
	{
		if (buttonId == ButtonId::SNEAK)
			return true;
		m_buttonPressed[buttonId] = false;
		//LordLogInfo("onButtonReleased %d", (int)(buttonId));
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(buttonId), false);
		if (buttonId == ButtonId::FORWARD)
		{
			m_isShowTopLeftAndTopRight = true;
		}
		else if (buttonId != ButtonId::JUMP && buttonId != ButtonId::DROP && buttonId != ButtonId::UP && buttonId != ButtonId::DOWN)
		{
			m_buttons[ButtonId::TOP_LEFT]->SetVisible(false);
			m_buttons[ButtonId::TOP_RIGHT]->SetVisible(false);
			m_buttons[ButtonId::TOP_LEFT_BG]->SetVisible(false);
			m_buttons[ButtonId::TOP_RIGHT_BG]->SetVisible(false);
		}
		return true;
	}

	bool gui_mainControl::onCheckChange(const EventArgs &)
	{
		bool isShowGunDetail = PlayerPrefs::Instance()->getBoolPrefs("IsShowGunDetail");
		if (!isShowGunDetail)
			return false;
		refreshGunPreview();
		getWindow("Main-GunDetailsIcon")->SetVisible(!getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked());
		return true;
	}

	bool gui_mainControl::onGlideCheckChenge(const EventArgs &)
	{
		if (!m_buttonPressed[ButtonId::JUMP])
		{
			bool isGlide = Blockman::Instance()->m_pPlayer->isGlide();
			if (!isGlide)
			{
				playSoundByType(ST_glide_wing);
			}
			Blockman::Instance()->m_pPlayer->setGlide(!isGlide);
			ClientNetwork::Instance()->getSender()->sendGlideOperation(!isGlide);

			m_glideCd = 500;
		}
		return true;
	}

	bool gui_mainControl::isForceUpdate()
	{
		return true;
	}

	bool gui_mainControl::onPixelGunSlotTouchUp(const EventArgs & events)
	{
		auto& touchEvent = dynamic_cast<const WindowEventArgs&>(events);
		auto&& window = dynamic_cast<GuiItemSlot*>(touchEvent.window);
		NULL_RETURN(window, false);

		int index = window->GetUserData();
		//int id = window->GetUserData2();

		auto it = m_mapSchematicData.find(index);
		if (it == m_mapSchematicData.end())
		{
			return false;
		}

		if (it->second == 0)
		{
			BottomSystemNotificationEvent::emit(LanguageManager::Instance()->getString("gui_pixelgun_1"), 3);
			return false;
		}

		const auto data = ModuleBlockSetting::getModuleBlockData(it->second);
		NULL_RETURN(data, false);

		auto&& stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		NULL_RETURN(stack, false);

		if (stack->stackSize < data->_costBlockNum)
		{
			BottomSystemNotificationEvent::emit(LanguageManager::Instance()->getString("gui_pixelgun_2"), 3); 
			return false;
		}

		{
			window->setSelected(true);

			if (m_lastGuiItemSlot)
			{
				m_lastGuiItemSlot->setSelected(false);
			}

			m_lastGuiItemSlot = window->isSelected() ? window : nullptr;
		}

		{
			auto&& pPlayer = Blockman::Instance()->m_pPlayer;
			NULL_RETURN(pPlayer, false);

			auto&& inventory = pPlayer->inventory;
			NULL_RETURN(inventory, false);
		}

		{
			PlayerPrefs::Instance()->SetProperty<int>("SchematicId", window->isSelected() ? data->_id : 0);
		}

		return false;
	}

	bool gui_mainControl::onItemStackTouchUp(const EventArgs& events)
	{
		auto& touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		LordAssert(touchEvent.row == 0);
		onChangeCurrentItem(touchEvent.column, true);
		return true;
	}

	bool gui_mainControl::onChangeCurrentItem(int itemIndex, bool isShowItemName)
	{
		if (itemIndex >= 0 && itemIndex < InventoryPlayer::HOTBAR_COUNT) {
			Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = itemIndex;
			ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			CheckGunUI();
			CheckFishingBtn();
			//CheckUseItemBtn();
			checkAimUI();
			refreshItemSkillShowUi();
			showPixelGunSlot(false);
			if (stack && stack->stackSize > 0)
			{
				if (isShowItemName)
				{
					getParent()->showItemNameUi(stack);
				}
				if (getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked())
				{
					refreshGunPreview();
				}

				bool isShowGunDetail = PlayerPrefs::Instance()->getBoolPrefs("IsShowGunDetail");
				getWindow("Main-GunPreviewCheckBox")->SetVisible(isShowGunDetail);
				getWindow("Main-GunDetailsIcon")->SetVisible(isShowGunDetail);

				checkPixelGunSlot(itemIndex, stack);
			}
			ClientNetwork::Instance()->getSender()->sendChangeCurrentItemInfo(itemIndex);
			return true;
		}
		return false;
	}

	void gui_mainControl::checkPixelGunSlot(int itemIndex, BLOCKMAN::ItemStackPtr &stack)
	{
		if (itemIndex == InventoryPlayer::HOTBAR_COUNT - 1
			&& stack->getItem() && stack->getItem()->isBlockItem(stack->itemID)
			&& (GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunHall
				|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunGameTeam
				|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunGamePerson
				|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunGame1v1
				|| GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunChicken))
		{
			showPixelGunSlot(true);
		}
	}

	bool gui_mainControl::onItemStackLongTouchStart(const EventArgs & events)
	{
		auto& touchEvent = dynamic_cast<const GuiSlotTable::ItemStackTouchEventArgs&>(events);
		m_dropItemIndex = touchEvent.column;
		Blockman::Instance()->m_pPlayer->inventory->currentItemIndex = touchEvent.column;
		ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->hotbar[touchEvent.column];
		CheckGunUI();
		if (stack && stack->stackSize > 0)
		{
			m_isLongTouch = true;
			m_touchTime = 300;
			getParent()->showItemNameUi(stack);
		}
		return true;
	}

	bool gui_mainControl::onItemStackLongTouchEnd(const EventArgs & events)
	{
		m_isLongTouch = false;
		m_touchTime = 0;
		updateDropItemProgress();
		m_dropItemIndex = -1;
		getWindow("Main-DropItemProgress")->SetVisible(false);
		return true;
	}

	void gui_mainControl::onAgainDropItem(int m_dropItemIndex)
	{
		dropItem(m_dropItemIndex);
	}

	void gui_mainControl::onUpdate(ui32 nTimeElapse)
	{

		if (!Blockman::Instance()->m_pPlayer || !isShown())
		{
			return;
		}

		if (!m_isCanChangeCameraStatus)
		{
			m_ChangeCameraStatusCd = m_ChangeCameraStatusCd + nTimeElapse;
			if (m_ChangeCameraStatusCd > 1000)
				m_isCanChangeCameraStatus = true;
		}

		auto alphaNormal = PlayerPrefs::Instance()->getFloatPrefs("MainControlKeyAlphaNormal");
		auto alphaPress = PlayerPrefs::Instance()->getFloatPrefs("MainControlKeyAlphaPress");
		for (ButtonId id : ALL_BUTTON_ID)
		{
			auto button = dynamic_cast<GUIStaticImage*>(m_buttons[id]);
			if (m_buttonPressed[id])
			{
				button->SetAlpha(alphaPress);
				button->SetActionMatrixScale(HOLD_SCALE);
				button->SetImage(m_buttonImages[id][1].c_str());
			}
			else
			{
				button->SetAlpha(alphaNormal);
				button->SetActionMatrixScale(RELEASE_SCALE);
				button->SetImage(m_buttonImages[id][0].c_str());
			}
		}

		bool isSneak = Blockman::Instance()->m_gameSettings->m_clientMovementInput->sneakPressed;
		auto sneak = dynamic_cast<GUIStaticImage*>(m_buttons[ButtonId::SNEAK]);
		if (sneak->IsVisible())
		{
			sneak->SetAlpha(isSneak ? 0.9f : alphaNormal);
			sneak->SetImage(isSneak ? m_buttonImages[ButtonId::SNEAK][1].c_str() : m_buttonImages[ButtonId::SNEAK][0].c_str());
		}

		m_flyingControls->SetVisible(Blockman::Instance()->m_pPlayer->capabilities.isFlying);

		useWatchModeUI(Blockman::Instance()->m_pPlayer->capabilities.isWatchMode);

		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		LordAssert(player);
		InventoryPlayer* inventory = player->inventory;
		LordAssert(inventory);
		inventory->updateInventoryCd((float)nTimeElapse / 1000);

		for (int i = 0; i < MAX_ITEM_COUNT; ++i)
		{
			m_guiSlotTable->setItemStack(0, i, inventory->hotbar[i]);
			if (inventory->hotbar[i] != nullptr && inventory->hotbar[i]->m_needPlayHotbarEffect)
			{
				m_guiSlotTable->setPlayEffect(0, i, RootGuiLayout::Instance()->isMainControlShown());
				inventory->hotbar[i]->m_needPlayHotbarEffect = false;
			}
		}
		m_guiSlotTable->selectItemSlot(inventory->currentItemIndex);
		m_touchTime = m_isLongTouch ? m_touchTime + nTimeElapse : 0;
		m_removeMessageKeepTime = m_removeMessageKeepTime + nTimeElapse;
		if (m_removeMessageKeepTime >= 4000)
		{
			removeChatMessage();
			m_removeMessageKeepTime = 0;
		}
		if (m_isShowTopLeftAndTopRight)
		{
			m_keepShowTopLeftAndTopRightTime += nTimeElapse;
			if (m_keepShowTopLeftAndTopRightTime > 150)
			{
				m_buttons[ButtonId::TOP_LEFT]->SetVisible(false);
				m_buttons[ButtonId::TOP_RIGHT]->SetVisible(false);
				m_buttons[ButtonId::TOP_LEFT_BG]->SetVisible(false);
				m_buttons[ButtonId::TOP_RIGHT_BG]->SetVisible(false);
			}
		}
		else
		{
			m_keepShowTopLeftAndTopRightTime = 0;
		}

		m_keepShowTopSystemNotificationTime = m_keepShowTopSystemNotificationTime > 0 ? m_keepShowTopSystemNotificationTime - nTimeElapse : 0;
		m_keepShowBottomSystemNotificationTime = m_keepShowBottomSystemNotificationTime > 0 ? m_keepShowBottomSystemNotificationTime - nTimeElapse : 0;
		m_keepShowCenterSystemNotificationTime = m_keepShowCenterSystemNotificationTime > 0 ? m_keepShowCenterSystemNotificationTime - nTimeElapse : 0;
		
		m_topSystemNotification->SetVisible(m_keepShowTopSystemNotificationTime > 0);
		m_bottomSystemNotification->SetVisible(m_keepShowBottomSystemNotificationTime > 0);
		m_centerSystemNotification->SetVisible(m_keepShowCenterSystemNotificationTime > 0);

		URect  mainControlArea = getWindow("Main-Control")->GetArea();
		mainControlArea.setWidth(UDim(Blockman::Instance()->m_gameSettings->getMainGuiWidth(), 0.0f));
		mainControlArea.setHeight(UDim(Blockman::Instance()->m_gameSettings->getMainGuiHeight(), 0.0f));
		getWindow("Main-Control")->SetArea(mainControlArea);

		URect  mainJumpControlsArea = getWindow("Main-Jump-Controls")->GetArea();
		mainJumpControlsArea.setWidth(UDim(Blockman::Instance()->m_gameSettings->getMainGuiWidth() * 0.382f, 0.0f));
		mainJumpControlsArea.setHeight(UDim(Blockman::Instance()->m_gameSettings->getMainGuiHeight(), 0.0f));
		getWindow("Main-Jump-Controls")->SetArea(mainJumpControlsArea);

		URect  mainFlyingControlsArea = getWindow("Main-FlyingControls")->GetArea();
		mainFlyingControlsArea.setWidth(UDim(Blockman::Instance()->m_gameSettings->getMainGuiWidth() * 0.382f, 0.0f));
		mainFlyingControlsArea.setHeight(UDim(Blockman::Instance()->m_gameSettings->getMainGuiHeight(), 0.0f));
		getWindow("Main-FlyingControls")->SetArea(mainFlyingControlsArea);

		checkMoveControl();

		if (m_isLongTouch)
		{
			updateDropItemProgress();
		}

		checkHideGunUI();

		ItemStackPtr stack = inventory->getCurrentItem();
		if (!(stack && stack->getGunSetting() && (stack->getGunSetting()->gunType == LASER_EFFECT_GUN || stack->getGunSetting()->gunType == ELECTRICITY_EFFECT_GUN)))
		{
			updateShootProgress();
		}

		if (m_teleportBtnCountdown > 0)
		{
			m_teleportBtnCountdown = m_teleportBtnCountdown - nTimeElapse; 
		}
		m_teleportBtn->SetEnabled(m_teleportBtnCountdown <= 0);
		m_teleportBtn->SetText(m_teleportBtnCountdown > 0 ? StringUtil::ToString(m_teleportBtnCountdown / 1000 + 1).c_str() : "");
		m_teleportBtn->SetTextScale(1.6f);
		m_teleportBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Sand_Digger);
		m_skillBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Zombie_Infecting && m_skillType != SkillType::NONE);
		m_showManorBtn->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_manorBtnVisible);
		m_btnPutFurnitureSure->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_putFurnitureId > 0);
		m_btnPutFurnitureCancel->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_putFurnitureId > 0);
		m_btnManorPraise->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town && m_putFurnitureId<=0 && manorCheckPlayerLocation() != 0);
		updateSkillBtnInfo(nTimeElapse);
		updateExpBar();
		checkPersonalShopUI();
		checkCrossHairIsCanAttack();
		updateItemSkillBtnInfo(nTimeElapse);
		checkSkillIsHaveAttackEntity();
		updateMaskInfo(nTimeElapse);
		checkFlyBtnShow();
		checkOpenFlyingShow();
		birdTaskUpdate(nTimeElapse);
		blockCityUpdate(nTimeElapse);

		if (m_showShootWarning)
		{
			if (m_shootWarningAlpha <= 0.0f)
			{
				m_showShootWarning = false;
				m_warningLayout->SetVisible(false);
				m_shootWarning->SetVisible(false);
			}
			else
			{
				m_shootWarningAlpha -= 0.2f * Root::Instance()->getFrameTime() / 100;
				m_shootWarning->SetAlpha(Math::Clamp(m_shootWarningAlpha, 0.0f, 1.0f));
				m_shootWarning->SetRotate(m_shootWarningRotateAngle);
				m_shootWarning->SetVisible(true);
			}
		}

		if (m_shootingFeedbackHeadRemainingAlpha > 0.0f)
		{
			m_shootingFeedbackHeadRemainingAlpha -= nTimeElapse * 0.001f * m_shootingFeedbackSpeedAlpha;
			m_shootingFeedbackHeadRemainingScale -= nTimeElapse * 0.001f * m_shootingFeedbackSpeedScale;
			if (m_shootingFeedbackHeadRemainingAlpha <= 0.0f)
			{
				m_shootingFeedbackHeadRemainingAlpha = 0.0f;
				m_feedbackHead->SetVisible(false);
			}
			m_feedbackHead->SetAlpha(m_shootingFeedbackHeadRemainingAlpha);
			m_feedbackHead->SetScale(Vector3::ONE * Math::Clamp(m_shootingFeedbackHeadRemainingScale, 0.6f, 1.0f));
		}
		if (m_shootingFeedbackBodyRemainingAlpha > 0.0f)
		{
			m_shootingFeedbackBodyRemainingAlpha -= nTimeElapse * 0.001f * m_shootingFeedbackSpeedAlpha;
			m_shootingFeedbackBodyRemainingScale -= nTimeElapse * 0.001f * m_shootingFeedbackSpeedScale;
			if (m_shootingFeedbackBodyRemainingAlpha <= 0.0f)
			{
				m_shootingFeedbackBodyRemainingAlpha = 0.0f;
				m_feedbackBody->SetVisible(false);
			}
			m_feedbackBody->SetAlpha(m_shootingFeedbackBodyRemainingAlpha);
			m_feedbackBody->SetScale(Vector3::ONE * Math::Clamp(m_shootingFeedbackBodyRemainingScale, 0.6f, 1.0f));
		}

		if (m_playEndingAnimation)
		{
			updateEndingAnimation();
		}

		m_btnPlaceBuilding->SetVisible( checkPlaceBuildingViewVisible());
		updateHideAndSeekYawChange(nTimeElapse);
		onBirdDataUpdate(nTimeElapse);
		onBirdMessageUpdate(nTimeElapse);
		checkBlockCityStoreShow();
		onAttackUpdate(nTimeElapse);
		updateFloatTip(nTimeElapse);
		updateCameraDistance();
	}

	void gui_mainControl::checkPersonalShopUI()
	{
		bool canShowPersonalShop = getParent()->canPersonalShop();
		m_personalShopBtn->SetVisible(canShowPersonalShop);
		m_buyButtleBtn->SetVisible(canShowPersonalShop && PlayerPrefs::Instance()->getBoolPrefs("IsShowBuyBullet"));
	}

	bool gui_mainControl::canUseByUseItemBtn(Item* item)
	{
		if (item->itemID == BLOCK_ID_TNT 
			|| item->itemID == Item::snowball->itemID
			|| dynamic_cast<ItemFood*>(item)
			|| dynamic_cast<ItemTeleportScroll*>(item)
			|| dynamic_cast<ItemCreateBridge*>(item)
			|| dynamic_cast<ItemPotion*>(item)
			|| dynamic_cast<ItemGrenade*>(item)
			|| dynamic_cast<ItemFireball*>(item)
			|| dynamic_cast<ItemMedichine*>(item))
		{
			return true;
		}
		return false;
	}

	void gui_mainControl::CheckUseItemBtn()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			m_btnUseItem->SetVisible(false);
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			m_btnUseItem->SetVisible(false);
			return;
		}

		if (canUseByUseItemBtn(pCurrentItem->getItem()))
		{
			m_btnUseItem->SetVisible(true);
			return;
		}

		m_btnUseItem->SetVisible(false);
	}

	bool gui_mainControl::onClickUseItemBtn(const EventArgs &)
	{
		if (!Blockman::Instance()->m_pPlayer->inventory)
			return false;

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();

		if (pCurrentItem)
		{
			if (canUseByUseItemBtn(pCurrentItem->getItem()))
			{
				pCurrentItem->useItemRightClick(Blockman::Instance()->m_pWorld, Blockman::Instance()->m_pPlayer);
				Blockman::Instance()->m_pPlayer->m_isUsingItemByBtn = true;
				ItemPotion* potion = dynamic_cast<ItemPotion*>(pCurrentItem->getItem());
				if (pCurrentItem->itemID == Item::snowball->itemID || (potion && potion->isSplash()))
				{
					Blockman::Instance()->m_pPlayer->m_isUsingItemByBtn = false;
				}
				return true;
			}
		}

		return false;
	}

	bool gui_mainControl::showPlayerInventory(const EventArgs & events)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			getParent()->showBlockCityStore();
		}
		else if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
		{
			getParent()->showBlockFortPack();
		}
		else
		{
			getParent()->showPlayerInventory();
		}
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_mainControl::addChatMessage(const String&  message, bool isChatInformation)
	{
		if (!SCRIPT_EVENT::ShowChatMessageReceiveEvent::invoke(isChatInformation))
		{
			return false;
		}

		static unsigned count = 0;
		String strTextName = StringUtil::Format("Main-Chat-Message-List-item-%d", count++).c_str();
		GUIStaticText* pStaticText = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, strTextName.c_str());
		pStaticText->SetText(message.c_str());
		pStaticText->SetTouchable(false);
		pStaticText->SetTextVertAlign(VA_CENTRE);
		pStaticText->SetHorizontalAlignment(HA_CENTRE);
		pStaticText->SetVerticalAlignment(VA_CENTRE);
		pStaticText->SetTextScale(1.2f);
		pStaticText->SetWordWrap(true);
		float h = 30.f;
		if (message.length() <30.0f)
		{
			h = 30.f;
		}
		else if (message.length() < 60.0f)
		{
			h = 50.f;
		} 
		else if (message.length() < 90.0f) 
		{
			h = 70.f;
		}
		else
		{
			h = 90.f;
		}

		pStaticText->SetArea(UDim(0, 0), UDim(0, 0), UDim(1, 0), UDim(0, h));
		m_chatMessageListBox->AddItem(pStaticText, false);
		SCRIPT_EVENT::AddChatMessageEvent::invoke(strTextName);

		if (m_chatMessageListBox->getContainerWindow()->GetChildCount() == 1)
		{
			m_removeMessageKeepTime = 0;
		}
		else
		{
			scrollBottom();
		}

		return true;
	}

	bool gui_mainControl::showTopSystemNotification(const String & message, i32 keepTime)
	{
		m_topSystemNotification->SetText(message.c_str());
		m_topSystemNotification->SetVisible(true);
		m_topSystemNotification->SetTouchable(false);
		m_keepShowTopSystemNotificationTime = keepTime * 1000;
		return true;
	}

	bool gui_mainControl::showBottomSystemNotification(const String & message, i32 keepTime)
	{
		m_bottomSystemNotification->SetText(message.c_str());
		m_bottomSystemNotification->SetVisible(true);
		m_bottomSystemNotification->SetTouchable(false);
		m_keepShowBottomSystemNotificationTime = keepTime * 1000;
		return true;
	}

	bool gui_mainControl::showCenterSystemNotification(const String & message, i32 keepTime)
	{
		m_centerSystemNotification->SetText(message.c_str());
		m_centerSystemNotification->SetVisible(true);
		m_centerSystemNotification->SetTouchable(false);
		m_keepShowCenterSystemNotificationTime = keepTime * 1000;
		onBirdAddMessage(message);
		return true;
	}

	bool gui_mainControl::onBreakBlockProgress(float progress, RAYTRACE_TYPE type)
	{
		if (m_breakBlockProgress && m_breakBlockProgress->GetChildByIndex(0))
		{
			m_breakBlockProgress->GetChildByIndex(0)->SetArea(UDim(0, 0), UDim(0, 0), UDim(progress, 0), UDim(progress, 0));
		}
		if (progress > 0 && type == RAYTRACE_TYPE_ENTITY)
		{
			m_hitEntityProgress->SetVisible(true);
			m_hitEntityProgress->SetProgress(progress);
		}
		else
		{
			m_hitEntityProgress->SetVisible(false);
			m_hitEntityProgress->SetProgress(0.0f);
		}
		return true;
	}

	bool gui_mainControl::breakBlockUiManage(bool isShow, float x, float y)
	{
		if (m_breakBlockProgress && !PlayerPrefs::Instance()->getBoolPrefs("DisableBreakBlockUi"))
		{
			m_breakBlockProgress->SetVisible(isShow);
			m_breakBlockProgress->SetArea(UDim(0, x - 90.0f), UDim(0, y - 90.0f), UDim(0, 180.f), UDim(0, 180.f));
		}
		return true;
	}

	bool gui_mainControl::checkMoveControl()
	{
		auto player = Blockman::Instance()->m_pPlayer;
		bool driving = player && player->isDriver();
		if (driving)
		{
			m_arrowControlWindow->SetVisible(false);
			m_poleControlWindow->SetVisible(false);
			m_poleMoveWindow->SetVisible(false);
			m_jumpControlWindow->SetVisible(false);
			m_moveStateWindow->SetVisible(false);
			return true;
		}

		m_moveStateWindow->SetVisible(!player->getIsFlying());
		m_jumpControlWindow->SetVisible(!player->getIsFlying());
		
		bool usePole = Blockman::Instance()->m_gameSettings->usePole > 0;
		m_arrowControlWindow->SetVisible(!usePole);
		m_poleControlWindow->SetVisible(usePole);
		m_poleMoveWindow->SetVisible(usePole);
		if (usePole)
		{
			m_buttons[ButtonId::JUMP]->SetVisible(true);
		}
		else
		{
			m_buttons[ButtonId::SNEAK]->SetVisible(true);
			if (m_isCanJump)
			{
				m_buttons[ButtonId::JUMP]->SetVisible(true);
			}
			else
			{
				m_buttons[ButtonId::JUMP]->SetVisible(false);
			}
		}
		return true;
	}

	void gui_mainControl::useWatchModeUI(bool isWatchMode)
	{
		m_playerExpProgress->SetVisible(!isWatchMode);
		m_itemBarBgImage->SetVisible(!isWatchMode);
		m_watchBattleWindow->SetVisible(isWatchMode);
		if ((checkGameType(ClientGameType::SkyBlockMain) || checkGameType(ClientGameType::SkyBlockMineCave) || checkGameType(ClientGameType::SkyBlockProduct)))
		{
			m_watchBattleWindow->SetVisible(false);
		}
		if (isWatchMode && Blockman::Instance()->m_gameSettings->getAutoRunStatus())
		{
			ForceAutoRun();
		}

		if (checkGameType({ ClientGameType::LiftingSimulator, ClientGameType::LSChampionShip }))
		{
			m_itemBarBgImage->SetVisible(false);
			m_playerExpProgress->SetVisible(false);
			getWindow("Main-VisibleBar")->SetVisible(false);
		}
		else
			getWindow("Main-VisibleBar")->SetVisible(!isWatchMode);
	}

	void gui_mainControl::removeChatMessage()
	{
		if (m_chatMessageListBox && m_chatMessageListBox->getContainerWindow())
		{
			if (m_chatMessageListBox->getContainerWindow()->GetChildCount() > 0)
			{
				m_chatMessageListBox->DeleteItem(0);
				scrollBottom();
				if (m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y < 0)
				{
					removeChatMessage();
				}
			}
		}
	}

	void gui_mainControl::scrollBottom()
	{
		if (m_chatMessageListBox->getContainerWindow() && m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y < 0)
		{
			m_chatMessageListBox->SetScrollOffset(m_chatMessageListBox->GetPixelSize().y - m_chatMessageListBox->getContainerWindow()->GetPixelSize().y);
		}
	}
	void gui_mainControl::updateDropItemProgress()
	{
		GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
		GUIProgressBar* dropItemProgress = dynamic_cast<GUIProgressBar*>(getWindow("Main-DropItemProgress"));
		if (slotTable && m_dropItemIndex >= 0)
		{
			float progress = (float)m_touchTime / 1200.f;
			slotTable->setDropItemProgress(m_dropItemIndex, progress >= 1.0f ? 0 : progress);
			dropItemProgress->SetProgress(progress > 1 ? 0 : progress);
			dropItemProgress->SetVisible(progress <= 1);
			if (progress >= 1)
			{
				EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
				LordAssert(player);
				InventoryPlayer* inventory = player->inventory;
				ItemStackPtr stack = inventory->hotbar[m_dropItemIndex];
				if (stack && stack->stackSize != 0)
				{
 					if (SCRIPT_EVENT::DropItemEvent::invoke(stack->itemID, m_dropItemIndex, stack->itemDamage))
 					{
						ClientNetwork::Instance()->getSender()->sendDropItemPosition(inventory->getMainInventory()->findItemStack(stack));
						inventory->getMainInventory()->decrStackSize(inventory->getMainInventory()->findItemStack(stack), stack->stackSize);
						HideGunUI();
					}	
				}
				m_isLongTouch = false;
				m_touchTime = 0;
				m_dropItemIndex = -1;
				dropItemProgress->SetVisible(false);
			}
		}
		else
		{
			dropItemProgress->SetProgress(0.f);
			dropItemProgress->SetVisible(false);
		}
	}

	void gui_mainControl::dropItem(int m_dropItemIndex)
	{
		EntityPlayerSP* player = Blockman::Instance()->m_pPlayer;
		LordAssert(player);
		InventoryPlayer* inventory = player->inventory;
		ItemStackPtr stack = inventory->hotbar[m_dropItemIndex];
		if (stack && stack->stackSize != 0)
		{
			ClientNetwork::Instance()->getSender()->sendDropItemPosition(inventory->getMainInventory()->findItemStack(stack));
			inventory->getMainInventory()->decrStackSize(inventory->getMainInventory()->findItemStack(stack), stack->stackSize);
			HideGunUI();
		}
	}

	void gui_mainControl::HideGunUI()
	{
		m_gunOperationWindow->SetVisible(false);
		m_isTouchLeftShootBtn = false;
		m_isTouchRightShootBtn = false;
		m_isReleaseRightShootBtn = true;
		setAimState(false);
	}

	void gui_mainControl::CheckGunUI()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (Blockman::Instance()->m_pPlayer->isOnVehicle() || Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			HideGunUI();
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			HideGunUI();
			return;
		}

		ItemGun* pGun = dynamic_cast<ItemGun*>(pCurrentItem->getItem());
		if (!pGun)
		{
			HideGunUI();
			return;
		}
		m_gunOperationWindow->SetVisible(true);
		setAimState(false);
		const GunSetting* pSetting = pGun->getGunSetting();
		if (pSetting)
		{
			m_aimBtn->SetVisible(pSetting->sniperDistance > 0.0f);
		}
	}

	bool gui_mainControl::tryFireGun(bool bSingleClick)
	{
		if (!CanShoot(bSingleClick))
			return false;

		Blockman::Instance()->m_pPlayer->swingItem();
		Blockman::Instance()->m_pPlayer->gunFire();

		auto pPlayer = Blockman::Instance()->m_pPlayer;
		ItemStackPtr stack = pPlayer->inventory->getCurrentItem();
		if (stack)
		{
			ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
			if (pGun)
			{
				const GunSetting* pGunSetting = pGun->getGunSetting();
				if (pGunSetting)
				{
					m_crossHairMoveStepLength = pGunSetting->crossHairMoveSpeed;
					m_crossHairMoveRange = pGunSetting->crossHairMoveDistance;
					if (pGunSetting->gunType == SNIPER_GUN || pGunSetting->gunType == SNIPER_EFFECT_GUN) {
							HideGunUI();
							//Blockman::Instance()->m_pPlayer->playReloadProgress();
							CheckGunUI();
					}
				}
			}
		}
		return true;
	}

	bool gui_mainControl::CanShoot(bool bSingleClick)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (pPlayer->isOnVehicle())
			return false;

		if (pPlayer->m_isReloadBullet)
			return false;

		ItemStackPtr stack = pPlayer->inventory->getCurrentItem();
		if (!stack || stack->stackSize < 1)
			return false;

		ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
		if (!pGun)
		{
			LordLogError("CurrentStack is not type of ItemGun");
			return false;
		}

		const GunSetting* pGunSetting = pGun->getGunSetting();
		if (!pGunSetting)
		{
			LordLogError("The GunSetting of currentStack not found");
			return false;
		}

		if (stack->isOnCd())
			return false;

		int bulletNum = stack->getCurrentClipBulletNum();
		int totalNum = stack->getTotalBulletNum();
		if (bulletNum < 1)
		{
			int nCountInInventory = pPlayer->inventory->getItemNum(stack->itemID);
			if (nCountInInventory > 0)
			{
				Blockman::Instance()->m_pPlayer->beginReloadBullet();
			}
			else if (bSingleClick)
			{
				playSoundByType((SoundType)pGunSetting->emptySoundType);
			}

			return false;
		}

		stack->consumeBullet();
		stack->setCd(pPlayer->getGunFireCd(pGunSetting));

		int curBulletNum = stack->getCurrentClipBulletNum();
		if (curBulletNum < 1)
		{
			int nCountInInventory = pPlayer->inventory->getItemNum(stack->itemID);
			if (nCountInInventory > 0)
			{
				Blockman::Instance()->m_pPlayer->beginReloadBullet();
			}
		}
		return true;
	}

	bool gui_mainControl::onClickReloadBulletBtn(const EventArgs &)
	{
		HideGunUI();
		bool bResult = Blockman::Instance()->m_pPlayer->beginReloadBullet();
		CheckGunUI();
		return bResult;
	}

	bool gui_mainControl::onClickAimBtn(const EventArgs & events)
	{
		m_aimBtn->SetVisible(false);
		setAimState(true);
		AimingStateChangeEvent::emit(false, NONE_TYPE);
		return true;
	}

	bool gui_mainControl::onClickCancelAimBtn(const EventArgs & events)
	{
		onCancelAim();
		return true;
	}

	void gui_mainControl::onCancelAim()
	{
		m_aimBtn->SetVisible(true);
		setAimState(false);
		AimingStateChangeEvent::emit(true, RIFLE_CROSS_HAIR);
	}

	bool gui_mainControl::onClickThrowGunBtn(const EventArgs &)
	{
		int index = Blockman::Instance()->m_pPlayer->inventory->currentItemIndex;
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendDropItemPosition(index);
		return true;
	}

	void gui_mainControl::setWatchRespawnUI()
	{
		getWindow<GUIStaticText>("Main-Watch-Battle-Text")->SetText(getString(LanguageKey::GUI_MAIN_WATCHING_BATTLE).c_str());
		auto btnWatchRespawn = getWindow<GUIButton>("Main-Watch-Battle-Respawn");
		btnWatchRespawn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onWatchRepawnClick, this, std::placeholders::_1));
		btnWatchRespawn->SetText(getString(LanguageKey::GUI_REBIRTH));
		btnWatchRespawn->SetVisible(false);
	}

	void gui_mainControl::refreshGunPreview()
	{
		ItemStackPtr itemStack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!itemStack || itemStack->stackSize <= 0)
		{
			getWindow("Main-Gun-Preview")->SetVisible(false);
			getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->SetCheckedNoEvent(false);
			return;
		}

		ItemGun* pGun = dynamic_cast<ItemGun*>(itemStack->getItem());
		if (!pGun)
		{
			getWindow("Main-Gun-Preview")->SetVisible(false);
			getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->SetCheckedNoEvent(false);
			return;
		}

		getWindow("Main-Gun-Preview")->SetVisible(getWindow<GUICheckBox>("Main-GunPreviewCheckBox")->GetChecked());

		const GunSetting* gunSetting = pGun->getGunSetting();
		if (!gunSetting)
		{
			return;
		}
		getWindow("Main-Gun-Harm-Value")->SetText(StringUtil::ToString(gunSetting->damage).c_str());
		getWindow("Main-Gun-Range-Value")->SetText(StringUtil::Format(getString(LanguageKey::GUI_STR_GUN_RANGE_VALUE).c_str(), gunSetting->shootRange).c_str());
		getWindow("Main-Gun-FiringRate-Value")->SetText(StringUtil::Format(getString(LanguageKey::GUI_STR_GUN_FIRING_RATE_VALUE).c_str(), gunSetting->cdTime).c_str());
		getWindow("Main-Gun-Deviation-Value")->SetText(StringUtil::Format(getString(LanguageKey::GUI_STR_GUN_DEVIATION_VALUE).c_str(), gunSetting->bulletOffset).c_str());
		getWindow("Main-Gun-Name")->SetText(LanguageManager::Instance()->getItemName(itemStack->getItemName()).c_str());
		const BulletClipSetting*  bulletClipSetting = itemStack->getBulletClipSetting();
		if (bulletClipSetting)
		{
			getWindow<GUIStaticImage>("Main-Gun-Bullet-Icon")->SetImage(bulletClipSetting->bulletIcon.c_str());
		}
		m_actorWindow->SetActor("gun.actor", "idle");
		m_actorWindow->SetRotateY(90);
		StringArray array = StringUtil::Split(gunSetting->previewMesh, ".", 1);
		if (array.size() == 2)
		{
			m_actorWindow->UseBodyPart(array[0], array[1]);
		}
	}

	bool gui_mainControl::onLeftShootBtnTouchDown(const EventArgs& events)
	{
		if (!m_isTouchLeftShootBtn)
		{
			m_isTouchLeftShootBtn = true;
			bool result = tryFireGun(true);
			m_bChangeCrosshair = result ? true : m_bChangeCrosshair;
			return result;
		}

		return true;
	}

	bool gui_mainControl::onLeftShootBtnTouchUp(const EventArgs& events)
	{
		m_isTouchLeftShootBtn = false;

		InventoryPlayer* inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr stack = inventory ? inventory->getCurrentItem() : nullptr;
		if (stack && stack->getGunSetting() && (stack->getGunSetting()->gunType == LASER_EFFECT_GUN || stack->getGunSetting()->gunType == ELECTRICITY_EFFECT_GUN))
		{
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
		}

		return true;
	}

	bool gui_mainControl::onRightShootBtnTouchDown(const EventArgs& events)
	{
		if (!m_isTouchRightShootBtn)
		{
			m_isTouchRightShootBtn = true;
			bool result = tryFireGun(true);
			m_bChangeCrosshair = result ? true : m_bChangeCrosshair;
			return result;
		}

		return true;
	}

	bool gui_mainControl::onRightShootBtnTouchUp(const EventArgs& events)
	{
		m_isTouchRightShootBtn = false;
		m_isReleaseRightShootBtn = true;
		m_rightShootBtn->SetPosition(m_originButtonPos);
		//Blockman::Instance()->m_gameSettings->mouseSensitivity = m_prevMouseSensitive;
		Blockman::Instance()->m_gameSettings->setMouseMoveState(false);

		InventoryPlayer* inventory = Blockman::Instance()->m_pPlayer->inventory;
		ItemStackPtr stack = inventory ? inventory->getCurrentItem() : nullptr;
		if (stack && stack->getGunSetting() && (stack->getGunSetting()->gunType == LASER_EFFECT_GUN || stack->getGunSetting()->gunType == ELECTRICITY_EFFECT_GUN))
		{
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
		}

		return true;
	}

	bool gui_mainControl::onRightShootBtnMoved(const EventArgs & events)
	{
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		if (m_isReleaseRightShootBtn)
		{
			m_isReleaseRightShootBtn = false;
			Blockman::Instance()->m_gameSettings->beginMouseMove(mouseEventArgs.position.x, mouseEventArgs.position.y);
		}
		else
		{
			Blockman::Instance()->m_gameSettings->setMousePos(mouseEventArgs.position.x, mouseEventArgs.position.y);
		}

		float offset = m_rightShootBtn->GetWidth().d_offset;
		m_rightShootBtn->SetPosition(UVector2({ 0, mouseEventArgs.position.x - offset / 2 }, {0, mouseEventArgs.position.y - offset / 2}));
		return false;
	}

	void gui_mainControl::updateShootProgress()
	{
		if (!m_isTouchLeftShootBtn && !m_isTouchRightShootBtn)
			return;

		m_bChangeCrosshair = tryFireGun(false) ? true : m_bChangeCrosshair;
	}

	void gui_mainControl::setAimState(bool bState)
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		m_gunSniperSight->SetVisible(bState);
		m_cancelAimBtn->SetVisible(bState);
		if (!bState)
		{
			if (Blockman::Instance()->m_pPlayer->m_bIsInAimSight)
			{
				Blockman::Instance()->m_gameSettings->setFovSetting(m_prevFovSetting);
				Blockman::Instance()->m_gameSettings->setMouseSensitivity(m_prevMouseSensitive);
				Blockman::Instance()->setPersonView(m_prevCameraView);
			}
		}
		else
		{
			ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (stack)
			{
				const GunSetting* pSetting = stack->getGunSetting();
				if (pSetting)
				{
					m_prevFovSetting = Blockman::Instance()->m_gameSettings->getFovSetting();
					m_prevMouseSensitive = Blockman::Instance()->m_gameSettings->getMouseSensitivity();
					m_prevCameraView = Blockman::Instance()->m_gameSettings->getPersonView();
					float fov = -(float)pSetting->sniperDistance / 10;
					fov = Math::Max(-1.4f, fov); // must > -1.5
					Blockman::Instance()->m_gameSettings->setFovSetting(fov);
					Blockman::Instance()->m_gameSettings->setMouseSensitivity(pSetting->mouseSensitivity);
					Blockman::Instance()->setPersonView(GameSettings::SPVT_FIRST);
				}
			}
		}

		Blockman::Instance()->m_pPlayer->m_bIsInAimSight = bState;
	}

	void gui_mainControl::checkHideGunUI()
	{
		/*if (!m_gunOperationWindow->IsVisible())
			return;*/
		m_gunOperationWindow->SetVisible(true);

		if (Blockman::Instance()->m_pPlayer->isOnVehicle() || Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			HideGunUI();
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			HideGunUI();
			return;
		}

		ItemGun* pGun = dynamic_cast<ItemGun*>(pCurrentItem->getItem());
		if (!pGun)
		{
			HideGunUI();
			return;
		}
	}

	void gui_mainControl::checkAimUI()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		ItemStackPtr ptr = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (ptr && !Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			if (ptr->itemID == BLOCKMAN::Item::snowball->itemID || ptr->isGunItem() ||
				ptr->isThrowItemSkill() || ptr->itemID == BLOCKMAN::Item::fishingRod->itemID || 
				dynamic_cast<ItemBow*>(ptr->getItem()))
			{
				int crossHairType = COMMON_CROSS_HAIR;
				if (ptr->getGunSetting())
				{
					crossHairType = Blockman::Instance()->m_pPlayer->getCrossHairByGunType(ptr->getGunSetting()->gunType);
				}
				if (ptr->isThrowItemSkill())
				{
					crossHairType = RIFLE_CROSS_HAIR;
				}
				AimingStateChangeEvent::emit(true, crossHairType);
			}
			else
			{
				AimingStateChangeEvent::emit(false, NONE_TYPE);
			}
		}
		else
		{
			AimingStateChangeEvent::emit(false, NONE_TYPE);
		}
	}

	void gui_mainControl::updateCameraBtnUI()
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			m_cameraLockImage->SetVisible(true);
			m_cameraUnLockImage->SetVisible(false);
		}
		else
		{
			m_cameraLockImage->SetVisible(false);
			m_cameraUnLockImage->SetVisible(true);
		}
	}

	bool gui_mainControl::onClickTeleportBtn(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickTeleport();
		m_teleportBtnCountdown = 9 * 1000;
		return true;
	}

	bool gui_mainControl::showParachuteBtn(bool isNeedOpen)
	{
		if (m_parachuteBtn) {
			if (isNeedOpen) {
				m_parachuteBtn->SetVisible(true);
			}
			else {
				m_parachuteBtn->SetVisible(false);
			}
			
		}
		return true;
	}

	void gui_mainControl::showFlyBtn(bool isNeedOpen)
	{
		if (m_flyBtn) {
			if (isNeedOpen) {
				m_flyBtn->SetVisible(true);
			}
			else {
				m_flyBtn->SetVisible(false);
			}
		}
	}

	void gui_mainControl::showBuildWarBlockBtn(bool isNeedOpen)
	{
		if (m_buildWarBlockBtn) {
			if (isNeedOpen) {
				m_buildWarBlockBtn->SetVisible(true);
			}
			else {
				m_buildWarBlockBtn->SetVisible(false);
			}
		}
	}

	bool gui_mainControl::onPoleTouchDown(const EventArgs & events)
	{
		auto poleWidth = m_poleControlWindow->GetPixelSize().x;
		auto poleHeight = m_poleControlWindow->GetPixelSize().y;
		if (Blockman::Instance()->m_gameSettings->usePole == 2) // free pole
		{
			m_poleMoveWindow->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, 3600.0f), UDim(0.0f, 2400.0f));
			auto& rect = m_poleControlWindow->GetUnclippedOuterRect();
			auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
			if (!(mouseEventArgs.position.x <= rect.right && mouseEventArgs.position.y >= rect.top))
			{
				m_poleControlWindow->SetXPosition({ 0.0f,
					Math::Max(Math::Min(mouseEventArgs.position.x - poleWidth / 2, poleWidth / 2), 50.0f) });
				m_poleControlWindow->SetYPosition({ 0.0f,
					Math::Min(Math::Max(mouseEventArgs.position.y + poleHeight - m_rootWindow->GetPixelSize().y, -poleHeight / 2), -70.0f) });
			}
		}
		else
		{
			m_poleMoveWindow->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, poleWidth * 2), UDim(0.0f, poleWidth * 2));
			m_poleControlWindow->SetPosition({ {0.0f, 50.0f}, {0.0f, -70.0f} });
		}
		auto rect = m_poleControlWindow->GetUnclippedOuterRect();
		m_originPoleAbsPos = UVector2(UDim(0.0f, rect.left + m_poleControlWindow->GetPixelSize().x / 2),
			UDim(0.0f, rect.top + m_poleControlWindow->GetPixelSize().y / 2));
		m_poleBgWindow->SetAlpha(0.5f);
		SCRIPT_EVENT::ClickMainControlEvent::invoke(true);
		return true;
	}

	bool gui_mainControl::onPoleTouchUp(const EventArgs & events)
	{
		m_poleControlWindow->SetPosition({ {0.0f, 50.0f}, {0.0f, -70.0f} });
		auto rect = m_poleControlWindow->GetUnclippedOuterRect();
		m_originPoleAbsPos = UVector2(UDim(0.0f, rect.left + m_poleControlWindow->GetPixelSize().x / 2),
			UDim(0.0f, rect.top + m_poleControlWindow->GetPixelSize().y / 2));

		auto poleWidth = m_poleControlWindow->GetPixelSize().x;
		auto poleHeight = m_poleControlWindow->GetPixelSize().y;

		if (Blockman::Instance()->m_gameSettings->usePole == 2) // free pole
			m_poleMoveWindow->SetArea(UDim(0.0f, 0.0f), UDim(0.0f, 0.0f), UDim(0.0f, poleWidth * 1.8f), UDim(0.0f, poleWidth * 2.0f));
		else
			m_poleMoveWindow->SetArea(UDim(0.0f, 50.0f), UDim(0.0f, -70.0f), UDim(0.0f, poleWidth), UDim(0.0f, poleHeight));
		
		m_poleCenterWindow->SetPosition(m_originPolePos);
		m_poleBgWindow->SetAlpha(0.75f);
		m_poleArrowWindow->SetVisible(false);
		Blockman::Instance()->m_gameSettings->m_poleForward = 0.0f;
		Blockman::Instance()->m_gameSettings->m_poleStrafe = 0.0f;
		return true;
	}

	bool gui_mainControl::onPoleTouchMove(const EventArgs & events)
	{
		m_poleArrowWindow->SetVisible(true);
		setPoleCenterPos(events);
		return true;
	}

	bool gui_mainControl::setPoleCenterPos(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->isOnAircraft() || Blockman::Instance()->m_pPlayer->m_isUseElevator || Blockman::Instance()->m_pPlayer->isOnDecoration() && !Blockman::Instance()->m_pPlayer->m_isCameraLocked)
		{
			return false;
		}
		auto mouseEventArgs = dynamic_cast<const MouseEventArgs&>(events);
		float fMaxDis = PlayerPrefs::Instance()->getFloatPrefs("PoleControlMaxDistance");
		float offX = mouseEventArgs.position.x - m_originPoleAbsPos.d_x.d_offset;
		float offY = mouseEventArgs.position.y - m_originPoleAbsPos.d_y.d_offset;
		float disSqr = offX * offX + offY * offY;
		if (disSqr > fMaxDis * fMaxDis)
		{
			float rate = Math::Sqrt(fMaxDis * fMaxDis / disSqr);
			offX = offX * rate;
			offY = offY * rate;
			disSqr = fMaxDis * fMaxDis;
		}

		auto newPos = UVector2({ 0, m_originPolePos.d_x.d_offset + offX }, { 0, m_originPolePos.d_y.d_offset + offY });
		m_poleCenterWindow->SetPosition(newPos);
		Blockman::Instance()->m_gameSettings->m_poleForward = -offY / Math::Sqrt(disSqr);
		Blockman::Instance()->m_gameSettings->m_poleStrafe = -offX / Math::Sqrt(disSqr);

		float radian = Math::ATan2(Blockman::Instance()->m_gameSettings->m_poleForward, Blockman::Instance()->m_gameSettings->m_poleStrafe);
		float angle = radian / Math::PI_DIV180 - 90.0f;
		m_poleArrowWindow->SetRotate(angle);

		return true;
	}

	bool gui_mainControl::onClickSwitchMoveState(const EventArgs & events)
	{
		Blockman::Instance()->m_gameSettings->switchMoveState();
		refreshMoveState();
		return true;
	}

	void gui_mainControl::refreshMoveState()
	{
		auto pGameSetting = Blockman::Instance()->m_gameSettings;
		MoveState curState = pGameSetting->m_curMoveState;
		m_sneakStateWindow->SetVisible(curState == MoveState::SneakState);
		m_runStateWindow->SetVisible(curState == MoveState::RunState);
		m_sprintStateWindow->SetVisible(curState == MoveState::SprintState);
		pGameSetting->m_clientMovementInput->sneakPressed = (curState == MoveState::SneakState);
	}

	void gui_mainControl::updateSkillBtnInfo(ui32 timeElapse)
	{

		if (m_skillResidueCDTime > 0)
		{
			m_skillResidueCDTime = m_skillResidueCDTime - timeElapse;
		}
		m_skillBtn->SetEnabled(m_skillResidueCDTime <= 0);
		m_skillBtn->SetText(m_skillResidueCDTime > 0 ? StringUtil::ToString(m_skillResidueCDTime / 1000 + 1).c_str() : "");
		m_skillBtn->SetTextScale(1.6f);

		switch (m_skillType)
		{
		case SkillType::DEFENSE:
			m_skillBtn->SetNormalImage("set:chicken_icon.json image:btn_defense");
			m_skillBtn->SetPushedImage("set:chicken_icon.json image:btn_defense");
			break;
		case SkillType::SPRINT:
			m_skillBtn->SetNormalImage("set:chicken_icon.json image:btn_sprint");
			m_skillBtn->SetPushedImage("set:chicken_icon.json image:btn_sprint");
			break;
		case SkillType::RELEASE_TOXIC:
			m_skillBtn->SetNormalImage("set:chicken_icon.json image:btn_poison");
			m_skillBtn->SetPushedImage("set:chicken_icon.json image:btn_poison");
			break;
		default:
			m_skillBtn->SetNormalImage("");
			m_skillBtn->SetPushedImage("");
			break;
		}
	}

	bool gui_mainControl::onClickParachuteBtn(const EventArgs & events)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (!pPlayer)
			return false;

		if (pPlayer->isParachute())
		{
			playSoundByType(ST_glide_wing);
			Blockman::Instance()->m_pPlayer->setGlide(true);
			ClientNetwork::Instance()->getSender()->sendGlideOperation(true);
			return true;
		}
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendLeaveAircraft();
		return true;
	}

	bool gui_mainControl::onClickFlyBtnDown(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying())
		{
			KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(ButtonId::JUMP), true);
			Blockman::Instance()->m_pPlayer->capabilities.isFlying = true;
			ClientNetwork::Instance()->getSender()->sendSetFlying(true);
			Blockman::Instance()->m_pPlayer->movementInput->down = false;
			Blockman::Instance()->m_pPlayer->movementInput->jump = false;
			m_buttonPressed[ButtonId::DROP] = false;
			m_buttonPressed[ButtonId::UP] = false;
			m_buttonPressed[ButtonId::DOWN] = false;
			playSoundByType(ST_open_fly);
		}
		return true;
	}

	bool gui_mainControl::onClickFlyBtnUp(const EventArgs & events)
	{
		KeyBinding::setKeyBindState(BUTTON_TO_KEY_BINDINGS.at(ButtonId::JUMP), false);
		return true;
	}

	bool gui_mainControl::onClickBuildWarBlockBtn(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendBuildWarOpenShop();
		return true;
	}

	bool gui_mainControl::onClickShowManorBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showManor();
		return true;
	}

	bool gui_mainControl::onClickShowExItemBtn(const EventArgs & events)
	{
		getWindow<GUIStaticImage>("Main-Show-RanchItem-RedPoint")->SetVisible(false);
		if (!m_ranchExItemShow)
		{
			getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(true);
			m_ranchExItemShow = true;
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(false);
			m_ranchExItemShow = false;
		}
		return true;
	}

	bool gui_mainControl::onClickChangeActorBtn(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendClickChangeActor();
		return true;
	}

	bool gui_mainControl::onClickChangeActorYawBtn(const EventArgs & events, bool left)
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			return true;
		}

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(Blockman::Instance()->renderViewEntity);
		if (!actor)
			return true;

		float offset = left ? 5.0f : -5.0f;
		Quaternion curQ = actor->GetOrientation();
		Real radian = curQ.getRadian() + offset * Math::DEG2RAD;

		// 0 or 360
		if (radian > 6.2)
		{
			radian = 0.0f;
		}
		if (radian < 0.0)
		{
			radian = 6.2f;
		}
		Blockman::Instance()->m_pPlayer->spYawRadian = radian;
		ClientNetwork::Instance()->getSender()->sendSetSpYaw(Blockman::Instance()->m_pPlayer->spYaw, Blockman::Instance()->m_pPlayer->spYawRadian);

		return true;
	}

	bool gui_mainControl::onClickChangeActorYawLongTouchStartBtn(const EventArgs & events, bool left)
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			return true;
		}

		m_hideandseekYawChange = true;
		m_hideandseekYawLeft = left;
		return true;
	}

	bool gui_mainControl::onClickChangeActorYawLongTouchEndBtn(const EventArgs & events, bool left)
	{
		if (Blockman::Instance()->m_pPlayer->getCurrCameraStatus())
		{
			return true;
		}

		m_hideandseekYawChange = false;
		m_hideandseekYawLeft = left;

		return true;
	}

	bool gui_mainControl::showHideAndSeekBtn(bool isShowChangeActoBtn, bool isShowLockCameraBtn, bool isShowOthenBtn)
	{
		m_hideAndSeekControlWindow->SetVisible(isShowChangeActoBtn || isShowLockCameraBtn || isShowOthenBtn);
		m_changeActorBtn->SetVisible(isShowChangeActoBtn);
		m_cameraChangeStatusBtn->SetVisible(isShowLockCameraBtn);

		m_hideandseekChangeYaw->SetVisible(isShowOthenBtn);
		Blockman::Instance()->m_pPlayer->spYaw = isShowOthenBtn;
		ClientNetwork::Instance()->getSender()->sendSetSpYaw(Blockman::Instance()->m_pPlayer->spYaw, Blockman::Instance()->m_pPlayer->spYawRadian);

		updateCameraBtnUI();
		return true;
	}

	bool gui_mainControl::refreshHideAndSeekBtn(const int status)
	{
		m_gameStatus = status == 1 ? GRME_ROUND_STATUS_START : GRME_ROUND_STATUS_WAIT;
		if (m_gameStatus == GRME_ROUND_STATUS_WAIT)
		{
			if (Blockman::Instance()->m_pPlayer->m_isCameraLocked)
				Blockman::Instance()->m_pPlayer->setCameraLocked(false);
			if (m_hideAndSeekControlWindow->IsVisible())
			{
				m_hideAndSeekControlWindow->SetVisible(false);
				m_changeActorBtn->SetVisible(false);
				m_cameraChangeStatusBtn->SetVisible(false);
			}
		}
		return true;
	}

	bool gui_mainControl::setPutFurnitureId(i32 furnitureId)
	{
		this->m_putFurnitureId = furnitureId;
		RootGuiLayout::Instance()->showMainControl();
		m_currentCameraView = Blockman::Instance()->m_gameSettings->getPersonView();
		Blockman::Instance()->setPersonView(1);
		return true;
	}

	bool gui_mainControl::showPlayerOperation(ui64 targetUserId, const String& name)
	{
		if (m_playerOperationLayout && m_guiPlayerOperation)
		{
			m_guiPlayerOperation->setTargetUserId(targetUserId);
			m_guiPlayerOperation->setTargetUserName(name);
			bool isEnableRemoveBtn = false;
			if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
			{
				isEnableRemoveBtn = UICommon::checkBlockCityIsInManor();
			}
			else if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockFort)
			{
				isEnableRemoveBtn = UICommon::checkBlockFortIsInManor();
			}
			m_guiPlayerOperation->setRemoveBtnEnabled(isEnableRemoveBtn);
			m_playerOperationLayout->SetVisible(true);
		}
		return true;
	}

	bool gui_mainControl::setManorBtnVisible(bool visible)
	{
		m_manorBtnVisible = visible;
		return true;
	}

	bool gui_mainControl::onClickChangeCameraStatusBtn(const EventArgs & events)
	{
		if (!m_cameraLockImage || !m_cameraUnLockImage)
			return false;

		if (Blockman::Instance()->m_pPlayer->getBaseAction() != AS_IDLE)
			return false;

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASHidden || GameClient::CGame::Instance()->GetGameType() == ClientGameType::HASChase)
		{
			if (!m_isCanChangeCameraStatus)
				return false;
			m_isCanChangeCameraStatus = false;
			m_ChangeCameraStatusCd = 0;
		}
		Blockman::Instance()->m_pPlayer->setCameraLocked(!Blockman::Instance()->m_pPlayer->getCurrCameraStatus());
		updateCameraBtnUI();

		return true;
	}

	bool gui_mainControl::onVehicleStateChanged(bool bOnoff, bool bIsDriver)
	{
		CheckGunUI();
		checkAimUI();
		CheckFishingBtn();

		m_driveControl->hide();
		if (bIsDriver && bOnoff)
		{
			m_driveControl->show();
		}
		return true;
	}

	bool gui_mainControl::hidePlayerOperationView(const EventArgs & events)
	{
		if (m_playerOperationLayout)
		{
			m_playerOperationLayout->SetVisible(false);
		}
		return true;
	}

	ui64 gui_mainControl::manorCheckPlayerLocation()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Tiny_Town)
		{
			return dynamic_cast<WorldClient*>(Blockman::Instance()->m_pWorld)->findManorByPlayerPosition(Blockman::Instance()->m_pPlayer);
		}
		return 0;
	}

	bool gui_mainControl::checkPlaceBuildingViewVisible()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher && Blockman::Instance()->m_pPlayer && Blockman::Instance()->objectMouseOver.typeOfHit == RAYTRACE_TYPE_BLOCK)
		{
			ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			return pCurrentItem && (dynamic_cast<ItemBuild*>(pCurrentItem->getItem()) || dynamic_cast<ItemHouse*>(pCurrentItem->getItem()));
		}
		return false;
	}

	void gui_mainControl::updateExpBar()
	{
		EntityPlayerSPMP* player = Blockman::Instance()->m_pPlayer;
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::Rancher || GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore)
		{
			m_playerExpProgress->SetVisible(false);
		}

		if (player)
		{
			if (player->experienceTotal != 0)
			{
				m_playerExpProgress->SetProgress(player->experience / player->experienceTotal);
			}
		}
	}

	bool gui_mainControl::onWatchRepawnClick(const EventArgs & events)
	{
		GameClient::CGame::Instance()->getNetwork()->getSender()->sendWatchRespawn();
		return true;
	}

	bool gui_mainControl::refreshBossLife(int currentBossLife, int maxBossLife)
	{
		m_bossLifeProgress->SetVisible(currentBossLife > 0);
		m_bossLifeProgress->SetProgress((float)currentBossLife / (float)maxBossLife);
		String txt = StringUtil::Format(getString(LanguageKey::GUI_BOSS_LIFE).c_str(), currentBossLife, maxBossLife);
		getWindow<GUIStaticText>("Main-Boss-Life-Info")->SetText(txt.c_str());
		return true;
	}

	bool gui_mainControl::refreshBuildProgress(const String & name, int curProgress, int maxProgress, const String & iconPath)
	{
		if (iconPath != "")
		{
			m_bossLifeIcon->SetImage(iconPath.c_str());
		}
		m_bossLifeProgress->SetVisible(curProgress > 0);
		m_bossLifeProgress->SetProgress((float)curProgress / (float)maxProgress);
		String content = getString(name).c_str();
		if (content.empty())
		{
			String txt = StringUtil::Format(getString(LanguageKey::GUI_BUILD_PROGRESS).c_str(), curProgress, maxProgress, name.c_str());
			getWindow<GUIStaticText>("Main-Boss-Life-Info")->SetText(txt.c_str());
		}
		else
		{
			String txt = StringUtil::Format(content.c_str(), curProgress);
			getWindow<GUIStaticText>("Main-Boss-Life-Info")->SetText(txt.c_str());
		}
		return true;
	}

	bool gui_mainControl::getTaskInfo(const String & info, std::vector<TaskInfo>& task_list)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_mainControl::getTaskInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("groupId") || !doc->HasMember("task_list"))
		{
			LordLogError("gui_mainControl::getTaskInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("task_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("task_id")
				|| !arr[i].HasMember("task_status")
				|| !arr[i].HasMember("task_num")
				|| !arr[i].HasMember("task_type")
				|| !arr[i].HasMember("task_des")
				|| !arr[i].HasMember("task_param")
				|| !arr[i].HasMember("task_icon"))
			{
				LordLogError("gui_toolBar::getTaskInfo The game result content player_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int task_id = arr[i].FindMember("task_id")->value.GetInt();
			int task_status = arr[i].FindMember("task_status")->value.GetInt();
			int task_num = arr[i].FindMember("task_num")->value.GetInt();
			int task_type = arr[i].FindMember("task_type")->value.GetInt();
			int task_des = arr[i].FindMember("task_des")->value.GetInt();
			int task_param = arr[i].FindMember("task_param")->value.GetInt();
			String task_icon = arr[i].FindMember("task_icon")->value.GetString();

			TaskInfo taskInfo;
			taskInfo.task_id = task_id;
			taskInfo.task_status = task_status;
			taskInfo.task_num = task_num;
			taskInfo.task_type = task_type;
			taskInfo.task_des = task_des;
			taskInfo.task_param = task_param;
			taskInfo.task_icon = task_icon;

			task_list.push_back(taskInfo);
		}

		LordDelete(doc);
		return true;
	}

	bool gui_mainControl::getRanchExItemInfo(const String & info, std::vector<RanchExItemInfo>& item_list)
	{
		rapidjson::Document* doc = new rapidjson::Document;
		doc->Parse(info.c_str());

		if (doc->HasParseError())
		{
			LordLogError("gui_mainControl::getTaskInfo HasParseError.");
			LordDelete(doc);
			return false;
		}

		if (!doc->HasMember("item_list"))
		{
			LordLogError("gui_mainControl::getTaskInfo The info result content missed some field.");
			LordDelete(doc);
			return false;
		}

		rapidjson::Value arr = doc->FindMember("item_list")->value.GetArray();
		for (size_t i = 0; i < arr.Size(); ++i)
		{
			if (!arr[i].HasMember("itemId")
				|| !arr[i].HasMember("itemNum")
				|| !arr[i].HasMember("itemImg"))
			{
				LordLogError("gui_toolBar::getRanchExItemInfo The game result content player_info missed some field. %d", i);
				LordDelete(doc);
				return false;
			}

			int itemId = arr[i].FindMember("itemId")->value.GetInt();
			int itemNum = arr[i].FindMember("itemNum")->value.GetInt();
			String itemImg = arr[i].FindMember("itemImg")->value.GetString();

			RanchExItemInfo itemInfo;
			itemInfo.item_id = itemId;
			itemInfo.item_num = itemNum;
			itemInfo.item_img = itemImg;

			item_list.push_back(itemInfo);
		}

		LordDelete(doc);
		return true;
	}

	bool gui_mainControl::showRanchExTask(bool show, String task)
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore) return true;

		if (show)
		{
			getWindow<GUILayout>("Main-RanchExTask")->SetVisible(true);
			getWindow<GUIStaticText>("Main-RanchExTitle")->SetText(getString(LanguageKey::TOOL_BAR_RANCHEX_TASK_TITLE).c_str());

			std::vector<TaskInfo> task_list;

			if (getTaskInfo(task, task_list))
			{

				for (int i = 0; i < (int)task_list.size() && i < TashInfoMax; ++i)
				{
					String lanType = LanguageManager::Instance()->getCurrentLanguage();
					String task_txt = "";

					task_txt += StringUtil::Format(getString(LanguageKey::TOOL_BAR_RANCHEX_TASK_INFO).c_str(),
						MultiLanTipSetting::getMessage(lanType, task_list[i].task_des, StringUtil::ToString(task_list[i].task_num).c_str()).c_str());

					getWindow<GUIStaticText>(StringUtil::Format("Main-RanchExTaskTxt%d", i + 1).c_str())->SetText(task_txt.c_str());

					if (task_list[i].task_icon.length() > 0)
					{
						getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetVisible(true);
						getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetImage(task_list[i].task_icon.c_str());

						if (task_list[i].task_status > 0)
						{
							getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetAlpha(0.2f);
						}
						else
						{
							getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetAlpha(1.0f);
						}
					}
					else
					{
						getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExTaskIcon%d", i + 1).c_str())->SetVisible(false);
					}
				}
			}
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExTask")->SetVisible(false);
		}

		return true;
	}

	bool gui_mainControl::showRanchExCurrentItemTip(bool show, String itemInfo, int type, int num)
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore) return true;

		if (show)
		{
			getWindow<GUILayout>("Main-RanchExCurrentItemLayout")->SetVisible(true);
			if (type == 0)
			{
				getWindow<GUIStaticText>("Main-RanchExCurrentItemTxt")->SetText(StringUtil::Format(getString(itemInfo.c_str()).c_str(), num).c_str());
				if (num <= 0)
				{
					getWindow<GUILayout>("Main-RanchExCurrentItemLayout")->SetVisible(false);
				}
			}
			else
			{
				getWindow<GUIStaticText>("Main-RanchExCurrentItemTxt")->SetText(getString(itemInfo.c_str()).c_str());
			}
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExCurrentItemLayout")->SetVisible(false);
		}

		return true;
	}

	bool gui_mainControl::showRanchExItem(bool show, String item)
	{
		if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::RanchersExplore) return true;

		if (show)
		{
			// getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(true);

			std::vector<RanchExItemInfo> item_list;

			if (getRanchExItemInfo(item, item_list))
			{
				getWindow<GUIStaticImage>("Main-Show-RanchItem-RedPoint")->SetVisible(true);
				for (int i = 0; i < (int)item_list.size() && i < MAX_RANCHEX_ITEM_COUNT; ++i)
				{
					m_RanchExItemImg[i]->SetVisible(true);
					m_RanchExItemImg[i]->SetImage(item_list[i].item_img.c_str());
					m_RanchExItemTxt[i]->SetText(StringUtil::ToString(item_list[i].item_num).c_str());
				}
			}
		}
		else
		{
			getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(false);
		}

		return true;
	}

	bool gui_mainControl::refreshBasementLife(float curLife, float maxLife)
	{
		m_basementLifeLayout->SetVisible(curLife > 0);
		float progress = curLife / maxLife;
		m_basementLifeProgress->SetProgress(progress);
		String value = StringUtil::Format("[%.0f", Math::Ceil(curLife * 100 / maxLife));
		String txt = StringUtil::Format(getString(LanguageKey::GUI_BASEMENT_LIFE).c_str(), value.c_str());
		txt += "%]";
		m_basementLifeInfo->SetText(txt.c_str());
		if (progress > 0.8f)
			m_basementLifeProgress->SetProgressImage("set:creepy_aliens.json image:basement_life_green");
		else if (progress > 0.3f)
			m_basementLifeProgress->SetProgressImage("set:creepy_aliens.json image:basement_life_yellow");
		else
			m_basementLifeProgress->SetProgressImage("set:creepy_aliens.json image:basement_life_red");
		return true;
	}

	bool gui_mainControl::onClickPersonalShopBtn(const EventArgs & events)
	{
		getParent()->showPersonalShop();
		return true;
	}

	bool gui_mainControl::onClickBuyBulletBtn(const EventArgs & events)
	{
		ClientNetwork::Instance()->getSender()->sendBuyBullet();
		return true;
	}

	void gui_mainControl::gunFireNotBulletHideOtherUi()
	{
		m_reloadBulletBtn->SetVisible(false);
	}

	void gui_mainControl::switchCrossHair(int crossHairType)
	{
		if (m_bChangeCrosshair && m_crossHairMoveSum < m_crossHairMoveRange)
		{
			m_crossHairMoveSum += m_crossHairMoveStepLength * Root::Instance()->getFrameTime() / 10;
			m_crossHairMoveSum = Math::Min(m_crossHairMoveSum, m_crossHairMoveRange);
		}
		else if (!m_bChangeCrosshair && m_crossHairMoveSum > 0.0f)
		{
			m_crossHairMoveSum -= m_crossHairMoveStepLength * Root::Instance()->getFrameTime() / 10;
			m_crossHairMoveSum = Math::Max(m_crossHairMoveSum, 0.0f);
		}
		if (m_crossHairMoveSum >= m_crossHairMoveRange)
		{
			m_bChangeCrosshair = false;
		}

		m_crossHairType = crossHairType;

		switch (crossHairType)
		{
		case RIFLE_CROSS_HAIR:
		case MACHINE_CROSS_HAIR:
			m_commonCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetVisible(true);
			m_moveLeftCrossHair->SetVisible(true);
			m_moveBottomCrossHair->SetVisible(true);
			m_moveRightCrossHair->SetVisible(true);
			m_shotgunMoveTopCrossHair->SetVisible(false);
			m_shotgunMoveLeftCrossHair->SetVisible(false);
			m_shotgunMoveBottomCrossHair->SetVisible(false);
			m_shotgunMoveRightCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetYPosition(LORD::UDim(0.0f, m_moveTopCrossHairBeginYPos - m_crossHairMoveSum));
			m_moveLeftCrossHair->SetXPosition(LORD::UDim(0.0f, m_moveLeftCrossHairBeginXPos - m_crossHairMoveSum));
			m_moveBottomCrossHair->SetYPosition(LORD::UDim(0.0f, m_moveBottomCrossHairBeginYPos + m_crossHairMoveSum));
			m_moveRightCrossHair->SetXPosition(LORD::UDim(0.0f, m_moveRightCrossHairBeginXPos + m_crossHairMoveSum));
			break;
		case COMMON_CROSS_HAIR:
			m_commonCrossHair->SetVisible(true);
			m_moveTopCrossHair->SetVisible(false);
			m_moveLeftCrossHair->SetVisible(false);
			m_moveBottomCrossHair->SetVisible(false);
			m_moveRightCrossHair->SetVisible(false);
			m_shotgunMoveTopCrossHair->SetVisible(false);
			m_shotgunMoveLeftCrossHair->SetVisible(false);
			m_shotgunMoveBottomCrossHair->SetVisible(false);
			m_shotgunMoveRightCrossHair->SetVisible(false);
			break;
		case SHOTGUN_CROSS_HAIR:
			m_commonCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetVisible(false);
			m_moveLeftCrossHair->SetVisible(false);
			m_moveBottomCrossHair->SetVisible(false);
			m_moveRightCrossHair->SetVisible(false);
			m_shotgunMoveTopCrossHair->SetVisible(true);
			m_shotgunMoveLeftCrossHair->SetVisible(true);
			m_shotgunMoveBottomCrossHair->SetVisible(true);
			m_shotgunMoveRightCrossHair->SetVisible(true);
			m_shotgunMoveTopCrossHair->SetYPosition(LORD::UDim(0.0f, m_shotgunMoveTopCrossHairBeginYPos - m_crossHairMoveSum));
			m_shotgunMoveLeftCrossHair->SetXPosition(LORD::UDim(0.0f, m_shotgunMoveLeftCrossHairBeginXPos - m_crossHairMoveSum));
			m_shotgunMoveBottomCrossHair->SetYPosition(LORD::UDim(0.0f, m_shotgunMoveBottomCrossHairBeginYPos + m_crossHairMoveSum));
			m_shotgunMoveRightCrossHair->SetXPosition(LORD::UDim(0.0f, m_shotgunMoveRightCrossHairBeginXPos + m_crossHairMoveSum));
			break;
		case NONE_TYPE:
			m_commonCrossHair->SetVisible(false);
			m_moveTopCrossHair->SetVisible(false);
			m_moveLeftCrossHair->SetVisible(false);
			m_moveBottomCrossHair->SetVisible(false);
			m_moveRightCrossHair->SetVisible(false);
			m_shotgunMoveTopCrossHair->SetVisible(false);
			m_shotgunMoveLeftCrossHair->SetVisible(false);
			m_shotgunMoveBottomCrossHair->SetVisible(false);
			m_shotgunMoveRightCrossHair->SetVisible(false);
			break;
		}
	}

	void  gui_mainControl::checkCrossHairIsCanAttack()
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (stack && pPlayer)
		{
			ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
			if (pGun)
			{
				const GunSetting* pGunSetting = pGun->getGunSetting();
				if (pGunSetting)
				{
					int crossHairType = Blockman::Instance()->m_pPlayer->getCrossHairByGunType(pGunSetting->gunType);
					const float range = pGunSetting->shootRange;
					checkDistance(range, crossHairType, pGunSetting->isAutoShoot);
				}
			}
		}
	}

	void gui_mainControl::checkFlyBtnShow()
	{
		bool sign = Blockman::Instance()->m_pPlayer->capabilities.isFlying;
		if (Blockman::Instance()->m_pPlayer->capabilities.canBeFlying() && !sign)
		{
			showFlyBtn(true);
		}
		else
		{
			showFlyBtn(false);
		}
	}

	void gui_mainControl::switchCrossHairColor(int crossHairType)
	{
		bool isRedAim = PlayerPrefs::Instance()->getBoolPrefs("IsRedAim");
		if (!isRedAim)
			return;

		switch (crossHairType)
		{
		case RIFLE_CROSS_HAIR:
		case MACHINE_CROSS_HAIR:
			m_moveTopCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_moveLeftCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_moveBottomCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_moveRightCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		case COMMON_CROSS_HAIR:
			m_commonCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		case SHOTGUN_CROSS_HAIR:
			m_shotgunMoveTopCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_shotgunMoveLeftCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_shotgunMoveBottomCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			m_shotgunMoveRightCrossHair->SetDrawColor(Color(1.0f, 0.0f, 0.0f, 1.0f));
			break;
		}
	}
	void gui_mainControl::clearCrossHairColor()
	{
		m_crossHairLayout->SetVisible(true);
		if (checkGameType(ClientGameType::BlockCity))
		{
			m_crossHairLayout->SetVisible(!m_isOpenCapture);
		}
		m_commonCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveTopCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveLeftCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveBottomCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_moveRightCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveTopCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveLeftCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveBottomCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
		m_shotgunMoveRightCrossHair->SetDrawColor(Color(1.0f, 1.0f, 1.0f, 1.0f));
	}

	float gui_mainControl::CheckCollisionCreature(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		float min_dis = range;
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		//ENTITY_CLASS_CREATURE
		Entity* tracyEntity = nullptr;
		EntityArr tracyEntityCreature = Blockman::Instance()->getWorld()->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_CREATURE, beginPos, dir, range * rangeRatio);
		for (int i = 0; i < int(tracyEntityCreature.size()); ++i)
		{
			Entity* pEntity = tracyEntityCreature[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}
		auto pCreature = dynamic_cast<EntityCreature*>(tracyEntity);
		if (pCreature)
		{
			return pCreature->isBase() ? 1000000.0f : min_dis;
		}
		return 1000000.0f;
	}

	float gui_mainControl::CheckCollisionEntityPlayer(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		float min_dis = range;
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		//ENTITY_CLASS_PLAYER
		Entity* tracyEntity = nullptr;
		EntityArr tracyEntityPlayers = Blockman::Instance()->getWorld()->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_PLAYER, beginPos, dir, range * rangeRatio);
		for (int i = 0; i < int(tracyEntityPlayers.size()); ++i)
		{
			Entity* pEntity = tracyEntityPlayers[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis && pEntity != pPlayer)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}
		bool CollisionEntityPlayer = tracyEntity != NULL;
		if (CollisionEntityPlayer)
		{
			bool singleGame = LogicSetting::Instance()->getShowGunEffectWithSingleStatus();
			if (!singleGame && dynamic_cast<EntityPlayer*>(tracyEntity)->getTeamId() == pPlayer->getTeamId())
			{
				return 1000000.0f;
			}
			return min_dis;
		}
		return 1000000.0f;
	}

	float gui_mainControl::CheckCollisionActorNpc(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		float min_dis = range;
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		//ENTITY_CLASS_ACTOR_NPC
		Entity* tracyEntity = nullptr;
		EntityArr tracyEntityActorNpc = Blockman::Instance()->getWorld()->getEntitiesWithinRayTracy(pPlayer, ENTITY_CLASS_ACTOR_NPC, beginPos, dir, range * rangeRatio);
		for (int i = 0; i < int(tracyEntityActorNpc.size()); ++i)
		{
			Entity* pEntity = tracyEntityActorNpc[i];
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= min_dis)
			{
				tracyEntity = pEntity;
				min_dis = distance;
			}
		}
		if (tracyEntity)
		{
			return min_dis;
		}
		return 1000000.0f;
	}

	float gui_mainControl::CheckCollisionBlock(Vector3 beginPos, Vector3 dir, float range, float rangeRatio)
	{
		//trace the world's block.
		Vector3 endPos = beginPos + dir * range;
		RayTraceResult trace = Blockman::Instance()->getWorld()->rayTraceBlocks(beginPos, endPos, false, true);
		if (trace.result)
		{
			return beginPos.distanceTo(trace.hitPointPos);
		}
		return 1000000.0f;
	}

	void gui_mainControl::LoadSkillReleaseBtn()
	{
		m_skillOperationWindow = getWindow("Main-Main-Skill-Operate");
		m_skillOperationWindow->SetVisible(false);
		m_skillReleaseBtn = getWindow<GUIButton>("Main-Skill-Release-btn");
		m_skillReleaseBtn->SetText("");
		m_skillReleaseBtn->SetVisible(false);
		m_skillReleaseBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onSkillReleaseBtn, this, std::placeholders::_1));
	}

	bool gui_mainControl::onSkillReleaseBtn(const EventArgs & events)
	{
		castSkill();
		return true;
	}

	bool gui_mainControl::onAutoCastSkill() 
	{
		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (pCurrentItem && pCurrentItem->isAutoCastSkill())
		{
			castSkill();
		}
		return true;
	}

	bool gui_mainControl::castSkill()
	{
		if (m_useItemStatus == NOT_CAN_USE_ITEM)
		{
			return false;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();


		if (pCurrentItem && pCurrentItem->isItemSkill())
		{
			SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(pCurrentItem->itemID, Blockman::Instance()->m_pPlayer->getOccupation());
			if (m_ItemSkillCdTime[pCurrentItem->itemID].x <= 0)
			{
				if (pSkillItem)
				{
					if (pCurrentItem->isThrowItemSkill())
					{
						Blockman::Instance()->m_pPlayer->swingItem();
					}
					else {
						UseEatSkill();
					}

					bool canCD = true;
					if (pCurrentItem->isCanFallNoUse() && !Blockman::Instance()->m_pPlayer->canPlayerFallEx())
					{
						canCD = false;
					}

					if (canCD)
					{
						int soltIndex = Blockman::Instance()->m_pPlayer->inventory->currentItemIndex;
						int skillCd = (int)(pSkillItem->ItemSkillCd * 1000);
						m_ItemSkillCdTime[pCurrentItem->itemID] = { skillCd , skillCd };
						GameClient::CGame::Instance()->getNetwork()->getSender()->sendRelaseItemSkill(pCurrentItem->itemID);

						if (pCurrentItem->isCanFallNoUse())
						{
							Blockman::Instance()->m_pPlayer->motion.y = 0;
						}
					}
					else
					{
						String lanType = LanguageManager::Instance()->getCurrentLanguage();
						String msg = MultiLanTipSetting::getMessage(lanType, pCurrentItem->getCanFallNoUseTip(), "").c_str();
						BottomSystemNotificationEvent::emit(msg, 2);
					}
				}
			}
		}
		return true;
	}

	void gui_mainControl::UseEatSkill()
	{
		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (pCurrentItem)
		{
			float		rotationPitch = Blockman::Instance()->m_pPlayer->rotationPitch;
			float		rotationYaw = Blockman::Instance()->m_pPlayer->prevRotationYaw;
			Vector3		position = Blockman::Instance()->m_pPlayer->position;
			Random& rand = Blockman::Instance()->m_pPlayer->world->m_Rand;
			for (int i = 0; i < 5; ++i)
			{
				String paritcleName = StringUtil::Format("iconcrack_%d", pCurrentItem->getItem()->itemID);
			
				for (int i = 0; i < 5; ++i)
				{
					Vector3 var4((rand.nextFloat() - 0.5f) * 0.1f, Math::UnitRandom() * 0.1f + 0.1f, 0.0f);
					var4.rotateAroundX(-rotationPitch * Math::DEG2RAD);
					var4.rotateAroundY(-rotationYaw * Math::DEG2RAD);
					var4.y += 0.05f;
					Vector3 var5((rand.nextFloat() - 0.5f) * 0.3f, (-rand.nextFloat()) * 0.6f - 0.3f, 0.6f);
					var5.rotateAroundX(-rotationPitch * Math::DEG2RAD);
					var5.rotateAroundY(-rotationYaw * Math::DEG2RAD);
					var5 = var5 + position;
					var5.y += Blockman::Instance()->m_pPlayer->getEyeHeight();
					Blockman::Instance()->m_pPlayer->world->spawnParticle(paritcleName, var5, var4);
				}
			}
		}
	}

	void gui_mainControl::HideSkillUI()
	{
		m_skillOperationWindow->SetVisible(false);
		m_skillReleaseBtn->SetVisible(false);
	}

	void  gui_mainControl::checkHideReleaseSkillBtnUI()
	{
		if (!m_skillOperationWindow || !m_skillReleaseBtn)
		{
			return;
		}
		m_skillOperationWindow->SetVisible(true);
		m_skillReleaseBtn->SetVisible(true);
		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			HideSkillUI();
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			HideSkillUI();
			return;
		}
		
		if (!pCurrentItem->isItemSkill() || pCurrentItem->isAutoCastSkill() || pCurrentItem->isLongTouch())
		{
			HideSkillUI();
			return;
		}

		if (m_throwPotLayout && m_throwPotLayout->IsVisible()) // small skill button
		{
			HideSkillUI();
			return;
		}

	}

	void gui_mainControl::updateItemSkillBtnInfo(ui32 timeElapse)
	{
		map<int, Vector2i>::iterator m_it;
		for (int i = 0; i < 9; ++i)
		{
			ItemStackPtr ItemStack = Blockman::Instance()->m_pPlayer->inventory->getItemIdBySlot(i);
			if (ItemStack && ItemStack->isItemSkill())
			{
				if (m_ItemSkillCdTime.find(ItemStack->itemID) == m_ItemSkillCdTime.end())
				{
					m_ItemSkillCdTime[ItemStack->itemID] = { 0, 0 };
				}
			}
		}
		GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
		for (m_it = m_ItemSkillCdTime.begin(); m_it != m_ItemSkillCdTime.end(); m_it++)
		{
			if (m_it->first > 0 && m_it->second.x > 0 && m_it->second.y > 0)
			{
				m_it->second.x = m_it->second.x - timeElapse;
			}
			int nSlot = Blockman::Instance()->m_pPlayer->inventory->getItemSlotByHotbat(m_it->first);
			if (m_it->second.x <= 0)
			{
				m_it->second.x = 0;
				if (slotTable)
				{
					if (nSlot >= 0)
						slotTable->setItemSkillCdProgress(nSlot, 0, 0);
				}
			}
			else
			{
				if (slotTable)
				{
					if (nSlot >= 0)
						slotTable->setItemSkillCdProgress(nSlot, m_it->second.y - m_it->second.x, m_it->second.y);
				}
			}
		}		
	}

	void gui_mainControl::refreshItemSkillShowUi()
	{
		int soltIndex = Blockman::Instance()->m_pPlayer->inventory->currentItemIndex;
		GuiSlotTable* slotTable = dynamic_cast<GuiSlotTable*>(getWindow("Main-VisibleBar-SlotTable"));
		if (slotTable)
			slotTable->setItemSkillCdProgress(soltIndex, 0, 0);
	}

	void gui_mainControl::checkSkillIsHaveAttackEntity()
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (pPlayer)
		{
			ItemStackPtr stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
			if (stack && stack->isThrowItemSkill() && stack->isShowCrossHair())
			{
				SkillItem* pSkillItem = LogicSetting::Instance()->getSkillItem(stack->itemID, Blockman::Instance()->m_pPlayer->getOccupation());
				if (pSkillItem)
				{
					int crossHairType = RIFLE_CROSS_HAIR;
					const float range = pSkillItem->ItemSkillDistace;
					checkDistance(range, crossHairType, false);
				}
			}
		}
	}

	void gui_mainControl::updateMaskInfo(ui32 nTimeElapse)
	{
		auto player = Blockman::Instance()->m_pPlayer;

		if (m_frozenMask && player)
		{
			m_frozenMask->SetVisible(player->isFrozen());
		}

		if (m_hurtMask && player)
		{
			m_hurtMask->SetVisible(player->isHurted());
		}
	}

	void gui_mainControl::checkDistance(float Distace, int crossHairType, bool isAutoShoot)
	{
		auto pPlayer = Blockman::Instance()->m_pPlayer;
		if (pPlayer)
		{
			Vector3 beginPos = pPlayer->position;
			beginPos.y += pPlayer->getEyeHeight();
			beginPos += pPlayer->m_thirdPersonCameraOffset;
			float yaw = pPlayer->rotationYaw;
			float pitch = pPlayer->rotationPitch;
			Vector3 dir;
			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			Vector3 endPos = beginPos + dir * Distace;
			float rangeRatio = 1.0f;
			
			float BlockDistance = CheckCollisionBlock(beginPos, dir, Distace, rangeRatio);
			float ActorNpcDistance = CheckCollisionActorNpc(beginPos, dir, Distace, rangeRatio);
			float CreatureDistance = CheckCollisionCreature(beginPos, dir, Distace, rangeRatio);
			float PlayerDistance = CheckCollisionEntityPlayer(beginPos, dir, Distace, rangeRatio);
			
			bool CollisionBlock = BlockDistance < 999999.0f;
			bool CollisionActorNpc = ActorNpcDistance < 999999.0f;
			bool CollisionCreature = CreatureDistance < 999999.0f;
			bool CollisionEntityPlayer = PlayerDistance < 999999.0f;

			float MinDistance = Math::Min(BlockDistance, ActorNpcDistance);
			MinDistance = Math::Min(MinDistance, CreatureDistance);
			MinDistance = Math::Min(MinDistance, PlayerDistance);

			if (CollisionBlock)
			{
				bool isRedAimBlock = PlayerPrefs::Instance()->getBoolPrefs("IsRedAimBlock");
				if (checkGameType(ClientGameType::BlockCity))
				{
					isRedAimBlock &= !m_isOpenCapture;
				}
				if (isRedAimBlock)
				{
					switchCrossHairColor(crossHairType);
					switchCrossHair(crossHairType);
				}
				else
				{
					clearCrossHairColor();
					switchCrossHair(crossHairType);
				}
				if (MinDistance == BlockDistance)
				{
					return;
				}
			}

			if (CollisionActorNpc)
			{
				bool isRedAimActor = PlayerPrefs::Instance()->getBoolPrefs("IsRedAimActor");
				if (isRedAimActor)
				{
					switchCrossHairColor(crossHairType);
					switchCrossHair(crossHairType);
				}
				else
				{
					clearCrossHairColor();
					switchCrossHair(crossHairType);
				}
				if (MinDistance == ActorNpcDistance)
				{
					return;
				}
			}
			
			if (CollisionCreature || CollisionEntityPlayer)
			{
				switchCrossHairColor(crossHairType);
				switchCrossHair(crossHairType);
			}
			else
			{
				clearCrossHairColor();
				switchCrossHair(crossHairType);
			}

			bool enableAutoShoot = LogicSetting::Instance()->getAutoShootEnable();
			if (enableAutoShoot && isAutoShoot &&
                ((CollisionEntityPlayer && MinDistance == PlayerDistance) || (CollisionCreature && MinDistance == CreatureDistance)))
			{
				tryFireGun(true);
			}
		}
	}

	bool gui_mainControl::setGameStatus(const int status)
	{
		m_gameStatus = status == 1 ? GRME_ROUND_STATUS_START : GRME_ROUND_STATUS_WAIT;
		if (m_gameStatus == GRME_ROUND_STATUS_WAIT)
		{
			ForceAutoRun();
			m_ItemSkillCdTime.clear();
			updateItemSkillBtnInfo(0);
		}
		return true;
	}

	bool gui_mainControl::LoadThrowPotUi()
	{
		m_throwPotLayout = getWindow<GUILayout>("Main-throwpot-Controls");
		m_autoRunBtnFalseStatus = getWindow("Main-AutoRun-False-Btn");
		m_autoRunBtnTrueStatus = getWindow("Main-AutoRun-True-Btn");
		m_attackBtn = getWindow<GUIButton>("Main-Attack-Btn");

		m_throwPotLayout->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::Throw_Pot);
		m_autoRunBtnFalseStatus->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onAutoRunBtn, this, std::placeholders::_1));
		m_autoRunBtnTrueStatus->subscribeEvent(EventWindowClick, std::bind(&gui_mainControl::onCloseAutoRunBtn, this, std::placeholders::_1));
		m_attackBtn->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onAttackBtn, this, std::placeholders::_1));
		m_autoRunBtnTrueStatus->SetVisible(false);
		m_autoRunBtnFalseStatus->SetVisible(true);
		return true;
	}

	bool gui_mainControl::onAutoRunBtn(const EventArgs & events)
	{
		//LordLogInfo("onAutoRunClick");
		m_autoRunBtnTrueStatus->SetVisible(true);
		m_autoRunBtnFalseStatus->SetVisible(false);
		Blockman::Instance()->m_pPlayer->setSprinting(true);
		Blockman::Instance()->m_gameSettings->setAutoRunStatus(true);
		return true;
	}

	bool gui_mainControl::onCloseAutoRunBtn(const EventArgs & events)
	{
		//LordLogInfo("onCloseAutoRunClick");
		m_autoRunBtnTrueStatus->SetVisible(false);
		m_autoRunBtnFalseStatus->SetVisible(true);
		Blockman::Instance()->m_pPlayer->setSprinting(false);
		Blockman::Instance()->m_gameSettings->setAutoRunStatus(false);
		return true;
	}

	bool gui_mainControl::onAttackBtn(const EventArgs & events)
	{
		if (!Blockman::Instance()->m_pPlayer)
			return false;

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (pCurrentItem && pCurrentItem->isItemSkill())
		{
			GameClient::CGame::Instance()->getNetwork()->getSender()->sendRelaseItemSkill(pCurrentItem->itemID);
		}
		else
		{
			Blockman::Instance()->ManualAttack();
		}
		return true;
	}

	void gui_mainControl::ForceAutoRun()
	{
		if (Blockman::Instance()->m_gameSettings->getAutoRunStatus())
		{
			m_autoRunBtnTrueStatus->SetVisible(false);
			m_autoRunBtnFalseStatus->SetVisible(true);
			Blockman::Instance()->m_pPlayer->setSprinting(false);
			Blockman::Instance()->m_gameSettings->setAutoRunStatus(false);
		}
	}

	void gui_mainControl::updateEndingAnimation()
	{
		float frameTime = (float)Root::Instance()->getFrameTime();
		m_endingAnimationRotatingTime += frameTime;

		if (m_endingAnimationRotatingTime > 5000)
		{
			m_playEndingAnimation = false;
			m_endingStatus->SetVisible(false);
			ReceiveFinalSummaryEvent::emit(m_gameResult, m_isNextServer);
		}
		else if (m_endingAnimationRotatingTime > 2000.0f)
		{
			m_endingStatus->SetVisible(true);

			if (m_isZoomInEndingImg && m_endingAnimationScale < 1.2f)
			{
				m_endingAnimationScale += frameTime * 0.005f;
			}
			else if (!m_isZoomInEndingImg && m_endingAnimationScale > 1.0f)
			{
				m_endingAnimationScale -= frameTime * 0.005f;
			}

			if (m_isZoomInEndingImg && m_endingAnimationScale >= 1.2f)
			{
				m_isZoomInEndingImg = false;
				m_endingAnimationScale = 1.2f;
			}
			else if (!m_isZoomInEndingImg && m_endingAnimationScale < 1.0f)
			{
				m_endingAnimationScale = 1.0f;
			}

			m_endingStatus->SetScale(Vector3(m_endingAnimationScale, m_endingAnimationScale, m_endingAnimationScale));
		}
	}

	bool gui_mainControl::onClickFishingBtn(const EventArgs &)
	{
		if (!Blockman::Instance()->m_pPlayer->inventory)
			return false;

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();

		if (pCurrentItem)
		{
			if (pCurrentItem->itemID == Item::fishingRod->itemID)
			{
				pCurrentItem->useItemRightClick(Blockman::Instance()->m_pWorld, Blockman::Instance()->m_pPlayer);
				return true;
			}
		}

		return false;
	}

	void gui_mainControl::CheckFishingBtn()
	{
		if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->inventory)
		{
			return;
		}

		if (Blockman::Instance()->m_pPlayer->isOnVehicle())
		{
			m_btnFishing->SetVisible(false);
			return;
		}

		ItemStackPtr pCurrentItem = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		if (!pCurrentItem)
		{
			m_btnFishing->SetVisible(false);
			return;
		}

		ItemFishingRod* pFishingRod = dynamic_cast<ItemFishingRod*>(pCurrentItem->getItem());
		if (!pFishingRod)
		{
			m_btnFishing->SetVisible(false);
			return;
		}
		m_btnFishing->SetVisible(true);
	}

	bool gui_mainControl::onClickCannonBtn(const EventArgs &)
	{
		EntityActorCannonClient* pCannon = dynamic_cast<EntityActorCannonClient*>(Blockman::Instance()->getWorld()->getEntity(m_cannonEntityId));
		if (!pCannon)
		{
			return false;
		}

		pCannon->m_hasBeenLaunched = true;
		ClientNetwork::Instance()->getSender()->sendUseCannon(m_cannonEntityId);
		SoundSystem::Instance()->playEffectByType(ST_Cannon);
		return true;
	}

	bool gui_mainControl::showCannonBtn(bool isShow, int cannonEntityId)
	{
		m_btnCannon->SetVisible(isShow);
		m_cannonEntityId = cannonEntityId;
		return true;
	}

	bool gui_mainControl::hideBirdActivity()
	{
		m_btnBirdActivity->SetVisible(false);
		return true;
	}

	bool gui_mainControl::onClickQuicklyBuildBlockBtn(const EventArgs & events)
	{
		onClickQuicklyBuildBlockBtnImpl(true);
		return true;
	}

	bool gui_mainControl::onClickQuicklyBuildBlockBtnImpl(bool click_btn)
	{
		if (!Blockman::Instance()->m_pPlayer)
		{
			return true;
		}

		if (click_btn)
		{
			if (!Blockman::Instance()->m_pPlayer->m_canBuildBlockQuickly)
			{
				getParent()->showQuicklyBuildBlockTip();
			}
			else
			{
				setQuicklyBuildBlockBtn(!Blockman::Instance()->m_pPlayer->m_quicklyBuildBlock);
			}
		}
		else
		{
			if (!Blockman::Instance()->m_pPlayer->m_canBuildBlockQuickly)
			{
				getParent()->showQuicklyBuildBlockTip();
			}
		}

		return true;
	}

	void gui_mainControl::setQuicklyBuildBlockBtn(bool status)
	{
		if (checkGameType(ClientGameType::SkyWarLuckyBlock))
		{
			m_btnQuicklyBuildBlock->SetVisible(Blockman::Instance()->m_pPlayer->m_canBuildBlockQuickly);
		}

		if (Blockman::Instance()->m_pPlayer->m_canBuildBlockQuickly && status)
		{
			Blockman::Instance()->m_pPlayer->m_quicklyBuildBlock = true;
			m_btnQuicklyBuildBlock->SetNormalImage("set:bedwarsmall.json image:quick_block_open");
			m_btnQuicklyBuildBlock->SetPushedImage("set:bedwarsmall.json image:quick_block_open");
			String lanType = LanguageManager::Instance()->getCurrentLanguage();
			String msg = MultiLanTipSetting::getMessage(lanType, 1008025, "").c_str();
			BottomSystemNotificationEvent::emit(msg, 2);
		}
		else
		{
			Blockman::Instance()->m_pPlayer->m_quicklyBuildBlock = false;
			m_btnQuicklyBuildBlock->SetNormalImage("set:bedwarsmall.json image:quick_block_close");
			m_btnQuicklyBuildBlock->SetPushedImage("set:bedwarsmall.json image:quick_block_close");
			String lanType = LanguageManager::Instance()->getCurrentLanguage();
			String msg = MultiLanTipSetting::getMessage(lanType, 1008026, "").c_str();
			BottomSystemNotificationEvent::emit(msg, 2);
		}
	}

	void gui_mainControl::LoadRanchExUI()
	{
		getWindow<GUILayout>("Main-RanchExItemLayout")->SetVisible(false);
		getWindow<GUIButton>("MainControl-Show-RanchItem")->SetVisible(GameClient::CGame::Instance()->GetGameType() == ClientGameType::RanchersExplore);
		getWindow<GUIStaticImage>("Main-Show-RanchItem-RedPoint")->SetVisible(false);
		getWindow<GUIButton>("MainControl-Show-RanchItem")->subscribeEvent(EventButtonClick, std::bind(&gui_mainControl::onClickShowExItemBtn, this, std::placeholders::_1));

		for (int i = 0; i < MAX_RANCHEX_ITEM_COUNT; ++i)
		{
			m_RanchExItemImg[i] = getWindow<GUIStaticImage>(StringUtil::Format("Main-RanchExItemImg%d", i + 1).c_str());
			m_RanchExItemTxt[i] = getWindow<GUIStaticText>(StringUtil::Format("Main-RanchExItemTxt%d", i + 1).c_str());
			m_RanchExItemImg[i]->SetVisible(false);
		}
	}

	void gui_mainControl::checkOpenFlyingShow()
	{
		m_btnOpenFlying->SetVisible(UICommon::checkOpenFlying());
	}

	bool gui_mainControl::onClickOpenFlying(const EventArgs & args)
	{
		if (Blockman::Instance()->m_pPlayer)
		{
			bool sign = Blockman::Instance()->m_pPlayer->capabilities.isFlying;
			ClientNetwork::Instance()->getSender()->sendSetFlying(!sign);
		}
		return true;
	}

	bool gui_mainControl::onPlaceBuildingClick(const EventArgs & args)
	{
		if (Blockman::Instance())
		{
			Blockman::Instance()->ManualAttack();
		}
		return true;
	}
	
	void gui_mainControl::updateHideAndSeekYawChange(ui32 nTimeElapse)
	{
		if (!m_hideandseekYawChange) return;

		ActorObject* actor = EntityRenderManager::Instance()->getEntityActor(Blockman::Instance()->renderViewEntity);
		if (!actor)
			return;

		float offset = m_hideandseekYawLeft ? 1.5f : -1.5f;
		Quaternion curQ = actor->GetOrientation();
		Real radian = curQ.getRadian() + offset * Math::DEG2RAD;
		// LordLogInfo("updateHideAndSeekYawChange offset: %f", offset);

		// 0 or 360
		if (radian > 6.2)
		{
			radian = 0.0f;
		}
		if (radian < 0.0)
		{
			radian = 6.2f;
		}
		Blockman::Instance()->m_pPlayer->spYawRadian = radian;
		ClientNetwork::Instance()->getSender()->sendSetSpYaw(Blockman::Instance()->m_pPlayer->spYaw, Blockman::Instance()->m_pPlayer->spYawRadian);
	}

	bool gui_mainControl::onBirdAddScore(i32 score, i32 scoreType)
	{
		if (isShown())
		{
			static int viewCount = 0;
			String viewName = StringUtil::Format("Main-BirdAddScore-%d", viewCount++);
			GUIStaticText* itemView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, viewName.c_str());
			itemView->SetText(StringUtil::Format(scoreType > 0 ? "+%d" : "-%d", score).c_str());
			itemView->SetSize(UVector2(UDim(0, 50.f), UDim(0, 25.f)));
			itemView->SetTextHorzAlign(HA_CENTRE);
			itemView->SetTextVertAlign(VA_CENTRE);
			itemView->SetTextBoader(Color::BLACK);
			switch (scoreType)
			{
			case -1:
				itemView->SetTextColor(Color::RED);
				break;
			case 1:
				itemView->SetTextColor(Color::RED);
				break;
			case 2:
				itemView->SetTextColor(Color::YELLOW);
				break;
			case 3:
				itemView->SetTextColor(Color::BLUE);
				break;
			case 4:
				itemView->SetTextColor(Color::GREEN);
				break;
			}
			itemView->SetFont(GUIFontManager::Instance()->GetFont("HT16"));
			itemView->SetHorizontalAlignment(HA_CENTRE);
			m_birdScoreViews.push_back(BirdScoreView(itemView, scoreType));
			m_rootWindow->AddChildWindow(itemView);
		}
		return true;
	}

	void gui_mainControl::onBirdDataUpdate(ui32 nTimeElapse)
	{

		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator ||
			GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			bool hasTip = false;
			int index = 0;
			float widthOffset = 0;

			for (auto& item : m_birdScoreViews)
			{
				if (item.timeLeft <= 0 && item.itemView)
				{
					m_rootWindow->RemoveChildWindow(item.itemView);
					GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
					item.itemView = nullptr;
				}
				else
				{
					if (widthOffset < 200.f)
					{
						widthOffset += 15.f;
					}
				}
			}

			for (auto& item : m_birdScoreViews)
			{
				if (item.itemView)
				{
					item.timeLeft -= nTimeElapse;
					if (item.timeLeft > 0)
					{
						float scale = (1000 - item.timeLeft) / 1000.f;
						float h = 0.5f - 0.25f * scale;
						item.itemView->SetTextScale(0.5f + scale);
						if (item.type == -1)
						{
							item.itemView->SetPosition(UVector2(UDim(0, 0), UDim(h, 0)));
						}
						else
						{
							item.itemView->SetPosition(UVector2(UDim(0, 30.f *  (index % 13)- widthOffset), UDim(h, index % 2 * 50.f  + index )));
						}
					
						index++;
						hasTip = true;
					}
					else
					{
						item.itemView->SetVisible(false);
					}
				}
			}

			if (!hasTip)
			{
				for (auto& item : m_birdScoreViews)
				{
					if (item.itemView)
					{
						m_rootWindow->RemoveChildWindow(item.itemView);
						GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
						item.itemView = nullptr;
					}
				}
				m_birdScoreViews.clear();
			}

			getWindow("Main-BirdTask-RedDot")->SetVisible(RedDotManager::checkBirdTask());
			getWindow("Main-Bird-Guide-Pack")->SetVisible(m_isShowBirdPackGuide);

			if (UICommon::checkPlayerParam())
			{
				auto pPlayer = Blockman::Instance()->m_pPlayer;
				m_glideCd -= nTimeElapse;
				m_glideCd = m_glideCd > 0 ? m_glideCd : 0;
				m_cbGlide->SetEnabled(m_glideCd == 0);
				m_cbGlide->SetVisible(!m_buttonPressed[ButtonId::JUMP] && !pPlayer->onGround && pPlayer->hasFlyWing() && ((checkGameType(ClientGameType::BlockCity) ? (!pPlayer->isOnLadder() && !pPlayer->isInWater()) : true)));
				m_cbGlide->SetCheckedNoEvent(Blockman::Instance()->m_pPlayer->isGlide());
				m_buttons[ButtonId::JUMP]->SetVisible(!m_cbGlide->IsVisible());
			}
		}
	}
	
	bool gui_mainControl::onClickBirdHelpBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdHelp();
		return true;
	}

	bool gui_mainControl::onClickBirdPackBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdPackAndFuse();
		if (m_isShowBirdPackGuide)
		{
			m_isShowBirdPackGuide = false;
		}
		return true;
	}

	bool gui_mainControl::onClickCheckinBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showCheckin();
		return true;
	}

	bool gui_mainControl::onClickBirdPersonalShopBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdPersonShop();
		return true;
	}

	bool gui_mainControl::onClickBirdAtlasBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdAtlas();
		return true;
	}

	bool gui_mainControl::onClickBirdActivityBtn(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBirdActivity();
		return true;
	}

	bool gui_mainControl::onHideBirdTaskClick(const EventArgs & events)
	{
		m_isShowBirdTask = false;
		m_birdTaskViewLoadTime = 300;
		return true;
	}

	bool gui_mainControl::onShowBirdTaskClick(const EventArgs & events)
	{
		m_isShowBirdTask = true;
		m_birdTaskViewLoadTime = 300;
		onBirdTaskChange();
		return true;
	}

	bool gui_mainControl::onBirdTaskChange()
	{
		if (!UICommon::checkBirdParam() || !m_isShowBirdTask)
		{
			return false;
		}

		const auto& tasks = Blockman::Instance()->m_pPlayer->m_birdSimulator->getTasks();
		float  oldOffset = tasks.size() > 0 ? m_birdTaskList->GetScrollOffset() : 0;
		m_birdTaskList->ClearAllItem();
		static int taskCount = 0;
		for (auto& task : tasks)
		{
			String iterLayout = StringUtil::Format("Main-Bird-Task-List-Item-%d", taskCount++).c_str();
			GuiBirdTaskItem* titleItem = (GuiBirdTaskItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_TASK_ITEM, iterLayout.c_str());
			titleItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.f, 0), UDim(0.0f, 28.f));
			titleItem->setTitle(getString(task.taskName).c_str());
			m_birdTaskList->AddItem(titleItem, false);
			for (auto& item : task.contents)
			{
				String itemLayout = StringUtil::Format("Main-Bird-Task-List-Item-%d", taskCount++).c_str();
				GuiBirdTaskItem* infoItem = (GuiBirdTaskItem*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_BIRD_TASK_ITEM, itemLayout.c_str());
				infoItem->SetArea(UDim(0, 0), UDim(0, 0.0f), UDim(1.f, 0), UDim(0.0f, 46.f));
				infoItem->setTask(task.taskId, item.id);
				m_birdTaskList->AddItem(infoItem, false);
			}
		}
		m_birdTaskList->SetScrollOffset(oldOffset);
		return true;
	}

	void gui_mainControl::birdTaskUpdate(i32 nTimeElapse)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			if (m_birdTaskViewLoadTime > 0)
			{
				m_birdTaskViewLoadTime -= nTimeElapse;
				m_birdTaskViewLoadTime = m_birdTaskViewLoadTime > 0 ? m_birdTaskViewLoadTime : 0;
			}

			if (m_isShowBirdTask)
			{
				float with = 258.f * (1.f - m_birdTaskViewLoadTime / 300.f);
				m_birdTaskLayout->SetWidth(UDim(0, with));
				m_birdTaskLayout->SetVisible(true);
				m_btnHideTask->SetVisible(true);
				m_btnShowTask->SetVisible(false);
			}
			else
			{
				float with = 258.f * m_birdTaskViewLoadTime / 300.f;
				m_birdTaskLayout->SetWidth(UDim(0, with));
				m_birdTaskLayout->SetVisible(m_birdTaskViewLoadTime > 0);
				m_btnHideTask->SetVisible(m_birdTaskViewLoadTime > 0);
				m_btnShowTask->SetVisible(true);
			}
		}
		else
		{
			m_birdTaskLayout->SetVisible(false);
			m_btnHideTask->SetVisible(false);
			m_btnShowTask->SetVisible(false);
		}
	}

	bool gui_mainControl::onBirdAddMessage(const String& message)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			if (m_birdMessage.size() >= 8)
			{
				auto iter = m_birdMessage.begin();
				m_birdMessage.erase(iter);
				m_birdMessageShowTime = 0;
			}
			String newStr = StringUtil::Replace(message, "*100", "");
			m_birdMessage.push_back(newStr);
			m_isRefreshBirdMsg = true;
		}
		return true;
	}

	void gui_mainControl::onBirdMessageUpdate(ui32 nTimeElapse)
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BirdSimulator)
		{
			if (m_birdMessageShowTime > 0)
			{
				m_birdMessageShowTime = (m_birdMessageShowTime - nTimeElapse) > 0 ? (m_birdMessageShowTime - nTimeElapse) : 0;
				if (m_birdMessageShowTime <= 0)
				{
					m_birdMsgList->SetVisible(false);
					m_birdTakeOutMsg->SetVisible(true);
				}
			}
			else
			{
				if (m_isRefreshBirdMsg)
				{
					m_birdMsgList->SetVisible(true);
					m_birdTakeOutMsg->SetVisible(false);
					onShowBirdMsg(m_birdMessage);
					m_birdMessageShowTime = 5000;
					m_isRefreshBirdMsg = false;
				}
			}
		}
		else
		{
			m_birdMsgList->SetVisible(false);
			m_birdTakeOutMsg->SetVisible(false);
		}
	}

	void gui_mainControl::onShowBirdMsg(std::vector<String>& msgs)
	{
		m_birdMsgList->ClearAllItem();
		static int msgCount = 0;
		for (auto& msg : msgs)
		{
			String msgName = StringUtil::Format("Main-Bird-Message-Item-%d", msgCount++).c_str();
			GUIStaticText* msgItem = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, msgName.c_str());
			msgItem->SetArea(UDim(0, 0.0f), UDim(0, 0.0f), UDim(1.f, 0.f), UDim(0.0f, 30.f));
			msgItem->SetText(msg.c_str());
			msgItem->SetTouchable(false);
			msgItem->SetTextVertAlign(VA_CENTRE);
			m_birdMsgList->AddItem(msgItem);
		}
	}

	bool gui_mainControl::onShowBirdStoreMsg()
	{
		m_isRefreshBirdMsg = true;
		return true;
	}

	bool gui_mainControl::onShowBirdPackGuide()
	{
		m_isShowBirdPackGuide = true;
		return true;
	}

	bool gui_mainControl::onClick(const EventArgs & args, ViewId viewId)
	{
		if (m_needRotateId == 0 || m_needType == 0)
		{
			getWindow("Main-Rotate")->SetVisible(false);
			return false;
		}
		switch (viewId)
		{
		case ViewId::BTN_ROTATE_MAIN:
			if (checkGameType(ClientGameType::BlockFort))
			{
				break;
			}
			m_needType = 0;
			m_needRotateId = 0;
			getWindow("Main-Rotate")->SetVisible(false);
			playSoundByType(ST_rotate_done);
			break;
		case ViewId::BTN_ROTATE_LEFT:
			playSoundByType(ST_rotate_click);
			ClientNetwork::Instance()->getCommonSender()->sendRotateOperation(m_needType, m_needRotateId, 1);
			break;
		case ViewId::BTN_ROTATE_RIGHT:
			playSoundByType(ST_rotate_click);
			ClientNetwork::Instance()->getCommonSender()->sendRotateOperation(m_needType, m_needRotateId, 2);
			break;
		case ViewId::BTN_ROTATE_SURE:
			if (!checkGameType(ClientGameType::BlockFort))
			{
				break;
			}
			m_needType = 0;
			m_needRotateId = 0;
			getWindow("Main-Rotate")->SetVisible(false);
			playSoundByType(ST_rotate_done);
			break;
		}
		return true;
	}

	bool gui_mainControl::onAttackScore(i32 score, i32 scoreType)
	{
		if (isShown())
		{
			static int attackViewCount = 0;
			String viewName = StringUtil::Format("Main-AttackScore-%d", attackViewCount++);
			GUIStaticText* itemView = (GUIStaticText*)GUIWindowManager::Instance()->CreateGUIWindow(GWT_STATIC_TEXT, viewName.c_str());
			itemView->SetSize(UVector2(UDim(0, 50.f), UDim(0, 25.f)));
			itemView->SetTextHorzAlign(HA_CENTRE);
			itemView->SetTextVertAlign(VA_CENTRE);
			itemView->SetTextBoader(Color::BLACK);
			switch (scoreType)
			{
			case AST_CRIT:
				itemView->SetText(getString(LanguageKey::GUI_ATTACK_CRIT).c_str());
				itemView->SetTextColor(Color::RED);
				break;
			default:
				itemView->SetText(StringUtil::Format(score > 0 ? "+%d" : "-%d", score).c_str());
				itemView->SetTextColor(Color::RED);
				break;
			}
			itemView->SetFont(GUIFontManager::Instance()->GetFont("HT24"));
			itemView->SetHorizontalAlignment(HA_CENTRE);
			m_attackScoreView.push_back(AttackScoreView(itemView, scoreType));
			m_rootWindow->AddChildWindow(itemView);
		}
		return true;
	}

	bool gui_mainControl::showRotateView(i32 type, i32 id)
	{
		m_needType = type;
		m_needRotateId = id;
		getWindow("Main-Rotate")->SetVisible(true);
		getWindow("Main-Rotate-Sure")->SetVisible(checkGameType(ClientGameType::BlockFort));
		return true;
	}

	void gui_mainControl::onAttackUpdate(ui32 nTimeElapse)
	{
		bool hasTip = false;
		int index = 0;
		float widthOffset = 0;

		for (auto& item : m_attackScoreView)
		{
			if (item.timeLeft <= 0 && item.itemView)
			{
				m_rootWindow->RemoveChildWindow(item.itemView);
				GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
				item.itemView = nullptr;
			}
			else
			{
				if (widthOffset < 200.f)
				{
					widthOffset += 30.f;
				}
			}
		}

		for (auto& item : m_attackScoreView)
		{
			if (item.itemView)
			{
				item.timeLeft -= nTimeElapse;
				if (item.timeLeft > 0)
				{
					float scale = (200 - item.timeLeft) / 200.f;
					float h = 0.5f - 0.25f * scale;
					item.itemView->SetTextScale(0.5f + scale);

					switch (item.type)
					{
					case AST_CRIT:
						item.itemView->SetPosition(UVector2(UDim(0, 30.f *  (index % 13) - widthOffset), UDim(h, index % 2 * 50.f + index)));
						break;
					default:
						item.itemView->SetPosition(UVector2(UDim(0, 0), UDim(h, 0)));
						break;
					}

					index++;
					hasTip = true;
				}
				else
				{
					item.itemView->SetVisible(false);
				}
			}
		}

		if (!hasTip)
		{
			for (auto& item : m_attackScoreView)
			{
				if (item.itemView)
				{
					m_rootWindow->RemoveChildWindow(item.itemView);
					GUIWindowManager::Instance()->DestroyGUIWindow(item.itemView);
					item.itemView = nullptr;
				}
			}
			m_attackScoreView.clear();
		}
	}

	void gui_mainControl::showPixelGunSlot(bool show)
	{
		auto&& window = getWindow<GUILayout>("Main-PixelGun");
		NULL_RETURN_VOID(window);
		window->SetVisible(show);

		if (!show)
		{
			return;
		}

		updateSchematicData(false);

		/*auto&& stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		NULL_RETURN_VOID(stack);*/

		/*auto&& ItemBlock = dynamic_cast<BLOCKMAN::ItemBlock*>(stack->getItem());
		NULL_RETURN_VOID(ItemBlock);*/
	}

	void gui_mainControl::updateSchematicData(bool resetChecked/* = true*/)
	{
		if (resetChecked)
		{
			PlayerPrefs::Instance()->SetProperty<int>("SchematicId", 0);
		}

		auto&& stack = Blockman::Instance()->m_pPlayer->inventory->getCurrentItem();
		NULL_RETURN_VOID(stack);

		auto&& ItemBlock = dynamic_cast<BLOCKMAN::ItemBlock*>(stack->getItem());
		NULL_RETURN_VOID(ItemBlock);

		for (int i = 1; i <= PixelGunSlotNum; ++i)
		{
			String slot_name = "Main-ItemSlot" + StringUtil::ToString(i);
			auto&& window = dynamic_cast<GuiItemSlot*>(getWindow(slot_name.c_str()));
			NULL_CONTINUE(window);
			if (resetChecked)
			{
				window->setSelected(false);
			}

			String mask_name = "Main-ItemSlot-StaticImage-Mask" + StringUtil::ToString(i);
			auto&& pMask = getWindow(mask_name.c_str());
			NULL_CONTINUE(pMask);
			pMask->SetVisible(false);

			int index = window->GetUserData();
			auto it = m_mapSchematicData.find(index);
			if (it == m_mapSchematicData.end())
			{
				continue;
			}

			const auto data = ModuleBlockSetting::getModuleBlockData(it->second);

			String image_name = "Main-ItemSlot-StaticImage" + StringUtil::ToString(i);
			auto&& pStaticImage = dynamic_cast<GUIStaticImage*>(getWindow(image_name.c_str()));
			NULL_CONTINUE(pStaticImage);
			pStaticImage->SetImage(data ? data->_moduleIcon : "");

			window->SetUserData2(it->second);

			if (data)
			{
				pMask->SetVisible(stack->stackSize < data->_costBlockNum);
				if (!resetChecked && pMask->IsVisible())
				{
					int nSchematicId = PlayerPrefs::Instance()->GetProperty<int>("SchematicId");
					if (data->_id == nSchematicId)
					{
						window->setSelected(false);
						PlayerPrefs::Instance()->SetProperty<int>("SchematicId", 0);
						m_lastGuiItemSlot = nullptr;
					}
				}
			}
		}

		if (resetChecked)
		{
			m_lastGuiItemSlot = nullptr;
		}
	}

	bool gui_mainControl::setAllowUseItem(int status)
	{
		m_useItemStatus = status;
		return true;
	}

	void gui_mainControl::checkBlockCityStoreShow()
	{
		if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity)
		{
			if (!Blockman::Instance() || !Blockman::Instance()->m_pPlayer || !Blockman::Instance()->m_pPlayer->m_blockCity->getManorInfo())
			{
				return;
			}

			bool isEditMode = Blockman::Instance()->m_pPlayer->m_blockCity->getPlayerInfo()->isEditMode;
			//m_btnInventory->SetEnabled(isEditMode);
			m_btnInventory->SetVisible(isEditMode);
			m_btnBlockCityPack->SetVisible(!isEditMode);
			m_btnBlockCityDelete->SetVisible(isEditMode);
		}
	}

	bool gui_mainControl::onClickBlockCityDelete(const EventArgs &args)
	{
		playSoundByType(ST_Click);
		ClientNetwork::Instance()->getCommonSender()->sendBlockCityOperation(4);
		return true;
	}

	void gui_mainControl::blockCityUpdate(i32 nTimeElapse)
	{
		if (checkGameType(ClientGameType::BlockCity))
		{
			if (UICommon::checkPlayerParam() && UICommon::checkBlockCityParam())
			{
				const auto& player = Blockman::Instance()->m_pPlayer;
				const auto& item = player->inventory->getCurrentItem();
				const auto& playerInfo = player->m_blockCity->getPlayerInfo();

				if (playerInfo) 
				{
					bool isEditMode = playerInfo->isEditMode;
					m_flyBtn->SetVisible((isEditMode || m_cbCaptureOpen->GetChecked()) && !player->isOnVehicle());
					if (item)
				{
					i32 num = 0;
					const auto& city = player->m_blockCity;
						if (isEditMode)
					{
						num = city->getItemNum(item->itemID, item->subItemID);
					}
					else
					{
						num = city->getShopItemNum(item->itemID);
					}
					getWindow("Main-ItemNum")->SetText(StringUtil::ToString(num).c_str());
				}
				else
				{
					getWindow("Main-ItemNum")->SetText("");
				}
				}

				getWindow("Main-MoveState")->SetVisible(!m_cbCaptureOpen->GetChecked() && !player->isDriver());
				getWindow("Main-Player-Exp")->SetVisible(!m_cbCaptureOpen->GetChecked() && !player->isDriver());
				getWindow("Main-ItemBarBg")->SetVisible(!m_cbCaptureOpen->GetChecked() && !player->isDriver());
				getWindow("Main-VisibleBar")->SetVisible(!m_cbCaptureOpen->GetChecked() && !player->isDriver());
				getWindow("Main-Capture")->SetVisible(true);
			}
		}
		else
		{
			getWindow("Main-Capture")->SetVisible(false);
		}

	}

	void gui_mainControl::updateFloatTip(i32 nTimeElapse)
	{
		m_txFloatTip->SetVisible(m_floatTipLeftTime > 0);
		if (m_floatTipLeftTime > 0)
		{
			m_floatTipLeftTime = m_floatTipLeftTime - nTimeElapse > 0 ? m_floatTipLeftTime - nTimeElapse : 0.f;
			float h = (m_floatTipLeftTime / m_showFloatTipTime) - 1.f;
			m_txFloatTip->SetArea(UDim(0, 0), UDim(h, 0), UDim(0.7f, 0), UDim(0, 60.f));

			if (m_floatTipLeftTime <= 0)
			{
				m_txFloatTip->SetVisible(false);
			}
		}
	}

	bool gui_mainControl::showFloatTip(const String& message)
	{
		m_txFloatTip->SetText(message.c_str());
		m_txFloatTip->SetVisible(true);
		m_txFloatTip->SetTouchable(false);
		m_txFloatTip->SetTextScale(3.0f);
		m_floatTipLeftTime = 2500.f;
		return true;
	}

	bool gui_mainControl::onClickBlockFortDelete(const EventArgs &args)
	{
		playSoundByType(ST_Click);
		Blockman::Instance()->m_pPlayer->inventory->removeAllItemFromHotBar();
		return true;
	}

	bool gui_mainControl::onShowBlockCityPack(const EventArgs & events)
	{
		RootGuiLayout::Instance()->showBlockCityPack();
		playSoundByType(ST_Click);
		return true;
	}

	bool gui_mainControl::onCaptureChange(const EventArgs & events)
	{
		if (Blockman::Instance()->m_pPlayer->isOnVehicle() || Blockman::Instance()->m_pPlayer->m_isUseElevator)
		{
			m_cbCaptureOpen->SetCheckedNoEvent(false);
			return false;
		}

		if (m_cbCaptureOpen->GetChecked())
		{
			m_isOpenCapture = true;
			getWindow("Main-Capture-Open-Layout")->SetVisible(false);
			getWindow("Main-Capture-Close")->SetVisible(true);
			getWindow("Main-Capture-Sure")->SetVisible(true);
		}
		else
		{
			m_isOpenCapture = false;
			getWindow("Main-Capture-Open-Layout")->SetVisible(true);
			getWindow("Main-Capture-Close")->SetVisible(false);
			getWindow("Main-Capture-Sure")->SetVisible(false);
		}
		if (checkGameType(ClientGameType::BlockCity))
		{
			BlockCityCaptureStatusEvent::emit(m_isOpenCapture);
			m_crossHairLayout->SetVisible(m_crossHairLayout->IsVisible() && !m_isOpenCapture);
		}
		Blockman::Instance()->setPersonView(1);
		PlayerPrefs::Instance()->putBoolPrefs("IsViewBobbing", Blockman::Instance()->m_pPlayer->getCurrCameraStatus());
		Blockman::Instance()->m_pPlayer->setCameraLocked(!Blockman::Instance()->m_pPlayer->getCurrCameraStatus());
		updateCameraBtnUI();
		return false;
	}

	bool gui_mainControl::onCaptureClick(const EventArgs & events, CaptureViewId viewId)
	{
		switch (viewId)
		{
		case CaptureViewId::CAPTURE_CLOSE:
			m_cbCaptureOpen->SetChecked(false);
			break;
		case CaptureViewId::CAPTURE_SURE:
			GameClient::CGame::Instance()->getShellInterface()->onAppActionTrigger(APP_ACTION_SCREEN_CAPTURE);
			Blockman::Instance()->hideGui();
			break;
		}
		return false;
	}

	bool gui_mainControl::updateBlockCityEditEffect(GUIString name, bool isShow)
	{
		getWindow(name)->SetVisible(isShow);
		return true;
	}

	bool gui_mainControl::onCloseCaptureIfBeginRaceVehicle(bool begin)
	{
		if (begin && m_cbCaptureOpen->GetChecked())
		{
			onCaptureClick(EventArgs(), CaptureViewId::CAPTURE_CLOSE);
		}
		return true;
	}
}
