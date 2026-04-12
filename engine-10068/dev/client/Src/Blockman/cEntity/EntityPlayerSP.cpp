#include "EntityPlayerSP.h"
#include "Entity/EntityVehicle.h"
#include "Entity/EntityActorNpc.h"
#include "Entity/EntitySessionNpc.h"
#include "Entity/EntityCreature.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "cEntity/PlayerControlller.h"
#include "cWorld/Blockman.h"
#include "cWorld/PlayerInteractionManager.h"
#include "Render/RenderEntity.h"
#include "Render/RenderGlobal.h"
#include "Game/BirdSimulator.h"

/** header files from common. */
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Item/Item.h"
#include "Item/Items.h"
#include "Item/ItemStack.h"
#include "Item/Potion.h"
#include "Entity/DamageSource.h"
#include "Inventory/InventoryPlayer.h"
#include "World/World.h"
#include "World/GameSettings.h"
#include "Network/ClientNetwork.h"
#include "Enums/Enums.h"
#include "Audio/SoundSystem.h"
#include "Util/LanguageKey.h"
#include "Util/LanguageManager.h"
#include "Util/ClientEvents.h"
#include "Util/UICommon.h"
#include "Util/PlayerPrefs.h"
#include "Setting/LogicSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Entity/EntityBuildNpc.h"
#include "game.h"
#include "GUI/RootGuiLayout.h"
#include "cWorld/WorldClient.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Plugins/GameClientEvents.h"

namespace BLOCKMAN
{

	MovementInputFromOptions::MovementInputFromOptions(GameSettings* pGameSettings)
		: gameSettings(pGameSettings)
	{
		lastSneak	= false;
		sneak		= false;
		isSprintInput		= false;
	}

	void MovementInputFromOptions::updatePlayerMoveState(EntityPlayer* player)
	{
		gameSettings->updateClientInput();
		if (player->isOnDecoration() && !player->m_isCameraLocked || player->isUseElevator())
		{
			gameSettings->m_clientMovementInput->moveStrafe = 0.0F;
			gameSettings->m_clientMovementInput->moveForward = 0.0F;
		}

		if (Blockman::Instance()->m_entityRender)
		{
			CameraController cameraCtrl = Blockman::Instance()->m_entityRender->m_cameraController;
			if (cameraCtrl.isSeparate())
			{
				float fStrafe = gameSettings->m_clientMovementInput->moveStrafe;
				float fForward = gameSettings->m_clientMovementInput->moveForward;
				if (Math::Abs(fStrafe) > 0.0f || Math::Abs(fForward) > 0.0f)
				{
					float dYaw = Math::ATan2(-fStrafe, fForward) * Math::RAD2DEG;
					player->rotationYaw = cameraCtrl.getCameraYaw() + dYaw;
					player->rotationPitch = cameraCtrl.getCameraPitch();
					gameSettings->m_clientMovementInput->moveStrafe = 0.0F;
					gameSettings->m_clientMovementInput->moveForward = Math::Sqrt(fStrafe * fStrafe + fForward * fForward);
				}
			}
		}

		sneak = gameSettings->m_clientMovementInput->sneak;
		if (isSprintInput != gameSettings->m_clientMovementInput->isSprintInput)
		{
			isSprintInput = gameSettings->m_clientMovementInput->isSprintInput;
			ClientNetwork::Instance()->getSender()->sendPlayerSprint(isSprintInput);
		}


		/* apply input immediately */
		moveForward = gameSettings->m_clientMovementInput->moveForward;
		moveStrafe = gameSettings->m_clientMovementInput->moveStrafe;
		down = gameSettings->m_clientMovementInput->down;
		jump = gameSettings->m_clientMovementInput->jump;

		if (lastSneak != sneak) {
			if (sneak) {
				// send start sneak action to server
				ClientNetwork::Instance()->getSender()->sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::START_SNEAK);
			}
			else {
				// send stop sneak action to server
				ClientNetwork::Instance()->getSender()->sendPlayerAction(NETWORK_DEFINE::PacketPlayerActionType::STOP_SNEAK);
			}

			lastSneak = sneak;
		}
}

EntityPlayerSP::EntityPlayerSP(Blockman* pbm, World* pWorld, int dim, PlayerInteractionManager* interactionMgr)
	//: EntityPlayer(pWorld, "singlePlayer")
	: EntityPlayer(pWorld, GameClient::CGame::Instance()->getPlayerName())
{
	bm = pbm;
	dimension = dim;

	sprintToggleTimer = 0;
	sprintingTicksLeft = 0;
	renderArmYaw = 0.f;
	renderArmPitch = 0.f;
	prevRenderArmYaw = 0.f;
	prevRenderArmPitch = 0.f;
	timeInPortal = 0.f;
	prevTimeInPortal = 0.f;
	movementInput = NULL;
	autoJumpEnabled = true;
	autoJumpTime = 0;
	m_iterationMgr = NULL;
	isLastFrameFastMoveForward = false;

	if (interactionMgr)
	{
		interactionMgr->playerSP = this;
		interactionMgr->setGameType(pbm->m_worldSettings->getGameType());
		m_iterationMgr = interactionMgr;
	}
}

void EntityPlayerSP::updateEntityActionState()
{
	applyMovementInput();

	if (needUpdateActionState())
	{
		EntityPlayer::updateEntityActionState();
		updateReloadProgress();
	}

	prevRenderArmYaw = renderArmYaw;
	prevRenderArmPitch = renderArmPitch;
	renderArmPitch = renderArmPitch + (rotationPitch - renderArmPitch) * 0.5f;
	renderArmYaw = renderArmYaw + (rotationYaw - renderArmYaw) * 0.5f;
}

void EntityPlayerSP::onUpdate()
{
	if(m_iterationMgr)
		m_iterationMgr->updateBlockRemoving();
	EntityPlayer::onUpdate();
	onUpdateDefendProgress();

	auto faceDir = this->getLookVec();
	faceDir.y = 0;
	faceDir.normalize();
	SoundSystem::Instance()->update3DSoundListenerPos(position, faceDir);
}

void EntityPlayerSP::onLivingUpdate()
{
	if (sprintingTicksLeft > 0)
	{
		--sprintingTicksLeft;

		if (sprintingTicksLeft == 0)
		{
			setSprinting(false);
		}
	}

	if (sprintToggleTimer > 0)
	{
		--sprintToggleTimer;
	}

	if (bm->m_playerControl->enableEverythingIsScrewedUpMode())
	{
		position.x = 0.0f;
		position.z = 0.0f;
		rotationYaw = ticksExisted / 12.0F;
		rotationPitch = 10.0F;
		position.y = 68.5f;
	}
	else
	{
		if (bm->m_playerControl->getDisableMovement())
		{
			KeyBinding::unPressAllKeys();
			if (!bm->m_playerControl->getPerspectiveMove())
				Blockman::Instance()->m_gameSettings->setMouseMoveState(false);
			Blockman::Instance()->m_gameSettings->m_poleForward = 0.0f;
			Blockman::Instance()->m_gameSettings->m_poleStrafe = 0.0f;
		}

		prevTimeInPortal = timeInPortal;

		if (isPotionActive(Potion::confusion) && getActivePotionEffect(Potion::confusion)->getDuration() > 60)
		{
			timeInPortal += 0.006666667F;

			if (timeInPortal > 1.0F)
			{
				timeInPortal = 1.0F;
			}
		}
		else
		{
			if (timeInPortal > 0.0F)
			{
				timeInPortal -= 0.05F;
			}

			if (timeInPortal < 0.0F)
			{
				timeInPortal = 0.0F;
			}
		}

		if (timeUntilPortal > 0)
		{
			--timeUntilPortal;
		}

		bool isjump = movementInput->jump;
		float movespeed = 0.8F;
		movementInput->updatePlayerMoveState(this);
		auto pClientInput = Blockman::Instance()->m_gameSettings->m_clientMovementInput;
		bool isCurFrameFastMoveForward = movementInput->moveForward >= movespeed;
		if (isUsingItem())
		{
			pClientInput->moveStrafe *= 0.2F;
			pClientInput->moveForward *= 0.2F;
			sprintToggleTimer = 0;
		}

		if (movementInput->sneak && m_yPositionDecrement < 0.2F)
		{
			m_yPositionDecrement = 0.2F;
		}

		bool flag3 = false;
		/*
		if (autoJumpTime > 0)
		{
			--autoJumpTime;
			flag3 = true;
			movementInput->jump = true;
		}
		*/

		Vector3 playerPos = getWidelyPos();
		float f = 0.35f;
		pushOutOfBlocks(playerPos + Vector3(-width*f, 0, width*f));
		pushOutOfBlocks(playerPos + Vector3(-width*f, 0, -width*f));
		pushOutOfBlocks(playerPos + Vector3(width*f, 0, -width*f));
		pushOutOfBlocks(playerPos + Vector3(width*f, 0, width*f));
		bool hasEnoughEnergy = getFoodStats()->getFoodLevel() > 6.0F || capabilities.allowFlying;

		if ((onGround && isCurFrameFastMoveForward && !isSprinting() && hasEnoughEnergy && !isUsingItem() && !isPotionActive(Potion::blindness))
			&& (movementInput->isSprintInput || !isLastFrameFastMoveForward))
		{
			if (sprintToggleTimer == 0)
			{
				sprintToggleTimer = 7;
			}
			else
			{
				setSprinting(true);
				sprintToggleTimer = 0;
			}
		}

		isLastFrameFastMoveForward = isCurFrameFastMoveForward;

		if (isSneaking())
		{
			sprintToggleTimer = 0;
		}

		if (isSprinting() && (!isCurFrameFastMoveForward || isCollidedHorizontally || !hasEnoughEnergy))
		{
			setSprinting(false);
		}

		if (isOnVehicle() && isjump)
		{
			askGetOffVehicle();
		}

		if (isOnDecoration() && isjump)
		{
			ClientNetwork::Instance()->getCommonSender()->sendInteractionWithDecoration(m_decorationEntityId, "");
		}

		if (isOnVehicle() && isjump && !isDriver())
		{
			askGetOffVehicle();
		}

		EntityPlayer::onLivingUpdate();
	}
}

float EntityPlayerSP::getFOVMultiplier()
{
	float fovScale = 1.0F;

	if (capabilities.isFlying)
	{
		fovScale *= 1.1F;
	}

	IAttributeInstance* att_movespeed = getEntityAttribute(SharedMonsterAttributes::MOVEMENT_SPEED);
	fovScale = fovScale * ((att_movespeed->getAttributeValue() / capabilities.getWalkSpeed() + 1.0f) / 2.0f);

	if (isUsingItem() && dynamic_cast<ItemBow*>(getItemInUse()->getItem()))
	{
		int duration = getItemInUseDuration();
		float duration_sec = duration / 20.0F;

		if (duration_sec > 1.0F)
		{
			duration_sec = 1.0F;
		}
		else
		{
			duration_sec *= duration_sec;
		}

		fovScale *= 1.0F - duration_sec * 0.15F;
	}

	return fovScale;
}

void EntityPlayerSP::closeScreen()
{
	EntityPlayer::closeScreen();
	// bm.displayGuiScreen((GuiScreen)null);
}

void EntityPlayerSP::onCriticalHit(Entity* pEntity)
{
	// bm.effectRenderer.addEffect(new EntityCrit2FX(bm.theWorld, par1Entity));
}

void EntityPlayerSP::onEnchantmentCritical(Entity* pEntity)
{
	// EntityCrit2FX var2 = new EntityCrit2FX(bm.theWorld, par1Entity, "magicCrit");
	// bm.effectRenderer.addEffect(var2);
}

void EntityPlayerSP::setHealth(float hp)
{
	float damageAmount = getHealth() - hp;

	if (damageAmount <= 0.0F)
	{
		setEntityHealth(hp);

		if (damageAmount < 0.0F)
		{
			hurtResistantTime = maxHurtResistantTime / 2;
		}
	}
	else
	{
		lastDamage = damageAmount;
		setEntityHealth(getHealth());
		hurtResistantTime = maxHurtResistantTime;
		damageEntity(DamageSource::generic, damageAmount);
		hurtTime = maxHurtTime = 10;
	}
}

void EntityPlayerSP::addChatMessage(const String& str)
{
	// bm.ingameGUI.getChatGUI().addTranslatedMessage(par1Str, new Object[0]);
}

bool EntityPlayerSP::canBlockPushOutEntity(const BlockPos& pos)
{
	if (!world->isBlockNormalCube(pos))
	{
		return false;
	}
	auto collided = false;
	auto block = BlockManager::sBlocks[world->getBlockId(pos)];
	if (!block)
	{
		return false;
	}
	AABBList collisionBoxList; 
	block->addCollisionBoxesToList(world, pos, boundingBox, collisionBoxList, this);
	for (const auto& box : collisionBoxList)
	{
		if (box.isIntersected(boundingBox))
		{
			return true;
		}
	}
	return false;
}

bool EntityPlayerSP::pushOutOfBlocks(const Vector3& vec)
{
	BlockPos pos = vec.getFloor();
	float x_off = vec.x - pos.x;
	float z_off = vec.z - pos.z;
	BlockPos pos1 = pos.getPosY();

	if (canBlockPushOutEntity(pos) || canBlockPushOutEntity(pos1))
	{
		bool stop_w = !canBlockPushOutEntity(pos.getNegX()) && !canBlockPushOutEntity(pos1.getNegX());
		bool stop_e = !canBlockPushOutEntity(pos.getPosX()) && !canBlockPushOutEntity(pos1.getPosX());
		bool stop_n = !canBlockPushOutEntity(pos.getNegZ()) && !canBlockPushOutEntity(pos1.getNegZ());
		bool stop_s = !canBlockPushOutEntity(pos.getPosZ()) && !canBlockPushOutEntity(pos1.getPosZ());
		i8 dir = -1;
		float min_off = 9999.0f;

		if (stop_w && x_off < min_off)
		{
			min_off = x_off;
			dir = 0;
		}

		if (stop_e && 1.0f - x_off < min_off)
		{
			min_off = 1.0f - x_off;
			dir = 1;
		}

		if (stop_n && z_off < min_off)
		{
			min_off = z_off;
			dir = 4;
		}

		if (stop_s && 1.0f - z_off < min_off)
		{
			min_off = 1.0f - z_off;
			dir = 5;
		}

		float push_speed = 0.1F;

		if (dir == 0)
		{
			motion.x = -push_speed;
		}

		if (dir == 1)
		{
			motion.x = push_speed;
		}

		if (dir == 4)
		{
			motion.z = -push_speed;
		}

		if (dir == 5)
		{
			motion.z = push_speed;
		}
	}

	return false;
}

void EntityPlayerSP::setSprinting(bool sprinting)
{
	if (!canSprinting())
		return;
	EntityLivingBase::setSprinting(sprinting);
	ClientNetwork::Instance()->getSender()->sendPlayerAction(
		sprinting ? NETWORK_DEFINE::PacketPlayerActionType::START_SPRINTING : NETWORK_DEFINE::PacketPlayerActionType::STOP_SPRINTING);
	sprintingTicksLeft = sprinting ? 600 : 0;
}

bool EntityPlayerSP::canSprinting()
{
	return PlayerPrefs::Instance()->getBoolPrefs("IsCanSprint");
}

void EntityPlayerSP::setXPStats(float exp, int totalExp, int lvl)
{
	experience = exp;
	experienceTotal = totalExp;
	experienceLevel = lvl;
}

void EntityPlayerSP::gunFire()
{
	if (!isHoldGun)
		return;

	ItemStackPtr gun = inventory->getCurrentItem();
	ItemGun* pgunItem = dynamic_cast<ItemGun*>(gun->getItem());
	if (!pgunItem)
		return;
	pgunItem->fire(gun, world, this);
}

Vector3i EntityPlayerSP::getPlayerCoordinates()
{
	Vector3 pos = position + Vector3(0.5f, 0.5f, 0.5f);
	pos.floor();
	return Vector3i(int(pos.x), int(pos.y), int(pos.z));
}

void EntityPlayerSP::askGetOnVehicle(EntityVehicle* vehicle, bool notRace)
{
	if (m_isActorChange)
	{
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, LanguageKey::GUI_MANOR_EDIT_BAN_GET_ON);
		return;
	}
	if (position.distanceTo(vehicle->position) > 4.f && !(GameClient::CGame::Instance()->GetGameType() == ClientGameType::BlockCity && !notRace))
		return;

	if (isOnVehicle() || isOnDecoration() || isOnFlyingMount() || isOnFloatingMount())
		return;

	bool hasDriver = vehicle->hasDriver();
	if (hasDriver)
	{
		if (vehicle->isLocked())
		{
			String msg = LanguageManager::Instance()->getString(LanguageKey::VEHICLE_LOCKED);
			ChatMessageReceiveEvent::emit(msg, false);
			return;
		}

		if (vehicle->getFreeSeatNum() <= 0)
		{
			String msg = LanguageManager::Instance()->getString(LanguageKey::VEHICLE_FULL_LOADED);
			ChatMessageReceiveEvent::emit(msg, false);
			return;
		}
	}

	if (!hasDriver && !vehicle->canDrivedBy(this))
	{
		ShowVehicleTipDialogEvent::emit(vehicle->getVehicleId(), vehicle->entityId);
		return;
	}
	SoundSystem::Instance()->playEffectByType(ST_take_car);
	ClientNetwork::Instance()->getSender()->sendTakeVehicle(true, vehicle->entityId);
}

void EntityPlayerSP::askGetOffVehicle()
{
	if (!isOnVehicle())
		return;

	ClientNetwork::Instance()->getSender()->sendTakeVehicle(false, m_vehicleEntityId);
}

void EntityPlayerSP::attackActorNpc(Entity * entity)
{
	auto npc = dynamic_cast<EntityActorNpc*>(entity);
	if (npc)
	{
		bool needSend = SCRIPT_EVENT::AttackActorNpcEvent::invoke(entity->entityId);
		if (needSend)
			ClientNetwork::Instance()->getSender()->sendAttackActorNpc(entity->entityId);
	}
}

void EntityPlayerSP::attackSessionNpc(EntitySessionNpc * npc)
{
	if (!npc)
	{
		return;
	}

	switch (npc->getSessionType())
	{
	case SessionType::DEFAULT:
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, npc->getSessionContent().c_str());
		break;
	case SessionType::GET_MANOR:
		ShowGetManorTipDialogEvent::emit();
		break;
	case SessionType::SELL_MANOR:
		ShowSellManorTipDialogEvent::emit();
		break;
	case SessionType::TIP_NPC:
		ShowResultTipDialogEvent::emit(ResultCode::COMMON, npc->getSessionContent().c_str());
		break;
	case SessionType::MANOR_FURNITURE:
		if (Blockman::Instance()->m_gameSettings->m_isEditFurniture)
		{
			auto clientWorld = dynamic_cast<WorldClient*>(world);
			if (clientWorld->findManorByPlayerPosition(this) > 0 || m_isActorChange)
			{
				LordLogError("manor furniture edit failure: is not oneself's manor or actor change");
				return;
			}
			int id = atoi(npc->getSessionContent().c_str());
			if (id == 0)
			{
				LordLogError("manor furniture edit failure: session npc content is null");
				return;
			}
			SetPutFurnitureIdEvent::emit(id);
			ClientNetwork::Instance()->getSender()->sendPutFurniture(id, PutFurnitureStatus::EDIT_START);
		}
		break;

	case SessionType::MULTI_TIP_NPC:
		ShowMulitTipDialogEvent::emit(npc->getSessionContent().c_str());
		break;

	case SessionType::PERSONAL_SHOP:
		OpenPersonalShopEvent::emit();
		break;

	case SessionType::STATUE_NPC:
		ClientNetwork::Instance()->getSender()->sendAttackEntity(npc->entityId);
		break;
		
	case SessionType::SUPER_SHOP:
		OpenSuperShopEvent::emit();
		break;
		
	case SessionType::MULTI_TIP_INTERACTION_NPC:
		ShowMulitTipInteractionDialogEvent::emit(npc->getSessionContent().c_str());
		break;
	case SessionType::RANCHERS_EXPLORE_BEGIN:
		ShowRanchExBeginTipDialogEvent::emit();
		break;
	case SessionType::RANCHERS_TASK_FINISH:
		ShowRanchExTaskFinishTipDialogEvent::emit();
	case SessionType::ENCHANTMENT:
		ClientNetwork::Instance()->getSender()->sendOpenEnchantMent();
		break;
	case SessionType::MODE_SELECT:
		ShowPixelGunModeSelectEvent::emit();
		break;
	case SessionType::GUN_STORE:
		OpenGunStoreEvent::emit();
		break;
	case SessionType::CHEST_LOTTERY:
		RootGuiLayout::Instance()->showWindowByName("ChestLottery.json");
		break;
	case SessionType::ARMOR_UPGRADE:
		ShowArmorUpgradeEvent::emit();
		break;
	case SessionType::SEASON_RANK:
		OpenSeasonRankEvent::emit();
		break;
	case SessionType::TREASURE_CHEST:
		if (npc->getTimeLeft() > 0)
		{
			ShowBirdOpenChestTipEvent::emit(npc->entityId, 1, npc->getTimeLeft(), npc->getSessionContent().c_str());
		}
		else
		{
			ClientNetwork::Instance()->getSender()->sendBirdOpenTreasureChest(npc->entityId);
		}
		break;
	case SessionType::BIRD_LOTTERY:
		ShowBirdLotteryByNpcEvent::emit(npc->entityId);
		break;
	case SessionType::BIRD_STORE_ITEM:
		ShowBirdStoreByNpcEvent::emit(npc->getSessionContent());
		break;
	case SessionType::BIRD_TASK_TIP:
		ShowBirdTaskTipEvent::emit(npc->getSessionContent());
		break;
	case SessionType::BIRD_EGG_CHEST:
		if (npc->getTimeLeft() > 0)
		{
			ShowBirdCommonTipEvent::emit("gui_bird_tip_not_egg_chest");
		}
		else
		{
			if (UICommon::checkBirdBagParam())
			{
				auto bag = Blockman::Instance()->m_pPlayer->m_birdSimulator->getBag();
				if (bag->curCapacity >= bag->maxCapacity)
				{
					ShowBirdCapacityFullTipEvent::emit(bag->expandCapacityPrice, bag->expandCurrencyType);
				}
				else
				{
					npc->setTimeLeft(5 * 1000);
					ClientNetwork::Instance()->getSender()->sendBirdOpenEggChest(npc->entityId);
				}
			}
		}
		break;
	case BLOCK_CITY_TIGER_LOTTERY:
		if (npc->getTimeLeft() > 0)
		{
			ShowBlockCityTigerLotteryTipEvent::emit(npc->entityId);
		}
		else
		{
			SetBlockCityLotteryByNpcEvent::emit(npc->entityId);
		}
		break;
	case BLOCK_CITY_MANAGER:
		{
			ui64 userId = StringUtil::ParseUI64(npc->getSessionContent());
			if (userId == getPlatformUserId())
			{
				BlockCityShowManorEvent::emit();
			}
			else
			{
				ShowBlockCityTipEvent::emit("gui_blockcity_manager_is_not_your_home");
			}
		}
		break;
	case BLOCK_CITY_HOUSE_SHOP_ITEM:
		ShowBlockCityHouseShopByNpcEvent::emit(npc->getSessionContent());
		break; 
	case SessionType::SKYBLOCK_PRODUCT_NPC:
		ClientNetwork::Instance()->getSender()->sendAttackEntity(npc->entityId);
		break;
	case SessionType::SKY_BLOCK_CHRISTMAS:
		ClientNetwork::Instance()->getSender()->sendAttackEntity(npc->entityId);
		break;
	default:
		break;
	}
}

void EntityPlayerSP::attackBuildNpc(EntityBuildNpc * npc)
{
	switch (npc->getBuildType())
	{
	case BuildType::BUILD_DEFAULT:
		break;
	case BuildType::BUILD_FARM:
		ShowRanchBuildFarmEvent::emit(npc->entityId);
		break;
	case BuildType::BUILD_PLANT:
		ShowRanchBuildFactoryEvent::emit(npc->entityId);
		break;
	case BuildType::BUILD_WAREHOUSE:
		RanchStorageOpenEvent::emit();
		break;
	}
}

bool EntityPlayerSP::startParachute() {
	capabilities.isParachute = true;
	preStartParachuteSerpece = Blockman::Instance()->getCurrPersonView();
	Blockman::Instance()->setPerspece(GameSettings::SPVT_THIRD);
	if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::PixelGunChicken)
	{
		auto sender = ClientNetwork::Instance()->getSender();
		sender->sendChangeClothes(3000, "custom_wing");
	}
	SoundSystem::Instance()->stopAllEffects();
	return true;
}

bool EntityPlayerSP::parachuteEnd() {
	capabilities.isParachute = false;
	Blockman::Instance()->setPerspece(preStartParachuteSerpece);
	auto sender = ClientNetwork::Instance()->getSender();

	if (isGlide() && GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunChicken)
		sender->sendChangeClothes(105302, "custom_wing");
	else
		sender->sendChangeClothes(0, "custom_wing");
	playSoundByType(SoundType::ST_Landing);
	return true;
}

bool EntityPlayerSP::playReloadProgress() {

	m_isReloadBullet = true;
	m_reloadBulletInt = 0;
	m_reloadPregress = m_prevReloadPregress = 0.f;
	ItemStackPtr stack = inventory->getCurrentItem();
	LordAssert(stack && stack->stackSize >= 1); // for the isHoldGun!

	ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
	LordAssert(pGun);

	const BulletClipSetting* pSetting = stack->getBulletClipSetting();
	if (!pSetting)
		return false;

	const GunSetting* pGunSetting = stack->getGunSetting();
	if (!pGunSetting)
		return false;

	SoundSystem::Instance()->playEffectByType((SoundType)pGunSetting->switchClipSoundType);
	isPlayReloadProgress = true;
	return true;
}

void EntityPlayerSP::playDefendProgress(float durationTime)
{
	m_defendProgress = durationTime;
	if (durationTime == 0.f)
	{
		ClientNetwork::Instance()->getSender()->sendSkillType(SkillType::DEFENSE, false);
	}
}

void EntityPlayerSP::onUpdateDefendProgress()
{
	if (m_defendProgress >0.f)
	{
		m_defendProgress -= world->getFrameDeltaTime();
	}
}

bool EntityPlayerSP::beginReloadBullet(bool bForce)
{
	if (!isHoldGun)		// no gun in hand?
		return false;
	if (isHoldGunFire)	// gun is fire?
		return false;

	ItemStackPtr stack = inventory->getCurrentItem();
	LordAssert(stack && stack->stackSize >= 1); // for the isHoldGun!

	ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
	LordAssert(pGun);

	const BulletClipSetting* pSetting = stack->getBulletClipSetting();
	if (!pSetting)
		return false;

	const GunSetting* pGunSetting = stack->getGunSetting();
	if (!pGunSetting)
		return false;

	int bulletId = pGunSetting->adaptedBulletId;
	int nCountBulletInInventory = inventory->getItemNum(bulletId);
	int bulletNum = stack->getCurrentClipBulletNum();
	int totalBulletNum = stack->getTotalBulletNum();

	if (bulletNum == totalBulletNum && totalBulletNum > 0)
		return false;

	if (!bForce)
	{
		if (totalBulletNum == 0) {
			int MaxBulletNum = getGunBulletNum(pGunSetting);
			if (MaxBulletNum > 0) {
				totalBulletNum = MaxBulletNum;
				stack->setTotalBulletNum(MaxBulletNum);
			}
		}

		if (!LogicSetting::Instance()->getGunIsNeedBulletStatus())
		{
			nCountBulletInInventory = getGunBulletNum(pGunSetting);
		}
		else
		{
			if (bulletNum >= totalBulletNum || nCountBulletInInventory <= 0) {
				return false;
			}
		}
	}

	m_isReloadBullet = true;
	m_reloadBulletInt = 0;
	m_reloadPregress = m_prevReloadPregress = 0.f;
	SoundSystem::Instance()->playEffectByType((SoundType)pGunSetting->switchClipSoundType);
	// play a reload sound please!

	return true;
}

void EntityPlayerSP::updateReloadProgress()
{
	if (isHoldGun && m_isReloadBullet)
	{
		int i = _getReloadAnimEnd(isPlayReloadProgress);
		if (i <= 0)
		{
			m_isReloadBullet = false;
			m_reloadBulletInt = 0;
			m_prevReloadPregress = m_reloadPregress = 0.f;
			isPlayReloadProgress = false;
			UpdateBulletReload::emit(0.f);
			return;
		}

		++m_reloadBulletInt;

		if (m_reloadBulletInt >= i)
		{
			_finishReloadBullet();
		}
		m_reloadPregress = (float)m_reloadBulletInt / (float)i;
		UpdateBulletReload::emit(m_reloadPregress);
	}
	else
	{
		m_reloadBulletInt = 0;
		UpdateBulletReload::emit(0.f);
	}
}

int EntityPlayerSP::_getReloadAnimEnd(bool isForce)
{
	if (!isHoldGun)		// no gun in hand?
		return 0;
	if (isHoldGunFire && !isForce)	// gun is fire?
		return 0;

	ItemStackPtr stack = inventory->getCurrentItem();
	LordAssert(stack && stack->stackSize >= 1); // for the isHoldGun!

	ItemGun* pGun = dynamic_cast<ItemGun*>(stack->getItem());
	LordAssert(pGun);

	return pGun->getGunSetting()->reloadTick;
	//return 40;
}

void EntityPlayerSP::_finishReloadBullet()
{
	m_reloadBulletInt = 0;
	m_isReloadBullet = false;
	m_prevReloadPregress = m_reloadPregress = 0.f;

	ItemStackPtr stack = inventory->getCurrentItem();
	if (!stack || stack->stackSize < 1)
		return;

	const GunSetting* pGunSetting = stack->getGunSetting();
	if (!pGunSetting)
		return ;
	
	int bulletId = pGunSetting->adaptedBulletId;
	int nCountBulletInInventory = inventory->getItemNum(bulletId);

	int bulletNum = stack->getCurrentClipBulletNum();
	int totalBulletNum = stack->getTotalBulletNum();

	if (isPlayReloadProgress && ( bulletNum > 0)) {
		isPlayReloadProgress = false;
		return;
	}

	int canAddBulletNum = totalBulletNum - bulletNum;
	if (LogicSetting::Instance()->getGunIsNeedBulletStatus())
	{
		canAddBulletNum = Math::Min(nCountBulletInInventory, canAddBulletNum);
		stack->reloadBulletClip(canAddBulletNum);
	}
	else
	{
		stack->reloadBulletClip(canAddBulletNum);
	}
	// finished reload bullet
	ClientNetwork::Instance()->getSender()->sendReloadBullet();
}

float EntityPlayerSP::getReloadProcess(float rdt)
{
	if (isHoldGun && m_isReloadBullet)
	{
		float delta = m_reloadPregress - m_prevReloadPregress;
		if (delta < 0.f)
			++delta;
		return m_prevReloadPregress + delta * rdt;
	}
	return 0.f;
}

ItemStackPtr EntityPlayerSP::getHeldItem()
{
	return inventory->getCurrentItem();
}

void EntityPlayerSP::playStepSound(const BlockPos & pos, int blockID)
{
	if (PlayerPrefs::Instance()->getBoolPrefs("DisStepSound"))
	{
		return;
	}

	if (GameClient::CGame::Instance()->GetGameType() == ClientGameType::PixelGunHall
		&& GameClient::CGame::Instance()->isEnableIndie())
	{
		// indie app pixel gun hall do not play step sound
		return; // Don't ask me why I wrote this, required by the tester
	}
	auto stepSound = cBlockManager::scBlocks[blockID]->getStepSound();

	if (world->getBlockId({ pos.x, pos.y + 1, pos.z }) == BlockManager::snow->getBlockID())
	{
		//playSound(stepSound.getStepSound(), stepSound.volume * 0.15F, stepSound.pitch);
		playSoundByType(stepSound.stepSound);
	}
	else if (!BlockManager::sBlocks[blockID]->getMaterial().isLiquid())
	{
		//playSound(stepSound.getStepSound(), stepSound.volume * 0.15F, stepSound.pitch);
		playSoundByType(stepSound.stepSound);
	}
}

void EntityPlayerSP::playSound(SoundType soundType)
{
	world->playSoundByType({ position.x, position.y - yOffset, position.z }, soundType);
}

void EntityPlayerSP::do_Entity_moveEntity_with_debgu_info(const Vector3& vel)
{
	Vector3 prePos = position;

	if (noClip)
	{
		boundingBox.move(vel);
		position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
		position.y = boundingBox.vMin.y + yOffset - m_yPositionDecrement;
		position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;
	}
	else
	{
		m_yPositionDecrement *= 0.4F;
		Vector3 pos = position;
		float x = vel.x;
		float y = vel.y;
		float z = vel.z;

		if (isInWeb)
		{
			isInWeb = false;
			x *= 0.25f;
			y *= 0.05f;
			z *= 0.25f;
			motion = Vector3::ZERO;
		}

		float tx = x;
		float ty = y;
		float tz = z;
		Box bb = boundingBox;
		EntityPlayer* pplayer = dynamic_cast<EntityPlayer*>(this);
		bool flag = onGround && isSneaking() && pplayer;

		if (flag)
		{
			float f;

			for (f = 0.05f; x != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(x, -1.0f, 0.0f)).empty(); tx = x)
			{
				if (x < f && x >= -f)
					x = 0.0f;
				else if (x > 0.0f)
					x -= f;
				else
					x += f;
			}

			for (; z != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(0.0f, -1.0f, z)).empty(); tz = z)
			{
				if (z < f && z >= -f)
					z = 0.0f;
				else if (z > 0.0f)
					z -= f;
				else
					z += f;
			}

			while (x != 0.0f && z != 0.0f && world->getCollidingBoundingBoxes(this, boundingBox.getOffset(x, -1.0f, z)).empty())
			{
				if (x < f && x >= -f)
					x = 0.0f;
				else if (x > 0.0f)
					x -= f;
				else
					x += f;

				if (z < f && z >= -f)
					z = 0.0f;
				else if (z > 0.0f)
					z -= f;
				else
					z += f;

				tx = x;
				tz = z;
			}
		}

		Box extendBox = boundingBox.addCoord(x, y, z);
		AABBList& aabbs = world->getCollidingBoundingBoxes(this, extendBox);
		int* flags = new int[aabbs.size() + 1];
		memset(flags, 0, sizeof(int) * (aabbs.size() + 1));
		float ox = tx;
		float oy = ty;
		float oz = tz;

		for (int i = 0; i < int(aabbs.size()); ++i)
		{
			y = aabbs[i].getYOffset(boundingBox, y);
			if (y != oy)
			{
				oy = y;
				flags[i] = 1;
			}
		}

		boundingBox.move(0.0f, y, 0.0f);

		bool flag2 = onGround || ty != y && ty < 0.0f;
		int i;

		for (i = 0; i < int(aabbs.size()); ++i)
		{
			x = aabbs[i].getXOffset(boundingBox, x);
			if (x != ox)
			{
				ox = x;
				flags[i] = 1;
			}
		}

		boundingBox.move(x, 0.0f, 0.0f);

		for (i = 0; i < int(aabbs.size()); ++i)
		{
			z = aabbs[i].getZOffset(boundingBox, z);
			if (z != oz)
			{
				oz = z;
				flags[i] = 1;
			}
		}

		boundingBox.move(0.0f, 0.0f, z);

		// add all the collision box(red) and tested box to render visual helps.
		RenderGlobal* render = Blockman::Instance()->m_globalRender;
		render->visualHelps.clear();
		for (size_t i = 0; i < aabbs.size(); ++i)
		{
			ui32 color = flags[i] == 1 ? 0xFF9999FF : 0xFF99FF99;
			render->visualHelps.push_back(std::make_pair(aabbs[i], color));
		}
		render->visualHelps.push_back(std::make_pair(extendBox, 0xFFFF9999));
		render->drawVisualHelpBox();

		float dy;
		float dz;
		float dx;
		if (stepHeight > 0.0F && flag2 && (flag || m_yPositionDecrement < 0.05F) && (tx != x || tz != z))
		{
			dx = x;
			dy = y;
			dz = z;
			x = tx;
			y = stepHeight;
			z = tz;
			Box bb1 = boundingBox;
			boundingBox = bb;
			aabbs = world->getCollidingBoundingBoxes(this, boundingBox.addCoord(tx, y, tz));

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}

			boundingBox.move(0.0f, y, 0.0f);

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				x = aabbs[i].getXOffset(boundingBox, x);
			}

			boundingBox.move(x, 0.0f, 0.0f);

			for (i = 0; i < int(aabbs.size()); ++i)
			{
				z = aabbs[i].getZOffset(boundingBox, z);
			}
			boundingBox.move(0.0f, 0.0f, z);

			y = -stepHeight;
			for (i = 0; i < int(aabbs.size()); ++i)
			{
				y = aabbs[i].getYOffset(boundingBox, y);
			}
			boundingBox.move(0.0f, y, 0.0f);

			if (dx * dx + dz * dz >= x * x + z * z)
			{
				x = dx;
				y = dy;
				z = dz;
				boundingBox = bb1;
			}
		}

		position.x = (boundingBox.vMin.x + boundingBox.vMax.x) / 2.0f;
		position.y = boundingBox.vMin.y + yOffset - m_yPositionDecrement;
		position.z = (boundingBox.vMin.z + boundingBox.vMax.z) / 2.0f;
		isCollidedHorizontally = (Math::Abs(tx - x) > 0.001f) || Math::Abs(tz - z) > 0.001f;
		isCollidedVertically = (Math::Abs(ty - y) > 0.001f);
		onGround = isCollidedVertically && ty < 0.0f;
		isCollided = isCollidedHorizontally || isCollidedVertically;
		updateFallState(y, onGround);

		if (tx != x)
			motion.x = 0.0f;

		if (ty != y)
			motion.y = 0.0f;

		if (tz != z)
			motion.z = 0.0f;

		dx = position.x - pos.x;
		dy = position.y - pos.y;
		dz = position.z - pos.z;

		if (canTriggerWalking() && !flag)
		{
			BlockPos pos = (position + Vector3(0.f, -0.2f - yOffset, 0.f)).getFloor();
			int blockID = world->getBlockId(pos);

			if (blockID == 0)
			{
				int blockID1 = world->getBlockId(pos.getNegY());

				if (blockID1 == BLOCK_ID_FENCE ||
					blockID1 == BLOCK_ID_COBBLE_STONE_WALL ||
					blockID1 == BLOCK_ID_FENCE_GATE)
				{
					blockID = blockID1;
				}
			}

			if (blockID != BLOCK_ID_LADDER)
				dy = 0.0f;

			distanceWalkedModified = distanceWalkedModified + Math::Sqrt(dx * dx + dz * dz) * 0.6f;
			distanceWalkedOnStepModified = distanceWalkedOnStepModified + Math::Sqrt(dx * dx + dy * dy + dz * dz) * 0.6f;

			if (distanceWalkedOnStepModified > nextStepDistance && blockID > 0)
			{
				nextStepDistance = (int)distanceWalkedOnStepModified + 1;

				if (isInWater())
				{
					float var42 = Math::Sqrt(motion.x * motion.x * 0.2f + motion.y * motion.y + motion.z * motion.z * 0.2f) * 0.35F;

					if (var42 > 1.0F)
						var42 = 1.0F;

					//playSound("liquid.swim", var42, 1.0F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
					if (world->canPlaySpecialSound(2))
					{
						playSoundByType(ST_Swim);
					}
				}

				BlockPos pos1(pos.x, i, pos.z);
				playStepSound(pos1, blockID);
				BlockManager::sBlocks[blockID]->onEntityWalking(world, pos1, this);
			}
		}

		doBlockCollisions();

		bool wet = isWet();

		if (world->isBoundingBoxBurning(boundingBox.contract(0.001f, 0.001f, 0.001f)))
		{
			dealFireDamage(1);

			if (!wet)
			{
				++fire;

				if (fire == 0)
				{
					setFire(8);
				}
			}
		}
		else if (fire <= 0)
		{
			fire = -fireResistance;
		}

		if (wet && fire > 0)
		{
			playSoundByType(ST_Fizz);
			//playSound("random.fizz", 0.7F, 1.6F + (rand->nextFloat() - rand->nextFloat()) * 0.4F);
			fire = -fireResistance;
		}
	}

}

Vector3 EntityPlayerSP::getLook(float ticktime)
{
	Camera* mainCamera = SceneManager::Instance()->getMainCamera();
	GameSettings* setting = Blockman::Instance()->m_gameSettings;
	Ray ray;
	mainCamera->getCameraRay(ray, setting->getMousePos());
	return ray.dir;
}

bool EntityPlayerSP::isZombie()
{
	if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::Zombie_Infecting)
	{
		return false;
	}
	return !m_isPeopleActor;
}

void EntityPlayerSP::setPositionAndRotation2(const Vector3 & pos, float yaw, float pitch, int increment)
{
	setPositionAndRotation(pos + Vector3(0.0f, yOffset, 0.0f), yaw, pitch);
	setVelocity(Vector3(0, 0, 0));
}

void  EntityPlayerSP::attAckEntityCreature(EntityCreature* entityCreature)
{
	if (entityCreature)
	{
		ClientNetwork::Instance()->getSender()->sendAttackCreatureEntity(entityCreature->entityId);
	}
}

void EntityPlayerSP::removeArmStack()
{
	if (armStack)
		armStack = nullptr;
}

void EntityPlayerSP::setArmStack(ItemStackPtr stack)
{
	removeArmStack();
	armStack = stack;
}

ItemStackPtr EntityPlayerSP::getArmStack()
{
	return armStack;
}

}
