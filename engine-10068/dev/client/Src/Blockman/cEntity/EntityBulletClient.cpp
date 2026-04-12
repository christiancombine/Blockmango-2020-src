#include "EntityBulletClient.h"
#include "Script/Event/LogicScriptEvents.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "cWorld/Blockman.h"
#include "World/GameSettings.h"
#include "cEffects/EntityFx.h"
#include "Network/ClientNetwork.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/World.h"
#include "World/WorldEffectManager.h"
#include "Render/TextureAtlas.h"
#include "Tessolator/SkinedRenderable.h"
#include "Tessolator/TessRenderable.h"
#include "Tessolator/TessManager.h"
#include "Scene/ModelEntity.h"
#include "Model/MeshManager.h"
#include "Model/Mesh.h"
#include "Actor/ActorManager.h"
#include "Setting/LogicSetting.h"
#include "Object/Root.h"
#include "Util/PlayerPrefs.h"
#include "Item/Items.h"

namespace BLOCKMAN
{
	EntityBulletClient::EntityBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos, float yawOffset, float pitchOffset)
		: EntityBullet(pWorld)
	{
		if (!pSetting || !pShooting || pShooting->isDead)
		{
			setDead();
			return;
		}
		m_hasBeenDead = false;
		m_shootingEntity = pShooting;
		m_gunSetting = pSetting;
		m_shootingEntityId = pShooting->entityId;
		m_bounceTimes = pSetting->bounceTimes;
		m_isShootingByMe = m_shootingEntityId == Blockman::Instance()->m_pPlayer->entityId;

		Vector3 beginPosTemp = calcPosition(pShooting, pSetting);
		m_hasBlockTarget = hasBlockTarget;
		m_blockTargetPos = blockTargetPos;
		m_aimPoint = endPos;
		m_aimDis = beginPosTemp.distanceTo(endPos);
		if (m_gunSetting->gravityVelocity > 0.0f && m_aimDis > 5.0f)
		{
			m_aimDis = 5.0f;
		}

		float dx = endPos.x - beginPosTemp.x;
		float dy = endPos.y - beginPosTemp.y;
		float dz = endPos.z - beginPosTemp.z;
		float dxz = Math::Sqrt(dx * dx + dz * dz);
		float beginYaw = (Math::ATan2(dz, dx) * Math::RAD2DEG) - 90.0F;
		float beginPitch = -(Math::ATan2(dy, dxz) * Math::RAD2DEG);
		if (Vector3(dx, dy, dz).normalizedCopy().len() < 0.00001f)
		{
			beginYaw = pShooting->rotationYaw;
			beginPitch = pShooting->rotationPitch;
		}

		setShootingRange(pSetting->shootRange);
		setLocationAndAngles(beginPosTemp, beginYaw + yawOffset, beginPitch + pitchOffset);
		setGunID(pSetting->gunId);

		Vector3 dir;
		dir.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
		dir.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
		dir.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD));
		float velocity = pSetting->bulletSpeed;
		setThrowableHeading(dir, velocity * 1.5F, 1.0F);
		
		m_bulletEffectName = pSetting->bulletEffect;

		if (pShooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
		{
			m_isFirstPerspective = true;
		}
	}

	EntityBulletClient::~EntityBulletClient()
	{
		if (m_needDeleteEffect && m_bulletEffect != nullptr)
		{
			m_bulletEffect->mDuration = 0;
			m_bulletEffect = nullptr;
		}
	}

	void EntityBulletClient::onUpdate()
	{
		if (m_hasBeenDead)
		{
			setDead();
			return;
		}

		Entity::onUpdate();

		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		RayTraceResult trace = getRayTraceAtBlock();

		Entity* tracyEntity = nullptr;
		Vector3 hitPos = Vector3::ZERO;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * m_ratio, motion.y * m_ratio, motion.z * m_ratio));
		bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
		for (auto entity : entities)
		{
			if (!isGunShootActor && entity->isClass(ENTITY_CLASS_ACTOR_NPC))
			{
				continue;
			}
			if (entity && entity->canBePushed() && m_begin.distanceTo(entity->position) <= m_shootingRange && entity->entityId != m_shootingEntityId)
			{
				Box entity_box = entity->boundingBox;
				RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_begin, m_entityEnd);
				if (tracy_entity.result)
				{
					float dis = m_begin.distanceTo(tracy_entity.hitPointPos);
					if (dis < m_minDis)
					{
						tracyEntity = entity;
						hitPos = tracy_entity.hitPointPos;
						m_minDis = dis;
					}
				}
			}
		}


		if (tracyEntity)
		{
			trace.set(tracyEntity, hitPos);
		}

		if (trace.result && trace.entityHit)
		{
			EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(trace.entityHit);
			if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
			{
				trace.reset();
			}
		}

		if (trace.result)
		{
			trySendGunFireResult(trace);
			if (trace.entityHit)
			{
				motion = trace.hitPointPos - position;
				position = trace.hitPointPos - motion.normalizedCopy() * 0.05f;
				m_hasBeenDead = true;
			}
			else
			{
				if (m_bounceTimes > 0 && trace.sideHit)
				{
					position = trace.hitPointPos - motion.normalizedCopy() * 0.05f;
					motion = motion.Reflect(trace.sideHit->getDirectionVec());
					m_bounceTimes--;
				}
				else
				{
					motion = trace.hitPointPos - position;
					position = trace.hitPointPos - motion.normalizedCopy() * 0.05f;
					m_hasBeenDead = true;
				}
			}
			trySendGunFireExplosionResult(trace.hitPointPos);
		}
		else
		{
			position += motion;
			m_shootingRange -= motion.len();
			if (m_shootingRange <= 0.0f)
			{
				if (m_gunSetting->isFireworks)
				{
					trySendGunFireExplosionResult(position);
				}
				setDead();
			}
		}

		setPosition(position);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
		}
	}

	Vector3 EntityBulletClient::calcPosition(EntityLivingBase* shooting, GunSetting* gunSetting, float rdt)
	{
		if (!shooting || !gunSetting)
		{
			return Vector3::ZERO;
		}

		m_isFirstPerspective = shooting == Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST;
		Camera* mainCamera = SceneManager::Instance()->getMainCamera();
		Vector3 pos, dir, right, down;
		float frontOff, rightOff, downOff;
		if (m_isFirstPerspective)
		{
			pos = mainCamera->getPosition();

			dir = mainCamera->getDirection();
			right = mainCamera->getRight();
			down = -mainCamera->getUp();

			frontOff = gunSetting->flame_1_frontOff;
			rightOff = gunSetting->flame_1_rightOff;
			downOff = gunSetting->flame_1_downOff;
		}
		else
		{
			pos = shooting->getPosition(rdt);
			pos.y += shooting->getEyeHeight();

			if (shooting == (EntityPlayer*)(Blockman::Instance()->m_pPlayer))
			{
				pos.y += 0.08f;
			}

			float yaw = shooting->prevRenderYawOffset + (shooting->renderYawOffset - shooting->prevRenderYawOffset) * rdt;
			float pitch = 0.f;

			dir.x = -float(Math::Sin(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.z = float(Math::Cos(double(yaw) * Math::DEG2RAD) * Math::Cos(double(pitch) * Math::DEG2RAD));
			dir.y = -float(Math::Sin(double(pitch) * Math::DEG2RAD));
			right = Vector3::NEG_UNIT_Y.cross(dir);
			down = Vector3::NEG_UNIT_Y;

			frontOff = gunSetting->flame_3_frontOff;
			rightOff = gunSetting->flame_3_rightOff;
			downOff = gunSetting->flame_3_downOff;
		}

		pos += dir * frontOff;
		pos += right * rightOff;
		pos += down * downOff;

		return pos;
	}

	void EntityBulletClient::trySendGunFireResult(RayTraceResult trace)
	{
		if (!m_isShootingByMe)
			return;

		if (!trace.result)
			return;

		Entity* entityHit = trace.entityHit;
		if (entityHit)
		{
			bool isHeadshot = false;
			if (m_isShootingByMe && LogicSetting::Instance()->getAllowHeadshotStatus() && entityHit->isClass(ENTITY_CLASS_PLAYER))
			{
				EntityPlayer* pTarget = dynamic_cast<EntityPlayer*>(entityHit);
				EntityPlayer* pShooting = dynamic_cast<EntityPlayer*>(m_shootingEntity);
				if (pTarget && pShooting && (LogicSetting::Instance()->getShowGunEffectWithSingleStatus() || pTarget->getTeamId() != pShooting->getTeamId()))
				{
					isHeadshot = world->isEntityHeadWithinRayTracy(Box(Vector3(entityHit->boundingBox.vMin.x,
						entityHit->boundingBox.vMin.y + entityHit->getHeight() * 5 / 7, entityHit->boundingBox.vMin.z),
						entityHit->boundingBox.vMax), m_begin, motion.normalizedCopy(), m_begin.distanceTo(m_blockEnd)); // head:body = 1:2.5
				}
			}
			int traceType = NONE_TRACE_TYPE;
			if (trace.entityHit->isClass(ENTITY_CLASS_PLAYER))
			{
				traceType = ENTITY_PLAYER_TRACE_TYPE;
			}
			else if (trace.entityHit->isClass(ENTITY_CLASS_CREATURE))
			{
				traceType = ENTITY_CREATUREAI_TRACE_TYPE;
			}
			else if (trace.entityHit->isClass(ENTITY_CLASS_ACTOR_NPC))
			{
				traceType = ENTITY_ACTOR_NPC_TRACE_TYPE;
			}
			else if (trace.entityHit->isClass(ENTITY_CLASS_FLYING_MOUNT))
			{
				traceType = ENTITY_FLYING_MOUNT_TRACE_TYPE;
			}
			else if (trace.entityHit->isClass(ENTITY_CLASS_FLOATING_MOUNT))
			{
				traceType = ENTITY_FLOATING_MOUNT_TRACE_TYPE;
			}
			int targetId = ClientNetwork::Instance()->getDataCache()->getServerId(trace.entityHit->entityId);
			ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(targetId, trace.hitPointPos, motion.normalizedCopy(), getGunID(), traceType, trace.getBlockPos(), isHeadshot);
		}
		else
		{
			if (world->getBlockId(trace.getBlockPos()) > 0)
			{
				ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(0, trace.hitPointPos, motion.normalizedCopy(), getGunID(), BLOCK_TRACE_TYPE, trace.getBlockPos());
			}
		}
	}

	void EntityBulletClient::trySendGunFireExplosionResult(const Vector3& hitPos)
	{
		if (!m_isShootingByMe)
			return;
		if (m_gunSetting->explosionRange <= 0.00001f)
			return;
		if (m_gunSetting->explosionEffect.size())
		{
			ClientNetwork::Instance()->getSender()->sendEffectGunFireExplosionResult(hitPos, m_gunSetting->gunId);
		}
	}

	RayTraceResult EntityBulletClient::getRayTraceAtBlock()
	{
		/*
		// The first method
		m_begin = position;
		m_entityEnd = position + motion;
		m_blockEnd = position + motion;
		RayTraceResult trace = world->rayTraceBlocks(m_begin, m_blockEnd, false, true);
		m_minDis = m_shootingRange;
		m_ratio = 1.0f;
		if (trace.result)
		{
			m_blockEnd = trace.hitVec;
			m_minDis = m_begin.distanceTo(m_blockEnd);
			if (m_minDis <= motion.len() && motion.len() > 0.00001f)
			{
				m_ratio = m_minDis / motion.len();
			}
			else
			{
				m_minDis = m_shootingRange;
			}
		}
		*/

		// The second method
		m_begin = position;
		m_entityEnd = position + motion;
		m_blockEnd = position + motion;
		m_minDis = m_shootingRange;
		m_ratio = 1.0f;
		RayTraceResult trace;

		if (!m_aimFailed)
		{
			m_aimDis -= motion.len();
			if (m_aimDis < 0.00001f)
			{
				if (m_hasBlockTarget && world->getBlockId(m_blockTargetPos) > 0)
				{
					int blockId = world->getBlockId(m_blockTargetPos);
					Block* pBlock = BlockManager::sBlocks[blockId];
					trace = pBlock->collisionRayTrace(world, m_blockTargetPos, m_begin, m_blockEnd);

					if (!trace.result)
					{
						trace = world->rayTraceBlocks(m_begin + motion.normalizedCopy() * (m_aimDis + motion.len()), m_blockEnd, false, true);
					}

					if (trace.result)
					{
						m_blockEnd = trace.hitPointPos;
						m_minDis = m_begin.distanceTo(m_blockEnd);
						if (m_minDis <= motion.len() && motion.len() > 0.00001f)
						{
							m_ratio = m_minDis / motion.len();
						}
						else
						{
							m_minDis = m_shootingRange;
						}
					}
				}
				else
				{
					trace = world->rayTraceBlocks(m_begin + motion.normalizedCopy() * (m_aimDis + motion.len()), m_blockEnd, false, true);
					if (trace.result)
					{
						m_blockEnd = trace.hitPointPos;
						m_minDis = m_begin.distanceTo(m_blockEnd);
						if (m_minDis <= motion.len() && motion.len() > 0.00001f)
						{
							m_ratio = m_minDis / motion.len();
						}
						else
						{
							m_minDis = m_shootingRange;
						}
					}

				}
				m_aimFailed = true;
			}

		}
		else
		{
			trace = world->rayTraceBlocks(m_begin, m_blockEnd, false, true);
			if (trace.result)
			{
				m_blockEnd = trace.hitPointPos;
				m_minDis = m_begin.distanceTo(m_blockEnd);
				if (m_minDis <= motion.len() && motion.len() > 0.00001f)
				{
					m_ratio = m_minDis / motion.len();
				}
				else
				{
					m_minDis = m_shootingRange;
				}
			}
		}

		return trace;
	}

	//------------------------------------------------------------------------

	EntityMortarBulletClient::EntityMortarBulletClient(World * pWorld, EntityLivingBase * pShooting, GunSetting * pSetting, const Vector3 & endPos, bool hasBlockTarget, const Vector3i & blockTargetPos)
		: EntityBulletClient(pWorld, pShooting, pSetting, endPos, hasBlockTarget, blockTargetPos)
	{
		Mesh* pMesh = MeshManager::Instance()->createMesh("bullet_mortar.mesh");
		pMesh->load();
		ModelEntity* entity = LordNew ModelEntity;
		SceneNode* node = SceneManager::Instance()->getRootNode()->createChild();

		Quaternion quatYaw = Quaternion(Vector3::UNIT_Y, -(pShooting->rotationYaw + 90.0f) * Math::DEG2RAD);
		Quaternion quatPitch = Quaternion(Vector3::UNIT_Z, -pShooting->rotationPitch * Math::DEG2RAD);
		node->setLocalOrientation(quatYaw * quatPitch);
		node->setLocalScaling(Vector3(0.7f, 0.7f, 0.7f));

		entity->setMesh(pMesh);
		entity->attachTo(node);
		entity->setRenderType(ModelEntity::RT_FOR_ACTOR_TRANSPARENT);
		entity->setMaterial(ActorManager::Instance()->GetStaticMaterial_transparent());
		entity->createRenderable(true);
		entity->prepareTexture(true);
		m_modelEntity = entity;

		m_bulletType = BT_MORTAR;
	}

	EntityMortarBulletClient::~EntityMortarBulletClient()
	{
		if (m_modelEntity != nullptr)
		{
			SceneNode* node = m_modelEntity->getSceneNode();
			Mesh* mesh = m_modelEntity->getMesh();
			SceneManager::Instance()->getRootNode()->destroyChild(node);
			MeshManager::Instance()->releaseResource(mesh);
			LordDelete(m_modelEntity);
			m_modelEntity = nullptr;
		}
	}

	void EntityMortarBulletClient::onUpdate()
	{
		if (m_hasBeenDead)
		{
			setDead();
			return;
		}

		Entity::onUpdate();

		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		motion.y -= m_gunSetting->gravityVelocity;
		float motion_xz = Math::Sqrt(motion.x * motion.x + motion.z * motion.z);
		rotationYaw = Math::ATan2(motion.x, motion.z) * Math::RAD2DEG;
		rotationPitch = Math::ATan2(motion.y, motion_xz) * Math::RAD2DEG;

		RayTraceResult trace = getRayTraceAtBlock();

		Entity* tracyEntity = nullptr;
		Vector3 hitPos = Vector3::ZERO;
		EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * m_ratio, motion.y * m_ratio, motion.z * m_ratio));
		bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
		for (auto entity : entities)
		{
			if (!isGunShootActor && entity->isClass(ENTITY_CLASS_ACTOR_NPC))
			{
				continue;
			}
			if (entity && entity->canBePushed() && m_begin.distanceTo(entity->position) <= m_shootingRange && entity->entityId != m_shootingEntityId)
			{
				Box entity_box = entity->boundingBox;
				RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_begin, m_entityEnd);
				if (tracy_entity.result)
				{
					float dis = m_begin.distanceTo(tracy_entity.hitPointPos);
					if (dis < m_minDis)
					{
						tracyEntity = entity;
						hitPos = tracy_entity.hitPointPos;
						m_minDis = dis;
					}
				}
			}
		}

		if (tracyEntity)
		{
			trace.set(tracyEntity, hitPos);
		}

		if (trace.result)
		{
			trySendGunFireResult(trace);
			if (m_isShootingByMe)
			{
				Box box = Box(trace.hitPointPos, trace.hitPointPos);
				int hitEntityId = -1;
				if (trace.entityHit)
				{
					box = trace.entityHit->boundingBox;
					hitEntityId = trace.entityHit->entityId;
				}

				float explosionRange = m_gunSetting->explosionRange;
				if (explosionRange > 0.00001f)
				{
					trySendGunFireExplosionResult(trace.hitPointPos);
					EntityArr explosionEntities = world->getEntitiesWithinAABBExcludingEntity(this, box.contract(-explosionRange, -explosionRange, -explosionRange));
					for (auto entity : explosionEntities)
					{
						if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId && entity->entityId != hitEntityId)
						{
							RayTraceResult tracy_entity;
							if (entity->isClass(ENTITY_CLASS_PLAYER))
							{
								EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
								if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
								{
									continue;
								}
							}
							tracy_entity.set(entity, entity->position);
							tracy_entity.blockPos = trace.getBlockPos();
							trySendGunFireResult(tracy_entity);
						}
					}

					Vector3i centerPos = trace.hitPointPos.getFloor();
					int range = int(Math::Floor(explosionRange));
					if (range != 0)
					{
						for (int x = -range; x <= range; x++)
						{
							for (int y = -range; y <= range; y++)
							{
								for (int z = -range; z <= range; z++)
								{
									Vector3i pos = Vector3i(centerPos.x + x, centerPos.y + y, centerPos.z + z);
									if (world->getBlockId(pos) > 0)
									{
										ClientNetwork::Instance()->getSender()->sendEffectGunFireResult(0, trace.hitPointPos, motion.normalizedCopy(), getGunID(), BLOCK_TRACE_TYPE, pos);
									}
								}
							}
						}
					}
				}
			}

			motion = trace.hitPointPos - position;
			position = trace.hitPointPos - motion.normalizedCopy() * 0.05f;
			m_hasBeenDead = true;
		}
		else
		{
			position += motion;
			m_shootingRange -= motion.len();
			if (m_shootingRange <= 0.f)
			{
				setDead();
			}
		}

		setPosition(position);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
		}
	}

	//------------------------------------------------------------------------

	EntityLaserBulletClient::EntityLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3 & endPos, bool hasBlockTarget, const Vector3i & blockTargetPos, float contractiveValue)
		: EntityBulletClient(pWorld, pShooting, pSetting, endPos, hasBlockTarget, blockTargetPos)
	{
		setSize(1.0f, 1.0f);

		m_laserBeginPos = position;
		motion = (endPos - position).normalizedCopy() * m_shootingRange;
		m_laserEndPos = m_laserBeginPos + motion;
		m_lastLaserEndPos = m_laserEndPos;

		m_bulletEffectName = pSetting->bulletEffect != "effect_gun_default_bullet.effect" ? pSetting->bulletEffect : "effect_gun_laser_blue.effect";
		m_startEffectName = m_bulletEffectName.length() > 7 ? m_bulletEffectName.substr(0, m_bulletEffectName.length() - 7) + "_flame" + m_bulletEffectName.substr(m_bulletEffectName.length() - 7, m_bulletEffectName.length()) : "effect_gun_laser_blue_flame.effect";
		m_contractiveValue = contractiveValue;
		m_bulletType = BT_LASER;

		m_currTime = Root::Instance()->getCurrentTime();
		m_sound_type = pSetting->soundType;
		m_lastConsumeBulletTime = m_currTime;
	}

	EntityLaserBulletClient::~EntityLaserBulletClient()
	{
		if (m_needDeleteEffect && m_startEffect != nullptr)
		{
			m_startEffect->mDuration = 0;
			m_startEffect = nullptr;
		}
	}

	void EntityLaserBulletClient::onUpdate()
	{
		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		if (!m_shootingEntity)
		{
			ClientNetwork::Instance()->getSender()->sendStopLaserGun();
			setDead();
			return;
		}

		m_lastLaserEndPos = m_laserEndPos;

		m_record_time += (Root::Instance()->getCurrentTime() - m_currTime);

		if (m_record_time > 1130)
		{
			m_record_time = 0;
		}

		if (m_record_time == 0)
		{
			playSoundByType((SoundType)m_sound_type);
		}

		m_currTime = Root::Instance()->getCurrentTime();
		if (m_isShootingByMe)
		{
			if (m_currTime - m_lastConsumeBulletTime >= m_gunSetting->cdTime * 1000)
			{
				auto pPlayer = Blockman::Instance()->m_pPlayer;
				ItemStackPtr stack = pPlayer->inventory->getCurrentItem();

				if (!stack || stack->stackSize < 1)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}

				ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
				if (!pGun)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}

				const GunSetting* pGunSetting = pGun->getGunSetting();
				if (!pGunSetting || pGunSetting->gunId != m_gunID)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}

				int bulletNum = stack->getCurrentClipBulletNum();
				if (bulletNum < 1)
				{
					ClientNetwork::Instance()->getSender()->sendStopLaserGun();
					setDead();
					return;
				}
				else
				{
					stack->consumeBullet();
					ClientNetwork::Instance()->getSender()->sendConsumeBullet();
				}

				m_lastConsumeBulletTime = m_currTime;
				m_canHurt = true;
			}
		}

		setPosition(calcPosition(m_shootingEntity, m_gunSetting, 0.0f));
		Entity::onUpdate();

		rotationYaw = m_shootingEntity->rotationYaw;
		rotationPitch = m_shootingEntity->rotationPitch;

		Vector3 dir;
		dir.x = -float(Math::Sin(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));
		dir.y = -float(Math::Sin(double(rotationPitch) * Math::DEG2RAD));
		dir.z = float(Math::Cos(double(rotationYaw) * Math::DEG2RAD) * Math::Cos(double(rotationPitch) * Math::DEG2RAD));

		Vector3 beginPos;
		if (m_isShootingByMe)
		{
			beginPos = m_shootingEntity->position + Blockman::Instance()->m_pPlayer->m_thirdPersonCameraOffset;// position of crossHair
		}
		else
		{
			beginPos = calcPosition(m_shootingEntity, m_gunSetting, 0.0f);
		}
		Vector3 endPos = beginPos + dir * m_shootingRange;

		RayTraceResult trace = world->rayTraceBlocks(beginPos, endPos, false, true);
		m_minDis = m_shootingRange;
		m_ratio = 1.0f;
		if (trace.result)
		{
			endPos = trace.hitPointPos;
			m_minDis = beginPos.distanceTo(endPos);
			if (m_minDis <= m_shootingRange && m_shootingRange > 0.00001f)
			{
				m_ratio = m_minDis / m_shootingRange;
			}
			else
			{
				m_minDis = m_shootingRange;
			}
		}

		Entity* tracyEntity = nullptr;
		EntityArr playerEntities = world->getEntitiesWithinRayTracy(m_shootingEntity, ENTITY_CLASS_ENTITY, beginPos, dir, m_shootingRange * m_ratio);
		bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
		for (int i = 0; i < int(playerEntities.size()); ++i)
		{
			Entity* pEntity = playerEntities[i];
			if (!isGunShootActor && pEntity->isClass(ENTITY_CLASS_ACTOR_NPC))
			{
				continue;
			}
			float distance = beginPos.distanceTo(pEntity->position);
			if (pEntity->canBePushed() && distance <= m_minDis && pEntity != m_shootingEntity)
			{
				tracyEntity = pEntity;
				m_minDis = distance;
			}
		}

		if (tracyEntity)
		{
			LORD::Ray ray(beginPos, endPos - beginPos);
			LORD::HitInfo hitinfo;
			float trim;
			const Box& aabb = tracyEntity->boundingBox;
			bool hit = ray.hitBox(aabb, trim, hitinfo);
			if (hit)
			{
				endPos = hitinfo.hitPos;
			}
		}

		m_laserBeginPos = calcPosition(m_shootingEntity, m_gunSetting, 1.0f);
		motion = (endPos - m_laserBeginPos).normalizedCopy() * m_shootingRange;
		m_laserEndPos = m_laserBeginPos + motion;

		trace = world->rayTraceBlocks(m_laserBeginPos, m_laserEndPos, false, true);
		m_minDis = m_shootingRange;
		m_ratio = 1.0f;
		if (trace.result)
		{
			m_laserEndPos = trace.hitPointPos;
			m_minDis = m_laserBeginPos.distanceTo(m_laserEndPos);
			if (m_minDis <= motion.len() && motion.len() > 0.00001f)
			{
				m_ratio = m_minDis / motion.len();
			}
			else
			{
				m_minDis = m_shootingRange;
			}
		}

		if (m_isShootingByMe)
		{
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * m_ratio, motion.y * m_ratio, motion.z * m_ratio));
			bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
			for (auto entity : entities)
			{
				if (!isGunShootActor && entity->isClass(ENTITY_CLASS_ACTOR_NPC))
				{
					continue;
				}
				if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId)
				{
					Box entity_box = entity->boundingBox;
					RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box.contract(-m_contractiveValue), m_laserBeginPos, m_laserBeginPos + motion);
					if (tracy_entity.result && m_canHurt)
					{
						if (entity->isClass(ENTITY_CLASS_PLAYER))
						{
							EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
							if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
							{
								continue;
							}
						}
						tracy_entity.set(entity, tracy_entity.hitPointPos);
						tracy_entity.blockPos = trace.getBlockPos();
						trySendGunFireResult(tracy_entity);
						m_canHurt = false;
					}
				}
			}
		}

		setPosition(m_laserBeginPos);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
			m_bulletEffect->setExtenalParam(position, m_laserEndPos);
			if (m_startEffect)
			{
				m_startEffect->mPosition = position;
			}
		}
	}

	//------------------------------------------------------------------------

	void EntityArmorPiercingBulletClient::onUpdate()
	{
		if (m_hasBeenDead)
		{
			setDead();
			return;
		}

		Entity::onUpdate();

		if (m_shootingEntity && m_shootingEntity->isDead)
		{
			m_shootingEntity = nullptr;
		}

		RayTraceResult trace = getRayTraceAtBlock();

		if (m_isShootingByMe)
		{
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * m_ratio, motion.y * m_ratio, motion.z * m_ratio));
			bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
			for (auto entity : entities)
			{
				if (!isGunShootActor && entity->isClass(ENTITY_CLASS_ACTOR_NPC))
				{
					continue;
				}
				if (entity && entity->canBePushed() && m_begin.distanceTo(entity->position) <= m_shootingRange && entity->entityId != m_shootingEntityId)
				{
					auto iter = std::find(hitEntities.begin(), hitEntities.end(), entity->entityId);
					if (iter == hitEntities.end())
					{
						Box entity_box = entity->boundingBox;
						RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_begin, m_entityEnd);
						if (tracy_entity.result)
						{
							if (entity->isClass(ENTITY_CLASS_PLAYER))
							{
								EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
								if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
								{
									continue;
								}
							}
							tracy_entity.set(entity, tracy_entity.hitPointPos);
							tracy_entity.blockPos = trace.getBlockPos();
							trySendGunFireResult(tracy_entity);
							hitEntities.push_back(entity->entityId);
						}
					}
				}
			}
		}

		if (trace.result && trace.typeOfHit == RAYTRACE_TYPE_BLOCK)
		{
			trySendGunFireResult(trace);

			if (m_bounceTimes > 0 && trace.sideHit)
			{
				position = trace.hitPointPos - motion.normalizedCopy() * 0.05f;
				motion = motion.Reflect(trace.sideHit->getDirectionVec());
				m_bounceTimes--;
			}
			else
			{
				motion = trace.hitPointPos - position;
				position = trace.hitPointPos - motion.normalizedCopy() * 0.05f;
				m_hasBeenDead = true;
			}
		}
		else
		{
			position += motion;
			m_shootingRange -= motion.len();
			if (m_shootingRange <= 0.0f)
			{
				setDead();
			}
		}

		setPosition(position);

		if (m_bulletEffect)
		{
			m_bulletEffect->mPosition = position;
		}
	}

	//------------------------------------------------------------------------

	EntityMomentaryLaserBulletClient::EntityMomentaryLaserBulletClient(World* pWorld, EntityLivingBase* pShooting, GunSetting* pSetting, const Vector3& endPos, bool hasBlockTarget, const Vector3i& blockTargetPos)
		: EntityLaserBulletClient(pWorld, pShooting, pSetting, endPos, hasBlockTarget, blockTargetPos)
	{
		m_bulletType = BT_MOMENTARYLASER;
		m_needDeleteEffect = false;
	}

	void EntityMomentaryLaserBulletClient::onUpdate()
	{
		if (!m_hasUsed)
		{
			m_hasUsed = true;
			Entity::onUpdate();

			RayTraceResult trace = getRayTraceAtBlock();
			if (trace.result)
			{
				m_laserEndPos = trace.hitPointPos;
			}

			Entity* tracyEntity = nullptr;
			Vector3 hitPos = Vector3::ZERO;
			EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * m_ratio, motion.y * m_ratio, motion.z * m_ratio));
			bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
			for (auto entity : entities)
			{
				if (!isGunShootActor && entity->isClass(ENTITY_CLASS_ACTOR_NPC))
				{
					continue;
				}
				if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId)
				{
					Box entity_box = entity->boundingBox;
					RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_laserBeginPos, m_laserBeginPos + motion);
					if (tracy_entity.result)
					{
						float dis = m_laserBeginPos.distanceTo(tracy_entity.hitPointPos);
						if (dis < m_minDis)
						{
							tracyEntity = entity;
							hitPos = tracy_entity.hitPointPos;
							m_minDis = dis;
						}
					}
				}
			}
			if (tracyEntity)
			{
				trace.set(tracyEntity, hitPos);
				m_laserEndPos = hitPos;
			}

			trySendGunFireResult(trace);
		}
		else if (m_existenceCnt >= 20)
		{
			setDead();
		}
		else
		{
			m_existenceCnt++;
		}
	}

	//------------------------------------------------------------------------

	void EntityMomentaryPiercingLaserBulletClient::onUpdate()
	{
		if (!m_hasUsed)
		{
			m_hasUsed = true;
			Entity::onUpdate();

			RayTraceResult trace = getRayTraceAtBlock();
			if (trace.result)
			{
				m_laserEndPos = trace.hitPointPos;
			}

			if (m_isShootingByMe)
			{
				EntityArr entities = world->getEntitiesWithinAABBExcludingEntity(this, boundingBox.addCoord(motion.x * m_ratio, motion.y * m_ratio, motion.z * m_ratio));
				bool isGunShootActor = PlayerPrefs::Instance()->getBoolPrefs("IsGunShootActor");
				for (auto entity : entities)
				{
					if (!isGunShootActor && entity->isClass(ENTITY_CLASS_ACTOR_NPC))
					{
						continue;
					}
					if (entity && entity->canBePushed() && entity->entityId != m_shootingEntityId)
					{
						Box entity_box = entity->boundingBox;
						RayTraceResult tracy_entity = RayTraceResult::calculateIntercept(entity_box, m_laserBeginPos, m_laserBeginPos + motion);
						if (tracy_entity.result)
						{
							if (entity->isClass(ENTITY_CLASS_PLAYER))
							{
								EntityPlayer* tracyPlayer = dynamic_cast<EntityPlayer*>(entity);
								if (tracyPlayer && tracyPlayer->capabilities.disableDamage)
								{
									continue;
								}
							}
							tracy_entity.set(entity, tracy_entity.hitPointPos);
							tracy_entity.blockPos = trace.getBlockPos();
							trySendGunFireResult(tracy_entity);
						}
					}
				}
			}
		}
		else if (m_existenceCnt >= 20)
		{
			setDead();
		}
		else
		{
			m_existenceCnt++;
		}
	}

}