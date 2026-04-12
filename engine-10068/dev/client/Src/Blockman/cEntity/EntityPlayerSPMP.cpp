#include "EntityPlayerSPMP.h"
#include "game.h"
#include "World/World.h"
#include "Inventory/InventoryPlayer.h"
#include "cItem/cItem.h"
#include "Entity/EntityAircraft.h"
#include "Network/ClientNetwork.h"
#include "Util/CommonEvents.h"
#include "Item/Item.h"
#include "Object/Root.h"
#include "World/GameSettings.h"
#include "cEntity/PlayerControlller.h"
#include "Setting/LogicSetting.h"
#include "Setting/GuideSystemSetting.h"
#include "cWorld/Blockman.h"
#include "Plugins/GameClientEvents.h"
#include "Util/PlayerPrefs.h"

using namespace GameClient;

namespace BLOCKMAN
{
EntityPlayerSPMP::EntityPlayerSPMP(Blockman* bm, World* pWorld, PlayerInteractionManager* interactionMgr)
	: EntityPlayerSP(bm, pWorld, 0, interactionMgr)
{
	oldPos = Vector3::ZERO;
	oldMinY = 0.f;
	oldYaw = 0.f;
	oldPitch = 0.f;
	wasOnGround = false;
	shouldStopSneaking = false;
	wasSneaking = false;
	m_moveTick = 0;
	hasSetHealth = false;
	m_bIsInAimSight = false;
	m_setEnableMovement = true;

	monitorItemInHandChange();
	monitorActionChange();
	deleteAllGuideArrow();
}

EntityPlayerSPMP::~EntityPlayerSPMP()
{
	deleteAllGuideArrow();
}

bool EntityPlayerSPMP::onBaseActionModified(ActionState oldAction, ActionState newAction)
{
	if (oldAction != newAction) {
		m_baseActionChanged = true;
	}
	
	return true;
}

bool EntityPlayerSPMP::onUpperActionModified(ActionState oldAction, ActionState newAction)
{
	if (oldAction != newAction) {
		m_upperActionChanged = true;
	}

	return true;
}

void EntityPlayerSPMP::monitorActionChange()
{
	using namespace std::placeholders;
	m_subscriptionGuard.add(m_baseAction.subscribe(std::bind(&EntityPlayerSPMP::onBaseActionModified, this, _1, _2)));
	m_subscriptionGuard.add(m_upperAction.subscribe(std::bind(&EntityPlayerSPMP::onUpperActionModified, this, _1, _2)));
}

void EntityPlayerSPMP::monitorItemInHandChange()
{
	m_subscriptionGuard.add(inventory->onItemInHandChanged([this] {
		int index = -1;
		ItemStackPtr ptr = inventory->getCurrentItem();
		if (ptr)
		{
			index = inventory->findItemStack(ptr);
			// check the holding item is gun?
			if(ptr->isGunItem())
				isHoldGun = true;
			else
				isHoldGun = false;
		}
		else
		{
			isHoldGun = false;
		}

		m_itemInHandChanged = true;
		m_itemInHandIndex = index;
	}));
}

void EntityPlayerSPMP::onUpdate()
{
	if (m_itemInHandChanged) {
		m_itemInHandChanged = false;

		resetActionProcess();
		m_needUpdateUpperAction = true;

		ClientNetwork::Instance()->getSender()->sendChangeItemInHand(m_itemInHandIndex, this->inventory->currentItemIndex);
		// if the item in had is mesh (gun), the actor will reAttach all the slave.
		ItemStackPtr stack = inventory->getCurrentItem();
		if (stack)
		{
			cItem* citem = dynamic_cast<cItem*>(stack->getItem());
			if (citem && citem->getRenderType() == ItemRenderType::MESH && citem->getMeshName() != StringUtil::BLANK)
			{
				if (m_lastHandMesh != citem->getMeshName())
				{
					m_lastHandMesh = citem->getMeshName();
					m_handMeshChanged = true;
				}
			}
			else if (m_lastHandMesh != StringUtil::BLANK)
			{
				m_lastHandMesh = StringUtil::BLANK;
				m_handMeshChanged = true;
			}
		}
		else if (m_lastHandMesh != StringUtil::BLANK)
		{
			m_lastHandMesh = StringUtil::BLANK;
			m_handMeshChanged = true;
		}
	}

	if (m_hasShotLaserGun)
	{
		ItemStackPtr stack = inventory ? inventory->getCurrentItem() : nullptr;
		if (!stack || !stack->getGunSetting() || (stack->getGunSetting()->gunType != LASER_EFFECT_GUN && stack->getGunSetting()->gunType != ELECTRICITY_EFFECT_GUN))
		{
			m_hasShotLaserGun = false;
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
		}
	}

	if (world->blockExists(BlockPos(int(Math::Floor(position.x)), 0, int(Math::Floor(position.z)))))
	{
		if (m_isCameraLocked)
		{
			position = m_cameraLockPos;
			m_prevCameraLockPos = m_cameraLockPos;
		}
			
		EntityPlayerSP::onUpdate();

		if (m_setEnableMovement)
		{
			Blockman::Instance()->m_playerControl->setDisableMovement(false);
			m_setEnableMovement = false;
		}
		sendMotionUpdates();
	}

	updatePlayerAnimState(Blockman::Instance()->m_gameSettings->getPersonView() == 0);
	updateInsertUpperActionTime();

	for (auto iter = m_guideArrowList.begin(); iter != m_guideArrowList.end(); iter++)
	{
		auto& guide_arrow = *iter;
		Vector3 posTemp = this->position;
		posTemp.y -= 1.0f;
		float distance = guide_arrow.endPos.distanceTo(posTemp);
		bool isGameType = LogicSetting::Instance()->checkGameType({ ClientGameType::SkyBlockMain, ClientGameType::SkyBlockMineCave, ClientGameType::SkyBlockProduct });
		if (distance <= 100.0f || isGameType)
		{
			if (!guide_arrow.effect)
			{
				String templateName = "guide_arrow.effect";
				if (LogicSetting::Instance()->checkGameType(ClientGameType::SkyBlockMineCave))
				{
					templateName = "g1048_blue_arrow.effect";
				}
				SimpleEffect* arrowEffect = WorldEffectManager::Instance()->addSimpleEffect(templateName, posTemp, 0.0f, -1);
				if (!arrowEffect)
				{
					LordLogError("can't create guide arrow!");
					continue;
				}
				guide_arrow.effect = arrowEffect;
			}
			guide_arrow.effect->setExtenalParam(posTemp, guide_arrow.endPos);
			guide_arrow.effect->mPosition = posTemp;
		}
		else
		{
			if (guide_arrow.effect)
			{
				guide_arrow.effect->mDuration = 0;
				guide_arrow.effect = nullptr;
			}
		}
	}
}

void EntityPlayerSPMP::onLivingUpdate()
{
	EntityPlayerSP::onLivingUpdate();
	if (onGround && capabilities.isFlying)
	{
		capabilities.isFlying = false;
		ClientNetwork::Instance()->getSender()->sendSetFlying(false);
		sendPlayerAbilities();
	}
}

void EntityPlayerSPMP::sendMotionUpdates()
{
	updateSneakAndSprint();
	
	sendMovementInputUpdate();

	sendPlayerMovement();

	sendAnimateUpdate();
}

EntityItem* EntityPlayerSPMP::dropOneItem(bool all)
{
	return NULL;
}

void EntityPlayerSPMP::sendChatMessage(const String& msg)
{
	//
}

void EntityPlayerSPMP::swingItem()
{
	EntityLivingBase::swingItem();
	if (m_defendProgress > 0.f)
	{
		playDefendProgress(0.f);
	}
}

void EntityPlayerSPMP::respawnPlayer()
{
	//
}

void EntityPlayerSPMP::damageEntity(DamageSource* pSource, float amount)
{
	if (!isEntityInvulnerable())
	{
		setEntityHealth(getHealth() - amount);
	}
}

void EntityPlayerSPMP::closeScreen()
{
	closeScreenAndDropStack();
}

void EntityPlayerSPMP::closeScreenAndDropStack()
{
	inventory->setItemStack(NULL);
	EntityPlayerSP::closeScreen();
}

void EntityPlayerSPMP::setHealth(float hp)
{
	if (hasSetHealth)
	{
		EntityPlayerSP::setHealth(hp);
	}
	else
	{
		setEntityHealth(hp);
		hasSetHealth = true;
	}
}

void EntityPlayerSPMP::sendPlayerAbilities()
{
	//
}

void EntityPlayerSPMP::setScale(float s)
{
	m_scale = s;
}

void EntityPlayerSPMP::updateSneakAndSprint()
{
	bool isSprintInput = isSprinting();

	if (isSprintInput != wasSneaking)
	{
		wasSneaking = isSprintInput;
	}

	bool sneak = isSneaking();

	if (sneak != shouldStopSneaking)
	{
		shouldStopSneaking = sneak;
	}
}

void EntityPlayerSPMP::sendMovementInputUpdate()
{
	if (isOnAircraft() || isOnVehicle() || isOnFlyingMount() || isOnFloatingMount() || getParentEntity())
		return;

	if (m_isCameraLocked || isWatchMode())
	{
		return;
	}

	auto pClientMovementInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
	bool moveForwardChanged = pClientMovementInput->moveForward != m_lastSyncMoveForward;
	bool moveStrafeChanged = pClientMovementInput->moveStrafe != m_lastSyncMoveStrafe;
	bool jumpChanged = pClientMovementInput->jump != m_lastSyncJump;
	bool downChanged = pClientMovementInput->down != m_lastSyncDown;

	m_lastSyncMoveForward = pClientMovementInput->moveForward;
	m_lastSyncMoveStrafe = pClientMovementInput->moveStrafe;
	m_lastSyncJump = pClientMovementInput->jump;
	m_lastSyncDown = pClientMovementInput->down;

	auto sender = ClientNetwork::Instance()->getSender();

	if (moveForwardChanged || moveStrafeChanged)
	{
		sender->sendMovementInputChanged(m_lastSyncMoveForward, m_lastSyncMoveStrafe);
	}

	if (jumpChanged)
	{
		sender->sendJumpChanged(m_lastSyncJump);
	}

	if (downChanged)
	{
		sender->sendDownChanged(m_lastSyncDown);
	}
}

void EntityPlayerSPMP::sendPlayerMovement(bool precise/* = false*/)
{
	if (isOnFlyingMount() || isOnFloatingMount())
		return;

	bool moved = isPlayerMoved(precise);

	float dyaw = rotationYaw - oldYaw;
	float dPitch = rotationPitch - oldPitch;
	bool rotated = Math::Abs(dyaw) > 2.0f || Math::Abs(dPitch) > 2.0f;

	bool onGroundChanged = onGround != wasOnGround;

	if (isOnVehicle() || isOnAircraft() || isUseElevator())
	{
		moved = false;
	}

	if (m_isCameraLocked)
	{
		m_cameraLockPos = position;
		position = m_originCameraLockPos;
		prevPos = position;
		if (moved)
			ClientNetwork::Instance()->getSender()->sendCameraLockMovement(m_cameraLockPos);

		return;
	}

	if (moved || onGroundChanged || rotated)
	{
		syncClientPositionToServer();
	}

	if (rotated && !isOnAircraft())
	{
		ClientNetwork::Instance()->getSender()->sendPlayerRotationChanged(rotationYaw, rotationPitch);
	}

	if (onGroundChanged) {
		wasOnGround = onGround;
	}

	++m_moveTick;

	if (moved)
	{
		recordOldPosSPMP();
	}

	if (rotated)
	{
		oldYaw = rotationYaw;
		oldPitch = rotationPitch;
	}
}

int EntityPlayerSPMP::catchEntity(float boxExpland)
{
	int entityId = 0;
	if (getParentEntity())
		return entityId;

	Entity* childEntity = nullptr;
	if (getHealth() > 0.0F && !capabilities.isWatchMode)
	{
		Box aabb = boundingBox.expland(boxExpland, 0.0f, boxExpland);

		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, aabb);

		float minDistance = 10000.0f;

		bool bHasPlayer{false};
		for (int i = 0; i < int(entities.size()); ++i)
		{
			Entity* pEntity = entities[i];
			auto player = dynamic_cast<EntityPlayer*>(pEntity);
			if (bHasPlayer && player == nullptr)
			{
				continue;
			}

			if (!pEntity->isDead)
			{
				if (!SCRIPT_EVENT::CatchEntityEvent::invoke(this, pEntity, pEntity->getClassID()))
				{
					continue;
				}

				if (!bHasPlayer && player)
				{
					bHasPlayer = true;
				}

				float distance = pEntity->getDistanceToEntity(this);
				if (distance < minDistance)
				{
					minDistance = distance;
					childEntity = pEntity;
				}
			}
		}
	}
	if (childEntity)
	{
		entityId = childEntity->entityId;
	}
	return entityId;
}

bool EntityPlayerSPMP::isPlayerMoved(bool precise/* = false*/)
{
	float dx = position.x - oldPos.x;
	float dy = boundingBox.vMin.y - oldMinY;
	float dz = position.z - oldPos.z;
	float dist = 0.25f;
	if (precise) {
		dist = 0.0001f;
	}
	if (isParachute())
	{
		float speed = LogicSetting::Instance()->getParachuteDownSpeed();
		dist = speed * 5.f;
	}
	return dx * dx + dy * dy + dz * dz > dist || m_moveTick >= 100;
}

void EntityPlayerSPMP::recordOldPosSPMP()
{
	oldPos = position;
	oldMinY = boundingBox.vMin.y;
	m_moveTick = 0;
}

void EntityPlayerSPMP::addGuideArrow(Vector3 arrowPos)
{
	bool alreadyHave = false;
	for (auto iter = m_guideArrowList.begin(); iter != m_guideArrowList.end(); iter++)
	{
		float distance = (*iter).endPos.distanceTo(arrowPos);
		if (distance <= 1.0f)
		{
			alreadyHave = true;
		}
	}
	if (!alreadyHave)
	{
		GuideArrow guide = GuideArrow();
		guide.endPos = arrowPos;
		m_guideArrowList.push_back(guide);
	}
}

void EntityPlayerSPMP::deleteGuideArrow(Vector3 arrowPos)
{
	for (auto iter = m_guideArrowList.begin(); iter != m_guideArrowList.end(); )
	{
		auto& guide_arrow = *iter;
		float distance = guide_arrow.endPos.distanceTo(arrowPos);
		if (distance <= 1.0f)
		{
			if (guide_arrow.effect)
			{
				guide_arrow.effect->mDuration = 0;
				guide_arrow.effect = nullptr;
			}
			iter = m_guideArrowList.erase(iter);
		}
		else
		{
			iter++;
		}
	}
}

void EntityPlayerSPMP::deleteAllGuideArrow()
{
	for (auto guide_arrow : m_guideArrowList)
	{
		if (guide_arrow.effect)
		{
			guide_arrow.effect = nullptr;
		}
	}
	m_guideArrowList.clear();
}

void EntityPlayerSPMP::sendAnimateUpdate()
{
	if (m_baseActionChanged) {
		ClientNetwork::Instance()->getSender()->sendAnimate(entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION, (int32_t)m_baseAction);
		m_baseActionChanged = false;
	}

	if (m_upperActionChanged) {
		ClientNetwork::Instance()->getSender()->sendAnimate(entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_UPPER_ACTION, (int32_t)m_upperAction);
		m_upperActionChanged = false;
	}
}

void EntityPlayerSPMP::syncClientPositionToServer()
{
	LordAssert(!isOnFlyingMount() && !isOnFloatingMount());
	static uint32_t cntInWatchMode = 0;
	if(!isOnAircraft() && !isOnVehicle() && !getParentEntity() && PlayerPrefs::Instance()->GetProperty<bool>("SyncClientPositionToServer"))
	{
		if (isWatchMode()) {
			cntInWatchMode++;
			if (cntInWatchMode % 30 != 0) {
				return;
			}
		} else {
			cntInWatchMode = 0;
		}
		
		auto sender = ClientNetwork::Instance()->getSender();
		sender->sendPlayerMovement(onGround, position.x, boundingBox.vMin.y, position.z, rotationYaw, rotationPitch);
		GuideSystemSetting::onPlayerMove(Vector3(position.x, boundingBox.vMin.y, position.z));
	}		
}

bool EntityPlayerSPMP::getOnFlyingMount(EntityFlyingMount* mount)
{
	if (EntityPlayer::getOnFlyingMount(mount))
	{
		Blockman::Instance()->setPersonView(GameSettings::SPVT_THIRD);
		return true;
	}
	return false;
}

bool EntityPlayerSPMP::getOnFloatingMount(EntityFloatingMount* mount)
{
	if (EntityPlayer::getOnFloatingMount(mount))
	{
		Blockman::Instance()->setPersonView(GameSettings::SPVT_THIRD);
		return true;
	}
	return false;
}

void EntityPlayerSPMP::openHorseInventory()
{
	// todo.
	// sendQueue.addToSendQueue(new Packet19EntityAction(this, 7));
}

bool EntityPlayerSPMP::getCurrCameraStatus() 
{
	return m_isCameraLocked;
}

bool EntityPlayerSPMP::setCameraLocked(bool bLocked)
{
	EntityPlayer::setCameraLocked(bLocked);
	Blockman::Instance()->refreshActorHide();
	ClientNetwork::Instance()->getSender()->sendCameraLock(bLocked);
	return true;
}

bool EntityPlayerSPMP::gunFireIsNeedBullet()
{
	return LogicSetting::Instance()->getGunIsNeedBulletStatus();
}

int EntityPlayerSPMP::getCrossHairByGunType(int Type)
{
	int nCrossHairType = COMMON_CROSS_HAIR;
	switch (Type)
	{
	case PISTOL_GUN:
	case RIFLE_GUN:
	case RIFLE_EFFECT_GUN:
	case MORTAR_EFFECT_GUN:
	case LASER_EFFECT_GUN:
	case ELECTRICITY_EFFECT_GUN:
	case MOMENTARY_LASER_EFFECT_GUN:
	case MOMENTARY_PIERCING_LASER_EFFECT_GUN:
		nCrossHairType = RIFLE_CROSS_HAIR;
		break;
	case SNIPER_GUN:
	case SNIPER_EFFECT_GUN:
		nCrossHairType = NONE_TYPE;
		break;
	case SHOTGUN_GUN:
	case SHOTGUN_EFFECT_GUN:
		nCrossHairType = SHOTGUN_CROSS_HAIR;
		break;
	case SUBMACHINE_GUN:
		nCrossHairType = MACHINE_CROSS_HAIR;
		break;
	}
	return nCrossHairType;
}

}