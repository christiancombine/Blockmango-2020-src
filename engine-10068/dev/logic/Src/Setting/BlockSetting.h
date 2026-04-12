#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{

	class BlockSetInfo
	{
	public:
		int		id = 0;
		int		blockId = 0;
		int		meta = 0;
		float	defaultHurt = 1;

		std::map<int, float> m_breakBlockValue;
	};

	class BlockSetting : public Singleton<BlockSetting>, public ObjectAlloc
	{
	public:

		BlockSetting();
		~BlockSetting();
		bool loadSetting(bool bIsClient);
		void unloadSetting();
		float getBreakBlockValue(int toolId, int blockId);

	private:
		std::map<int, BlockSetInfo> m_blockSettingMap;
	};
}