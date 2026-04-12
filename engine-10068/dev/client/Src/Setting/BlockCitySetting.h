#pragma once
#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;
namespace BLOCKMAN
{
	class BlockCityItem {
	public:
		i32 id = 0;
		i32 price = 0;
		i32 score = 0;
		i32 cube = 1;
		String name = "";
		String desc = "";
		String icon = "";
		DEFINE_NETWORK_SERIALIZER(id, name, desc, icon, price, cube, score);
	};

	class BlockCitySetting
	{
	public:
		static void loadSetting();
		static void unloadSetting();
	public:
		static BlockCityItem* getItem(i32 id);
		static i32 getItemPrice(i32 id);
		static i32 getItemCube(i32 id);
		static String getItemIcon(i32 id);
		static String getItemName(i32 id);
		static String getItemDesc(i32 id);
		

	private:
		static std::map<i32, BlockCityItem*> m_blockCityItemMap;
	};
}
