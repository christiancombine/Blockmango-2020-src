#include "EntityPlayerAI.h"
#include "Blockman/Entity/EntityTracker.h"
#include "Network/ServerNetwork.h"
#include "Network/ClientPeer.h"
#include "Network/protocol/C2SPackets.h"
#include "Network/C2SPacketHandles/C2SInitPacketHandles.h"
#include "Network/C2SPacketHandles/C2SPlayerActionPacketHandles.h"
#include "Network/C2SPacketHandles/C2SBreakBlockPacketHandles.h"
#include "Script/GameServerEvents.h"
#include "World/World.h"

namespace BLOCKMAN
{

	EntityPlayerAI::EntityPlayerAI(World* pWorld, const String& worldName, const Vector3i& spawnPos, const ui64 rakssid)
		: EntityPlayerMP(pWorld, worldName, spawnPos)
	{
		ServerNetwork::Instance()->onServerNetworkConnSucc(rakssid, "127.0.0.1");
		m_clientPeer = ServerNetwork::Instance()->findPlayerByRakssid(rakssid);
		m_clientPeer->onSpawn(worldName, this);
	}

	EntityPlayerAI::~EntityPlayerAI()
	{
		m_clientPeer = nullptr;
	}

	void EntityPlayerAI::onUpdate()
	{
		if (m_clientPeer == nullptr)
			return;

		//update face
		if (lockFaceTime > 0)
		{
			lockFaceTime--;
		}

		auto faceEntity = world->getEntity(faceEntityId);
		if (faceEntity != nullptr && movePosition != Vector2::INVALID)
		{
			auto targetPos = faceEntity->getPosition();
			targetPos.y = targetPos.y + 1.62f;
			faceToPos(targetPos);
		}

		if (sneakTime > 0)
		{
			sneakTime--;
			if (sneakTime == 0)
				sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::STOP_SNEAK);
		}
	
		updateMovement();
		
		if (canMovement() == false)
			enableMovement();

		EntityPlayerMP::onUpdate();

		//LordLogError("motion  %f, %f, %f", motion.x, motion.y, motion.z);
		//LordLogError("position  %f, %f, %f", position.x, position.y, position.z);
		//LordLogError("movementInput  %f, %f", movementInput->moveForward, movementInput->moveStrafe);
		
		updatePlayerAnimState(false);
		updateInsertUpperActionTime();
		onUpdateAction();
		sendAnimateUpdate();
		sendPlayerMovement(onGround, position.x, position.y, position.z, rotationYaw, rotationPitch);
	}

	void EntityPlayerAI::onUpdateAction()
	{
		if (shootArrowTime > 0)
		{
			m_upperAction = ActionState::AS_AIM_BEGIN;
			shootArrowTime--;
			return;
		}
		else if(shootArrowTime == 0)
		{
			shootArrowTime--;
			m_upperAction = ActionState::AS_AIM_END;
			auto target = world->getEntity(attackTargetId);
			if (target != nullptr)
			{
				auto targetPos = target->getPosition();
				targetPos.y = targetPos.y + 1.62;
				faceToPos(targetPos);
				if (!isShootHit)
				{
					float dHorizontal = (std::rand() % 21) / 10.0 - 1;
					dHorizontal = dHorizontal > 0 ? dHorizontal + 1 : dHorizontal - 1;
					float dx = -sin((rotationYaw - 90) * Math::DEG2RAD) * dHorizontal;
					float dz = cos((rotationYaw - 90) * Math::DEG2RAD) * dHorizontal;
					float dy = (std::rand() % 20) / 10.0 - 1.2;
					faceToPos(Vector3(targetPos.x + dx, targetPos.y + dy, targetPos.z + dz));
				}
			}
			sendArrow();
			return;
		}

		if (attackActionTime >= 0)
		{
			m_upperAction = ActionState::AS_ATTACK;
			attackActionTime--;
			auto target = world->getEntity(attackTargetId);
			if (target != nullptr)
			{
				auto targetPos = target->getPosition();
				targetPos.y = targetPos.y + 1.62;
				faceToPos(targetPos);
				if (attackActionTime == -1 && isAttackHit)
					sendAttack(target->entityId, position.x, position.y, position.z);
			}
			return;
		}

		if (placeActionTime > 0)
		{
			m_upperAction = ActionState::AS_ATTACK;
			placeActionTime--;
			return;
		}
		
		if (breakActionTime > 0)
		{
			breakActionTime--;
			if (breakActionTime % 5 == 0)
				m_upperAction = ActionState::AS_IDLE;
			else
				m_upperAction = ActionState::AS_ATTACK;
			return;
		}
		else if (breakActionTime == 0)
		{
			breakActionTime--;
			sendFinishBreakBlock();
			return;
		}

	}

	void EntityPlayerAI::updateMovement()
	{
		if (movePosition != Vector2::INVALID)
		{
			tryMoveTime++;
			if (tryMoveTime > 40)
			{
				if (SCRIPT_EVENT::PlayerAIMoveFailEvent::invoke(m_clientPeer->getRakssid()))
				{
					stopMove();
					return;
				}
			}

			oldYaw = rotationYaw;
			float vector = atan2(movePosition.x - position.x, movePosition.y - position.z);
			float pi = 3.141592653;
			float targetYaw = vector / pi * -180;
			float angle = rotationYaw - targetYaw;
			float radian = (2 * pi / 360) * angle;
			if (cos(radian) > 0)
			{
				if (m_clientPeer->getEntityPlayer()->isSneaking())
				{
					sendMovementInputChanged(0.3f, 0);
				}
				else
				{
					sendMovementInputChanged(0.7f, 0);
				}
			}
			else
			{
				if (SCRIPT_EVENT::PlayerAIArriveEvent::invoke(m_clientPeer->getRakssid()))
				{
					stopMove();
				}
			}
		}

	}

	void EntityPlayerAI::updateEntityActionState()
	{
		if (knockBackLockInput > 0)
		{
			knockBackLockInput--;
			movementInput->moveStrafe = 0.f;
			movementInput->moveForward = 0.f;
		}

		
		applyMovementInput();
		movementInput->moveStrafe *= 2.0f;

		if (needUpdateActionState())
		{
			EntityPlayer::updateEntityActionState();
		}
	}

	void EntityPlayerAI::knockBack(Entity* pEntity, float strength, float xRatio, float zRatio)
	{
		knockBackLockInput = 10;
		EntityPlayerMP::knockBack(pEntity, strength, xRatio, zRatio);
		sendPlayerMovement(false, position.x, position.y, position.z, rotationYaw, rotationPitch);
	}

	void EntityPlayerAI::sendAnimateUpdate()
	{
		if (old_baseAction != m_baseAction) {
			sendAnimate(entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_BASE_ACTION, (int32_t)m_baseAction);
			old_baseAction = m_baseAction;
		}

		if (old_upperAction != m_upperAction) {
			sendAnimate(entityId, NETWORK_DEFINE::PacketAnimateType::ANIMATE_TYPE_UPPER_ACTION, (int32_t)m_upperAction);
			old_upperAction = m_upperAction;
		}
	}

	void EntityPlayerAI::sendLogin(ui64 platformUserId)
	{
		//login
		m_clientPeer->setPlatformUserId(platformUserId);
		setRaknetID(m_clientPeer->getRakssid());
		m_clientPeer->login("", false, -1);
		//auto packet = LORD::make_shared<C2SPacketLogin>();
		//packet->m_enableIndie = false;

		//C2SInitPacketHandles::handlePacket(m_clientPeer, packet);

		//ready

	}

	void EntityPlayerAI::sendLogout()
	{
		auto packet = LORD::make_shared<C2SPacketLogout>();
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendReady()
	{
		auto packet = LORD::make_shared<C2SPacketClientReady>();
		packet->m_status = 1;
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendAnimate(int32_t entityId, NETWORK_DEFINE::PacketAnimateType animateType, int32_t value/* = 0*/)
	{
		if (value == (int32_t)AS_SELECTABLE_ACTION)
			return;

		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketAnimate>();

		packet->m_entityRuntimeId = hashCode();
		packet->m_animateType = animateType;
		packet->m_value = value;
		packet->holdItemId = this->m_holdItemId;
		packet->holdItemActor = this->m_holdItemActor;

		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendPlayerMovement(bool onGround, float x, float y, float z, float yaw, float pitch)
	{
		if (m_clientPeer == nullptr)
			return;

		float dy = y - lastPos.y;
		float dx = x - lastPos.x;
		float dz = z - lastPos.z;
		float moveLenSqrXZ = dx * dx + dz * dz;
		float moveLenSqr = moveLenSqrXZ + dy * dy;
		
		if (moveLenSqr < 0.1f && canMovement())
			return;

		lastPos.y = y;
		lastPos.x = x;
		lastPos.z = z;

		auto packet = LORD::make_shared<C2SPacketPlayerMovement>();
		packet->m_onGround = onGround;
		packet->m_x = x;
		packet->m_y = y;
		packet->m_z = z;
		packet->m_yaw = yaw;
		packet->m_pitch = pitch;
		packet->m_moving = true;
		packet->m_rotating = true;

		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendMovementInputChanged(float forward, float strafe)
	{
		if (m_clientPeer == nullptr)
			return;

		if (!movementInput)
			return;
		
		if (Math::Abs(movementInput->moveForward - forward) + Math::Abs(movementInput->moveStrafe - strafe) < 0.1f)
			return;

		auto packet = LORD::make_shared<C2SPacketMovementInput>();
		packet->forward = forward;
		packet->strafe = strafe;
		
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendPlayerRotationChanged(float yaw, float pitch)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketSendPlayerRotation>();
		packet->yaw = yaw;
		packet->pitch = pitch;
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendJumpChanged(bool isJump)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketSendJumpChanged>();
		packet->isJump = isJump;
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendAttack(int targetEntityRuntimeId, real32 targetX, real32 targetY, real32 targetZ)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketAttack>();
		packet->m_targetEntityRuntimeId = targetEntityRuntimeId;
		packet->m_targetX = targetX;
		packet->m_targetY = targetY;
		packet->m_targetZ = targetZ;

		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType action)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketPlayerAction>();
		packet->action = action;
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::moveToPos(Vector3i targetPos)
	{
		movePosition = Vector2(targetPos.x + 0.5, targetPos.z + 0.5);
		tryMoveTime = 0;
		faceEntityId = 0;
		faceToPos(Vector3(movePosition.x, position.y + 1.62, movePosition.y));
	}

	void EntityPlayerAI::stopMove()
	{
		movePosition = Vector2::INVALID;
		tryMoveTime = 0;
		sendMovementInputChanged(0, 0);
		lockFaceTime = 0;
	}

	void EntityPlayerAI::sendArrow()
	{
		if (m_clientPeer == nullptr)
			return;

		addItem(Item::arrow->itemID, 1, 0);

		int ticktime = 60;
		float cdtime = (float)ticktime / 20.0F;
		cdtime = (cdtime * cdtime + cdtime * 2.0F) / 3.0F;

		auto packet = LORD::make_shared<C2SPacketLaunchArrow>();

		packet->cdTime = cdtime;
		packet->playerPos = position + Vector3(0.f, getEyeHeight(), 0.f);
		packet->playerYaw = rotationYaw;
		packet->playerPitch = rotationPitch;
		packet->arrowID = 262;

		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::relaseItemSkill(int skillItemId)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketRelaseItemSkill>();
		packet->skillItemId = skillItemId;

		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendStartBreakBlock(int x, int y, int z)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketStartBreakBlock>();
		packet->x = (i16)x;
		packet->y = (i16)y;
		packet->z = (i16)z;
		C2SBreakBlockPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendAbortBreakBlock()
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketAbortBreakBlock>();
		C2SBreakBlockPacketHandles::handlePacket(m_clientPeer, packet);
	}

	bool EntityPlayerAI::isPlayerMoved(bool precise)
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

	void EntityPlayerAI::faceToPos(Vector3 facePos)
	{
		if (lockFaceTime > 0)
			return;

		//LordLogError("facePos  %f, %f, %f", facePos.x, facePos.y, facePos.z);
		Vector3 eyePosition = Vector3(position.x, position.y + 1.62, position.z);
		float vector = atan2(facePos.x - eyePosition.x, facePos.z - eyePosition.z);
		float pi = 3.141592653;
		float targetYaw = vector / pi * -180;

		vector = atan2(facePos.y - eyePosition.y, sqrt(pow(facePos.x - eyePosition.x, 2) + pow(facePos.z - eyePosition.z, 2)));
		float targetPitch = vector / pi * -180;

		if (Math::Abs(rotationYaw - targetYaw) > 10.0f || Math::Abs(rotationPitch - targetPitch) > 10.0f)
			sendPlayerRotationChanged(targetYaw, targetPitch);
	}

	void EntityPlayerAI::sendFinishBreakBlock()
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketFinishBreakBlock>();
		C2SBreakBlockPacketHandles::handlePacket(m_clientPeer, packet);
	}

	void EntityPlayerAI::sendPlaceBlock(const Vector3i & blockPos, int face, const Vector3 & hisPos)
	{
		if (m_clientPeer == nullptr)
			return;

		auto packet = LORD::make_shared<C2SPacketPlaceBlock>();
		packet->blockPosX = (i16)blockPos.x;
		packet->blockPosY = (i16)blockPos.y;
		packet->blockPosZ = (i16)blockPos.z;
		packet->face = face;
		packet->hitPos = hisPos;
		C2SPlayerActionPacketHandles::handlePacket(m_clientPeer, packet);
	}

	bool EntityPlayerAI::placeBlock(BlockPos pos)
	{
		if (placeActionTime == 0)
		{
			placeActionTime = 2;
			auto itemStack = inventory->getCurrentItem();
			if (itemStack == nullptr)
				return false;

			auto itemBlock = dynamic_cast<ItemBlock*>(Item::itemsList[itemStack->itemID]);
			if (itemBlock == nullptr)
				return false;

			lockFaceTime = 0;
			faceToPos(Vector3(pos.x + 0.5, pos.y + 0.5, pos.z + 0.5));
			lockFaceTime = 20;

			Vector3i hitPos;
			int face = 0;
			if (world->getBlockId(Vector3i(pos.x, pos.y - 1, pos.z)) != 0)
			{
				hitPos = Vector3i(pos.x, pos.y - 1, pos.z);
				face = 1;
			}
			else if(world->getBlockId(Vector3i(pos.x, pos.y, pos.z + 1)) != 0)
			{
				hitPos = Vector3i(pos.x, pos.y, pos.z + 1);
				face = 2;
			}
			else if (world->getBlockId(Vector3i(pos.x, pos.y, pos.z - 1)) != 0)
			{
				hitPos = Vector3i(pos.x, pos.y, pos.z - 1);
				face = 3;
			}
			else if (world->getBlockId(Vector3i(pos.x + 1, pos.y, pos.z)) != 0)
			{
				hitPos = Vector3i(pos.x + 1, pos.y, pos.z);
				face = 4;
			}
			else if (world->getBlockId(Vector3i(pos.x - 1, pos.y, pos.z)) != 0)
			{
				hitPos = Vector3i(pos.x - 1, pos.y, pos.z);
				face = 5;
			}

			if (face == 0)
				return false;

			sendPlaceBlock(hitPos, face, Vector3::ZERO);
			return true;
		}
		return false;
	}

	bool EntityPlayerAI::breakBlock(BlockPos pos)
	{
		if (breakActionTime != -1)
			return false;

		int blockID = world->getBlockId(pos);
		auto itemStack = inventory->getCurrentItem();
		Block* pBlock = BlockManager::sBlocks[blockID];
		if (pBlock != nullptr)
		{
			breakActionTime = (int)(1 / pBlock->getPlayerRelativeBlockHardness(this, world, pos));
			//sub break block speed
			breakActionTime = Math::Floor(breakActionTime*1.3);

			faceToPos(pos);
			lockFaceTime = breakActionTime;
			sendStartBreakBlock(pos.x, pos.y, pos.z);
			return true;
		}
		return false;
	}

	void EntityPlayerAI::abortBreakBlock()
	{
		breakActionTime = -1;
		sendAbortBreakBlock();
	}

	void EntityPlayerAI::attackEntity(int targetId, bool isHit)
	{
		if (attackActionTime == -1)
		{
			attackActionTime = 2;
			isAttackHit = isHit;
			attackTargetId = targetId;
		}
	}



	void EntityPlayerAI::setHandItemId(int itemId)
	{
		if (itemId == 0)
		{
			inventory->removeCurrentItemFromHotbar();
			return;
		}
		ItemStackPtr pStack = inventory->getCurrentItem();
		if (pStack == nullptr || (pStack && pStack->itemID != itemId))
		{	
			int slot = inventory->getInventorySlotContainItem(itemId);
			if (slot < 0)
			{
				if (!inventory->isCanAddItem(itemId, 0, 1))
				{
					auto lastStack = inventory->mainInventory[35];
					if (lastStack != nullptr)
					{
						inventory->removeItem(lastStack->itemID, lastStack->stackSize);
					}
				}
				addItem(itemId, 1, 0);
				slot = inventory->getInventorySlotContainItem(itemId);
			}
			inventory->putItemToHotbar(slot);
		}
	}

	void EntityPlayerAI::startShootArrow(int targetId, bool isHit)
	{
		if (shootArrowTime == -1)
		{

			attackTargetId = targetId;
			isShootHit = isHit;
			Entity* target = world->getEntity(targetId);
			if (target != nullptr)
				faceToEntity(targetId);
			shootArrowTime = 10;
		}
	}

	void EntityPlayerAI::startSneak()
	{
		sneakTime = 30;
		if (!m_clientPeer->getEntityPlayer()->isSneaking())
		{
			sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK);
		}
	}
}