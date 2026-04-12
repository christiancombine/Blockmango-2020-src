#ifndef __CUSTOM_BLOCK_CLIENT_HEADER__
#define __CUSTOM_BLOCK_CLIENT_HEADER__

#include "cBlock/cBlock.h"
#include "Block/CustomBlock.h"

namespace BLOCKMAN
{
	class CustomBlockClient : public CustomBlock, public cBlock
	{
	public:
		CustomBlockClient(int id, const String &nameSpace, const String &name, const BlockConfig& config);
		void setTextures(const BlockConfig& config);
		void setBlocksAnimation(const BlockConfig& config);
		void setBlockFaceToPlayer(bool sign) { m_blockFaceToPlayer = sign; }
		void setBlockAnimParam(const std::unique_ptr<AtlasSprite>& sprite, const BlocksAnimConfig& config) const;
		virtual AtlasSprite* getIcon(int side, int meta) override;
		virtual int getRenderBlockPass() override;
		virtual int getRenderBlockPass(int side, int meta = 0) override;
		virtual bool shouldSideBeRendered(IBlockAccess* pBlockAccess, const BlockPos& pos, int side) override;
		virtual ui32 colorMultiplier(IBlockAccess* piBlockAccess, const BlockPos& pos) override;
		virtual int getRenderType() const { return m_jigsawTexture.jigsaw1 ? RENDER_TYPE_JIGSAW : RENDER_TYPE_TRAPDOOR; }

		virtual bool hasBlockAnimation() const override;
		virtual std::uint64_t caculateBlockAnimKey(int side, int meta = 0) override;
		virtual bool hasBlockFaceToPlayer() const override { return m_blockFaceToPlayer; }

	private:
		struct
		{
			std::unique_ptr<AtlasSprite> top;
			std::unique_ptr<AtlasSprite> bottom;
			std::unique_ptr<AtlasSprite> left;
			std::unique_ptr<AtlasSprite> right;
			std::unique_ptr<AtlasSprite> front;
			std::unique_ptr<AtlasSprite> back;
		} m_texture;
		struct
		{
			AtlasSprite* jigsaw1 = nullptr;
			AtlasSprite* jigsaw2 = nullptr;
			AtlasSprite* jigsaw3 = nullptr;
			AtlasSprite* jigsaw4 = nullptr;
		} m_jigsawTexture;
		TransparentMode m_transparentMode = TransparentMode::ALPHA_BLEND;
		bool m_transparentGridMode = false;
		bool m_renderable = true;
		LORD::Color m_color = LORD::Color::WHITE;

		bool m_blockFaceToPlayer = false;
		unsigned long m_startTime = 0;
		struct
		{
			BlocksAnimConfig top;
			BlocksAnimConfig bottom;
			BlocksAnimConfig left;
			BlocksAnimConfig right;
			BlocksAnimConfig front;
			BlocksAnimConfig  back;
		} m_animConfig;

		AtlasSprite* getSprite(const LORD::String& textureString);

		BlocksAnimConfig getBlocksAnimConfig(int side, int meta) const;

		std::unique_ptr<AtlasSprite> makeUniqueAtlasSpriteAnimation(AtlasSprite* sprite, const BlocksAnimConfig& config) const;

		int caculateFaceToward(int side, int meta) const;

		std::uint64_t caculateBlockAnimKey(const AtlasSprite* sprite, const BlocksAnimConfig& config, const Texture* tex) const;

		Vector2 caculateOffsetUV(const std::unique_ptr<AtlasSprite>& sprite, const BlocksAnimConfig& config) const;

		int caculateTotalFrame(const std::unique_ptr<AtlasSprite>& sprite, const BlocksAnimConfig& config) const;

		Vector2 caculateRotationAnimationAplitude(const std::unique_ptr<AtlasSprite>& sprite) const;
	};
}

#endif // !__CUSTOM_BLOCK_CLIENT_HEADER__
