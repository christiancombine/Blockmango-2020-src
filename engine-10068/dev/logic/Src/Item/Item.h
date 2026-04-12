/********************************************************************
filename: 	Item.h
file path:	dev\client\Src\Blockman

version:	1
author:		ajohn
company:	supernano
date:		2016-11-8
*********************************************************************/
#ifndef __ITEM_HEADER__
#define __ITEM_HEADER__

#include "BM_Container_def.h"
#include "Util/UUID.h"
#include "BM_TypeDef.h"
#include "Entity/Attributes.h"
#include "Block/BlockManager.h"

namespace BLOCKMAN
{
class Random;
class ItemStack;
class Entity;
class EntityLivingBase;
class TextureAtlasRegister;
class EntityPlayer;
class World;
class Block;
class RayTraceResult;
class ItemBow;
class ItemArmor;
class ItemFishingRod;
class ItemMap;
class ItemShears;
class ItemPotion;
class ItemEmptyMap;
class ItemEnchantedBook;
class ItemArrowPotion;
class ItemGrenade;
class ItemHandcuffs;
class ItemKey;
class ItemTeleportScroll;
class ItemCreateBridge;
class ItemToolDigger;
class ItemProp;
class ItemMedichine;
class ItemAppIcon;
class RanchSeeds;
class ItemTransparent;
class ItemHandGrenade;
class ItemDecoration;

class Item : public ObjectAlloc
{
protected:
	static UUID ATTACK_DAMAGE_MODIFIER;// = UUID.fromString("CB3F55D3-645C-4F38-A497-9C13A33DB5CF");

	/** The RNG used by the Item subclasses. */
	static Random* itemRand;// = new Random();

	/** Maximum size of the stack. */
	int maxStackSize = 0;

	/** Maximum damage an item can handle. */
	int maxDamage = 0;

	/** Some items (like dyes) have multiple subtypes on same item, this is field define this behavior */
	bool hasSubtypes = false;
	Item* containerItem = nullptr;
	String potionEffect;

	/** The unlocalized name of this item. */
	String unlocalizedName;

	AttributeModifier *m_modifierAttr = nullptr;

private:
	/* custom item */
	bool isCustomItem = false;

public:
	/*
	 * [0,255]:basic block item, [256,511]:basic item, [512,611]:potion item, 
	 * [700,1255]: extended item, [1256,1999]: extended block item, 
	 * [2000,2999]: custom block item, [6000,6999] custom item
	 */
	static constexpr int EC_BLOCK_TO_ITEM_OFFSET = 1000; // ec block => extended and custom block

	static constexpr int BASIC_BLOCK_ITEM_ID_BEGIN = 0;
	static constexpr int BASIC_BLOCK_ITEM_ID_END = 256;

	static constexpr int BASIC_ITEM_ID_BEGIN = 256;
	static constexpr int BASIC_ITEM_ID_END = 512;

	static constexpr int POTION_ITEM_ID_BEGIN = 512;
	static constexpr int POTION_ITEM_ID_END = 612;

	static constexpr int EXTENDED_ITEM_ID_BEGIN = 700;
	static constexpr int EXTENDED_ITEM_ID_END = EC_BLOCK_TO_ITEM_OFFSET + BlockManager::EXTENDED_BLOCK_ID_BEGIN;

	static constexpr int EXTENDED_BLOCK_ITEM_ID_BEGIN = EC_BLOCK_TO_ITEM_OFFSET + BlockManager::EXTENDED_BLOCK_ID_BEGIN;
	static constexpr int EXTENDED_BLOCK_ITEM_ID_END = EC_BLOCK_TO_ITEM_OFFSET + BlockManager::EXTENDED_BLOCK_ID_END;

	static constexpr int CUSTOM_BLOCK_ITEM_ID_BEGIN = EC_BLOCK_TO_ITEM_OFFSET + BlockManager::CUSTOM_BLOCK_ID_BEGIN;
	static constexpr int CUSTOM_BLOCK_ITEM_ID_END = EC_BLOCK_TO_ITEM_OFFSET + BlockManager::CUSTOM_BLOCK_ID_END;

	static constexpr int MAX_CUSTOM_ITEM_COUNT = 1000;
	static constexpr int CUSTOM_ITEM_ID_BEGIN = 6000;
	static constexpr int CUSTOM_ITEM_ID_END = CUSTOM_ITEM_ID_BEGIN + MAX_CUSTOM_ITEM_COUNT;

	static constexpr int MAX_ITEM_COUNT = CUSTOM_ITEM_ID_END;

	static constexpr int ARMOR_TYPE_EXTRA_PACKAGE = 4;

	static Item* itemsList[MAX_ITEM_COUNT];
	static map<String, int>::type itemNameToIdMap;
	/** A 3200 elements Item array. */
	static Item* shovelIron;//			 = (new ItemSpade(0, EnumToolMaterial.IRON)).setUnlocalizedName("shovelIron").func_111206_d("iron_shovel");
	static Item* pickaxeIron;//			 = (new ItemPickaxe(1, EnumToolMaterial.IRON)).setUnlocalizedName("pickaxeIron").func_111206_d("iron_pickaxe");
	static Item* axeIron;//				 = (new ItemAxe(2, EnumToolMaterial.IRON)).setUnlocalizedName("hatchetIron").func_111206_d("iron_axe");
	static Item* flintAndSteel;//		 = (new ItemFlintAndSteel(3)).setUnlocalizedName("flintAndSteel").func_111206_d("flint_and_steel");
	static Item* appleRed;//			 = (new ItemFood(4, 4, 0.3F, false)).setUnlocalizedName("apple").func_111206_d("apple");
	static ItemBow* bow;//				 = (ItemBow)(new ItemBow(5)).setUnlocalizedName("bow").func_111206_d("bow");
	static Item* arrow;//				 = (new Item(6)).setUnlocalizedName("arrow").setCreativeTab(CreativeTabs.tabCombat).func_111206_d("arrow");
	static Item* coal;//				 = (new ItemCoal(7)).setUnlocalizedName("coal").func_111206_d("coal");
	static Item* diamond;//				 = (new Item(8)).setUnlocalizedName("diamond").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("diamond");
	static Item* ingotIron;//			 = (new Item(9)).setUnlocalizedName("ingotIron").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("iron_ingot");
	static Item* ingotGold;//			 = (new Item(10)).setUnlocalizedName("ingotGold").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("gold_ingot");
	static Item* swordIron;//			 = (new ItemSword(11, EnumToolMaterial.IRON)).setUnlocalizedName("swordIron").func_111206_d("iron_sword");
	static Item* swordWood;//			 = (new ItemSword(12, EnumToolMaterial.WOOD)).setUnlocalizedName("swordWood").func_111206_d("wood_sword");
	static Item* shovelWood;//			 = (new ItemSpade(13, EnumToolMaterial.WOOD)).setUnlocalizedName("shovelWood").func_111206_d("wood_shovel");
	static Item* pickaxeWood;//			 = (new ItemPickaxe(14, EnumToolMaterial.WOOD)).setUnlocalizedName("pickaxeWood").func_111206_d("wood_pickaxe");
	static Item* axeWood;//				 = (new ItemAxe(15, EnumToolMaterial.WOOD)).setUnlocalizedName("hatchetWood").func_111206_d("wood_axe");
	static Item* swordStone;//			 = (new ItemSword(16, EnumToolMaterial.STONE)).setUnlocalizedName("swordStone").func_111206_d("stone_sword");
	static Item* shovelStone;//			 = (new ItemSpade(17, EnumToolMaterial.STONE)).setUnlocalizedName("shovelStone").func_111206_d("stone_shovel");
	static Item* pickaxeStone;//		 = (new ItemPickaxe(18, EnumToolMaterial.STONE)).setUnlocalizedName("pickaxeStone").func_111206_d("stone_pickaxe");
	static Item* axeStone;//			 = (new ItemAxe(19, EnumToolMaterial.STONE)).setUnlocalizedName("hatchetStone").func_111206_d("stone_axe");
	static Item* swordDiamond;//		 = (new ItemSword(20, EnumToolMaterial.EMERALD)).setUnlocalizedName("swordDiamond").func_111206_d("diamond_sword");
	static Item* shovelDiamond;//		 = (new ItemSpade(21, EnumToolMaterial.EMERALD)).setUnlocalizedName("shovelDiamond").func_111206_d("diamond_shovel");
	static Item* pickaxeDiamond;//		 = (new ItemPickaxe(22, EnumToolMaterial.EMERALD)).setUnlocalizedName("pickaxeDiamond").func_111206_d("diamond_pickaxe");
	static Item* axeDiamond;//			 = (new ItemAxe(23, EnumToolMaterial.EMERALD)).setUnlocalizedName("hatchetDiamond").func_111206_d("diamond_axe");
	static Item* stick;//				 = (new Item(24)).setFull3D().setUnlocalizedName("stick").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("stick");
	static Item* bowlEmpty;//			 = (new Item(25)).setUnlocalizedName("bowl").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("bowl");
	static Item* bowlSoup;//			 = (new ItemSoup(26, 6)).setUnlocalizedName("mushroomStew").func_111206_d("mushroom_stew");
	static Item* swordGold;//			 = (new ItemSword(27, EnumToolMaterial.GOLD)).setUnlocalizedName("swordGold").func_111206_d("gold_sword");
	static Item* shovelGold;//			 = (new ItemSpade(28, EnumToolMaterial.GOLD)).setUnlocalizedName("shovelGold").func_111206_d("gold_shovel");
	static Item* pickaxeGold;//			 = (new ItemPickaxe(29, EnumToolMaterial.GOLD)).setUnlocalizedName("pickaxeGold").func_111206_d("gold_pickaxe");
	static Item* axeGold;//				 = (new ItemAxe(30, EnumToolMaterial.GOLD)).setUnlocalizedName("hatchetGold").func_111206_d("gold_axe");
	static Item* silk;//				 = (new ItemReed(31, Block.tripWire)).setUnlocalizedName("string").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("string");
	static Item* feather;//				 = (new Item(32)).setUnlocalizedName("feather").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("feather");
	static Item* gunpowder;//			 = (new Item(33)).setUnlocalizedName("sulphur").setPotionEffect(PotionHelper.gunpowderEffect).setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("gunpowder");
	static Item* hoeWood;//				 = (new ItemHoe(34, EnumToolMaterial.WOOD)).setUnlocalizedName("hoeWood").func_111206_d("wood_hoe");
	static Item* hoeStone;//			 = (new ItemHoe(35, EnumToolMaterial.STONE)).setUnlocalizedName("hoeStone").func_111206_d("stone_hoe");
	static Item* hoeIron;//				 = (new ItemHoe(36, EnumToolMaterial.IRON)).setUnlocalizedName("hoeIron").func_111206_d("iron_hoe");
	static Item* hoeDiamond;//			 = (new ItemHoe(37, EnumToolMaterial.EMERALD)).setUnlocalizedName("hoeDiamond").func_111206_d("diamond_hoe");
	static Item* hoeGold;//				 = (new ItemHoe(38, EnumToolMaterial.GOLD)).setUnlocalizedName("hoeGold").func_111206_d("gold_hoe");
	static Item* seeds;//				 = (new ItemSeeds(39, Block.crops.blockID, Block.tilledField.blockID)).setUnlocalizedName("seeds").func_111206_d("seeds_wheat");
	static Item* wheat;//				 = (new Item(40)).setUnlocalizedName("wheat").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("wheat");
	static Item* bread;//				 = (new ItemFood(41, 5, 0.6F, false)).setUnlocalizedName("bread").func_111206_d("bread");
	static ItemArmor* helmetLeather;//	 = (ItemArmor)(new ItemArmor(42, EnumArmorMaterial.CLOTH, 0, 0)).setUnlocalizedName("helmetCloth").func_111206_d("leather_helmet");
	static ItemArmor* plateLeather;//	 = (ItemArmor)(new ItemArmor(43, EnumArmorMaterial.CLOTH, 0, 1)).setUnlocalizedName("chestplateCloth").func_111206_d("leather_chestplate");
	static ItemArmor* legsLeather;//	 = (ItemArmor)(new ItemArmor(44, EnumArmorMaterial.CLOTH, 0, 2)).setUnlocalizedName("leggingsCloth").func_111206_d("leather_leggings");
	static ItemArmor* bootsLeather;//	 = (ItemArmor)(new ItemArmor(45, EnumArmorMaterial.CLOTH, 0, 3)).setUnlocalizedName("bootsCloth").func_111206_d("leather_boots");
	static ItemArmor* helmetChain;//	 = (ItemArmor)(new ItemArmor(46, EnumArmorMaterial.CHAIN, 1, 0)).setUnlocalizedName("helmetChain").func_111206_d("chainmail_helmet");
	static ItemArmor* plateChain;//		 = (ItemArmor)(new ItemArmor(47, EnumArmorMaterial.CHAIN, 1, 1)).setUnlocalizedName("chestplateChain").func_111206_d("chainmail_chestplate");
	static ItemArmor* legsChain;//		 = (ItemArmor)(new ItemArmor(48, EnumArmorMaterial.CHAIN, 1, 2)).setUnlocalizedName("leggingsChain").func_111206_d("chainmail_leggings");
	static ItemArmor* bootsChain;//		 = (ItemArmor)(new ItemArmor(49, EnumArmorMaterial.CHAIN, 1, 3)).setUnlocalizedName("bootsChain").func_111206_d("chainmail_boots");
	static ItemArmor* helmetIron;//		 = (ItemArmor)(new ItemArmor(50, EnumArmorMaterial.IRON, 2, 0)).setUnlocalizedName("helmetIron").func_111206_d("iron_helmet");
	static ItemArmor* plateIron;//		 = (ItemArmor)(new ItemArmor(51, EnumArmorMaterial.IRON, 2, 1)).setUnlocalizedName("chestplateIron").func_111206_d("iron_chestplate");
	static ItemArmor* legsIron;//		 = (ItemArmor)(new ItemArmor(52, EnumArmorMaterial.IRON, 2, 2)).setUnlocalizedName("leggingsIron").func_111206_d("iron_leggings");
	static ItemArmor* bootsIron;//		 = (ItemArmor)(new ItemArmor(53, EnumArmorMaterial.IRON, 2, 3)).setUnlocalizedName("bootsIron").func_111206_d("iron_boots");
	static ItemArmor* helmetDiamond;//	 = (ItemArmor)(new ItemArmor(54, EnumArmorMaterial.DIAMOND, 3, 0)).setUnlocalizedName("helmetDiamond").func_111206_d("diamond_helmet");
	static ItemArmor* plateDiamond;//	 = (ItemArmor)(new ItemArmor(55, EnumArmorMaterial.DIAMOND, 3, 1)).setUnlocalizedName("chestplateDiamond").func_111206_d("diamond_chestplate");
	static ItemArmor* legsDiamond;//	 = (ItemArmor)(new ItemArmor(56, EnumArmorMaterial.DIAMOND, 3, 2)).setUnlocalizedName("leggingsDiamond").func_111206_d("diamond_leggings");
	static ItemArmor* bootsDiamond;//	 = (ItemArmor)(new ItemArmor(57, EnumArmorMaterial.DIAMOND, 3, 3)).setUnlocalizedName("bootsDiamond").func_111206_d("diamond_boots");
	static ItemArmor* helmetGold;//		 = (ItemArmor)(new ItemArmor(58, EnumArmorMaterial.GOLD, 4, 0)).setUnlocalizedName("helmetGold").func_111206_d("gold_helmet");
	static ItemArmor* plateGold;//		 = (ItemArmor)(new ItemArmor(59, EnumArmorMaterial.GOLD, 4, 1)).setUnlocalizedName("chestplateGold").func_111206_d("gold_chestplate");
	static ItemArmor* legsGold;//		 = (ItemArmor)(new ItemArmor(60, EnumArmorMaterial.GOLD, 4, 2)).setUnlocalizedName("leggingsGold").func_111206_d("gold_leggings");
	static ItemArmor* bootsGold;//		 = (ItemArmor)(new ItemArmor(61, EnumArmorMaterial.GOLD, 4, 3)).setUnlocalizedName("bootsGold").func_111206_d("gold_boots");
	static Item* flint;//				 = (new Item(62)).setUnlocalizedName("flint").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("flint");
	static Item* porkRaw;//				 = (new ItemFood(63, 3, 0.3F, true)).setUnlocalizedName("porkchopRaw").func_111206_d("porkchop_raw");
	static Item* porkCooked;//			 = (new ItemFood(64, 8, 0.8F, true)).setUnlocalizedName("porkchopCooked").func_111206_d("porkchop_cooked");
	static Item* painting;//			 = (new ItemHangingEntity(65, EntityPainting.class)).setUnlocalizedName("painting").func_111206_d("painting");
	static Item* appleGold;//			 = (new ItemAppleGold(66, 4, 1.2F, false)).setAlwaysEdible().setPotionEffect(Potion.regeneration.id, 5, 1, 1.0F).setUnlocalizedName("appleGold").func_111206_d("apple_golden");
	static Item* sign;//				 = (new ItemSign(67)).setUnlocalizedName("sign").func_111206_d("sign");
	static Item* doorWood;//			 = (new ItemDoor(68, Material.wood)).setUnlocalizedName("doorWood").func_111206_d("door_wood");
	static Item* bucketEmpty;//			 = (new ItemBucket(69, 0)).setUnlocalizedName("bucket").setMaxStackSize(16).func_111206_d("bucket_empty");
	static Item* bucketWater;//			 = (new ItemBucket(70, Block.waterMoving.blockID)).setUnlocalizedName("bucketWater").setContainerItem(bucketEmpty).func_111206_d("bucket_water");
	static Item* bucketLava;//			 = (new ItemBucket(71, Block.lavaMoving.blockID)).setUnlocalizedName("bucketLava").setContainerItem(bucketEmpty).func_111206_d("bucket_lava");
	static Item* blockmanEmpty;//		 = (new ItemBlockman(72, 0)).setUnlocalizedName("blockman").func_111206_d("minecart_normal");
	static Item* saddle;//				 = (new ItemSaddle(73)).setUnlocalizedName("saddle").func_111206_d("saddle");
	static Item* doorIron;//			 = (new ItemDoor(74, Material.iron)).setUnlocalizedName("doorIron").func_111206_d("door_iron");
	static Item* redstone;//			 = (new ItemRedstone(75)).setUnlocalizedName("redstone").setPotionEffect(PotionHelper.redstoneEffect).func_111206_d("redstone_dust");
	static Item* snowball;//			 = (new ItemSnowball(76)).setUnlocalizedName("snowball").func_111206_d("snowball");
	static Item* boat;//				 = (new ItemBoat(77)).setUnlocalizedName("boat").func_111206_d("boat");
	static Item* leather;//				 = (new Item(78)).setUnlocalizedName("leather").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("leather");
	static Item* bucketMilk;//			 = (new ItemBucketMilk(79)).setUnlocalizedName("milk").setContainerItem(bucketEmpty).func_111206_d("bucket_milk");
	static Item* brick;//				 = (new Item(80)).setUnlocalizedName("brick").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("brick");
	static Item* clay;//				 = (new Item(81)).setUnlocalizedName("clay").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("clay_ball");
	static Item* reed;//				 = (new ItemReed(82, Block.reed)).setUnlocalizedName("reeds").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("reeds");
	static Item* paper;//				 = (new Item(83)).setUnlocalizedName("paper").setCreativeTab(CreativeTabs.tabMisc).func_111206_d("paper");
	static Item* book;//				 = (new ItemBoo		k(84)).setUnlocalizedName("book").setCreativeTab(CreativeTabs.tabMisc).func_111206_d("book_normal");
	static Item* slimeBall;//			 = (new Item(85)).setUnlocalizedName("slimeball").setCreativeTab(CreativeTabs.tabMisc).func_111206_d("slimeball");
	static Item* blockmanCrate;//		 = (new ItemMinecart(86, 1)).setUnlocalizedName("minecartChest").func_111206_d("minecart_chest");
	static Item* blockmanPowered;//		 = (new ItemMinecart(87, 2)).setUnlocalizedName("minecartFurnace").func_111206_d("minecart_furnace");
	static Item* egg;//					 = (new ItemEgg(88)).setUnlocalizedName("egg").func_111206_d("egg");
	static Item* compass;//				 = (new Item(89)).setUnlocalizedName("compass").setCreativeTab(CreativeTabs.tabTools).func_111206_d("compass");
	static ItemFishingRod* fishingRod;// = (ItemFishingRod)(new ItemFishingRod(90)).setUnlocalizedName("fishingRod").func_111206_d("fishing_rod");
	static Item* pocketSundial;//		 = (new Item(91)).setUnlocalizedName("clock").setCreativeTab(CreativeTabs.tabTools).func_111206_d("clock");
	static Item* glowstone;//			 = (new Item(92)).setUnlocalizedName("yellowDust").setPotionEffect(PotionHelper.glowstoneEffect).setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("glowstone_dust");
	static Item* fishRaw;//				 = (new ItemFood(93, 2, 0.3F, false)).setUnlocalizedName("fishRaw").func_111206_d("fish_raw");
	static Item* fishCooked;//			 = (new ItemFood(94, 5, 0.6F, false)).setUnlocalizedName("fishCooked").func_111206_d("fish_cooked");
	static Item* dyePowder;//			 = (new ItemDye(95)).setUnlocalizedName("dyePowder").func_111206_d("dye_powder");
	static Item* bone;//				 = (new Item(96)).setUnlocalizedName("bone").setFull3D().setCreativeTab(CreativeTabs.tabMisc).func_111206_d("bone");
	static Item* sugar;//				 = (new Item(97)).setUnlocalizedName("sugar").setPotionEffect(PotionHelper.sugarEffect).setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("sugar");
	static Item* cake;//				 = (new ItemReed(98, Block.cake)).setMaxStackSize(1).setUnlocalizedName("cake").setCreativeTab(CreativeTabs.tabFood).func_111206_d("cake");
	static Item* bed;//					 = (new ItemBed(99)).setMaxStackSize(1).setUnlocalizedName("bed").func_111206_d("bed");
	static Item* redstoneRepeater;//	 = (new ItemReed(100, Block.redstoneRepeaterIdle)).setUnlocalizedName("diode").setCreativeTab(CreativeTabs.tabRedstone).func_111206_d("repeater");
	static Item* cookie;//				 = (new ItemFood(101, 2, 0.1F, false)).setUnlocalizedName("cookie").func_111206_d("cookie");
	static ItemMap* imap;//				 = (ItemMap)(new ItemMap(102)).setUnlocalizedName("map").func_111206_d("map_filled");
	static ItemShears* shears;//		 = (ItemShears)(new ItemShears(103)).setUnlocalizedName("shears").func_111206_d("shears");
	static Item* melon;//				 = (new ItemFood(104, 2, 0.3F, false)).setUnlocalizedName("melon").func_111206_d("melon");
	static Item* pumpkinSeeds;//		 = (new ItemSeeds(105, Block.pumpkinStem.blockID, Block.tilledField.blockID)).setUnlocalizedName("seeds_pumpkin").func_111206_d("seeds_pumpkin");
	static Item* melonSeeds;//			 = (new ItemSeeds(106, Block.melonStem.blockID, Block.tilledField.blockID)).setUnlocalizedName("seeds_melon").func_111206_d("seeds_melon");
	static Item* beefRaw;//				 = (new ItemFood(107, 3, 0.3F, true)).setUnlocalizedName("beefRaw").func_111206_d("beef_raw");
	static Item* beefCooked;//			 = (new ItemFood(108, 8, 0.8F, true)).setUnlocalizedName("beefCooked").func_111206_d("beef_cooked");
	static Item* chickenRaw;//			 = (new ItemFood(109, 2, 0.3F, true)).setPotionEffect(Potion.hunger.id, 30, 0, 0.3F).setUnlocalizedName("chickenRaw").func_111206_d("chicken_raw");
	static Item* chickenCooked;//		 = (new ItemFood(110, 6, 0.6F, true)).setUnlocalizedName("chickenCooked").func_111206_d("chicken_cooked");
	static Item* rottenFlesh;//			 = (new ItemFood(111, 4, 0.1F, true)).setPotionEffect(Potion.hunger.id, 30, 0, 0.8F).setUnlocalizedName("rottenFlesh").func_111206_d("rotten_flesh");
	static Item* enderPearl;//			 = (new ItemEnderPearl(112)).setUnlocalizedName("enderPearl").func_111206_d("ender_pearl");
	static Item* blazeRod;//			 = (new Item(113)).setUnlocalizedName("blazeRod").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("blaze_rod");
	static Item* ghastTear;//			 = (new Item(114)).setUnlocalizedName("ghastTear").setPotionEffect(PotionHelper.ghastTearEffect).setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("ghast_tear");
	static Item* goldNugget;//			 = (new Item(115)).setUnlocalizedName("goldNugget").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("gold_nugget");
	static Item* netherStalkSeeds;//	 = (new ItemSeeds(116, Block.netherStalk.blockID, Block.slowSand.blockID)).setUnlocalizedName("netherStalkSeeds").setPotionEffect("+4").func_111206_d("nether_wart");
	static ItemPotion* potion;//		 = (ItemPotion)(new ItemPotion(117)).setUnlocalizedName("potion").func_111206_d("potion");
	static Item* glassBottle;//			 = (new ItemGlassBottle(118)).setUnlocalizedName("glassBottle").func_111206_d("potion_bottle_empty");
	static Item* spiderEye;//			 = (new ItemFood(119, 2, 0.8F, false)).setPotionEffect(Potion.poison.id, 5, 0, 1.0F).setUnlocalizedName("spiderEye").setPotionEffect(PotionHelper.spiderEyeEffect).func_111206_d("spider_eye");
	static Item* fermentedSpiderEye;//	 = (new Item(120)).setUnlocalizedName("fermentedSpiderEye").setPotionEffect(PotionHelper.fermentedSpiderEyeEffect).setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("spider_eye_fermented");
	static Item* blazePowder;//			 = (new Item(121)).setUnlocalizedName("blazePowder").setPotionEffect(PotionHelper.blazePowderEffect).setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("blaze_powder");
	static Item* magmaCream;//			 = (new Item(122)).setUnlocalizedName("magmaCream").setPotionEffect(PotionHelper.magmaCreamEffect).setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("magma_cream");
	static Item* brewingStand;//		 = (new ItemReed(123, Block.brewingStand)).setUnlocalizedName("brewingStand").setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("brewing_stand");
	static Item* cauldron;//			 = (new ItemReed(124, Block.cauldron)).setUnlocalizedName("cauldron").setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("cauldron");
	static Item* eyeOfEnder;//			 = (new ItemEnderEye(125)).setUnlocalizedName("eyeOfEnder").func_111206_d("ender_eye");
	static Item* speckledMelon;//		 = (new Item(126)).setUnlocalizedName("speckledMelon").setPotionEffect(PotionHelper.speckledMelonEffect).setCreativeTab(CreativeTabs.tabBrewing).func_111206_d("melon_speckled");
	static Item* monsterPlacer;//		 = (new ItemMonsterPlacer(127)).setUnlocalizedName("monsterPlacer").func_111206_d("spawn_egg");
	static Item* expBottle;//			 = (new ItemExpBottle(128)).setUnlocalizedName("expBottle").func_111206_d("experience_bottle");
	static Item* fireballCharge;//		 = (new ItemFireball(129)).setUnlocalizedName("fireball").func_111206_d("fireball");
	static Item* writableBook;//		 = (new ItemWritableBook(130)).setUnlocalizedName("writingBook").setCreativeTab(CreativeTabs.tabMisc).func_111206_d("book_writable");
	static Item* writtenBook;//			 = (new ItemEditableBook(131)).setUnlocalizedName("writtenBook").func_111206_d("book_written");
	static Item* emerald;//				 = (new Item(132)).setUnlocalizedName("emerald").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("emerald");
	static Item* itemFrame;//			 = (new ItemHangingEntity(133, EntityItemFrame.class)).setUnlocalizedName("frame").func_111206_d("item_frame");
	static Item* flowerPot;//			 = (new ItemReed(134, Block.flowerPot)).setUnlocalizedName("flowerPot").setCreativeTab(CreativeTabs.tabDecorations).func_111206_d("flower_pot");
	static Item* carrot;//				 = (new ItemSeedFood(135, 4, 0.6F, Block.carrot.blockID, Block.tilledField.blockID)).setUnlocalizedName("carrots").func_111206_d("carrot");
	static Item* potato;//				 = (new ItemSeedFood(136, 1, 0.3F, Block.potato.blockID, Block.tilledField.blockID)).setUnlocalizedName("potato").func_111206_d("potato");
	static Item* bakedPotato;//			 = (new ItemFood(137, 6, 0.6F, false)).setUnlocalizedName("potatoBaked").func_111206_d("potato_baked");
	static Item* poisonousPotato;//		 = (new ItemFood(138, 2, 0.3F, false)).setPotionEffect(Potion.poison.id, 5, 0, 0.6F).setUnlocalizedName("potatoPoisonous").func_111206_d("potato_poisonous");
	static ItemEmptyMap* emptyMap;//	 = (ItemEmptyMap)(new ItemEmptyMap(139)).setUnlocalizedName("emptyMap").func_111206_d("map_empty");
	static Item* goldenCarrot;//		 = (new ItemFood(140, 6, 1.2F, false)).setUnlocalizedName("carrotGolden").setPotionEffect(PotionHelper.goldenCarrotEffect).func_111206_d("carrot_golden");
	static Item* skull;//				 = (new ItemSkull(141)).setUnlocalizedName("skull").func_111206_d("skull");
	static Item* carrotOnAStick;//		 = (new ItemCarrotOnAStick(142)).setUnlocalizedName("carrotOnAStick").func_111206_d("carrot_on_a_stick");
	static Item* netherStar;//			 = (new ItemSimpleFoiled(143)).setUnlocalizedName("netherStar").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("nether_star");
	static Item* pumpkinPie;//			 = (new ItemFood(144, 8, 0.3F, false)).setUnlocalizedName("pumpkinPie").setCreativeTab(CreativeTabs.tabFood).func_111206_d("pumpkin_pie");
	static Item* firework;//			 = (new ItemFirework(145)).setUnlocalizedName("fireworks").func_111206_d("fireworks");
	static Item* fireworkCharge;//		 = (new ItemFireworkCharge(146)).setUnlocalizedName("fireworksCharge").setCreativeTab(CreativeTabs.tabMisc).func_111206_d("fireworks_charge");
	static ItemEnchantedBook* enchantedBook;// = (ItemEnchantedBook)(new ItemEnchantedBook(147)).setMaxStackSize(1).setUnlocalizedName("enchantedBook").func_111206_d("book_enchanted");
	static Item* comparator;//			 = (new ItemReed(148, Block.redstoneComparatorIdle)).setUnlocalizedName("comparator").setCreativeTab(CreativeTabs.tabRedstone).func_111206_d("comparator");
	static Item* netherrackBrick;//		 = (new Item(149)).setUnlocalizedName("netherbrick").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("netherbrick");
	static Item* netherQuartz;//		 = (new Item(150)).setUnlocalizedName("netherquartz").setCreativeTab(CreativeTabs.tabMaterials).func_111206_d("quartz");
	static Item* blockmanTnt;//			 = (new ItemMinecart(151, 3)).setUnlocalizedName("minecartTnt").func_111206_d("minecart_tnt");
	static Item* blockmanHopper;//		 = (new ItemMinecart(152, 5)).setUnlocalizedName("minecartHopper").func_111206_d("minecart_hopper");
	static Item* prismarine_shard;
	static Item* prismarine_crystals;
	static Item* rabbitCooked;//		 = LordNew ItemFood(156, "rabbitCooked", 5, 1.2F, true);
	static Item* iron_horse_armor;//	 = (new Item(161)).setUnlocalizedName("horsearmormetal").setMaxStackSize(1).setCreativeTab(CreativeTabs.tabMisc).func_111206_d("iron_horse_armor");
	static Item* golden_horse_armor;//	 = (new Item(162)).setUnlocalizedName("horsearmorgold").setMaxStackSize(1).setCreativeTab(CreativeTabs.tabMisc).func_111206_d("gold_horse_armor");
	static Item* diamond_horse_armor;//	 = (new Item(163)).setUnlocalizedName("horsearmordiamond").setMaxStackSize(1).setCreativeTab(CreativeTabs.tabMisc).func_111206_d("diamond_horse_armor");
	static Item* lead;//				 = (new ItemLeash(164)).setUnlocalizedName("leash").func_111206_d("lead");
	static Item* name_tag;//			 = (new ItemNameTag(165)).setUnlocalizedName("nameTag").func_111206_d("name_tag");
	static ItemArrowPotion* arrowPoison;
	static ItemArrowPotion* arrowLongPoison;
	static ItemArrowPotion* arrowStrongPoison;
	static ItemGrenade* grenade;
	static Item* record13;//			 = (new ItemRecord(2000, "13")).setUnlocalizedName("record").func_111206_d("record_13");
	static Item* recordCat;//			 = (new ItemRecord(2001, "cat")).setUnlocalizedName("record").func_111206_d("record_cat");
	static Item* recordBlocks;//		 = (new ItemRecord(2002, "blocks")).setUnlocalizedName("record").func_111206_d("record_blocks");
	static Item* recordChirp;//			 = (new ItemRecord(2003, "chirp")).setUnlocalizedName("record").func_111206_d("record_chirp");
	static Item* recordFar;//			 = (new ItemRecord(2004, "far")).setUnlocalizedName("record").func_111206_d("record_far");
	static Item* recordMall;//			 = (new ItemRecord(2005, "mall")).setUnlocalizedName("record").func_111206_d("record_mall");
	static Item* recordMellohi;//		 = (new ItemRecord(2006, "mellohi")).setUnlocalizedName("record").func_111206_d("record_mellohi");
	static Item* recordStal;//			 = (new ItemRecord(2007, "stal")).setUnlocalizedName("record").func_111206_d("record_stal");
	static Item* recordStrad;//			 = (new ItemRecord(2008, "strad")).setUnlocalizedName("record").func_111206_d("record_strad");
	static Item* recordWard;//			 = (new ItemRecord(2009, "ward")).setUnlocalizedName("record").func_111206_d("record_ward");
	static Item* record11;//			 = (new ItemRecord(2010, "11")).setUnlocalizedName("record").func_111206_d("record_11");
	static Item* recordWait;//		 	 = (new ItemRecord(2011, "wait")).setUnlocalizedName("record").func_111206_d("record_wait");
	static ItemHandcuffs*  handcuffs;
	static ItemKey* key_01;
	static ItemKey* key_02;
	static ItemKey* key_03;
	static ItemKey* key_04;
	static ItemKey* key_05;
	static ItemKey* key_06;
	static ItemKey* key_07;
	static ItemKey* key_08;
	static ItemKey* key_09;
	static ItemKey* key_10;
	static ItemTeleportScroll* tpScroll_01;
	static ItemTeleportScroll* tpScroll_02;
	static ItemTeleportScroll* tpScroll_03;
	static ItemTeleportScroll* tpScroll_04;
	static ItemTeleportScroll* tpScroll_05;
	static ItemCreateBridge*   createBridge_01;
	static ItemCreateBridge*   createBridge_02;
	static ItemCreateBridge*   createBridge_03;
	static ItemCreateBridge*   createBridge_04;
	static ItemCreateBridge*   createBridge_05;
	static ItemToolDigger*	   shovelRenovation;
	static ItemToolDigger*	   spatula;
	static ItemToolDigger*	   shovelToy;
	static ItemToolDigger*	   digBucket;
	static ItemToolDigger*	   digShovelIron;
	static ItemToolDigger*	   shovelRake;
	static ItemToolDigger*	   shovelMarching;
	static ItemToolDigger*	   shovelBigScooper;
	static ItemToolDigger*	   shovelJackHammer;
	static ItemToolDigger*	   shovelDrill;
	static ItemToolDigger*	   shovelEngineerGold;
	static ItemProp*    goldHeart;
	static ItemProp*    goldShoes;
	static ItemProp*    goldArrow;
	static ItemProp*    questionMark;
	static ItemProp*    purple_diamond;
	static ItemMedichine*	   medichinePack;
	static ItemMedichine*	   medichinePotion;
	
	static ItemProp* propGold;
	static ItemProp* propExp;
	
	static Item* seedsOfNewCrops_01;
	static Item* seedsOfNewCrops_02;
	static Item* seedsOfNewCrops_03;
	static Item* seedsOfNewCrops_04;
	static Item* seedsOfNewCrops_05;

	static Item* tomato;
	static Item* cabbage;
	static Item* pea;
	static Item* flareBomb;
	static Item* rescue;
	static Item* goldApple;
	static Item* currencyMoney;
	static Item* doorBirch;
	static Item* carrotIron;
	static Item* cornIron;
	static Item* cottonIron;
	static Item* potatoIron;
	static Item* silkIron;
	static Item* strawberryIron;
	static Item* tomatoIron;
	static Item* wheatIron;

	static ItemProp* itemThrowableSkillProp_01 ;
	static ItemProp* itemThrowableSkillProp_02 ;
	static ItemProp* itemThrowableSkillProp_03 ;
	static ItemProp* itemThrowableSkillProp_04 ;
	static ItemProp* itemThrowableSkillProp_05 ;
	static ItemProp* itemThrowableSkillProp_06 ;
	static ItemProp* itemThrowableSkillProp_07 ;
	static ItemProp* itemThrowableSkillProp_08 ;
	static ItemProp* itemThrowableSkillProp_09 ;
	static ItemProp* itemThrowableSkillProp_10 ;

	static ItemProp* itemEatSkillProp_01 ;
	static ItemProp* itemEatSkillProp_02 ;
	static ItemProp* itemEatSkillProp_03 ;
	static ItemProp* itemEatSkillProp_04 ;
	static ItemProp* itemEatSkillProp_05 ;
	static ItemProp* itemEatSkillProp_06 ;
	static ItemProp* itemEatSkillProp_07 ;
	static ItemProp* itemEatSkillProp_08 ;
	static ItemProp* itemEatSkillProp_09 ;
	static ItemProp* itemEatSkillProp_10 ;
	static ItemProp* gemstone;

	static ItemProp* equipmentSet1;
	static ItemProp* equipmentSet2;
	static ItemProp* equipmentSet3;
	static ItemProp* equipmentSet4;
	static ItemProp* equipmentSet5;

	static Item* purplePickaxe;
	static ItemGrenade* smokeGrenade;

	static Item* tremorGrenade;
	static Item* frozenBall;

	static ItemProp* baseballStick;
	static ItemProp* spikeStick;
	static ItemProp* stonePillar;

	static ItemAppIcon* shortenRebirthTimeItem;
	static ItemAppIcon* increaseHpItem;
	static ItemAppIcon* diamondDefensePackItem;
	static Item* grenade_skill_01;
	static Item* grenade_skill_02;
	static Item* colorful_grenade_skill_01;
	static Item* colorful_grenade_skill_02;

	static RanchSeeds* ranchSeed_100001;
	static RanchSeeds* ranchSeed_100002;
	static RanchSeeds* ranchSeed_100003;
	static RanchSeeds* ranchSeed_100004;
	static RanchSeeds* ranchSeed_100005;
	static RanchSeeds* ranchSeed_100006;
	static RanchSeeds* ranchSeed_100007;
	static RanchSeeds* ranchSeed_100008;
	static RanchSeeds* ranchSeed_100009;
	static RanchSeeds* ranchSeed_100010;
	static RanchSeeds* ranchSeed_100011;
	static RanchSeeds* ranchSeed_100012;
	static RanchSeeds* ranchSeed_100013;
	static RanchSeeds* ranchSeed_100014;
	static RanchSeeds* ranchSeed_100015;
	static RanchSeeds* ranchSeed_100016;
	static RanchSeeds* ranchSeed_100017;

	static ItemTransparent* change_animal_skill_01;
	static ItemTransparent* change_animal_skill_02;
	static ItemTransparent* speed_up_skill_01;
	static ItemTransparent* speed_up_skill_02;

	static ItemProp* itemThrowableSkillProp_11;
	static ItemProp* itemThrowableSkillProp_12;
	static ItemProp* itemThrowableSkillProp_13;
	static ItemProp* itemThrowableSkillProp_14;
	static ItemProp* itemThrowableSkillProp_15;
	static ItemProp* itemThrowableSkillProp_16;
	static ItemProp* itemThrowableSkillProp_17;
	static ItemProp* itemThrowableSkillProp_18;
	static ItemProp* itemThrowableSkillProp_19;
	static ItemProp* itemThrowableSkillProp_20;
	static ItemProp* itemThrowableSkillProp_04_plus;
	static ItemProp* itemThrowableSkillProp_05_plus;
	static ItemProp* itemThrowableSkillProp_06_plus;
	static ItemProp* itemThrowableSkillProp_07_plus;
	static ItemProp* itemThrowableSkillProp_08_plus;
	static ItemProp* itemThrowableSkillProp_09_plus;
	static ItemProp* itemThrowableSkillProp_11_plus;
	static ItemProp* itemThrowableSkillProp_12_plus;
	static ItemProp* itemThrowableSkillProp_13_plus;
	static ItemProp* itemThrowableSkillProp_14_plus;
	static ItemProp* itemThrowableSkillProp_15_plus;
	static ItemProp* itemThrowableSkillProp_16_plus;
	static ItemProp* itemThrowableSkillProp_17_plus;
	static ItemProp* itemThrowableSkillProp_18_plus;
	static ItemProp* itemThrowableSkillProp_19_plus;
	static ItemProp* itemThrowableSkillProp_20_plus;

	static ItemBow* bow_red;
	static ItemBow* bow_black;

	static Item* redArrow;
	static Item* blackArrow;

	static ItemProp* weapon_01;
	static ItemProp* weapon_02;
	static ItemProp* weapon_04;
	static ItemProp* weapon_05;
	static ItemProp* weapon_06;
	static ItemProp* weapon_07;
	static ItemProp* weapon_08;
	static ItemProp* weapon_09;
	static ItemProp* weapon_10;
	static ItemProp* weapon_11;
	static ItemProp* weapon_12;
	static ItemProp* weapon_13;
	static ItemProp* weapon_14;
	static ItemProp* weapon_15;
	static ItemProp* weapon_16;
	static ItemProp* weapon_17;
	static ItemProp* weapon_18;
	static ItemHandGrenade* handGrenade;

	static ItemProp* pixel_gun_prop_1;
	static ItemProp* pixel_gun_prop_2;
	static ItemProp* pixel_gun_prop_3;
	static ItemProp* pixel_gun_prop_4;
	static ItemProp* pixel_gun_prop_5;
	static ItemProp* pixel_gun_prop_6;
	static ItemProp* pixel_gun_prop_7;
	static ItemProp* pixel_gun_prop_8;
	static ItemProp* pixel_gun_prop_9;
	static ItemProp* pixel_gun_prop_10;
	static ItemProp* pixel_gun_prop_11;
	static ItemProp* pixel_gun_prop_12;
	static ItemProp* pixel_gun_prop_13;
	static ItemProp* pixel_gun_prop_14;
	static ItemProp* pixel_gun_prop_15;
	static ItemProp* pixel_gun_prop_16;
	static ItemProp* pixel_gun_prop_17;
	static ItemProp* pixel_gun_prop_18;
	static ItemProp* pixel_gun_prop_19;
	static ItemProp* pixel_gun_prop_20;
	static ItemProp* pixel_gun_prop_21;
	static ItemProp* pixel_gun_prop_22;
	static ItemProp* pixel_gun_prop_23;
	static ItemProp* pixel_gun_prop_24;
	static ItemProp* pixel_gun_prop_25;
	static ItemProp* pixel_gun_prop_26;
	static ItemProp* pixel_gun_prop_27;
	static ItemProp* pixel_gun_prop_28;
	static ItemProp* pixel_gun_prop_29;
	static ItemProp* pixel_gun_prop_30;

	static Item* bow_01;
	static Item* bow_02;
	static Item* bow_03;

	static Item* book_1;
	static Item* book_2;
	static Item* book_3;
	static Item* book_4;
	static Item* book_5;
	static Item* book_6;
	static Item* book_7;
	static Item* book_8;

	static ItemDecoration* decoration_01;
	static ItemDecoration* decoration_02;
	static ItemDecoration* decoration_03;
	static ItemDecoration* decoration_04;
	static ItemDecoration* decoration_05;
	static ItemDecoration* decoration_06;
	static ItemDecoration* decoration_07;
	static ItemDecoration* decoration_08;
	static ItemDecoration* decoration_09;
	static ItemDecoration* decoration_10;
	static ItemDecoration* decoration_11;
	static ItemDecoration* decoration_12;
	static ItemDecoration* decoration_13;
	static ItemDecoration* decoration_14;
	static ItemDecoration* decoration_15;
	static ItemDecoration* decoration_16;
	static ItemDecoration* decoration_17;
	static ItemDecoration* decoration_18;
	static ItemDecoration* decoration_19;
	static ItemDecoration* decoration_20;
	static ItemDecoration* decoration_21;
	static ItemDecoration* decoration_22;
	static ItemDecoration* decoration_23;
	static ItemDecoration* decoration_24;
	static ItemDecoration* decoration_25;
	static ItemDecoration* decoration_26;
	static ItemDecoration* decoration_27;
	static ItemDecoration* decoration_28;
	static ItemDecoration* decoration_29;
	static ItemDecoration* decoration_30;
	static ItemDecoration* decoration_31;
	static ItemDecoration* decoration_32;
	static ItemDecoration* decoration_33;
	static ItemDecoration* decoration_34;
	static ItemDecoration* decoration_35;
	static ItemDecoration* decoration_36;
	static ItemDecoration* decoration_37;
	static ItemDecoration* decoration_38;
	static ItemDecoration* decoration_39;
	static ItemDecoration* decoration_40;
	static ItemDecoration* decoration_41;
	static ItemDecoration* decoration_42;
	static ItemDecoration* decoration_43;
	static ItemDecoration* decoration_44;
	static ItemDecoration* decoration_45;
	static ItemDecoration* decoration_46;
	static ItemDecoration* decoration_47;
	static ItemDecoration* decoration_48;
	static ItemDecoration* decoration_49;
	static ItemDecoration* decoration_50;
	static ItemDecoration* decoration_51;
	static ItemDecoration* decoration_52;
	static ItemDecoration* decoration_53;
	static ItemDecoration* decoration_54;
	static ItemDecoration* decoration_55;
	static ItemDecoration* decoration_56;
	static ItemDecoration* decoration_57;
	static ItemDecoration* decoration_58;
	static ItemDecoration* decoration_59;
	static ItemDecoration* decoration_60;
	static ItemDecoration* decoration_61;
	static ItemDecoration* decoration_62;
	static ItemDecoration* decoration_63;
	static ItemDecoration* decoration_64;
	static ItemDecoration* decoration_65;
	static ItemDecoration* decoration_66;
	static ItemDecoration* decoration_67;
	static ItemDecoration* decoration_68;
	static ItemDecoration* decoration_69;
	static ItemDecoration* decoration_70;
	static ItemDecoration* decoration_71;
	static ItemDecoration* decoration_72;
	static ItemDecoration* decoration_73;
	static ItemDecoration* decoration_74;
	static ItemDecoration* decoration_75;
	static ItemDecoration* decoration_76;
	static ItemDecoration* decoration_77;
	static ItemDecoration* decoration_78;
	static ItemDecoration* decoration_79;
	static ItemDecoration* decoration_80;
	static ItemDecoration* decoration_81;
	static ItemDecoration* decoration_82;
	static ItemDecoration* decoration_83;
	static ItemDecoration* decoration_84;
	static ItemDecoration* decoration_85;
	static ItemDecoration* decoration_86;
	static ItemDecoration* decoration_87;
	static ItemDecoration* decoration_88;
	static ItemDecoration* decoration_89;
	static ItemDecoration* decoration_90;
	static ItemDecoration* decoration_91;
	static ItemDecoration* decoration_92;
	static ItemDecoration* decoration_93;
	static ItemDecoration* decoration_94;
	static ItemDecoration* decoration_95;
	static ItemDecoration* decoration_96;
	static ItemDecoration* decoration_97;
	static ItemDecoration* decoration_98;
	static ItemDecoration* decoration_99;
	static ItemDecoration* decoration_100;

	static void initialize();
	static void initializeItemNameToIdMap();
	static void unInitialize();

	/** The ID of this item. */
	int itemID = 0;

protected:
	void setHasSubtypes(bool subtype) { hasSubtypes = subtype; }

	RayTraceResult getMovingObjectPositionFromPlayer(World* pWorld, EntityPlayer* pPlayer, bool useLiquids);

public:
	Item(int id, const String& name);

	virtual ~Item() {
		LordSafeDelete(m_modifierAttr);
	}

	/** set max damage of an Item */
	void setMaxDamage(int damage) { maxDamage = damage; }
	AttributeModifier *getModifierAttr() { return m_modifierAttr; }

	static bool isBasicBlockItem(int id)
	{
		return id >= BASIC_BLOCK_ITEM_ID_BEGIN && id < BASIC_BLOCK_ITEM_ID_END;
	}

	static bool isExtendedBlockItem(int id)
	{
		return id >= EXTENDED_BLOCK_ITEM_ID_BEGIN && id < EXTENDED_BLOCK_ITEM_ID_END;
	}

	static bool isCustomBlockItem(int id)
	{
		return id >= CUSTOM_BLOCK_ITEM_ID_BEGIN && id < CUSTOM_BLOCK_ITEM_ID_END;
	}
	
	static bool isBlockItem(int id)
	{
		return isBasicBlockItem(id)
			|| isExtendedBlockItem(id)
			|| isCustomBlockItem(id);
	}

	static Item* getItemByName(const String& name);
	static Item* getItemById(int id);
	void setMaxStackSize(int size) { maxStackSize = size; }

	/** Callback for item usage. If the item does something special on right clicking, he will have one of those. Return
	* True if something happen and false if it don't. This is for ITEMS, not BLOCKS */
	virtual bool onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit);

	/** Returns the strength of the stack against a given block. 1.0F base, (Quality+1)*2 if correct blocktype, 1.5F if sword */
	virtual float getStrVsBlock(ItemStackPtr pStack, Block* pBlock) { return 1.0F; }

	/** Called whenever this item is equipped and the right mouse button is pressed. Args: itemStack, world, entityPlayer */
	virtual ItemStackPtr onItemRightClick(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) { return pStack; }
	
	/** Called whenever this item is equipped and can eat and the right mouse button is pressed.*/
	virtual ItemStackPtr onEaten(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) { return pStack; }

	/** Returns the maximum size of the stack for a specific item. *Isn't this more a Set than a Get?* */
	int getItemStackLimit() { return maxStackSize; }

	/** Returns the metadata of the block which this Item (ItemBlock) can place */
	virtual int getMetadata(int par1) { return 0; }

	bool getHasSubtypes() { return hasSubtypes; }

	/** Returns the maximum damage an item can take. */
	int getMaxDamage() { return maxDamage; }

	bool isDamageable() { return maxDamage > 0 && !hasSubtypes; }

	/** Current implementations of this method in child classes do not use the entry argument beside ev. They just raise the damage on the stack. */
	virtual bool hitEntity(ItemStackPtr pStack, EntityLivingBase* pLiving1, EntityLivingBase* pLiving2) { return false; }

	virtual bool onBlockDestroyed(ItemStackPtr pStack, World* porld, int blockID, const BlockPos& pos, EntityLivingBase* pLiving) { return false; }

	/** Returns if the item (tool) can harvest results from the block type. */
	virtual bool canHarvestBlock(Block* pBlock) { return false; }

	virtual bool interactWithEntity(ItemStackPtr pStack, EntityPlayer* pPlayer, EntityLivingBase* pLiving) { return false; }

	/** Sets the unlocalized name of this item to the string passed as the parameter, prefixed by "item." */
	void setUnlocalizedName(String name) { unlocalizedName = name; }

	/** Gets the localized name of the given item stack. */
	String getLocalizedName(ItemStackPtr pStack);

	/** Returns the unlocalized name of this item. */
	virtual String getUnlocalizedName() { return "item." + unlocalizedName; }
	
	/** Returns the unlocalized name of this item. This version accepts an ItemStack so different stacks can have
	* different names based on their damage or NBT. */
	virtual String getUnlocalizedName(ItemStackPtr pStack);

	void setContainerItem(Item* pItem) { containerItem = pItem; }

	/** If this returns true, after a recipe involving this item is crafted the container item will be added to the
		player's inventory instead of remaining in the crafting grid. */
	bool doesContainerItemLeaveCraftingGrid(ItemStackPtr pStack) { return true; }

	/** If this function returns true (or the item is damageable), the ItemStack's NBT tag will be sent to the client. */
	bool getShareTag() { return true; }

	Item* getContainerItem() { return containerItem; }

	/** True if this Item has a container item (a.k.a. crafting result) */
	bool hasContainerItem() { return containerItem != NULL; }

	virtual String getItemDisplayName(ItemStackPtr pStack);

	String getStatName();

	String getItemStackDisplayName(ItemStackPtr pStack);
	
	/** Called each tick as long the item is on a player inventory. Uses by maps to check if is on a player hand and update it's contents. */
	void onUpdate(ItemStackPtr pStack, World* pWorld, Entity* pEntity, int slot, bool isSelected) {}

	/** Called when item is crafted/smelted. Used only by maps so far. */
	virtual void onCreated(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer) {}

	/** false for all Items except sub-classes of ItemMapBase */
	virtual bool isMap() { return false; }

	/** returns the action that specifies what animation to play when the items is being used */
	virtual ACTION_TYPE getItemUseAction(ItemStackPtr pStack) { return ACTION_TYPE_NONE; }

	/** How long it takes to use or consume an item */
	virtual int getMaxItemUseDuration(ItemStackPtr pStack) { return 0; }

	/** called when the player releases the use item button. Args: itemstack, world, entityplayer, itemInUseCount */
	virtual void onPlayerStoppedUsing(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer, int timeLeft) {}

	/** Returns a string representing what this item does to a potion. */
	const String& getPotionEffect() const { return potionEffect; }

	/** Returns true if this item serves as a potion ingredient (its ingredient information is not null). */
	bool isPotionIngredient() { return potionEffect != StringUtil::BLANK; }
	
	virtual bool hasEffect(ItemStackPtr pStack);

	/** Return an item rarity from EnumRarity */
	virtual RARITY_TYPE getRarity(ItemStackPtr pStack);

	/** Checks isDamagable and if it cannot be stacked */
	virtual bool isItemTool(ItemStackPtr pStack) { return getItemStackLimit() == 1 && isDamageable(); }

	/** Return the enchantability factor of the item, most of the time is based on material. */
	virtual int getItemEnchantability() { return 0; }

	/** Returns true if players can use this item to affect the world (e.g. placing blocks, placing ender eyes in portal) when not in creative */
	bool canItemEditBlocks() { return true; }

	/** Return whether this item is repairable in an anvil. */
	virtual bool getIsRepairable(ItemStackPtr pStack1, ItemStackPtr pStack2) { return false; }

	virtual void getAttributeModifiers(AttributeModifierMap& outMap) {}

	/** Sets the string representing this item's effect on a potion when used as an ingredient. */
	void setPotionEffect(const String& effectName) { potionEffect = effectName; }

	virtual vector<Vector3i>::type getPlacePositions(World* pWorld, BlockPos& pos, int face);

	virtual int getUseDurationTick() { return 8; }

	virtual bool canLongTouchUse() { return false; }

	virtual bool canContinuousUse() { return true; }

	virtual bool isFirstRender() { return true; }

	virtual bool isThirdRender() { return true; }

	virtual bool isRenderMark() { return false; }

	void setCustomItem(bool isCustomItem) { this->isCustomItem = isCustomItem; }

	String getClassName();
};

}

#endif
