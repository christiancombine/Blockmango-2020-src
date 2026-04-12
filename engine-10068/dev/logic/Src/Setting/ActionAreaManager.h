#ifndef __ACTION_AREA_HEADER__
#define __ACTION_AREA_HEADER__

#include <map>
#include "Core.h"
#include "Network/protocol/PacketDefine.h"

using namespace LORD;

namespace BLOCKMAN
{
	enum class AreaType
	{
		BLOCK_CITY_TIGER_LOTTERY = 104701,
		BLOCK_CITY_CANNON = 104702,
		BLOCK_CITY_MAILBOX = 104703,
		BLOCK_CITY_DECORATION = 104704,
		BLOCK_CITY_PUBLIC_CHAIR = 104705,
		BLOCK_CITY_MANAGER = 104706,
		BLOCK_CITY_HOUSE_SHOP = 104707,
	};

	class ActionArea
	{
	public:
		i32 id = 0;
		i32 type = 0;
		String titleNor = "";
		String titlePre = "";
		String backNor = "";
		String backPre = "";
		bool isVisible = false;
		Vector3i startPos = Vector3i::INVALID;
		Vector3i endPos = Vector3i::INVALID;
		DEFINE_NETWORK_SERIALIZER(id, type, titleNor, titlePre, backNor, backPre, startPos, endPos);
		ActionArea() {};
	};

	class ActionAreaManager
	{
	public:
		static void unloadSetting();
		static void setActionAreas(const vector<ActionArea>::type& areas);
		static ActionArea* ActionAreaByPosition(const Vector3& pos);
		

	private:
		static vector<ActionArea*>::type m_areas;
	};
	
}

#endif