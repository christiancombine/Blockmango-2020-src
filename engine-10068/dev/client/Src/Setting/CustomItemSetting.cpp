#include "CustomItemSetting.h"
#include "Util/CSVReader.h"
#include "Object/Root.h"
#include "Blockman/cItem/cItem.h"
#include "Blockman/Render/TextureAtlas.h"
#include "Network/protocol/CommonDataBuilder.h"
#include "Script/Setting/ScriptSetting.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"

namespace BLOCKMAN
{
	CustomItemSetting::CustomItemSetting()
	{
		loadSetting();
	}

	void CustomItemSetting::loadSetting()
	{
		// ----------------- load engine custom item -----------------
		String path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/ItemGun.csv");
		GunSetting::loadSetting(path);
		loadItemGunSetting(path);
		path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/ItemBulletClip.csv");
		BulletClipSetting::loadSetting(path);
		loadItemBulletClipSetting(path);
		path = PathUtil::ConcatPath(Root::Instance()->getRootPath(), "Media/Setting/ItemCustom.csv");
		loadItemCustomSetting(path);

		// ----------------- load game custom item -----------------
		String gameSettingDir = StringUtil::Replace(ScriptSetting::m_scriptPath, "ClientGame", "GameSetting");
		gameSettingDir = PathUtil::ConcatPath(Root::Instance()->getRootPath(), gameSettingDir);
		LordLogInfo("CustomItemSetting::loadSetting [gameSettingDir]:%s", gameSettingDir.c_str());
		path = PathUtil::ConcatPath(gameSettingDir, "ItemBuilding.csv").c_str();
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
		String mapDir = PathUtil::ConcatPath(Root::Instance()->getMapPath(), Root::Instance()->getWorldName());
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
			String name = "", icon = "", meshName = "";
			pReader->GetInt("GunId", itemId);
			pReader->GetString("Name", name);
			pReader->GetString("Model", meshName);
			pReader->GetString("GunIcon", icon);
			createItem(itemId, 0, icon, name, meshName, "ItemGun", "");
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
			pReader->GetString("ClipModel", meshName);
			pReader->GetString("Icon", icon);
			createItem(itemId, 0, icon, name, meshName, "ItemBulletClip", "");
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
			String name = "", icon = "", actorName = "", actorBody = "", actorBodyId = "", awaitAnimate = "",
				finishAnimate = "", manufactureAnimate = "", lockIcon = "";
			float length = 0.f, width = 0.f, height = 0.f;

			pReader->GetInt("ActorId", actorId);
			pReader->GetInt("ItemId", itemId);
			pReader->GetInt("Type", type);
			pReader->GetString("Name", name);
			pReader->GetString("Icon", icon);
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
			createItem(itemId, 0, icon, name, "", "ItemBuild", builder.getData());
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
			String name = "", icon = "", templateName = "";
			float length = 0.f, width = 0.f, height = 0.f;

			pReader->GetInt("ItemId", itemId);
			pReader->GetString("Name", name);
			pReader->GetString("Icon", icon);
			pReader->GetString("Template", templateName);
			pReader->GetFloat("Length", length);
			pReader->GetFloat("Width", width);
			pReader->GetFloat("Height", height);
			auto builder = CommonDataBuilder();
			builder.addStringParam("templateName", templateName);
			builder.addFloatParam("length", length);
			builder.addFloatParam("width", width);
			builder.addFloatParam("height", height);
			createItem(itemId, 0, icon, name, "", "ItemHouse", builder.getData());
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
			pReader->GetString("Icon", icon);
			pReader->GetString("Template", templateName);
			pReader->GetString("MeshName", meshName);
			pReader->GetFloat("Length", length);
			pReader->GetFloat("Width", width);
			pReader->GetFloat("Height", height);
			auto builder = CommonDataBuilder();
			builder.addStringParam("templateName", templateName);
			builder.addFloatParam("length", length);
			builder.addFloatParam("width", width);
			builder.addFloatParam("height", height);
			createItem(itemId, 0, icon, name, meshName, "ItemTemplate", builder.getData());
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
			String icon = "", name = "", meshName = "";
			pReader->GetInt("ItemId", itemId);
			pReader->GetString("Icon", icon);
			pReader->GetString("Name", name);
			pReader->GetString("MeshName", meshName);
			createItem(itemId, 0, icon, name, meshName, "ItemCustom", "");
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
			String icon = "", name = "", meshName = "", effectName = "", className = "", params = "", rotationStr = "";
			pReader->GetInt("ItemId", itemId);
			pReader->GetInt("RenderType", renderType);
			pReader->GetString("Icon", icon);
			pReader->GetString("Name", name);
			pReader->GetString("MeshName", meshName);
			pReader->GetString("EffectName", effectName);
			pReader->GetString("ClassName", className);
			pReader->GetString("Params", params);
			pReader->GetString("Rotation", rotationStr);
			
			auto item = createItem(itemId, renderType, icon, name, meshName, className, params);
			if (!item)
				continue;

			if (effectName.length() > 7)
				item->setEffectName(effectName);

			auto arr = StringUtil::Split(rotationStr, "#", 100);
			if (arr.size() >= 2)
			{
				float rotatedSpeed = StringUtil::ParseFloat(arr[0]);
				vector<int>::type actionStates;
				for (auto i = 1; i < (int)arr.size(); i++)
				{
					actionStates.push_back(StringUtil::ParseI32(arr[i]));
				}
				item->setRotation(rotatedSpeed, actionStates);
			}

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

	cItem * CustomItemSetting::createItem(int itemId, int renderType, String icon, String name, String meshName, String className, String params)
	{
		if (itemId == 0)
			return NULL;
		LordLogInfo("CustomItemSetting::createItem [itemId]:%d [className]:%s", itemId, className.c_str());
		if (ItemClient::citemsList[itemId])
		{
			LordSafeDelete(ItemClient::citemsList[itemId]);
			ItemClient::citemsList[itemId] = NULL;
			Item::itemsList[itemId] = NULL;
		}
		if (meshName.length() < 5)  // .mesh
			meshName = "";
		if (className.empty() || className == "#")
			className = "ItemCustom";
		if (params == "#")
			params = "";

		cItem * item = NULL;
		if (className == "ItemCustom")
		{
			item = LordNew cItemCustom(itemId - 256, name, icon, meshName);
		}
		else if (className == "ItemClient")
		{
			item = LordNew ItemClient(itemId - 256, name, icon, meshName);
		}
		else if (className == "ItemAppIcon")
		{
			item = LordNew cItemAppIcon(itemId - 256, name, icon);
		}
		else if (className == "ItemArmor")
		{
			String material = "cloth", renderName = "", extraName = "";
			int armorType = 0;
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					material = builder.getStringParam("material");
					armorType = builder.getIntParam("armorType");
					renderName = builder.getStringParam("renderName");
					extraName = builder.getStringParam("extraName");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					material = builder.getStringParam("0");
					armorType = builder.getIntParam("1");
					renderName = builder.getStringParam("2");
					extraName = builder.getStringParam("3");
				}
			}	
			item = LordNew cItemArmor(itemId - 256, name, icon, getArmorMaterial(material), armorType, meshName, renderName);
			if (!extraName.empty())
			{
				dynamic_cast<cItemArmor*>(item)->setMutex(false);
				dynamic_cast<cItemArmor*>(item)->setExtraName(extraName);
			}
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
			item = LordNew cItemSword(itemId - 256, name, icon, getToolMaterial(material), meshName);
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
			item = LordNew cItemSpade(itemId - 256, name, icon, getToolMaterial(material));
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
			item = LordNew cItemPickaxe(itemId - 256, name, icon, getToolMaterial(material), meshName);
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
			item = LordNew cItemAxe(itemId - 256, name, icon, getToolMaterial(material), meshName);
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
			item = LordNew cItemHoe(itemId - 256, name, icon, getToolMaterial(material));
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
			item = LordNew cItemToolGather(itemId - 256, name, icon, meshName, getToolMaterial(material));
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
			item = LordNew cItemFood(itemId - 256, name, icon, amount, modifier, favorite, meshName);
			if (alwaysEdible)
			{
				dynamic_cast<ItemFood*>(item)->setAlwaysEdible();
			}
		}
		else if (className == "ItemBow")
		{
			item = LordNew cItemBow(itemId - 256, name, icon, meshName);
		}
		else if (className == "ItemArrow")
		{
			bool isEffect = false;
			String effectName = "";
			bool isExplosion = false;
			String explosionEffectName = "";
			float explosionRadius = 0.f;
			float explosionDamage = 0.f;
			if (!params.empty())
			{
				CommonDataBuilder builder;
				if (StringUtil::Find(params, "="))
				{
					builder = CommonDataBuilder().fromData(params);
					isEffect = builder.getBoolParam("isEffect");
					effectName = builder.getStringParam("effectName");
					isExplosion = builder.getBoolParam("isExplosion");
					explosionEffectName = builder.getStringParam("explosionEffectName");
					explosionRadius = builder.getFloatParam("explosionRadius");
					explosionDamage = builder.getFloatParam("explosionDamage");
				}
				else
				{
					builder = CommonDataBuilder().fromListData(params);
					isEffect = builder.getBoolParam("0");
					effectName = builder.getStringParam("1");
					isExplosion = builder.getBoolParam("2");
					explosionEffectName = builder.getStringParam("3");
					explosionRadius = builder.getFloatParam("4");
					explosionDamage = builder.getFloatParam("5");
				}
			}
			item = LordNew cItemArrow(itemId - 256, name, icon, isEffect, effectName, isExplosion, explosionEffectName, explosionRadius, explosionDamage, meshName);
		}
		else if (className == "ItemGun")
		{
			item = LordNew cItemGun(itemId - 256, name, icon, meshName);
		}
		else if (className == "ItemBulletClip")
		{
			item = LordNew cItemBulletClip(itemId - 256, name, icon);
		}
		else if (className == "ItemTransparent")
		{
			item = LordNew cItemTransparent(itemId - 256, name, icon);
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
			item = LordNew cItemBuild(itemId - 256, name, icon);
			dynamic_cast<cItemBuild*>(item)->setActorInfo(actorId, actorType, actorName, actorBody, actorBodyId);
			dynamic_cast<cItemBuild*>(item)->setActionInfo(awaitAnimate, finishAnimate, manufactureAnimate);
			dynamic_cast<cItemBuild*>(item)->setSize(length, width, height);
		}
		else if (className == "ItemHouse")
		{
			auto builder = CommonDataBuilder().fromData(params);
			String templateName = builder.getStringParam("templateName");
			float length = builder.getFloatParam("length");
			float width = builder.getFloatParam("width");
			float height = builder.getFloatParam("height");
			item = LordNew cItemHouse(itemId - 256, name, icon);
			dynamic_cast<cItemHouse*>(item)->setItemInfo(templateName, length, width, height);
		}
		else if (className == "ItemTemplate")
		{
			auto builder = CommonDataBuilder().fromData(params);
			String templateName = builder.getStringParam("templateName");
			float length = builder.getFloatParam("length");
			float width = builder.getFloatParam("width");
			float height = builder.getFloatParam("height");
			item = LordNew cItemTemplate(itemId - 256, name, icon, meshName);
			dynamic_cast<cItemTemplate*>(item)->setItemInfo(templateName, length, width, height);
		}
		else if (className == "ItemShears")
		{
		item = LordNew cItemShears(itemId - 256, name, icon, meshName);
		}
		else if (className == "ItemCreateBridge")
		{
		item = LordNew cItemCreateBridge(itemId - 256, name, icon, meshName);
		}
		else if (className == "ItemBucket")
		{
			auto builder = CommonDataBuilder().fromData(params);
			int fillID = 0;
			bool addContainer = false;
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
			item = LordNew cItemBucket(itemId - 256, name, icon, fillID, meshName);
			if (addContainer)
			{
				dynamic_cast<cItemBucket*>(item)->setContainerItem(Item::bucketEmpty);
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
			item = LordNew cItemPotion(itemId - 256, name, icon, isSplash, meshName);
		}

		if (item)
		{
			dynamic_cast<Item*>(item)->setCustomItem(true);
			item->registerIcons(TextureAtlasRegister::Instance());
			ItemClient::citemsList[itemId] = item;
		}

		return item;
	}

}
