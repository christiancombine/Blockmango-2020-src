#ifndef __WALLET_HEADER__
#define __WALLET_HEADER__

#include "Core.h"
#include "Util/SubscriptionGuard.h"
using namespace LORD;

namespace BLOCKMAN
{
	class InventoryPlayer;

	enum CurrencyType
	{
		UNKNOWN = -1,
		PLATFORM_MONEY_GOLD = 0, //金魔方
		PLATFORM_MONEY = 1,
		PLATFORM_GOLD = 2,
		GAME_MONEY = 3,
		OTHER_MONEY = 4,
		KEY_MONEY = 5,
	};

	class Wallet : public ObjectAlloc
	{
	public:
		i64 m_diamondBlues = -1; //蓝魔方
		i64 m_diamondGolds = -1; //金魔方

	private:
		int m_coins[21] = {0};
		InventoryPlayer* m_inventory = nullptr;
		bool m_inventoryChanged = true;
		SubscriptionGuard m_subscriptionGuard;
		i64 m_golds = -1;
		i64 m_currency = -1;

	public:
		Wallet(InventoryPlayer* inventory);
		virtual ~Wallet() {};

		int getCoinNum(int coindId)
		{
			return m_coins[coindId];
		}

		void consumeCoin(int coinId, int num);
		void onUpdate();

		void setGolds(i64 golds) { m_golds = golds; }
		void setCurrency(i64 currency) { m_currency = currency; }
		void subCurrency(i32 currency) { m_currency = m_currency - currency >= 0 ? m_currency - currency : 0; }
		void addCurrency(i32 currency) { m_currency += currency; }
		i64 getAllDiamonds() { return m_diamondBlues + m_diamondGolds; }
		i64 getDiamondsBlues() { return m_diamondBlues; }
		i64 getDiamondsGolds() { return m_diamondGolds; }
		i64 getGolds() { return m_golds; }
		i64 getCurrency() { return m_currency; }

		i64 getMoneyByCoinId(i32 coinId)
		{
			switch (coinId)
			{
			case PLATFORM_MONEY_GOLD:
				return m_diamondGolds;
			case PLATFORM_MONEY:
				return m_diamondGolds;
			case PLATFORM_GOLD:
				return m_golds;
			case GAME_MONEY:
				return m_currency;
			default:
				return 999999999999;
			}
		}
	};
}

#endif // !__WALLET_HEADER__
