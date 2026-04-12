#pragma once
#ifndef __ENTITY_PLAYER_AI_HEADER__
#define __ENTITY_PLAYER_AI_HEADER__

#include "EntityPlayerMP.h"
class ClientPeer;
namespace BLOCKMAN
{
	class EntityPlayerAI : public EntityPlayerMP
	{
		RTTI_DECLARE(EntityPlayerAI);
	private:
		std::shared_ptr<ClientPeer> m_clientPeer;
		ActionState old_baseAction;
		ActionState old_upperAction;
		int faceEntityId;
		Vector3 lastPos = Vector3::INVALID;
		Vector2 movePosition = Vector2::INVALID;
		int tryMoveTime = 0;
		float oldYaw = Math::MAX_REAL;
		float oldDistance = Math::MAX_REAL;

		int lockFaceTime = 0;

		int placeActionTime = 0;

		int breakActionTime = -1;

		int attackTargetId = 0;
		int attackActionTime = -1;
		bool isAttackHit = false;

		int shootArrowTime = -1;
		bool isShootHit = false;

		int sneakTime = 0;

		int knockBackLockInput = 0;
	public:
		EntityPlayerAI(World* pWorld, const String& worldName, const Vector3i& spawnPos, const ui64 rakssid);
		virtual ~EntityPlayerAI();

		void setClientPeer(std::shared_ptr<ClientPeer> clientPeer) {
			m_clientPeer = clientPeer;
		}

		void onUpdate();

		void onUpdateAction();

		void updateMovement();

		void updateEntityActionState();

		void knockBack(Entity * pEntity, float strength, float xRatio, float zRatio);

		void sendAnimateUpdate();

		void sendLogin(ui64 platformUserId);

		void sendLogout();

		void sendReady();

		void sendAnimate(int32_t entityId, NETWORK_DEFINE::PacketAnimateType animateType, int32_t value);

		void sendPlayerMovement(bool onGround, float x, float y, float z, float yaw, float pitch);

		void sendMovementInputChanged(float forward, float strafe);

		void sendPlayerRotationChanged(float yaw, float pitch);

		void sendJumpChanged(bool isJump);

		void sendAttack(int targetEntityRuntimeId, real32 targetX, real32 targetY, real32 targetZ);

		void sendArrow();

		void sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType action);

		void moveToPos(Vector3i targetPos);

		void stopMove();

		void faceToPos(Vector3 facePos);

		void faceToEntity(int entityId)
		{
			faceEntityId =entityId;
		}

		//void sendAbortBreakBlock();

		void sendFinishBreakBlock();

		void sendPlaceBlock(const Vector3i & blockPos, int face, const Vector3 & hisPos);

		bool placeBlock(BlockPos pos);

		bool breakBlock(BlockPos pos);

		void abortBreakBlock();

		void attackEntity(int targetId, bool isHit);

		void setHandItemId(int itemId);

		void startShootArrow(int targetId, bool isHit);

		void startSneak();

		void relaseItemSkill(int skillItemId);

		void sendStartBreakBlock(int x, int y, int z);

		void sendAbortBreakBlock();

		bool isPlayerMoved(bool precise);

	protected:

	};

}

#endif
