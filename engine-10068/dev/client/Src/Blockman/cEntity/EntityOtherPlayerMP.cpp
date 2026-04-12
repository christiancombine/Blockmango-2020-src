#include "EntityOtherPlayerMP.h"

#include "World/World.h"
#include "Inventory/InventoryPlayer.h"
#include "Item/ItemStack.h"
#include "Item/Item.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "game.h"
#include "Setting/GameTypeSetting.h"
#include "Util/ClientEvents.h"
#include "cWorld/Blockman.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "Blockman/EntityRenders/EntityRenderManager.h"
#include "Actor/ActorObject.h"
#include "Util/PlayerPrefs.h"

namespace BLOCKMAN
{
	
EntityOtherPlayerMP::EntityOtherPlayerMP(World* pWorld, const String& worldName)
	: EntityPlayer(pWorld, worldName)
{
	oldPos = Vector3::ZERO;
	oldMinY = 0.f;
	yOffset = 0.0F;
	//stepHeight = 0.0F;
	noClip = false;
	renderBedOffset.y = 0.25F;
	renderDistanceWeight = PlayerPrefs::Instance()->getFloatPrefs("EntityOtherPlayerMPRenderDistanceWeight");
	
	isItemInUse = false;
	otherPlayerMPPosRotationIncrements = 0;
	otherPlayerPos = Vector3::ZERO;
	otherPlayerMPYaw = 0.f; 
	otherPlayerMPPitch = 0.f;
	movementInput = LordNew MovementInput();
}

EntityOtherPlayerMP::~EntityOtherPlayerMP()
{
	LordSafeDelete(movementInput);
}

void EntityOtherPlayerMP::setPositionAndRotation2(const Vector3& pos, float yaw, float pitch, int increment)
{
	float fEdgeLen = getEdgeLen();
	if (Blockman::Instance()->m_pPlayer && Blockman::Instance()->m_pPlayer->position.distanceTo(position) > fEdgeLen)
	{
		Entity::setPositionAndRotation(pos, yaw, pitch);
		return;
	}
	otherPlayerPos = pos;
	otherPlayerMPYaw = yaw;
	otherPlayerMPPitch = pitch;
	otherPlayerMPPosRotationIncrements = increment;
	SendChickenWarningEvent::emit(position, 2);
}

void EntityOtherPlayerMP::onUpdate()
{
	renderBedOffset.y = 0.0F;
	EntityPlayer::onUpdate();
	prevLimbYaw = limbYaw;
	float dx = position.x - prevPos.x;
	float dz = position.z - prevPos.z;
	float xzLen = Math::Sqrt(dx * dx + dz * dz) * 4.0F;

	if (xzLen > 1.f)
	{
		xzLen = 1.f;
	}

	limbYaw += (xzLen - limbYaw) * 0.4F;
	limbSwing += limbYaw;

	if (!isItemInUse && isEating() && inventory->getCurrentItem())
	{
		ItemStackPtr pStack = inventory->getCurrentItem();
		setItemInUse(pStack, Item::itemsList[pStack->itemID]->getMaxItemUseDuration(pStack));
		isItemInUse = true;
	}
	else if (isItemInUse && !isEating())
	{
		clearItemInUse();
		isItemInUse = false;
	}
}

void EntityOtherPlayerMP::onLivingUpdate()
{
	EntityPlayer::onLivingUpdate();
	//EntityPlayer::updateEntityActionState();

	if (otherPlayerMPPosRotationIncrements > 0)
	{
		Vector3 pos = position + (otherPlayerPos - position) / float(otherPlayerMPPosRotationIncrements);
		float yaw;

		for (yaw = otherPlayerMPYaw - rotationYaw; yaw < -180.f; yaw += 360.f)
		{
			;
		}

		while (yaw >= 180.0f)
		{
			yaw -= 360.0f;
		}

		rotationYaw = rotationYaw + yaw / otherPlayerMPPosRotationIncrements;
		rotationPitch = rotationPitch + (otherPlayerMPPitch - rotationPitch) / otherPlayerMPPosRotationIncrements;
		--otherPlayerMPPosRotationIncrements;
		setPosition(pos);
		setRotation(rotationYaw, rotationPitch);
	}

	prevCameraYaw = cameraYaw;
	float mxzLen = Math::Sqrt(motion.x * motion.x + motion.z * motion.z);
	float var2 = Math::ATan(-motion.y * 0.2f) * 15.0F;

	if (mxzLen > 0.1F)
	{
		mxzLen = 0.1F;
	}

	if (!onGround || getHealth() <= 0.0F)
	{
		mxzLen = 0.0F;
	}

	if (onGround || getHealth() <= 0.0F)
	{
		var2 = 0.0F;
	}

	cameraYaw += (mxzLen - cameraYaw) * 0.4F;
	cameraPitch += (var2 - cameraPitch) * 0.8F;
}

void EntityOtherPlayerMP::setCurrentItemOrArmor(int armorType, ItemStackPtr pArmor)
{
	if (armorType == 0)
	{
		inventory->setCurrentItem(pArmor);
	}
	else
	{
		inventory->armorInventory[armorType - 1] = pArmor;
	}
}

void EntityOtherPlayerMP::playStepSound(const BlockPos & pos, int blockID)
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

void EntityOtherPlayerMP::sendChatToPlayer(ChatMessageComponent* msg)
{
	// todo.
	// Blockman.getBlockman().ingameGUI.getChatGUI().printChatMessage(par1ChatMessageComponent.func_111068_a(true));
}

BlockPos EntityOtherPlayerMP::getPlayerCoordinates()
{
	BlockPos ret;
	ret.x = int(Math::Floor(position.x + 0.5f));
	ret.y = int(Math::Floor(position.y + 0.5f));
	ret.z = int(Math::Floor(position.z + 0.5f));
	return ret;
}

void EntityOtherPlayerMP::updateEntityActionState()
{
	applyMovementInput();
	if (needUpdateActionState())
	{
		EntityPlayer::updateEntityActionState();
	}
}

bool EntityOtherPlayerMP::isPlayerMoved(bool precise/* = false*/)
{
	float dx = position.x - oldPos.x;
	float dy = boundingBox.vMin.y - oldMinY;
	float dz = position.z - oldPos.z;
	return dx * dx + dy * dy + dz * dz > 0.25 || m_moveTick >= 100;
}

void EntityOtherPlayerMP::recordOldPos()
{
	oldPos = position;
	oldMinY = boundingBox.vMin.y;
	m_moveTick = 0;
}

bool EntityOtherPlayerMP::isZombie()
{
	if (GameClient::CGame::Instance()->GetGameType() != ClientGameType::Zombie_Infecting)
	{
		return false;
	}
	return !m_isPeopleActor;
}

}
