#ifndef __ABOUT_SHOPPING_HEADER__
#define __ABOUT_SHOPPING_HEADER__

#include "Network/protocol/PacketDefine.h"

namespace BLOCKMAN
{
	enum class CommodityCategory : ui8
	{
		TOOL,
		ARMOR,
		ARMS,
		BLOCK,
		FOOD,
		OTHERS,
		UPGRADE,
		COLOR,
		WOOD,
		STONE,
		METAL,
		SKILLBOOK,
		SKILLBOOKFRAGMENTATION,
		SELL,
		SUIT,
		POTION
	};

	enum class BuyResult
	{
		Invalid = -1,
		BuySuccess = 0,
		LackOfMoney = 1,
		HasNoGun = 2,
		LackingGood = 3,
		FullInventory = 4,
		LackOfMoneyGoAppShop = 5,
		LackOfItem = 6,
		HaveGotGood = 7,
	};

	class Commodity
	{
	public:
		i32 uniqueId = 0; // this is used to check if the Commodity in the client side and server side is the same
		i32 index = 0;
		i32 itemId = 0;
		i32 itemMeta = 0;
		i32 itemNum = 0;
		i32 coinId = 0;
		i32 price = 0;
		String desc = "";
		String image = "";
		i32 notify = 0;
		String tipDesc = "";
		bool videoAd = false;

		DEFINE_NETWORK_SERIALIZER(uniqueId, index, itemId, itemMeta, itemNum, coinId, price, desc, image, notify, tipDesc, videoAd);
	};

	class CommodityGroup
	{
	private:
		i32 m_index = 0;
		CommodityCategory m_category;
		vector<Commodity>::type m_commodities;

	public:
		CommodityGroup() = default;
		CommodityGroup(const CommodityGroup&) = default;
		CommodityGroup(CommodityGroup&&) = default;
		CommodityGroup& operator=(const CommodityGroup&) = default;
		CommodityGroup& operator=(CommodityGroup&&) = default;

		CommodityGroup(int index, CommodityCategory category)
			: m_index(index)
			, m_category(category)
		{}

		CommodityCategory getCategory() const
		{
			return m_category;
		}

		vector<Commodity>::type getCommodities()
		{
			return m_commodities;
		}

		Commodity& operator[](size_t index)
		{
			return m_commodities[index];
		}

		const Commodity& operator[](size_t index) const
		{
			return m_commodities[index];
		}

		size_t size() const
		{
			return m_commodities.size();
		}

		i32 getIndex() {
			return m_index;
		}

		void addCommodity(Commodity commodity)
		{
			commodity.index = m_commodities.size();
			m_commodities.push_back(commodity);
		}

		DEFINE_NETWORK_SERIALIZER(m_index, m_category, m_commodities);
	};

	class CommodityList : public ObjectAlloc
	{
	private:
		vector<CommodityGroup>::type m_commodities;
		int commodityCount = 0;
	public:
		bool isNew = true;

	public:
		CommodityList() = default;

		vector<CommodityGroup>::type getCommodities()
		{
			return m_commodities;
		}

		void addCommodity(CommodityCategory category, const Commodity & commodity);

		void clearCommoditys()
		{
			m_commodities.clear();
		}
	};

	class CommodityMapping
	{
	public:
		// player(rakssId)能看到merchant(entityId)的物品列表的index
		int entityId;
		ui64 rakssId;
		int index;

		CommodityMapping(int entityId, ui64 rakssId, int index)
		{
			this->entityId = entityId;
			this->rakssId = rakssId;
			this->index = index;
		}
	};

}

#endif // !__ABOUT_SHOPPING_HEADER__