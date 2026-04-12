#include "CustomItemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Item/Items.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Script/Setting/ScriptSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"

#if LORD_PLATFORM != LORD_PLATFORM_WINDOWS
#include "Server.h"
#endif

namespace BLOCKMAN
{
	CustomItemSetting::CustomItemSetting()
	{
		loadSetting();
		loadMapSetting();
	}

	void CustomItemSetting::loadSetting()
	{
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		String engineSettingPath = "Media/Setting";
		String gameSettingDir = StringUtil::Replace(ScriptSetting::m_scriptPath, "ServerGame", "GameSetting");
#else
		String engineSettingPath = "";
		ScriptSetting::loadGameScriptDir(Server::Instance()->getConfig().gameType.c_str(), true, false);
		String gameSettingDir = StringUtil::Replace(ScriptSetting::m_scriptPath, "ClientGame", "GameSetting");
		gameSettingDir = StringUtil::Replace(gameSettingDir, "Media/Scripts/", "");
#endif
		// ----------------- load engine custom item -----------------
		String path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), engineSettingPath, "ItemGun.csv");
		GunSetting::loadSetting(path); 
		loadItemGunSetting(path);
		path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), engineSettingPath, "ItemBulletClip.csv");
		BulletClipSetting::loadSetting(path);
		loadItemBulletClipSetting(path);
		path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), engineSettingPath, "ItemCustom.csv");
		loadItemCustomSetting(path);

		// ----------------- load game custom item -----------------
		gameSettingDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), gameSettingDir);
		LordLogInfo("CustomItemSetting::loadSetting [gameSettingDir]:%s", gameSettingDir.c_str());
		path = PathUtil::ConcatPath(gameSettingDir, "ItemBuilding.csv");
		loadItemBuildSetting(path);
		path = PathUtil::ConcatPath(gameSettingDir, "ItemHouse.csv").c_str();
		loadItemHouseSetting(path);
		path = PathUtil::ConcatPath(gameSettingDir, "ItemTemplate.csv").c_str();
		loadItemTemplateSetting(path);
		path = PathUtil::ConcatPath(gameSettingDir, "ItemFruitCurrency.csv").c_str();
		loadItemFruitCurrencySetting(path);
		path = PathUtil::ConcatPath(gameSettingDir, "ItemGun.csv").c_str();
		GunSetting::loadSetting(path);
		loadItemGunSetting(path);
		path = PathUtil::ConcatPath(gameSettingDir, "ItemBulletClip.csv").c_str();
		BulletClipSetting::loadSetting(path);
		loadItemBulletClipSetting(path);
		path = PathUtil::ConcatPath(gameSettingDir, "ItemCustom.csv").c_str();
		loadItemCustomSetting(path);
	}

	void CustomItemSetting::loadMapSetting()
	{
		// ----------------- load map custom item -----------------
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
		String mapDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName());
#else
		String mapDir = Root::Instance()->getMapPath();
#endif
		String path = PathUtil::ConcatPath(mapDir, "setting/ItemBuilding.csv");
		loadItemBuildSetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/ItemHouse.csv").c_str();
		loadItemHouseSetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/ItemTemplate.csv").c_str();
		loadItemTemplateSetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/ItemFruitCurrency.csv").c_str();
		loadItemFruitCurrencySetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/ItemGun.csv").c_str();
		GunSetting::loadSetting(path);
		loadItemGunSetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/ItemBulletClip.csv").c_str();
		BulletClipSetting::loadSetting(path);
		loadItemBulletClipSetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/ItemCustom.csv").c_str();
		loadItemCustomSetting(path);
		path = PathUtil::ConcatPath(mapDir, "setting/customItem.csv").c_str();
		loadItemCustomSetting(path);
	}

	void CustomItemSetting::loadItemGunSetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int itemId = 0;
			String name = "";
			pReader->GetInt("GunId", itemId);
			pReader->GetString("Name", name);
			createItem(itemId, 0, name, "ItemGun", "");
		}
	}

	void CustomItemSetting::loadItemBulletClipSetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int itemId = 0;
			String name = "", icon = "", meshName = "";
			pReader->GetInt("BulletClipId", itemId);
			pReader->GetString("Name", name);
			createItem(itemId, 0, name, "ItemBulletClip", "");
		}
	}

	void CustomItemSetting::loadItemBuildSetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int actorId = 0, itemId = 0, type = 0;
			String name = "", actorName = "", actorBody = "", actorBodyId = "", awaitAnimate = "",
				finishAnimate = "", manufactureAnimate = "", lockIcon = "";
			float length = 0.f, width = 0.f, height = 0.f;

			pReader->GetInt("ActorId", actorId);
			pReader->GetInt("ItemId", itemId);
			pReader->GetInt("Type", type);
			pReader->GetString("Name", name);
			pReader->GetString("ActorName", actorName);
			pReader->GetString("ActorBody", actorBody);
			pReader->GetString("ActorBodyId", actorBodyId);
			pReader->GetString("AwaitAnimate", awaitAnimate);
			pReader->GetString("FinishAnimate", finishAnimate);
			pReader->GetString("ManufactureAnimate", manufactureAnimate);
			pReader->GetFloat("Length", length);
			pReader->GetFloat("Width", width);
			pReader->GetFloat("Height", height);
			auto builder = CommonDataBuilder();
			builder.addIntParam("actorId", actorId);
			builder.addIntParam("actorType", type);
			builder.addStringParam("actorName", actorName);
			builder.addStringParam("actorBody", actorBody);
			builder.addStringParam("actorBodyId", actorBodyId);
			builder.addStringParam("awaitAnimate", awaitAnimate);
			builder.addStringParam("finishAnimate", finishAnimate);
			builder.addStringParam("manufactureAnimate", manufactureAnimate);
			builder.addFloatParam("length", length);
			builder.addFloatParam("width", width);
			builder.addFloatParam("height", height);
			createItem(itemId, 0, name, "ItemBuild", builder.getData());
		}
		LordSafeDelete(pReader);
	}

	void CustomItemSetting::loadItemHouseSetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int itemId = 0;
			String name = "", templateName = "";
			float length = 0.f, width = 0.f, height = 0.f;

			pReader->GetInt("ItemId", itemId);
			pReader->GetString("Name", name);
			pReader->GetFloat("Length", length);
			pReader->GetFloat("Width", width);
			pReader->GetFloat("Height", height);
			pReader->GetString("Template", templateName);
			auto builder = CommonDataBuilder();
			builder.addStringParam("templateName", templateName);	
			builder.addFloatParam("length", length);
			builder.addFloatParam("width", width);
			builder.addFloatParam("height", height);
			createItem(itemId, 0, name, "ItemHouse", builder.getData());
		}
		LordSafeDelete(pReader);
	}

	void CustomItemSetting::loadItemTemplateSetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int itemId = 0;
			String name = "", icon = "", templateName = "", meshName = "";
			float length = 0.f, width = 0.f, height = 0.f;

			pReader->GetInt("ItemId", itemId);
			pReader->GetString("Name", name);
			pReader->GetString("Template", templateName);
			pReader->GetFloat("Length", length);
			pReader->GetFloat("Width", width);
			pReader->GetFloat("Height", height);
			auto builder = CommonDataBuilder();
			builder.addStringParam("templateName", templateName);
			builder.addFloatParam("length", length);
			builder.addFloatParam("width", width);
			builder.addFloatParam("height", height);
			createItem(itemId, 0, name, "ItemTemplate", builder.getData());
		}
		LordSafeDelete(pReader);
	}

	void CustomItemSetting::loadItemFruitCurrencySetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int itemId = 0;
			String name = "";
			pReader->GetInt("ItemId", itemId);
			pReader->GetString("Name", name);
			createItem(itemId, 0, name, "ItemCustom", "");
		}
		LordSafeDelete(pReader);
	}

	void CustomItemSetting::loadItemCustomSetting(const String& path)
	{
		CsvReader* pReader = new CsvReader();
		bool bRet = pReader->LoadFile(path.c_str());
		if (!bRet)
		{
			LordSafeDelete(pReader);
			return;
		}
		pReader->Read(); // discard the second line
		while (pReader->Read())
		{
			int itemId = 0, renderType = 0;
			String name = "", className = "", params = "";
			pReader->GetInt("ItemId", itemId);
			pReader->GetInt("RenderType", renderType);
			pReader->GetString("Name", name);
			pReader->GetString("ClassName", className);
			pReader->GetString("Params", params);
			createItem(itemId, renderType, name, className, params);
		}
		LordSafeDelete(pReader);
	}

	ArmorMaterial* getArmorMaterial(const String& material)
	{
		auto armorMaterial = ArmorMaterial::CLOTH;
		if (material == "cloth")
			armorMaterial = ArmorMaterial::CLOTH;
		else if (material == "chain")
			armorMaterial = ArmorMaterial::CHAIN;
		else if (material == "iron")
			armorMaterial = ArmorMaterial::IRON;
		else if (material == "gold")
			armorMaterial = ArmorMaterial::GOLD;
		else if (material == "diamond")
			armorMaterial = ArmorMaterial::DIAMOND;
		return armorMaterial;
	}

	ToolMaterial* getToolMaterial(const String& material)
	{
		auto toolMaterial = ToolMaterial::WOOD;
		if (material == "wood")
			toolMaterial = ToolMaterial::WOOD;
		else if (material == "stone")
			toolMaterial = ToolMaterial::STONE;
		else if (material == "iron")
			toolMaterial = ToolMaterial::IRON;
		else if (material == "gold")
			toolMaterial = ToolMaterial::GOLD;
		else if (material == "emerald")
			toolMaterial = ToolMaterial::EMERALD;
		return toolMaterial;
	}

	void CustomItemSetting::createItem(int itemId, int renderType, String name, String className, String params)
	{
		if (itemId == 0)
			return;
		LordLogInfo("CustomItemSetting::createItem [itemId]:%d [className]:%s", itemId, className.c_str());
		if (Item::itemsList[itemId])
		{
			LordSafeDelete(Item::itemsList[itemId]);
			Item::itemsList[itemId] = NULL;
		}

		if (className.empty() || className == "#")
			className = "ItemCustom";

		Item * item = NULL;
		if (className == "ItemCustom")
		{
			item = LordNew ItemCustom(itemId - 256, name);
		}
		else if (className == "ItemClient")
		{
			item = LordNew Item(itemId - 256, name);
		}
		else if (className == "ItemAppIcon")
		{
			item = LordNew ItemAppIcon(itemId - 256, name);
		}
		else if (className == "ItemArmor")
		{
			String material = "cloth";
			int armorType = 0;
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
					armorType = builder.getIntParam("armorType");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
					armorType = builder.getIntParam("1");
				}
			}
			item = LordNew ItemArmor(itemId - 256, name, getArmorMaterial(material), armorType);
		}
		else if (className == "ItemSword")
		{
			String material = "wood";
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
				}
			}
			item = LordNew ItemSword(itemId - 256, name, getToolMaterial(material));
		}
		else if (className == "ItemSpade")
		{
			String material = "wood";
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
				}
			}
			item = LordNew ItemSpade(itemId - 256, name, getToolMaterial(material));
		}
		else if (className == "ItemPickaxe")
		{
			String material = "wood";
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
				}
			}
			item = LordNew ItemPickaxe(itemId - 256, name, getToolMaterial(material));
		}
		else if (className == "ItemAxe")
		{
			String material = "wood";
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
				}
			}
			item = LordNew ItemAxe(itemId - 256, name, getToolMaterial(material));
		}
		else if (className == "ItemHoe")
		{
			String material = "wood";
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
				}
			}
			item = LordNew ItemHoe(itemId - 256, name, getToolMaterial(material));
		}
		else if (className == "ItemToolGather")
		{
			String material = "wood";
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
				}
			}
			item = LordNew ItemToolGather(itemId - 256, name, getToolMaterial(material));
		}
		else if (className == "ItemFood")
		{
			int amount = 0;
			float modifier = 0.F;
			bool favorite = false;
			bool alwaysEdible = false;
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					amount = builder.getIntParam("amount");
					modifier = builder.getFloatParam("modifier");
					favorite = builder.getBoolParam("favorite");
					alwaysEdible = builder.getBoolParam("alwaysEdible");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					amount = builder.getIntParam("0");
					modifier = builder.getFloatParam("1");
					favorite = builder.getBoolParam("2");
					alwaysEdible = builder.getBoolParam("3");
				}
			}
			item = LordNew ItemFood(itemId - 256, name, amount, modifier, favorite);
			if (alwaysEdible)
			{
				dynamic_cast<ItemFood*>(item)->setAlwaysEdible();
			}
		}
		else if (className == "ItemBow")
		{
			item = LordNew ItemBow(itemId - 256, name);
		}
		else if (className == "ItemArrow")
		{
			item = LordNew ItemArrow(itemId - 256, name);
		}
		else if (className == "ItemGun")
		{
			item = LordNew ItemGun(itemId - 256, name);
		}
		else if (className == "ItemBulletClip")
		{
			item = LordNew ItemBulletClip(itemId - 256, name);
		}
		else if (className == "ItemTransparent")
		{
			item = LordNew ItemTransparent(itemId - 256, name);
		}
		else if (className == "ItemBuild")
		{
			auto builder = CommonDataBuilder().fromData(params);
			int actorId = builder.getIntParam("actorId");
			int actorType = builder.getIntParam("actorType");
			String actorName = builder.getStringParam("actorName");
			String actorBody = builder.getStringParam("actorBody");
			String actorBodyId = builder.getStringParam("actorBodyId");
			String awaitAnimate = builder.getStringParam("awaitAnimate");
			String finishAnimate = builder.getStringParam("finishAnimate");
			String manufactureAnimate = builder.getStringParam("manufactureAnimate");
			float length = builder.getFloatParam("length");
			float width = builder.getFloatParam("width");
			float height = builder.getFloatParam("height");
			item = LordNew ItemBuild(itemId - 256, name);
			dynamic_cast<ItemBuild*>(item)->setActorInfo(actorId, actorType, actorName, actorBody, actorBodyId);
			dynamic_cast<ItemBuild*>(item)->setActionInfo(awaitAnimate, finishAnimate, manufactureAnimate);
			dynamic_cast<ItemBuild*>(item)->setSize(length, width, height);
		}
		else if (className == "ItemHouse")
		{
			auto builder = CommonDataBuilder().fromData(params);
			String templateName = builder.getStringParam("templateName");
			float length = builder.getFloatParam("length");
			float width = builder.getFloatParam("width");
			float height = builder.getFloatParam("height");
			item = LordNew ItemHouse(itemId - 256, name);
			dynamic_cast<ItemHouse*>(item)->setItemInfo(templateName, length, width, height);
		}
		else if (className == "ItemTemplate")
		{
			auto builder = CommonDataBuilder().fromData(params);
			String templateName = builder.getStringParam("templateName");
			float length = builder.getFloatParam("length");
			float width = builder.getFloatParam("width");
			float height = builder.getFloatParam("height");
			item = LordNew ItemTemplate(itemId - 256, name);
			dynamic_cast<ItemTemplate*>(item)->setItemInfo(templateName, length, width, height);
		}
		else if (className == "ItemShears")
		{
			item = LordNew ItemShears(itemId - 256, name);
		}
		else if (className == "ItemCreateBridge")
		{
			item = LordNew ItemCreateBridge(itemId - 256, name);
		}
		else if (className == "ItemBucket")
		{
			int fillID = 0;
			bool addContainer = false;
			auto builder = CommonDataBuilder().fromData(params);
			if (StringUtil::Find(params, "="))
			{
				builder = CommonDataBuilder().fromData(params);
				fillID = builder.getIntParam("fillID");
				addContainer = builder.getBoolParam("addContainer");
			}
			else
			{
				builder = CommonDataBuilder().fromListData(params);
				fillID = builder.getIntParam("0");
				addContainer = builder.getBoolParam("1");
			}
			item = LordNew ItemBucket(itemId - 256, name, fillID);
			if (addContainer)
			{
				dynamic_cast<ItemBucket*>(item)->setContainerItem(Item::bucketEmpty);
			}
		}
		else if (className == "ItemPotion")
		{
			bool isSplash = false;
			auto builder = CommonDataBuilder().fromData(params);
			if (StringUtil::Find(params, "="))
			{
				builder = CommonDataBuilder().fromData(params);
				isSplash = builder.getBoolParam("isSplash");
			}
			else
			{
				builder = CommonDataBuilder().fromData(params);
				isSplash = builder.getBoolParam("0");
			}
			item = LordNew ItemPotion(itemId - 256, name, isSplash);
		}
	}
}
