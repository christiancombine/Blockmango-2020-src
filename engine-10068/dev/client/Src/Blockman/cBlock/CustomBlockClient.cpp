#include "CustomBlockClient.h"
#include "Render/TextureAtlas.h"
#include "World/IBlockAccess.h"
#include "Tessolator/TessManager.h"

using namespace LORD;

namespace BLOCKMAN
{
	CustomBlockClient::CustomBlockClient(int id, const String & nameSpace, const String & name, const BlockConfig & config)
		: CustomBlock(id, nameSpace, name, config)
		, cBlock("")
	{
		auto components = StringUtil::Split(config.texture.top, ":");
		switch (components.size())
		{
		case 1: // "my_texture"
			m_atlasName = "plugins/" + m_namespace + "/blocks/" + name + "/texture.json";
			break;
		case 3: // "block:my_block:my_texture"
			m_atlasName = "plugins/" + m_namespace + "/blocks/" + components[1] + "/texture.json";
			break;
		case 5: // "plugin:my_plugin:block:my_block:my_texture"
			m_atlasName = "plugins/" + components[1] + "/blocks/" + components[3] + "/texture.json";
			break;
		}

		setBlocksAnimation(config);
		setTextures(config);
		m_transparentMode = config.transparentMode;
		m_transparentGridMode = config.transparentGridMode;
		m_renderable = config.renderable;
		m_color = config.blockColor;
		m_blockFaceToPlayer = config.blockFaceToPlayer;

		setBlockAnimParam(m_texture.top, m_animConfig.top);
		setBlockAnimParam(m_texture.bottom, m_animConfig.bottom);
		setBlockAnimParam(m_texture.left, m_animConfig.left);
		setBlockAnimParam(m_texture.right, m_animConfig.right);
		setBlockAnimParam(m_texture.front, m_animConfig.front);
		setBlockAnimParam(m_texture.back, m_animConfig.back);
	}

	void CustomBlockClient::setTextures(const BlockConfig & config)
	{
		m_texture.top = makeUniqueAtlasSpriteAnimation(getSprite(config.texture.top), m_animConfig.top);
		m_texture.bottom = makeUniqueAtlasSpriteAnimation(getSprite(config.texture.bottom), m_animConfig.bottom);
		m_texture.left = makeUniqueAtlasSpriteAnimation(getSprite(config.texture.left), m_animConfig.left);
		m_texture.right = makeUniqueAtlasSpriteAnimation(getSprite(config.texture.right), m_animConfig.right);
		m_texture.front = makeUniqueAtlasSpriteAnimation(getSprite(config.texture.front), m_animConfig.front);
		m_texture.back = makeUniqueAtlasSpriteAnimation(getSprite(config.texture.back), m_animConfig.back);
		if (config.jigsawBeginBlockId != -1)
		{
			if (config.jigsawTexture.jigsaw1.size() > 0)
				m_jigsawTexture.jigsaw1 = getSprite(config.jigsawTexture.jigsaw1);
			if (config.jigsawTexture.jigsaw2.size() > 0)
				m_jigsawTexture.jigsaw2 = getSprite(config.jigsawTexture.jigsaw2);
			if (config.jigsawTexture.jigsaw3.size() > 0)
				m_jigsawTexture.jigsaw3 = getSprite(config.jigsawTexture.jigsaw3);
			if (config.jigsawTexture.jigsaw4.size() > 0)
				m_jigsawTexture.jigsaw4 = getSprite(config.jigsawTexture.jigsaw4);
			m_sprite = m_texture.top.get();
		}
	}

	void CustomBlockClient::setBlocksAnimation(const BlockConfig& config)
	{
		m_animConfig.top = config.anim.top;
		m_animConfig.bottom = config.anim.bottom;
		m_animConfig.left = config.anim.left;
		m_animConfig.right = config.anim.right;
		m_animConfig.front = config.anim.front;
		m_animConfig.back = config.anim.back;
	}

	AtlasSprite * CustomBlockClient::getIcon(int side, int meta)
	{
		if (isJigsawBlock() && meta > 0)
		{
			if (meta == 1)
				return m_jigsawTexture.jigsaw1;
			else if (meta == 2)
				return m_jigsawTexture.jigsaw2;
			else if (meta == 3)
				return m_jigsawTexture.jigsaw3;
			else if (meta == 4)
				return m_jigsawTexture.jigsaw4;
		}

		switch (caculateFaceToward(side, meta))
		{
		case BM_FACE_DOWN:
			return m_texture.bottom.get();
		case BM_FACE_UP:
			return m_texture.top.get();
		case BM_FACE_SOUTH:
			return m_texture.front.get();
		case BM_FACE_NORTH:
			return m_texture.back.get();
		case BM_FACE_EAST:
			return m_texture.right.get();
		case BM_FACE_WEST:
			return m_texture.left.get();
		default:
			LordLogError("icon of custom block of invalid side is queried, side: %d", side);
			return nullptr;
		}
	}

	AtlasSprite* CustomBlockClient::getSprite(const String& textureString)
	{
		if (StringUtil::StartWith(textureString, "blockymods:"))
		{
			return TextureAtlasRegister::Instance()->getAtlasSprite(textureString.substr(11));
		}

		auto components = StringUtil::Split(textureString, ":");
		switch (components.size())
		{
		case 1:
			return TextureAtlasRegister::Instance()->getAtlasSprite(
				"plugins:" + m_namespace + ":blocks:" + m_blockName + ":" + textureString);
		case 3:
			return TextureAtlasRegister::Instance()->getAtlasSprite("plugins:" + m_namespace + ":" + textureString);
		case 5:
			return TextureAtlasRegister::Instance()->getAtlasSprite(textureString);
		default:
			LordLogError("BlockConfig used invalid texture name: %s", textureString.c_str());
			return nullptr;
		}
	}

	BlocksAnimConfig CustomBlockClient::getBlocksAnimConfig(int side, int meta) const
	{
		switch (caculateFaceToward(side, meta))
		{
		case BM_FACE_DOWN:
			return m_animConfig.bottom;
		case BM_FACE_UP: 
			return m_animConfig.top;
		case BM_FACE_SOUTH:
			return m_animConfig.front;
		case BM_FACE_NORTH:
			return m_animConfig.back;
		case BM_FACE_EAST:
			return m_animConfig.right;
		case BM_FACE_WEST:
			return m_animConfig.left;
		default:
			LordLogError("animation param of custom block of invalid side is queried, side: %d", side);
			return BlocksAnimConfig();
		}
	}

	std::unique_ptr<AtlasSprite> CustomBlockClient::makeUniqueAtlasSpriteAnimation(AtlasSprite* sprite, const BlocksAnimConfig& config) const
	{
		return std::unique_ptr<AtlasSprite>(new AtlasSpriteAnimation(*sprite, config.speed, config.animType == AnimType::ROTATION));
	}

	int CustomBlockClient::caculateFaceToward(int side, int meta) const
	{
		if (!m_blockFaceToPlayer || meta == 0)
		{
			return side;
		}

		if (meta == 1)
		{
			switch (side)
			{
			case BM_FACE_SOUTH:
				return BM_FACE_EAST;
			case BM_FACE_NORTH:
				return BM_FACE_WEST;
			case BM_FACE_EAST:
				return BM_FACE_NORTH;
			case BM_FACE_WEST:
				return BM_FACE_SOUTH;
			default:
				return side;
			}
		}

		if (meta == 2)
		{
			switch (side)
			{
			case BM_FACE_SOUTH:
				return BM_FACE_NORTH;
			case BM_FACE_NORTH:
				return BM_FACE_SOUTH;
			case BM_FACE_EAST:
				return BM_FACE_WEST;
			case BM_FACE_WEST:
				return BM_FACE_EAST;
			default:
				return side;
			}
		}

		if (meta == 3)
		{
			switch (side)
			{
			case BM_FACE_SOUTH:
				return BM_FACE_WEST;
			case BM_FACE_NORTH:
				return BM_FACE_EAST;
			case BM_FACE_EAST:
				return BM_FACE_SOUTH;
			case BM_FACE_WEST:
				return BM_FACE_NORTH;
			default:
				return side;
			}
		}

		return side;
	}

	int CustomBlockClient::getRenderBlockPass()
	{
		if (m_transparent || m_state == BlockState::GAS)
		{
			return TRP_TRANSPARENT_2D;
		}
		else
		{
			return TRP_SOLID;
		}
	}

	int CustomBlockClient::getRenderBlockPass(int side, int meta)
	{
		return getBlocksAnimConfig(side, meta).animType != AnimType::ROTATION ? getRenderBlockPass() : 0;
	}

	bool CustomBlockClient::shouldSideBeRendered(IBlockAccess * pBlockAccess, const BlockPos & pos, int side)
	{
		if (!m_renderable)
		{
			return false;
		}
		if (m_state == BlockState::GAS || !m_transparentGridMode)
		{
			return pBlockAccess->getBlockId(pos) != m_blockID && Block::shouldSideBeRendered(pBlockAccess, pos, side);
		}
		else
		{
			return Block::shouldSideBeRendered(pBlockAccess, pos, side);
		}
	}

	ui32 CustomBlockClient::colorMultiplier(IBlockAccess * piBlockAccess, const BlockPos & pos)
	{
		return m_color.getABGR();
	}

	bool CustomBlockClient::hasBlockAnimation() const
	{
		return m_animConfig.top.animType != AnimType::NONE
			|| m_animConfig.bottom.animType != AnimType::NONE
			|| m_animConfig.left.animType != AnimType::NONE
			|| m_animConfig.right.animType != AnimType::NONE
			|| m_animConfig.front.animType != AnimType::NONE
			|| m_animConfig.back.animType != AnimType::NONE;
	}

	std::uint64_t CustomBlockClient::caculateBlockAnimKey(int side, int meta)
	{
		const auto& config = getBlocksAnimConfig(side, meta);
		const auto& sprite = getIcon(side, meta);
		const auto& tex = sprite->getTexture();
		return caculateBlockAnimKey(sprite, config, tex);
	}

	uint64_t CustomBlockClient::caculateBlockAnimKey(const AtlasSprite* sprite, const BlocksAnimConfig& config, const Texture* tex) const
	{
		const auto& frame = sprite->getFrameSize();
		const auto& pos = sprite->getFramePos();
		
		if (config.animType == AnimType::NONE)
		{
			return 0;
		}

		uint64_t key = uint(config.animType);
		key <<= 12;
		key += config.speed;
		key <<= 16;
		key += (long(tex) & 0xFFFF);
		key <<= 16;
		if (config.animType == AnimType::ROTATION)
		{
			key += pos.x;
			key <<= 16;
			key += pos.y;
		}
		else
		{
			key += frame.x;
			key <<= 16;
			key += frame.y;
		}
		return key;
	}
	
	Vector2 CustomBlockClient::caculateOffsetUV(const std::unique_ptr<AtlasSprite>& sprite, const BlocksAnimConfig& config) const
	{
		const auto& frame = sprite->getFrameSize();
		const auto& texture = sprite->getTexture();
		Vector2 offset = Vector2::ZERO;

		switch (config.animType)
		{
		case AnimType::FRAME:
			offset.x = float(frame.y) / texture->getWidth();
			break;
		case AnimType::ROLLING:
			offset.x = float(frame.x - frame.y) / texture->getWidth() / caculateTotalFrame(sprite, config);
			offset.x *= config.speed < 0 ? -1.0 : 1.0;
			break;
		case AnimType::VERTICAL_ROLLING:
			offset.y = float(frame.y - frame.x) / texture->getHeight() / caculateTotalFrame(sprite, config);
			offset.y *= config.speed < 0 ? 1.0 : -1.0;
			break;
		case AnimType::ROTATION:
			offset.x = (sprite->getMaxU() + sprite->getMinU()) / 2;
			offset.y = (sprite->getMaxV() + sprite->getMinV()) / 2;
			break;
		default:
			break;
		}

		return offset;
	}

	int CustomBlockClient::caculateTotalFrame(const std::unique_ptr<AtlasSprite>& sprite, const BlocksAnimConfig& config) const
	{

		const auto& frame = sprite->getFrameSize();
		int rollingDefualtFrameNumber = config.frameSize;
		int rotatingDefaultFrame = rollingDefualtFrameNumber * 4;

		switch (config.animType)
		{
		case AnimType::FRAME:
			return frame.x / frame.y;
		case AnimType::ROLLING:
			return (frame.x - frame.y) / (frame.y / rollingDefualtFrameNumber);
		case AnimType::VERTICAL_ROLLING:
			return (frame.y - frame.x) / (frame.x / rollingDefualtFrameNumber);;
		case AnimType::ROTATION:
			return config.speed > 0 ? rotatingDefaultFrame : -rotatingDefaultFrame;
		default:
			return 1;
		}
	}

	Vector2 CustomBlockClient::caculateRotationAnimationAplitude(const std::unique_ptr<AtlasSprite>& sprite) const
	{
		const auto& u = sprite->getMaxU() - sprite->getMinU();
		const auto& v = sprite->getMaxV() - sprite->getMinV();

		return u > v ? Vector2(u / v, 1.0) : Vector2(1.0, v / u);
	}

	void CustomBlockClient::setBlockAnimParam(const std::unique_ptr<AtlasSprite>& sprite, const BlocksAnimConfig& config) const
	{
		std::uint64_t key = caculateBlockAnimKey(sprite.get(), config, sprite->getTexture());
		const auto& manager = TessManager::Instance();
		/*if (!manager->contains(key))
		{
			manager->setBlockAnimParam(key, caculateOffsetUV(sprite, config), caculateTotalFrame(sprite, config),
				config.speed, Time::Instance()->getMilliseconds(), caculateRotationAnimationAplitude(sprite));
		}*/
	}
}