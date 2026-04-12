
/********************************************************************
filename: 	Blockman.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-15
*********************************************************************/
#pragma once

#ifndef __BLOCKMAN_HEADER__
#define __BLOCKMAN_HEADER__

/** header files from common. */
#include "BM_TypeDef.h"
#include "World/RayTracyResult.h"
#include "Util/SubscriptionGuard.h"
#include "Item/ItemStack.h"

namespace BLOCKMAN
{
class World;
class WorldClient;
class WorldSettings;
class TextureAtlasRegister;
class RenderGlobal;
class GameSettings;
class RenderEntity;
class PlayerController;
class EntityPlayerSPMP;
class EntityLivingBase;
class EntityPlayerRenderable;
class RightArmRenderable;
class PlayerInteractionManager;
class RootGuiLayout;
class EntityPlayer;
class VisualTessThread;


class Blockman : public Singleton<Blockman>, public ObjectAlloc
{
public:
	enum MOUSE_BUTTON_STATE
	{
		MBS_UP = 0,
		MBS_DOWN = 1,
		MBS_MOVE = 2,
		MBS_WHEEL = 3
	};

	struct MouseInfo
	{
		i16 key{};
		i16 state{};
		i16 x{};
		i16 y{};
	};

	typedef LORD::deque<MouseInfo>::type MouseInfos;

protected:
	void sendClickSceneToController(bool touchHold);
	void longTouchScene();
	void clickScene();
	bool longTouchUseType(ItemStackPtr pStack);
	bool canContinuousUse(ItemStackPtr pStack);

public:
	static void initialize();
	static void unInitialize();
	void ManualAttack();

	Blockman();
	~Blockman();

	void init();
	void tick(float dt);
	void runTick();
	void setPersonView(int view);
	void generateWorld(int sex, int defaultIdle);
	void refreshActorHide();
	void saveChunks();
	//void setDimensionAndSpawnPlayer(int dimension);
	void loadWorld(World* pWorld, const String& worldName, int sex, int defaultIdle);
	void setCanGameReady(bool canGameReady) { m_canGameReady = canGameReady; }
	void onGameReady();
	void rebirth(const Vector3& spawnPos, int sex, const String& showName, int defaultIdle, i32 vip);
	i32 getSystemTime();
	String getDebugString();
	String getPlayerString();
	String getRayTraceString();
	String getAsyncString() { return m_asyncString; }
	void caculateCpuTime();
	void setClientDebugString(String str) {
		m_clientDebugString = str;
	}
	String getClientDebugString() {
		return m_clientDebugString;
	}
	void setServerDebugString(String str) {
		m_serverDebugString = str;
	}
	String getServerDebugString() {
		return m_serverDebugString;
	}

	//add by maxicheng
	static void initializeNoGUI();
	void init(i64 seed);
	World * getWorld(){return m_pWorld;}

	void switchPerspece();
	int  getCurrPersonView();
	void setPerspece(int view);
	void syncBlockDestroyeFailure(bool destroyeStatus, int destroyeFailureType);

	void setBloomThreshold(float threshold);
	void setBloomSaturation(float saturation);
	void setBloomIntensity(float intensity);
	void setBloomBlurDeviation(float deviation);
	void setBloomBlurMultiplier(float multiplier);
	void enableFullscreenBloom(bool enabled);
	void setBlockLightEnhancement(float enhancement);

	void setBlockBloomOption(float deviation, float intensity, float saturation);

	Vector3 getPlayerPosition();
	BlockPos getBlockPosition();
	float getPlayerYaw();
	int getBlockId();

	void beginTessVisual_stage1(RenderGlobal* rg, WorldClient* world);
	void beginTessVisual_stage2(RenderGlobal* rg, WorldClient* world);
	void finishAsync();
	void initCloseup(bool isShow, const Vector3& playerPos, const Vector3& closeupPos, float farDistance, float nearDistance, float velocity, float duration, float yaw, float pitch);
	void resetCloseup();
	bool checkClickCoolingTime(ItemStackPtr heldItem);
	bool setAllowUseItem(int status);
	void renderMark();

	bool isGameReady() { return m_isGameReady; }
	void hideGui();
	void startCameraRotation(float firstStageTime = 0.0f, float secondStageTime = 1000.0f, float angleTotal = 180.0f, float angleDelta = 0.36f, float rotateRadius = -1.0f);
	void stopCameraRotation();

	EntityPlayerSPMP* getPlayer() { return m_pPlayer; }

private:
	String m_clientDebugString;
	String m_serverDebugString;
	SubscriptionGuard m_subscriptionGuard;
	
	Vector3 lastPos;
	bool exitPressed = false;
	int longtouchTick = 4;

public:
	World*				m_pWorld = nullptr;
	WorldSettings*		m_worldSettings = nullptr;
	GameSettings*		m_gameSettings = nullptr;
	String				m_worldName;	
	PlayerController*	m_playerControl = nullptr;
	EntityPlayerSPMP*	m_pPlayer = nullptr;
	
	RootGuiLayout*		m_rootGuiLayout = nullptr;
	int					m_accelerateTime = 1;
	int					m_teleportIdle = 0;
	int					m_firstLogin = true;

	PlayerInteractionManager* m_interactionMgr = nullptr;
	
	RenderEntity*		m_entityRender = nullptr;
	RenderGlobal*		m_globalRender = nullptr;
	VisualTessThread*	m_visualTessThread = nullptr;
	float				m_tickTime = 0.f;
	static const float	s_tickFPS;
	float				m_render_dt = 0.f;
	int					m_logicSystemTime = 0;

	EntityLivingBase*   renderViewEntity = nullptr;
	EntityLivingBase*	pointedEntityLiving = nullptr;

	RayTraceResult		objectMouseOver;

	int					joinPlayerCounter = 0;
	int					debugCrashKeyPressTime = 0;

	ui64                m_worlTick = 0;
	int					isBlockDestoryStatus = true;

	ui64				m_mainBegin = 0;
	ui64				m_exchangeBegin = 0;
	ui64				m_renderBegin = 0;

	ui64				m_waitActive = 0;
	ui64                m_visibiBegin = 0;
	ui64                m_findVisBegin = 0;
	ui64				m_lightingBegin = 0;
	ui64				m_lightingEnd = 0;
	ui64				m_meshBegin = 0;
	ui64				m_meshGenEnd = 0;
	ui64				m_asyncBegin = 0;
	ui64				m_asyncEnd = 0;
	String				m_asyncString;

	Matrix4				m_firstPersonItemMat;

	bool				m_closeup_isShow = false;
	Vector3				m_closeup_playerPos = Vector3::ZERO;
	Vector3				m_closeup_closeupPos = Vector3::ZERO;
	float				m_closeup_farDis = 0.0f;
	float				m_closeup_nearDis = 0.0f;
	float				m_closeup_velocity = 0.0f;
	float				m_closeup_duration = 0.0f;
	float				m_closeup_yaw = 0.0f;
	float				m_closeup_pitch = 0.0f;
	bool				m_isShowGunFlameCoordinate = false;

	int					m_useItemStatus = CAN_USE_ITEM;
	bool				m_isRotatingCamera = false;

private:
	/** Mouse click counter */
	int					m_clickCounter	= 0;
	int					m_lastUseItemId = -1;
	bool				m_isGameReady = false;
	bool				m_canGameReady = false;
	Vector3i				m_newBolckPos = BlockPos::ZERO;
	i32					m_useToolGatherTime = 0;
	bool				m_cdDone = true;
	i32 m_hideGuiTime = 0;
};

}

#endif
