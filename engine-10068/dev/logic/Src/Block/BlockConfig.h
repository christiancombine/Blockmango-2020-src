#ifndef __BLOCK_CONFIG_HEADER__
#define __BLOCK_CONFIG_HEADER__

#include "Core.h"

namespace BLOCKMAN
{
	enum class BlockState
	{
		SOLID,
		GAS
	};

	enum class TransparentMode
	{
		ALPHA_BLEND,
		MULTIPLY,
		LIGHT_MULTIPLY
	};

	enum class AnimType
	{
		NONE,
		FRAME,
		ROLLING,
		VERTICAL_ROLLING,
		ROTATION
	};

	struct BlocksAnimConfig
	{
		AnimType animType = AnimType::NONE;
		int speed = 0;
		int frameSize = 64;
	};

	class Trigger;
	using TriggerPtr = std::shared_ptr<Trigger>;
	using TriggerList = LORD::vector<TriggerPtr>::type;

	struct BlockConfig
	{
		struct TextureConfig
		{
			LORD::String top;
			LORD::String bottom;
			LORD::String left;
			LORD::String right;
			LORD::String front;
			LORD::String back;
		} texture;
		struct JigsawTextureConfig
		{
			LORD::String jigsaw1;
			LORD::String jigsaw2;
			LORD::String jigsaw3;
			LORD::String jigsaw4;
		} jigsawTexture;
		LORD::Color color = LORD::Color::WHITE;
		struct {
			int red = 0;
			int green = 0;
			int blue = 0;
		} lightEmitted;
		BlockState state = BlockState::SOLID;
		bool transparent = false;
		bool requiresNoTool = true;
		TransparentMode transparentMode = TransparentMode::ALPHA_BLEND;
		bool transparentGridMode = false;
		bool renderable = true;
		LORD::Box collisionBox = { {0,0,0},{1,1,1} };
		float resistance = 15;
		float hardness = 15;

		struct {
			int red = 0;
			int green = 0;
			int blue = 0;
		} lightDecay;

		LORD::i8 lightStrength = 0;
		
		bool flammable = false;
		TriggerList triggers;
		int jigsawBeginBlockId = -1;

		bool blockFaceToPlayer = false;
		LORD::Color blockColor = LORD::Color::WHITE;
		struct BlocksAnimation
		{
			BlocksAnimConfig top;
			BlocksAnimConfig bottom;
			BlocksAnimConfig left;
			BlocksAnimConfig right;
			BlocksAnimConfig front;
			BlocksAnimConfig back;
		} anim;

		static bool readJson(BlockConfig& output, const LORD::String& configPath);

		static bool readCsvToMap(std::unordered_map<int, BlockConfig>& configMap, const LORD::String& configPath);

	private:
		static bool validateTextureConfig(const TextureConfig& config);
		static bool validateJigsawTextureConfig(const JigsawTextureConfig& config);
	};
}

#endif // !__BLOCK_CONFIG_HEADER__
