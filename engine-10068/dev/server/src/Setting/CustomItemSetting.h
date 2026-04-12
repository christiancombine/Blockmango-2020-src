#pragma once
#include <map>
#include "Core.h"

using namespace LORD;
namespace BLOCKMAN
{
	class CustomItemSetting : public Singleton<CustomItemSetting>, public ObjectAlloc
	{
	public:
		CustomItemSetting();
		~CustomItemSetting() = default;

	private:
		void loadSetting();
		void loadMapSetting();

		void loadItemGunSetting(const String& path);
		void loadItemBulletClipSetting(const String& path);
		void loadItemBuildSetting(const String& path);
		void loadItemHouseSetting(const String& path);
		void loadItemTemplateSetting(const String& path);
		void loadItemFruitCurrencySetting(const String& path);
		void loadItemCustomSetting(const String& path);

	public:
		void createItem(int itemId, int renderType, String name, String className, String params);
	};
}
