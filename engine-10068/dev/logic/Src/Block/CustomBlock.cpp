#include "CustomBlock.h"
#include "World/World.h"
#include "Trigger/TriggerManagers.h"
#include "Entity/EntityLivingBase.h"

namespace BLOCKMAN
{
	CustomBlock::CustomBlock(int id, const String& nameSpace, const String& name, const BlockConfig& config)
		: Block(
			id,
			BM_Material(
				static_cast<int>(config.color.getRGB()),
				config.flammable,
				config.state == BlockState::GAS,
				config.transparent || config.state == BlockState::GAS,
				config.requiresNoTool,
				config.state == BlockState::GAS ? 1 : 0,
				false,
				config.state == BlockState::SOLID, 
				false, 
				config.state == BlockState::SOLID, 
				config.state == BlockState::SOLID /* TODO: I'm not sure what's the effect of this field */),
			name, 
			!config.transparent && config.state == BlockState::SOLID)
		, m_namespace(nameSpace)
		, m_state(config.state)
		, m_collisionBox(config.collisionBox)
	{
		setHardness(config.hardness);
		setResistance(config.resistance / 3);
		setLightDecay(config.lightDecay.red, config.lightDecay.green, config.lightDecay.blue);
		setLightValue(config.lightEmitted.red, config.lightEmitted.green, config.lightEmitted.blue);
		setLightStrength(config.lightStrength);
		TRIGGER_MANAGER::registerTriggers(getBlockID(), config.triggers);
		m_clickable = TriggerClickManager::Instance()->blockHasTrigger(getBlockID());
		m_transparent = config.transparent || config.state == BlockState::GAS;
	}

	CustomBlock::~CustomBlock()
	{
		TRIGGER_MANAGER::unregisterTriggers(getBlockID());
	}

	int CustomBlock::idDropped(int meta, Random & rand, int fortune)
	{
		if (m_state == BlockState::GAS)
		{
			return 0;
		}
		else
		{
			return Block::getItemBlockId();
		}
	}

	int CustomBlock::quantityDropped(Random & rand)
	{
		if (m_state == BlockState::GAS)
		{
			return 0;
		}
		else
		{
			return Block::quantityDropped(rand);
		}
	}

	Box CustomBlock::getCollisionBoundingBox(World * pWorld, const BlockPos & pos)
	{
		if (m_state == BlockState::GAS)
		{
			return Box();
		}
		else
		{
			return {m_collisionBox.vMin + pos, m_collisionBox.vMax + pos};
		}
	}

	bool CustomBlock::onBlockActivated(World * pWorld, const BlockPos & pos, EntityPlayer * pPlayer, int face, const Vector3 & hit)
	{
		Block::onBlockActivated(pWorld, pos, pPlayer, face, hit);
		TriggerClickManager::Instance()->checkTriggers(pWorld, pos, pPlayer);
		return m_clickable;
	}

	void CustomBlock::onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack)
	{
		if (!isJigsawBlock())
		{
			float yaw = pLiving->rotationYaw;
			yaw = std::fmod(yaw, 360.0f);
			int meta = 0;
			if (yaw < 0.0f)
				yaw += 360.0f;
			if (yaw > 45.0f && yaw <= 135.0f)
				meta = pLiving->position.x - pos.x - 1.0f > 0.f ? 3 : 1;
			else if (yaw > 135.0f && yaw <= 225.0f)
				meta = pLiving->position.z - pos.z - 1.0f > 0.f ? 0 : 2;
			else if (yaw > 225.0f && yaw <= 315.0f)
				meta = pLiving->position.x - pos.x + 0.5f < 0.f ? 1 : 3;
			else if (yaw > 315.0f || yaw <= 45.0f)
				meta = pLiving->position.z - pos.z + 0.5f < 0.f ? 2 : 0;
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			return;
		}

		if (pWorld->m_isClient)
			return;

		int meta = pWorld->getBlockMeta(pos);

		int blockNorth = pWorld->getBlockId(BlockPos(pos.x, pos.y, pos.z - 1));
		int blockSouth = pWorld->getBlockId(BlockPos(pos.x, pos.y, pos.z + 1));
		int blockWest = pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z));
		int blockEast = pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z));
		int blockUp = pWorld->getBlockId(BlockPos(pos.x, pos.y + 1, pos.z));
		int blockDown = pWorld->getBlockId(BlockPos(pos.x, pos.y - 1, pos.z));
		int metaNorth = pWorld->getBlockMeta(BlockPos(pos.x, pos.y, pos.z - 1));
		int metaSouth = pWorld->getBlockMeta(BlockPos(pos.x, pos.y, pos.z + 1));
		int metaWest = pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z));
		int metaEast = pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z));
		int metaUp = pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z));
		int metaDown = pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z));

		// x = 0
		int blockSouthUp = pWorld->getBlockId(BlockPos(pos.x, pos.y + 1, pos.z + 1));	// 1
		int blockNorthUp = pWorld->getBlockId(BlockPos(pos.x, pos.y + 1, pos.z - 1));	// 2
		int blockNorthDown = pWorld->getBlockId(BlockPos(pos.x, pos.y - 1, pos.z - 1));	// 3
		int blockSouthDown = pWorld->getBlockId(BlockPos(pos.x, pos.y - 1, pos.z + 1));	// 4
		int metaSouthUp = pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z + 1));
		int metaNorthUp = pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z - 1));
		int metaNorthDown = pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z - 1));
		int metaSouthDown = pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z + 1));

		if (isSameJigsaw(blockSouthUp) && isSameJigsaw(blockSouth) && isSameJigsaw(blockUp)
			&& (metaSouthUp & 0xC) == 0 && (metaSouth & 0xC) == 0 && (metaUp & 0xC) == 0)
		{
			metaUp = metaUp | 0x8;
			meta = meta | 0xC;
			metaSouth = metaSouth | 0x8;
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z + 1), metaSouthUp | 0x4, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z), metaUp, 3);
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y, pos.z + 1), metaSouth, 3);
		}
		else if (isSameJigsaw(blockNorthUp) && isSameJigsaw(blockNorth) && isSameJigsaw(blockUp)
			&& (metaNorthUp & 0xC) == 0 && (metaNorth & 0xC) == 0 && (metaUp & 0xC) == 0)
		{
			metaUp = metaUp | 0x4;
			metaNorth = metaNorth | 0xC;
			meta = meta | 0x8;
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z), metaUp, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z - 1), metaNorthUp | 0x8, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y, pos.z - 1), metaNorth, 3);
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
		}
		else if (isSameJigsaw(blockSouthDown) && isSameJigsaw(blockSouth) && isSameJigsaw(blockDown)
			&& (metaSouthDown & 0xC) == 0 && (metaSouth & 0xC) == 0 && (metaDown & 0xC) == 0)
		{
			metaSouth = metaSouth | 0x4;
			meta = meta | 0x8;
			metaDown = metaDown | 0xC;
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y, pos.z + 1), metaSouth, 3);
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z), metaDown, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z + 1), metaSouthDown | 0x8, 3);
		}
		else if (isSameJigsaw(blockNorthDown) && isSameJigsaw(blockNorth) && isSameJigsaw(blockDown)
			&& (metaNorthDown & 0xC) == 0 && (metaNorth & 0xC) == 0 && (metaDown & 0xC) == 0)
		{
			meta = meta | 0x4;
			metaNorth = metaNorth | 0x8;
			metaDown = metaDown | 0x8;
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y, pos.z - 1), metaNorth, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z - 1), metaNorthDown | 0xC, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z), metaDown, 3);
		}

		// z = 0
		int blockWestUp = pWorld->getBlockId(BlockPos(pos.x - 1, pos.y + 1, pos.z));	// 1
		int blockEastUp = pWorld->getBlockId(BlockPos(pos.x + 1, pos.y + 1, pos.z));	// 2
		int blockEastDown = pWorld->getBlockId(BlockPos(pos.x + 1, pos.y - 1, pos.z));	// 3
		int blockWestDown = pWorld->getBlockId(BlockPos(pos.x - 1, pos.y - 1, pos.z));	// 4
		int metaWestUp = pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y + 1, pos.z));
		int metaEastUp = pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y + 1, pos.z));
		int metaEastDown = pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y - 1, pos.z));
		int metaWestDown = pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y - 1, pos.z));

		if (isSameJigsaw(blockWestUp) && isSameJigsaw(blockWest) && isSameJigsaw(blockUp)
			&& (metaWestUp & 0x3) == 0 && (metaWest & 0x3) == 0 && (metaUp & 0x3) == 0)
		{
			metaUp = metaUp | 0x2;
			meta = meta | 0x3;
			metaWest = metaWest | 0x2;
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x - 1, pos.y + 1, pos.z), metaWestUp | 0x1, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z), metaUp, 3);
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x - 1, pos.y, pos.z), metaWest, 3);
		}
		else if (isSameJigsaw(blockEastUp) && isSameJigsaw(blockEast) && isSameJigsaw(blockUp)
			&& (metaEastUp & 0x3) == 0 && (metaEast & 0x3) == 0 && (metaUp & 0x3) == 0)
		{
			metaUp = metaUp | 0x1;
			metaEast = metaEast | 0x3;
			meta = meta | 0x2;
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z), metaUp, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x + 1, pos.y + 1, pos.z), metaEastUp | 0x2, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x + 1, pos.y, pos.z), metaEast, 3);
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
		}
		else if (isSameJigsaw(blockWestDown) && isSameJigsaw(blockWest) && isSameJigsaw(blockDown)
			&& (metaWestDown & 0x3) == 0 && (metaWest & 0x3) == 0 && (metaDown & 0x3) == 0)
		{
			metaWest = metaWest | 0x1;
			meta = meta | 0x2;
			metaDown = metaDown | 0x3;
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x - 1, pos.y, pos.z), metaWest, 3);
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z), metaDown, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x - 1, pos.y - 1, pos.z), metaWestDown | 0x2, 3);
		}
		else if (isSameJigsaw(blockEastDown) && isSameJigsaw(blockEast) && isSameJigsaw(blockDown)
			&& (metaEastDown & 0x3) == 0 && (metaEast & 0x3) == 0 && (metaDown & 0x3) == 0)
		{
			meta = meta | 0x1;
			metaEast = metaEast | 0x2;
			metaDown = metaDown | 0x2;
			pWorld->setBlockMetadataWithNotify(pos, meta, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x + 1, pos.y, pos.z), metaEast, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x + 1, pos.y - 1, pos.z), metaEastDown | 0x3, 3);
			pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z), metaDown, 3);
		}

		// y = 0
		int blockWestNorth = pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z - 1));	// 1
		int blockEastNorth = pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z - 1));	// 2
		int blockEastSouth = pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z + 1));	// 3
		int blockWestSouth = pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z + 1));	// 4
		int metaWestNorth = pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z - 1));
		int metaEastNorth = pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z - 1));
		int metaEastSouth = pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z + 1));
		int metaWestSouth = pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z + 1));

		float yaw = pLiving->rotationYaw;
		yaw = std::fmod(yaw, 360.0f);
		if (yaw < 0.0f)
			yaw += 360.0f;
		int orientation = 0;
		if (yaw > 45.0f && yaw <= 135.0f)
			orientation = 3;
		else if (yaw > 135.0f && yaw <= 225.0f)
			orientation = 0;
		else if (yaw > 225.0f && yaw <= 315.0f)
			orientation = 1;
		else if (yaw > 315.0f || yaw <= 45.0f)
			orientation = 2;

		int blockId_1 = m_jigsawBeginBlockId;
		int blockId_2 = m_jigsawBeginBlockId + 1;
		int blockId_3 = m_jigsawBeginBlockId + 2;
		int blockId_4 = m_jigsawBeginBlockId + 1;
		if (orientation == 1)
		{
			blockId_1 = m_jigsawBeginBlockId + 4;
			blockId_2 = m_jigsawBeginBlockId + 3;
			blockId_3 = m_jigsawBeginBlockId + 4;
			blockId_4 = m_jigsawBeginBlockId + 5;
		}
		else if (orientation == 2)
		{
			blockId_1 = m_jigsawBeginBlockId + 8;
			blockId_2 = m_jigsawBeginBlockId + 7;
			blockId_3 = m_jigsawBeginBlockId + 6;
			blockId_4 = m_jigsawBeginBlockId + 7;
		}
		else if (orientation == 3)
		{
			blockId_1 = m_jigsawBeginBlockId + 10;
			blockId_2 = m_jigsawBeginBlockId + 11;
			blockId_3 = m_jigsawBeginBlockId + 10;
			blockId_4 = m_jigsawBeginBlockId + 9;
		}

		if (blockWestNorth == m_blockID && blockWest == m_blockID && blockNorth == m_blockID)
		{
			pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z - 1), blockId_1, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z - 1)), 3);
			pWorld->setBlock(BlockPos(pos.x, pos.y, pos.z - 1), blockId_2, pWorld->getBlockMeta(BlockPos(pos.x, pos.y, pos.z - 1)), 3);
			pWorld->setBlock(pos, blockId_3, pWorld->getBlockMeta(pos), 3);
			pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z), blockId_4, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z)), 3);
		}
		else if (blockEastNorth == m_blockID && blockEast == m_blockID && blockNorth == m_blockID)
		{
			pWorld->setBlock(BlockPos(pos.x, pos.y, pos.z - 1), blockId_1, pWorld->getBlockMeta(BlockPos(pos.x, pos.y, pos.z - 1)), 3);
			pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z - 1), blockId_2, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z - 1)), 3);
			pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z), blockId_3, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z)), 3);
			pWorld->setBlock(pos, blockId_4, pWorld->getBlockMeta(pos), 3);
		}
		else if (blockWestSouth == m_blockID && blockWest == m_blockID && blockSouth == m_blockID)
		{
			pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z), blockId_1, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z)), 3);
			pWorld->setBlock(pos, blockId_2, pWorld->getBlockMeta(pos), 3);
			pWorld->setBlock(BlockPos(pos.x, pos.y, pos.z + 1), blockId_3, pWorld->getBlockMeta(BlockPos(pos.x, pos.y, pos.z + 1)), 3);
			pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z + 1), blockId_4, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z + 1)), 3);
		}
		else if (blockEastSouth == m_blockID && blockEast == m_blockID && blockSouth == m_blockID)
		{
			pWorld->setBlock(pos, blockId_1, pWorld->getBlockMeta(pos), 3);
			pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z), blockId_2, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z)), 3);
			pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z + 1), blockId_3, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z + 1)), 3);
			pWorld->setBlock(BlockPos(pos.x, pos.y, pos.z + 1), blockId_4, pWorld->getBlockMeta(BlockPos(pos.x, pos.y, pos.z + 1)), 3);
		}
	}

	void CustomBlock::onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta)
	{
		if (pWorld->m_isClient || !isJigsawBlock())
			return;

		// x = 0
		int metaX = meta & 0xC;
		if (metaX != 0)
		{
			if (metaX == 4
				&& isSameJigsaw(pWorld->getBlockId(pos.getNegZ())) && (pWorld->getBlockMeta(pos.getNegZ()) & 0xC) == 8
				&& isSameJigsaw(pWorld->getBlockId(pos.getNegY())) && (pWorld->getBlockMeta(pos.getNegY()) & 0xC) == 8
				&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x, pos.y - 1, pos.z - 1))) && (pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z - 1)) & 0xC) == 12)
			{
				pWorld->setBlockMetadataWithNotify(pos.getNegZ(), pWorld->getBlockMeta(pos.getNegZ()) & 0x3, 3);
				pWorld->setBlockMetadataWithNotify(pos.getNegY(), pWorld->getBlockMeta(pos.getNegY()) & 0x3, 3);
				pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z - 1), pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z - 1)) & 0x3, 3);
			}
			else if (metaX == 8)
			{
				if (isSameJigsaw(pWorld->getBlockId(pos.getPosZ())) && (pWorld->getBlockMeta(pos.getPosZ()) & 0xC) == 4
					&& isSameJigsaw(pWorld->getBlockId(pos.getNegY())) && (pWorld->getBlockMeta(pos.getNegY()) & 0xC) == 12
					&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x, pos.y - 1, pos.z + 1))) && (pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z + 1)) & 0xC) == 8)
				{
					pWorld->setBlockMetadataWithNotify(pos.getPosZ(), pWorld->getBlockMeta(pos.getPosZ()) & 0x3, 3);
					pWorld->setBlockMetadataWithNotify(pos.getNegY(), pWorld->getBlockMeta(pos.getNegY()) & 0x3, 3);
					pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y - 1, pos.z + 1), pWorld->getBlockMeta(BlockPos(pos.x, pos.y - 1, pos.z + 1)) & 0x3, 3);
				}
				else if (isSameJigsaw(pWorld->getBlockId(pos.getPosY())) && (pWorld->getBlockMeta(pos.getPosY()) & 0xC) == 4
					&& isSameJigsaw(pWorld->getBlockId(pos.getNegZ())) && (pWorld->getBlockMeta(pos.getNegZ()) & 0xC) == 12
					&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x, pos.y + 1, pos.z - 1))) && (pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z - 1)) & 0xC) == 8)
				{
					pWorld->setBlockMetadataWithNotify(pos.getNegZ(), pWorld->getBlockMeta(pos.getNegZ()) & 0x3, 3);
					pWorld->setBlockMetadataWithNotify(pos.getPosY(), pWorld->getBlockMeta(pos.getPosY()) & 0x3, 3);
					pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z - 1), pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z - 1)) & 0x3, 3);
				}
			}
			else if (metaX == 12
				&& isSameJigsaw(pWorld->getBlockId(pos.getPosZ())) && (pWorld->getBlockMeta(pos.getPosZ()) & 0xC) == 8
				&& isSameJigsaw(pWorld->getBlockId(pos.getPosY())) && (pWorld->getBlockMeta(pos.getPosY()) & 0xC) == 8
				&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x, pos.y + 1, pos.z + 1))) && (pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z + 1)) & 0xC) == 4)
			{
				pWorld->setBlockMetadataWithNotify(pos.getPosZ(), pWorld->getBlockMeta(pos.getPosZ()) & 0x3, 3);
				pWorld->setBlockMetadataWithNotify(pos.getPosY(), pWorld->getBlockMeta(pos.getPosY()) & 0x3, 3);
				pWorld->setBlockMetadataWithNotify(BlockPos(pos.x, pos.y + 1, pos.z + 1), pWorld->getBlockMeta(BlockPos(pos.x, pos.y + 1, pos.z + 1)) & 0x3, 3);
			}
		}

		// z = 0
		int metaZ = meta & 0x3;
		if (metaZ != 0)
		{
			if (metaZ == 1
				&& isSameJigsaw(pWorld->getBlockId(pos.getPosX())) && (pWorld->getBlockMeta(pos.getPosX()) & 0x3) == 2
				&& isSameJigsaw(pWorld->getBlockId(pos.getNegY())) && (pWorld->getBlockMeta(pos.getNegY()) & 0x3) == 2
				&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x + 1, pos.y - 1, pos.z))) && (pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y - 1, pos.z)) & 0x3) == 3)
			{
				pWorld->setBlockMetadataWithNotify(pos.getPosX(), pWorld->getBlockMeta(pos.getPosX()) & 0xC, 3);
				pWorld->setBlockMetadataWithNotify(pos.getNegY(), pWorld->getBlockMeta(pos.getNegY()) & 0xC, 3);
				pWorld->setBlockMetadataWithNotify(BlockPos(pos.x + 1, pos.y - 1, pos.z), pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y - 1, pos.z)) & 0xC, 3);
			}
			else if (metaZ == 2)
			{
				if (isSameJigsaw(pWorld->getBlockId(pos.getNegX())) && (pWorld->getBlockMeta(pos.getNegX()) & 0x3) == 1
					&& isSameJigsaw(pWorld->getBlockId(pos.getNegY())) && (pWorld->getBlockMeta(pos.getNegY()) & 0x3) == 3
					&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x - 1, pos.y - 1, pos.z))) && (pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y - 1, pos.z)) & 0x3) == 2)
				{
					pWorld->setBlockMetadataWithNotify(pos.getNegX(), pWorld->getBlockMeta(pos.getNegX()) & 0xC, 3);
					pWorld->setBlockMetadataWithNotify(pos.getNegY(), pWorld->getBlockMeta(pos.getNegY()) & 0xC, 3);
					pWorld->setBlockMetadataWithNotify(BlockPos(pos.x - 1, pos.y - 1, pos.z), pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y - 1, pos.z)) & 0xC, 3);
				}
				else if (isSameJigsaw(pWorld->getBlockId(pos.getPosY())) && (pWorld->getBlockMeta(pos.getPosY()) & 0x3) == 1
					&& isSameJigsaw(pWorld->getBlockId(pos.getPosX())) && (pWorld->getBlockMeta(pos.getPosX()) & 0x3) == 3
					&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x + 1, pos.y + 1, pos.z))) && (pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y + 1, pos.z)) & 0x3) == 2)
				{
					pWorld->setBlockMetadataWithNotify(pos.getPosX(), pWorld->getBlockMeta(pos.getPosX()) & 0xC, 3);
					pWorld->setBlockMetadataWithNotify(pos.getPosY(), pWorld->getBlockMeta(pos.getPosY()) & 0xC, 3);
					pWorld->setBlockMetadataWithNotify(BlockPos(pos.x + 1, pos.y + 1, pos.z), pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y + 1, pos.z)) & 0xC, 3);
				}
			}
			else if (metaZ == 3
				&& isSameJigsaw(pWorld->getBlockId(pos.getNegX())) && (pWorld->getBlockMeta(pos.getNegX()) & 0x3) == 2
				&& isSameJigsaw(pWorld->getBlockId(pos.getPosY())) && (pWorld->getBlockMeta(pos.getPosY()) & 0x3) == 2
				&& isSameJigsaw(pWorld->getBlockId(BlockPos(pos.x - 1, pos.y + 1, pos.z))) && (pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y + 1, pos.z)) & 0x3) == 1)
			{
				pWorld->setBlockMetadataWithNotify(pos.getNegX(), pWorld->getBlockMeta(pos.getNegX()) & 0xC, 3);
				pWorld->setBlockMetadataWithNotify(pos.getPosY(), pWorld->getBlockMeta(pos.getPosY()) & 0xC, 3);
				pWorld->setBlockMetadataWithNotify(BlockPos(pos.x - 1, pos.y + 1, pos.z), pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y + 1, pos.z)) & 0xC, 3);
			}
		}

		// y = 0
		if (getBlockID() >= m_jigsawBeginBlockId && getBlockID() < m_jigsawBeginBlockId + m_jigsawBlockSize)
		{
			int offsetY = getBlockID() - m_jigsawBeginBlockId;

			// face neg z
			if (offsetY == 0
				&& pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 1
				&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 2
				&& pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 1)
			{
				pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
				pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z + 1)), 3);
				pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
			}
			else if (offsetY == 1)
			{
				if (pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 0
					&& pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 2
					&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 1)
				{
					pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
					pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
					pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z + 1)), 3);
				}
				else if (pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 0
					&& pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 2
					&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 1)
				{
					pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
					pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
					pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z - 1)), 3);
				}
			}
			else if (offsetY == 2
				&& pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 1
				&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 0
				&& pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 1)
			{
				pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
				pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z - 1)), 3);
				pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
			}

			// face pos x
			else if (offsetY == 3
				&& pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 4
				&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 5
				&& pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 4)
			{
				pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
				pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z + 1)), 3);
				pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
			}
			else if (offsetY == 4)
			{
				if (pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 3
					&& pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 5
					&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 4)
				{
					pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
					pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
					pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z - 1)), 3);
				}
				else if (pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 3
					&& pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 5
					&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 4)
				{
					pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
					pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
					pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z + 1)), 3);
				}
			}
			else if (offsetY == 5
				&& pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 4
				&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 3
				&& pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 4)
			{
				pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
				pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z - 1)), 3);
				pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
			}

			// face pos z
			else if (offsetY == 6
				&& pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 7
				&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 8
				&& pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 7)
			{
				pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
				pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z - 1)), 3);
				pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
			}
			else if (offsetY == 7)
			{
				if (pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 6
					&& pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 8
					&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 7)
				{
					pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
					pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
					pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z - 1)), 3);
				}
				else if (pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 6
					&& pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 8
					&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 7)
				{
					pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
					pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
					pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z + 1)), 3);
				}
			}
			else if (offsetY == 8
				&& pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 7
				&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 6
				&& pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 7)
			{
				pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
				pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z + 1)), 3);
				pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
			}

			// face neg x
			else if (offsetY == 9
				&& pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 10
				&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 11
				&& pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 10)
			{
				pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
				pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z - 1)), 3);
				pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
			}
			else if (offsetY == 10)
			{
				if (pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 9
					&& pWorld->getBlockId(pos.getPosX()) - m_jigsawBeginBlockId == 11
					&& pWorld->getBlockId(BlockPos(pos.x + 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 10)
				{
					pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
					pWorld->setBlock(pos.getPosX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosX()), 3);
					pWorld->setBlock(BlockPos(pos.x + 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x + 1, pos.y, pos.z + 1)), 3);
				}
				else if (pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 9
					&& pWorld->getBlockId(pos.getNegZ()) - m_jigsawBeginBlockId == 11
					&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z - 1)) - m_jigsawBeginBlockId == 10)
				{
					pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
					pWorld->setBlock(pos.getNegZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegZ()), 3);
					pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z - 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z - 1)), 3);
				}
			}
			else if (offsetY == 11
				&& pWorld->getBlockId(pos.getPosZ()) - m_jigsawBeginBlockId == 10
				&& pWorld->getBlockId(BlockPos(pos.x - 1, pos.y, pos.z + 1)) - m_jigsawBeginBlockId == 9
				&& pWorld->getBlockId(pos.getNegX()) - m_jigsawBeginBlockId == 10)
			{
				pWorld->setBlock(pos.getPosZ(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getPosZ()), 3);
				pWorld->setBlock(BlockPos(pos.x - 1, pos.y, pos.z + 1), m_jigsawBlockId, pWorld->getBlockMeta(BlockPos(pos.x - 1, pos.y, pos.z + 1)), 3);
				pWorld->setBlock(pos.getNegX(), m_jigsawBlockId, pWorld->getBlockMeta(pos.getNegX()), 3);
			}
		}
	}

	void CustomBlock::harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage)
	{
		if (isJigsawBlock() && m_blockID != m_jigsawBlockId)
		{
			Block* pBlock = BlockManager::sBlocks[m_jigsawBlockId];
			if (pBlock)
			{
				pBlock->harvestBlock(pWorld, pPlayer, pos, 0);
			}
		}
		else
		{
			Block::harvestBlock(pWorld, pPlayer, pos, isJigsawBlock() ? 0 : damage);
		}
	}

	void CustomBlock::setJigsawBlock(int blockId, int beginBlockId, int blockSize)
	{
		m_jigsawBlockId = blockId;
		m_jigsawBeginBlockId = beginBlockId;
		m_jigsawBlockSize = blockSize;
	}

	bool CustomBlock::isSameJigsaw(int blockId)
	{
		return isJigsawBlock() && (blockId == m_jigsawBlockId || (blockId >= m_jigsawBeginBlockId && blockId < m_jigsawBeginBlockId + m_jigsawBlockSize));
	}

	bool CustomBlock::renderAsNormalBlock() const
	{
		return m_state == BlockState::SOLID && !m_transparent;
	}

	bool CustomBlock::isCollidable() const 
	{ 
		return m_state == BlockState::SOLID; 
	}
}
