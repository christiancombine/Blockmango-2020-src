#ifndef __BLOCK_FORT_ITEM_SETTING_HEADER__
#define __BLOCK_FORT_ITEM_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;
namespace LORD
{
	class CsvReader;
};
namespace BLOCKMAN
{
	class BlockFortItemInfo
	{
	public:
		i32 id = 0;
		String icon = "";
		String name = "";
		String value = "";
		String gain = "";
		BlockFortItemInfo() {}
		BlockFortItemInfo(i32 id, String n, String i) : id(id), name(n), icon(i) {}
	};

	class BlockFortItem
	{
	public:
		i32 id = 0;
		i32 itemId = 0;
		i32 type = 0;
		i32 price = 0;
		i32 cube = 1;
		i32 level = 1;
		i32 hp = 0;
		i32 exp = 0;
		i32 damage = 0;

		String name = "";
		String desc = "";
		String icon = "";
		String size = "";

		vector<BlockFortItemInfo>::type infos;

		BlockFortItem() {}
	};

	class BlockFortBuilding
	{
	public:
		i32 id = 0;
		i32 moneyType = 0;
		String price = "";
		String cube = "";
		vector<BlockFortItemInfo>::type infos;
		BlockFortBuilding() {}
	};

	class BlockFortItemSetting : public Singleton<BlockFortItemSetting>, public ObjectAlloc
	{
	public:
		BlockFortItemSetting(bool isClient);
		~BlockFortItemSetting();

	private:
		bool loadSetting(bool isClient);
		void unloadSetting();
		bool loadBuildingSetting(bool isClient);
		void unloadBuildingSetting();

	public:
		BlockFortItem* findItem(i32 id);
		BlockFortItem* findItemByItemId(i32 itemId);
		String getItemIcon(i32 id);
		String getItemName(i32 id);
		String getItemDesc(i32 id);
		i32 getItemPrice(i32 id, i32 moneyType);
		i32 getItemType(i32 id);
		const vector<BlockFortItemInfo>::type getItemInfos(i32 id);

	private:
		BlockFortItemInfo getItemInfo(String key, CsvReader* reader);

	private:
		map<String, BlockFortItemInfo>::type m_itemInfos;
		map<i32, BlockFortItem*>::type m_itemMap;

	public:
		BlockFortItemInfo getBuildingItemInfo(String key, CsvReader* reader);
		BlockFortBuilding* findBuilding(i32 id);

	private:
		map<i32, BlockFortBuilding*>::type m_buildingMap;
		map<String, BlockFortItemInfo>::type m_buildingItemInfos;

	};
}
#endif 
