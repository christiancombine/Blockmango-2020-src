#pragma once

#include "Any/Any.h"
#include "MacroUtil.h"
#include "Object/Singleton.h"
#include "BM_TypeDef.h"

using Vector3iBoolMap = std::vector<std::pair<Vector3i, bool>>;

using Vector3iBoolMap = std::vector<std::pair<Vector3i, bool>>;

namespace BLOCKMAN
{
    struct ChestInfo
    {
        BlockPos pos{};
        String headName{};
    };
    //using ChestInfoVec = std::vector<ChestInfo>;
    using ChestInfoMap = std::map<std::string, ChestInfo>;

	class PlayerPrefs : public Singleton<PlayerPrefs>, public ObjectAlloc
	{
	public:
		PlayerPrefs();

		void init();

		void putBoolPrefs(const String& key, bool value);
		bool getBoolPrefs(const String& key);

		void putIntPrefs(const String& key, int value);
		int getIntPrefs(const String& key);

		void putFloatPrefs(const String& key, float value);
		float getFloatPrefs(const String& key);

		void putStringPrefs(const String& key, String value);
		String getStringPrefs(const String& key);

		DECLARE_PROPERTY_MAP;
	};
}