#ifndef __CUSTOM_BLOCK_HEADER__
#define __CUSTOM_BLOCK_HEADER__

#include "Block.h"
#include "BlockConfig.h"
#include "Item/Item.h"

namespace BLOCKMAN
{
	class CustomBlock : public Block
	{
	public:
		CustomBlock(int id, const String &nameSpace, const String &name, const BlockConfig& config);
		~CustomBlock();
		int idDropped(int meta, Random& rand, int fortune) override;

		String getNamespace() const { return m_namespace; }
		String getFullName() const { return m_namespace + ":" + m_blockName; }
		virtual bool renderAsNormalBlock() const override;
		virtual bool isCollidable() const override;
		virtual int quantityDropped(Random& rand) override;
		virtual Box getCollisionBoundingBox(World* pWorld, const BlockPos& pos) override;
		virtual bool onBlockActivated(World* pWorld, const BlockPos& pos, EntityPlayer* pPlayer, int face, const Vector3& hit);
		virtual void onBlockPlacedBy(World* pWorld, const BlockPos& pos, EntityLivingBase* pLiving, ItemStackPtr pStack);
		virtual void onBlockDestroyedByPlayer(World* pWorld, const BlockPos& pos, int meta);
		virtual void harvestBlock(World* pWorld, EntityPlayer* pPlayer, const BlockPos& pos, int damage);

		void setJigsawBlock(int blockId, int beginBlockId, int blockSize);
		int getJigsawBeginBlockId() { return m_jigsawBeginBlockId; }
		int getJigsawBlockSize() { return m_jigsawBlockSize; }
		bool isJigsawBlock() { return m_jigsawBeginBlockId != -1; }
		bool isSameJigsaw(int blockId);

	protected:
		String m_namespace; // namespace
		BlockState m_state = BlockState::SOLID;
		LORD::Box m_collisionBox = LORD::Box({ 0,0,0 }, { 1,1,1 });
		bool m_clickable = false;
		bool m_transparent = false;
		int m_jigsawBlockId = -1;
		int m_jigsawBeginBlockId = -1;
		int m_jigsawBlockSize = 12;
	};
}

#endif // !__CUSTOM_BLOCK_HEADER__
