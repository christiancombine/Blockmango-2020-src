#ifndef __BLOCK_LOADER_HEADER__
#define __BLOCK_LOADER_HEADER__

#include <unordered_map>
#include "Core.h"
#include "Block/BlockArchive.h"
#include "Block/BlockConfig.h"

namespace BLOCKMAN
{
	class CustomBlock;
	struct BlockConfig;
	class ItemBlock;

	class BlockLoader : public LORD::ObjectAlloc, public LORD::Singleton<BlockLoader>
	{
	public:
		BlockLoader();
		virtual ~BlockLoader() = default;
		void loadCustomBlock(int blockId, const LORD::String& blockDir);
		void loadCustomBlocksInMap(const LORD::String& mapPath);

	protected:
		LORD::String getPluginName(const LORD::String& blockDir) const;
		LORD::String getBlockName(const LORD::String& blockDir) const;
		virtual void prepareBlock(CustomBlock* block, const BlockConfig& config);
		virtual bool loadBlockResources(const LORD::String& blockDir, const BlockConfig& config) { return true; }

	private:
		std::unique_ptr<BlockArchiveFactory> m_blockArchiveFactory;

		virtual CustomBlock* createBlock(int blockId, const LORD::String &pluginName, const LORD::String &blockName, const BlockConfig &config);
		virtual ItemBlock* createItemBlock(int itemId);
		bool readConfig(const LORD::String & blockDir, BlockConfig& config);
		bool readConfigToMap(const LORD::String blockDir);

		std::unordered_map<int, BlockConfig> configMap;
	};
}

#endif // !__BLOCK_LOADER_HEADER__
