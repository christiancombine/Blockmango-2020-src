#ifndef __STORE_TAB_SETTING_HEADER__
#define __STORE_TAB_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;
namespace LORD
{
	class CsvReader;
};
namespace BLOCKMAN
{
	class StoreTab
	{
	public:
		i32 id = 0;
		String icon = "";
		String name = "";
		StoreTab() {}
	};

	
	class StoreTabSetting : public Singleton<StoreTabSetting>, public ObjectAlloc
	{
	public:
		StoreTabSetting();
		~StoreTabSetting();

	private:
		bool loadSetting();
		void unloadSetting();

	public:
		StoreTab* findItem(i32 id);
		

	private:
		

	private:
		map<i32, StoreTab*>::type m_itemMap;
	};
}
#endif 
