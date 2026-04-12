#ifndef __BLOCKCITY_PORTAL_SETTING_HEADER__
#define __BLOCKCITY_PORTAL_SETTING_HEADER__

#include <map>
#include "Core.h"

using namespace LORD;
namespace LORD
{
	class CsvReader;
};
namespace BLOCKMAN
{
	class BlockCityPortalActorItem
	{
	public:
		i32 id = 0;
		Vector3 startPos = Vector3::ZERO;
		Vector3 endPos = Vector3::ZERO;
	};

	class BlockCityPortalItem
	{
	public:
		i32 id = 0;
		i32 portalId = 0;
		bool isOpen = false;
		String name = "";
	};

	class BlockCityPortalSetting : public Singleton<BlockCityPortalSetting>, public ObjectAlloc
	{
	public:
		BlockCityPortalSetting();
		~BlockCityPortalSetting();

	private:
		static bool loadSetting();
		static bool loadActorSetting();
		static void unloadSetting();
		static void unloadActorSetting();

	public:
		static i32 getPortalActorByPos(Vector3 pos);
		static vector<BlockCityPortalItem>::type getPortalItemsById(i32 portalId);
 		
	private:
		static map<i32, BlockCityPortalActorItem*>::type m_portalActorItemMap;
		static vector<BlockCityPortalItem>::type m_portalItems;

	};
}
#endif 
