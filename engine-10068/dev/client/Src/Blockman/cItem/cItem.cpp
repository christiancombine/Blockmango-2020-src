#include "cItem.h"

#include "cBlock/cBlock.h"
#include "cBlock/cBlockManager.h"
#include "Render/TextureAtlas.h"

#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Block/BM_Material.h"
#include "Item/ItemStack.h"
#include "Item/Potion.h"
#include "Item/PotionManager.h"
#include "cItem/cPotionManager.h"
#include "Entity/EntityPlayer.h"
#include "World/RayTracyResult.h"
#include "World/World.h"
#include "Network/ClientNetwork.h"
#include "Setting/GunSetting.h"
#include "Setting/BulletClipSetting.h"
#include "Setting/GrenadeSetting.h"
#include "cEntity/EntityPlayerSPMP.h"
#include "World/GameSettings.h"
#include "Setting/DecorationSetting.h"
#include "Setting/CustomItemSetting.h"
#include "Setting/LogicSetting.h"
#include "cWorld/Blockman.h"
#include "Util/ClientEvents.h"

namespace BLOCKMAN
{

cItem::cItem(const String& atlas, const String& effect)
: atlasName(atlas)
, m_effectName(effect)
, bFull3D(false)
, itemIcon(NULL)
{

}

ItemRenderType cItem::getRenderType()
{
	if (m_isGameTexture)
		return ItemRenderType::GAME_TEXTURE;
	else
		return ItemRenderType::TEXTURE;
}

AtlasSprite* cItem::getIconIndex(ItemStackPtr pStack)
{
	return getIconFromDamage(pStack->getItemDamage());
}

AtlasSprite* cItem::registerIcons(TextureAtlasRegister * atlas, const String & atlasName)
{
	AtlasSprite* sprite = atlas->getAtlasSprite("game_items.json", atlasName);
	if (sprite == NULL)
		sprite = atlas->getAtlasSprite("items.json", atlasName);
	else
		m_isGameTexture = true;
	return sprite;
}

void cItem::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = cItem::registerIcons(atlas, getAtlasName());
}

String cItem::getAtlasName()
{
	String result = atlasName;
	if (atlasName == StringUtil::BLANK)
	{
		// result = StringUtil::Format("MISSING_ICON_ITEM_%d_%s", itemID, unlocalizedName.c_str());
		result = "MISSING_ICON_ITEM";
	}
	return result;
}

cItem * cItem::getClientItemById(int itemId)
{
	Item* item = Item::getItemById(itemId);
	if (item == NULL)
	{
		return NULL;
	}
	return dynamic_cast<cItem*>(item);
}

void cItem::setRotation(float speed, const vector<int>::type& actionStates)
{
	m_isRotatable = true;
	m_rotatedSpeed = Math::Abs(speed);
	m_rotatedAngle = 0.0f;
	m_rotationActionStates = actionStates;
}

bool cItem::isRotatableAction(int actionState)
{
	return find(m_rotationActionStates.begin(), m_rotationActionStates.end(), actionState) != m_rotationActionStates.end();
}

float cItem::getRotatedAngle(int time)
{
	m_rotatedAngle += time * m_rotatedSpeed;
	while (m_rotatedAngle > 360.0f)
	{
		m_rotatedAngle -= 360.0f;
	}
	return m_rotatedAngle;
}

ItemClient::ItemClient(int id, const String& name, const String& atlas, const String& mesh_name)
: Item(id, name)
, cItem(atlas)
, m_meshName(mesh_name)
{}

ItemRenderType ItemClient::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& ItemClient::getMeshName() const
{
	return m_meshName;
}

cItem* ItemClient::citemsList[MAX_ITEM_COUNT];

void ItemClient::cInitialize()
{
	itemRand = LordNew Random();
	memset(itemsList, 0, sizeof(Item*) * MAX_ITEM_COUNT);
	memset(citemsList, 0, sizeof(cItem*) * MAX_ITEM_COUNT);

	shovelIron = LordNew cItemSpade(0, "shovelIron", "iron_shovel", ToolMaterial::IRON);
	pickaxeIron = LordNew cItemPickaxe(1, "pickaxeIron", "iron_pickaxe", ToolMaterial::IRON);
	axeIron = LordNew cItemAxe(2, "hatchetIron", "iron_axe", ToolMaterial::IRON);
	flintAndSteel = LordNew cItemFlintAndSteel(3, "flintAndSteel", "flint_and_steel");
	appleRed = LordNew cItemFood(4, "apple", "apple", 4, 0.3F, false);
	bow = LordNew cItemBow(5, "bow", "bow");
	arrow = LordNew cItemArrow(6, "arrow", "arrow");
	coal = LordNew cItemCoal(7, "coal", "charcoal");
	diamond = LordNew ItemClient(8, "diamond", "diamond");
	ingotIron = LordNew ItemClient(9, "ingotIron", "iron_ingot");
	ingotGold = LordNew ItemClient(10, "ingotGold", "gold_ingot");
	swordIron = LordNew cItemSword(11, "swordIron", "iron_sword", ToolMaterial::IRON);
	swordWood = LordNew cItemSword(12, "swordWood", "wood_sword", ToolMaterial::WOOD);
	shovelWood = LordNew cItemSpade(13, "shovelWood", "wood_shovel", ToolMaterial::WOOD);
	pickaxeWood = LordNew cItemPickaxe(14, "pickaxeWood", "wood_pickaxe", ToolMaterial::WOOD);
	axeWood = LordNew cItemAxe(15, "hatchetWood", "wood_axe", ToolMaterial::WOOD);
	swordStone = LordNew cItemSword(16, "swordStone", "stone_sword", ToolMaterial::STONE);
	shovelStone = LordNew cItemSpade(17, "shovelStone", "stone_shovel", ToolMaterial::STONE);
	pickaxeStone = LordNew cItemPickaxe(18, "pickaxeStone", "stone_pickaxe", ToolMaterial::STONE);
	axeStone = LordNew cItemAxe(19, "hatchetStone", "stone_axe", ToolMaterial::STONE);
	swordDiamond = LordNew cItemSword(20, "swordDiamond", "diamond_sword", ToolMaterial::EMERALD);
	shovelDiamond = LordNew cItemSpade(21, "shovelDiamond", "diamond_shovel", ToolMaterial::EMERALD);
	pickaxeDiamond = LordNew cItemPickaxe(22, "pickaxeDiamond", "diamond_pickaxe", ToolMaterial::EMERALD);
	axeDiamond = LordNew cItemAxe(23, "hatchetDiamond", "diamond_axe", ToolMaterial::EMERALD);
	stick = LordNew ItemClient(24, "stick", "stick");
	bowlEmpty = LordNew ItemClient(25, "bowl", "bowl");
	bowlSoup = LordNew cItemSoup(26, "mushroomStew", "mushroom_stew", 6);
	swordGold = LordNew cItemSword(27, "swordGold", "gold_sword", ToolMaterial::GOLD);
	shovelGold = LordNew cItemSpade(28, "shovelGold", "gold_shovel", ToolMaterial::GOLD);
	pickaxeGold = LordNew cItemPickaxe(29, "pickaxeGold", "gold_pickaxe", ToolMaterial::GOLD);
	axeGold = LordNew cItemAxe(30, "hatchetGold", "gold_axe", ToolMaterial::GOLD);
	silk	= LordNew cItemReed(31, "string", "string", BLOCK_ID_TRIP_WIRE); 
	feather = LordNew ItemClient(32, "feather", "feather");
	gunpowder = LordNew ItemClient(33, "sulphur", "gunpowder"); gunpowder->setPotionEffect(PotionHelper::gunpowderEffect);
	hoeWood = LordNew cItemHoe(34, "hoeWood", "wood_hoe", ToolMaterial::WOOD);
	hoeStone = LordNew cItemHoe(35, "hoeStone", "stone_hoe", ToolMaterial::STONE);
	hoeIron = LordNew cItemHoe(36, "hoeIron", "iron_hoe", ToolMaterial::IRON);
	hoeDiamond = LordNew cItemHoe(37, "hoeDiamond", "diamond_horse_armor", ToolMaterial::EMERALD);
	hoeGold = LordNew cItemHoe(38, "hoeGold", "gold_hoe", ToolMaterial::GOLD);
	seeds = LordNew cItemSeeds(39, "seeds", "seeds_wheat", BLOCK_ID_CROPS, BLOCK_ID_TILLED_FIELD);
	wheat = LordNew ItemClient(40, "wheat", "wheat");
	bread = LordNew cItemFood(41, "bread", "bread", 5, 0.6F, false);
	helmetLeather = LordNew cItemArmor(42, "helmetCloth", "leather_helmet", ArmorMaterial::CLOTH, 0);
	plateLeather = LordNew cItemArmor(43, "chestplateCloth", "leather_chestplate", ArmorMaterial::CLOTH, 1);
	legsLeather = LordNew cItemArmor(44, "leggingsCloth", "leather_leggings", ArmorMaterial::CLOTH, 2);
	bootsLeather = LordNew cItemArmor(45, "bootsCloth", "leather_boots", ArmorMaterial::CLOTH, 3);
	helmetChain = LordNew cItemArmor(46, "helmetChain", "chainmail_helmet", ArmorMaterial::CHAIN, 0);
	plateChain = LordNew cItemArmor(47, "chestplateChain", "chainmail_chestplate", ArmorMaterial::CHAIN, 1);
	legsChain = LordNew cItemArmor(48, "leggingsChain", "chainmail_leggings", ArmorMaterial::CHAIN, 2);
	bootsChain = LordNew cItemArmor(49, "bootsChain", "chainmail_boots", ArmorMaterial::CHAIN, 3);
	helmetIron = LordNew cItemArmor(50, "helmetIron", "iron_helmet", ArmorMaterial::IRON, 0);
	plateIron = LordNew cItemArmor(51, "chestplateIron", "iron_chestplate", ArmorMaterial::IRON, 1);
	legsIron = LordNew cItemArmor(52, "leggingsIron", "iron_leggings", ArmorMaterial::IRON, 2);
	bootsIron = LordNew cItemArmor(53, "bootsIron", "iron_boots", ArmorMaterial::IRON, 3);
	helmetDiamond = LordNew cItemArmor(54, "helmetDiamond", "diamond_helmet", ArmorMaterial::DIAMOND, 0);
	plateDiamond = LordNew cItemArmor(55, "chestplateDiamond", "diamond_chestplate", ArmorMaterial::DIAMOND, 1);
	legsDiamond = LordNew cItemArmor(56, "leggingsDiamond", "diamond_leggings", ArmorMaterial::DIAMOND, 2);
	bootsDiamond = LordNew cItemArmor(57, "bootsDiamond", "diamond_boots", ArmorMaterial::DIAMOND, 3);
	helmetGold = LordNew cItemArmor(58, "helmetGold", "gold_helmet", ArmorMaterial::GOLD, 0);
	plateGold = LordNew cItemArmor(59, "chestplateGold", "gold_chestplate", ArmorMaterial::GOLD, 1);
	legsGold = LordNew cItemArmor(60, "leggingsGold", "gold_leggings", ArmorMaterial::GOLD, 2);
	bootsGold = LordNew cItemArmor(61, "bootsGold", "gold_boots", ArmorMaterial::GOLD, 3);
	flint = LordNew ItemClient(62, "flint", "flint");
	porkRaw = LordNew cItemFood(63, "porkchopRaw", "porkchop_raw", 3, 0.3F, true);
	porkCooked = LordNew cItemFood(64, "porkchopCooked", "porkchop_cooked", 8, 0.8F, true);
	painting = LordNew cItemHangingEntity(65, "painting", "painting", 1);
	appleGold = LordNew cItemAppleGold(66, "appleGold", "apple_golden", 4, 1.2F, false); ((ItemFood*)appleGold)->setAlwaysEdible(); ((ItemFood*)appleGold)->setPotionEffect(Potion::regeneration->getId(), 5, 1, 1.0F);
	sign = LordNew cItemSign(67, "sign", "sign");
	doorWood = LordNew cItemDoor(68, "doorWood", "door_wood", BM_Material::BM_MAT_wood);
	bucketEmpty = LordNew cItemBucket(69, "bucket", "bucket_empty", 0); bucketEmpty->setMaxStackSize(16);
	bucketWater = LordNew cItemBucket(70, "bucketWater", "water_small", BLOCK_ID_WATERMOVING, "bed_water.mesh"); bucketWater->setContainerItem(bucketEmpty);
	bucketLava = LordNew cItemBucket(71, "bucketLava", "bucket_lava", BLOCK_ID_LAVAMOVING); bucketLava->setContainerItem(bucketEmpty);
	blockmanEmpty = LordNew cItemBlockman(72, "blockman", "minecart_normal", 0);
	saddle = LordNew cItemSaddle(73, "saddle", "saddle");
	doorIron = LordNew cItemDoor(74, "doorIron", "door_iron", BM_Material::BM_MAT_iron);
	redstone = LordNew cItemRedstone(75, "redstone", "redstone_dust"); redstone->setPotionEffect(PotionHelper::redstoneEffect);
	snowball = LordNew cItemSnowball(76, "snowball", "snowball");
	boat = LordNew cItemBoat(77, "boat", "boat");
	leather = LordNew ItemClient(78, "leather", "leather");
	bucketMilk = LordNew cItemBucketMilk(79, "milk", "milk_small", "bed_milk.mesh"); bucketMilk->setContainerItem(bucketEmpty);
	brick = LordNew ItemClient(80, "brick", "brick");
	clay = LordNew ItemClient(81, "clay", "clay_ball");
	reed			= LordNew cItemReed(82, "reeds", "reeds", BLOCK_ID_REED);
	paper = LordNew ItemClient(83, "paper", "paper");
	book = LordNew cItemBook(84, "book", "book_normal");
	slimeBall = LordNew ItemClient(85, "slimeball", "slimeball");
	blockmanCrate = LordNew cItemBlockman(86, "blockmanChest", "minecart_chest", 1);
	blockmanPowered = LordNew cItemBlockman(87, "blockmanFurnace", "minecart_furnace", 2);
	egg = LordNew cItemEgg(88, "egg", "egg");
	compass = LordNew cItemCompass(89, "compass", "compass");
	fishingRod = LordNew cItemFishingRod(90, "fishingRod", "fishing_rod");
	pocketSundial = LordNew cItemClock(91, "clock", "clock");
	glowstone = LordNew ItemClient(92, "yellowDust", "glowstone_dust"); glowstone->setPotionEffect(PotionHelper::glowstoneEffect);
	fishRaw = LordNew cItemFood(93, "fishRaw", "fish_raw", 2, 0.3F, false);
	fishCooked = LordNew cItemFood(94, "fishCooked", "fish_cooked", 5, 0.6F, false);
	dyePowder = LordNew cItemDye(95, "dyePowder", "dye_powder");
	bone = LordNew ItemClient(96, "bone", "bone");
	sugar = LordNew ItemClient(97, "sugar", "sugar"); sugar->setPotionEffect(PotionHelper::sugarEffect);
	cake			= LordNew cItemReed(98, "cake", "cake", BLOCK_ID_CAKE); cake->setMaxStackSize(1);
	bed = LordNew cItemBed(99, "bed", "bed"); bed->setMaxStackSize(1);
	redstoneRepeater = LordNew cItemReed(100, "diode", "repeater", BLOCK_ID_REDSTONE_REPEATER_IDLE);
	cookie = LordNew cItemFood(101, "cookie", "cookie", 2, 0.1F, false);
	//imap			= (ItemMap)(LordNew ItemMap(102)).setUnlocalizedName("map").setAtlasName("map_filled");
	shears = LordNew cItemShears(103, "shears", "shears");
	melon = LordNew cItemFood(104, "melon", "melon", 2, 0.3F, false);
	pumpkinSeeds = LordNew cItemSeeds(105, "seeds_pumpkin", "seeds_pumpkin", BLOCK_ID_PUMPKIN_STEM, BLOCK_ID_TILLED_FIELD);
	melonSeeds = LordNew cItemSeeds(106, "seeds_melon", "seeds_melon", BLOCK_ID_MELON_STEM, BLOCK_ID_TILLED_FIELD);
	beefRaw = LordNew cItemFood(107, "beefRaw", "beef_raw", 3, 0.3F, true);
	beefCooked = LordNew cItemFood(108, "beefCooked", "beef_cooked", 8, 0.8F, true);
	chickenRaw = LordNew cItemFood(109, "chickenRaw", "chicken_raw", 2, 0.3F, true); ((ItemFood*)chickenRaw)->setPotionEffect(Potion::hunger->getId(), 30, 0, 0.3F);
	chickenCooked = LordNew cItemFood(110, "chickenCooked", "chicken_cooked", 6, 0.6F, true);
	rottenFlesh = LordNew cItemFood(111, "rottenFlesh", "rotten_flesh", 4, 0.1F, true); ((ItemFood*)rottenFlesh)->setPotionEffect(Potion::hunger->getId(), 30, 0, 0.8F);
	enderPearl = LordNew ItemClient(112, "enderPearl", "end_small");
	blazeRod = LordNew ItemClient(113, "blazeRod", "blaze_rod");
	ghastTear = LordNew ItemClient(114, "ghastTear", "ghast_tear"); ghastTear->setPotionEffect(PotionHelper::ghastTearEffect);
	goldNugget = LordNew ItemClient(115, "goldNugget", "gold_nugget");
	netherStalkSeeds = LordNew cItemSeeds(116, "netherStalkSeeds", "nether_wart", BLOCK_ID_NETHER_STALK, BLOCK_ID_SLOW_SAND); netherStalkSeeds->setPotionEffect("+4");
	potion = LordNew cItemPotion(117, "potion", "potion");
	glassBottle = LordNew cItemGlassBottle(118, "glassBottle", "potion_bottle_empty");
	spiderEye = LordNew cItemFood(119, "spiderEye", "spider_eye", 2, 0.8F, false); ((ItemFood*)spiderEye)->setPotionEffect(Potion::poison->getId(), 5, 0, 1.0F); spiderEye->setPotionEffect(PotionHelper::spiderEyeEffect);
	fermentedSpiderEye = LordNew ItemClient(120, "fermentedSpiderEye", "spider_eye_fermented"); fermentedSpiderEye->setPotionEffect(PotionHelper::fermentedSpiderEyeEffect);
	blazePowder = LordNew ItemClient(121, "blazePowder", "blaze_powder"); blazePowder->setPotionEffect(PotionHelper::blazePowderEffect);
	magmaCream = LordNew ItemClient(122, "magmaCream", "magma_cream"); magmaCream->setPotionEffect(PotionHelper::magmaCreamEffect);
	brewingStand	= LordNew cItemReed(123, "brewingStand", "brewing_stand", BLOCK_ID_BREWING_STAND); 
	cauldron		= LordNew cItemReed(124, "cauldron", "cauldron", BLOCK_ID_CAULDRON);
	eyeOfEnder = LordNew cItemEnderEye(125, "eyeOfEnder", "ender_eye");
	speckledMelon = LordNew ItemClient(126, "speckledMelon", "melon_speckled"); speckledMelon->setPotionEffect(PotionHelper::speckledMelonEffect);
	monsterPlacer = LordNew cItemMonsterPlacer(127, "monsterPlacer", "spawn_egg");
	expBottle = LordNew cItemExpBottle(128, "expBottle", "experience_bottle");
	fireballCharge = LordNew cItemFireball(129, "fireball", "fireball");
	writableBook = LordNew cItemWritableBook(130, "writingBook", "book_writable");
	writtenBook = LordNew cItemEditableBook(131, "writtenBook", "book_written");
	emerald = LordNew ItemClient(132, "emerald", "emerald");
	itemFrame = LordNew cItemHangingEntity(133, "frame", "item_frame", 2);
	flowerPot		= LordNew cItemReed(134, "flowerPot", "flower_pot", BLOCK_ID_FLOWER_POT); 
	carrot = LordNew cItemSeedFood(135, "carrots", "carrot", 4, 0.6F, BLOCK_ID_CARROT, BLOCK_ID_TILLED_FIELD);
	potato = LordNew cItemSeedFood(136, "potato", "potato", 1, 0.3F, BLOCK_ID_POTATO, BLOCK_ID_TILLED_FIELD);
	bakedPotato = LordNew cItemFood(137, "potatoBaked", "potato_baked", 6, 0.6F, false);
	poisonousPotato = LordNew cItemFood(138, "potatoPoisonous", "potato_poisonous", 2, 0.3F, false); ((ItemFood*)poisonousPotato)->setPotionEffect(Potion::poison->getId(), 5, 0, 0.6F);
	//emptyMap		= LordNew ItemEmptyMap(139)).setUnlocalizedName("emptyMap").setAtlasName("map_empty");
	goldenCarrot = LordNew cItemFood(140, "carrotGolden", "carrot_golden", 6, 1.2F, false); goldenCarrot->setPotionEffect(PotionHelper::goldenCarrotEffect);
	skull = LordNew cItemSkull(141, "skull", "skull");
	carrotOnAStick = LordNew cItemCarrotOnAStick(142, "carrotOnAStick", "carrot_on_a_stick");
	netherStar = LordNew cItemSimpleFoiled(143, "netherStar", "nether_star");
	pumpkinPie = LordNew cItemFood(144, "pumpkinPie", "pumpkin_pie", 8, 0.3F, false);
	firework = LordNew cItemFirework(145, "fireworks", "fireworks");
	fireworkCharge = LordNew cItemFireworkCharge(146, "fireworksCharge", "fireworks_charge");
	enchantedBook = LordNew cItemEnchantedBook(147, "enchantedBook", "book_enchanted"); enchantedBook->setMaxStackSize(1);
	comparator	= LordNew cItemReed(148, "comparator", "comparator", BLOCK_ID_REDSTONE_COMPARATOR_IDLE);
	netherrackBrick = LordNew ItemClient(149, "netherbrick", "netherbrick");
	netherQuartz = LordNew ItemClient(150, "netherquartz", "quartz");
	blockmanTnt = LordNew cItemBlockman(151, "blockmanTnt", "minecart_tnt", 3);
	blockmanHopper= LordNew cItemBlockman(152,"blockmanHopper","minecart_hopper", 5);
	prismarine_shard = LordNew ItemClient(153, "prismarine_shard", "prismarine_shard"); 
	prismarine_crystals = LordNew ItemClient(154, "prismarine_crystals", "prismarine_crystals");
	rabbitCooked = LordNew cItemFood(156, "rabbitCooked", "rabbit_cooked", 5, 1.2F, true);
	iron_horse_armor = LordNew ItemClient(161, "horsearmormetal", "iron_horse_armor"); iron_horse_armor->setMaxStackSize(1);
	golden_horse_armor = LordNew ItemClient(162, "horsearmorgold", "gold_horse_armor"); golden_horse_armor->setMaxStackSize(1);
	diamond_horse_armor = LordNew ItemClient(163, "horsearmordiamond", "diamond_horse_armor"); diamond_horse_armor->setMaxStackSize(1);
	lead = LordNew cItemLeash(164, "leash", "lead");
	name_tag = LordNew cItemNameTag(165, "nameTag", "name_tag");
	arrowPoison = LordNew cItemArrowPotion(166, "arrow_poison", "arrow_poison"); arrowPoison->setPotionParams(PotionManager::effectPoison->getPotionID(), int(Math::Floor(PotionManager::effectPoison->getDuration() / 8.f)), PotionManager::effectPoison->getAmplifier());
	arrowLongPoison = LordNew cItemArrowPotion(167, "arrow_long_poison", "arrow_poison"); arrowLongPoison->setPotionParams(PotionManager::effectLongPoison->getPotionID(), int(Math::Floor(PotionManager::effectLongPoison->getDuration() / 8.f)), PotionManager::effectLongPoison->getAmplifier());
	arrowStrongPoison = LordNew cItemArrowPotion(168, "arrow_strong_poison", "arrow_poison"); arrowStrongPoison->setPotionParams(PotionManager::effectStrongPoison->getPotionID(), int(Math::Floor(PotionManager::effectStrongPoison->getDuration() / 8.f)), PotionManager::effectStrongPoison->getAmplifier());
	grenade = LordNew cItemGrenade(169, "grenade", "grenade");
	handcuffs = LordNew cItemHandcuffs(195, "handcuffs", "handcuffs");
	key_01 = LordNew cItemKey(196, "key01", "keyToDoor");
	key_02 = LordNew cItemKey(197, "key02", "keyToDoor");
	key_03 = LordNew cItemKey(198, "key03", "keyToDoor");
	key_04 = LordNew cItemKey(199, "key04", "keyToDoor");
	key_05 = LordNew cItemKey(200, "key05", "keyToDoor");
	key_06 = LordNew cItemKey(201, "key06", "keyToDoor");
	key_07 = LordNew cItemKey(202, "key07", "keyToDoor");
	key_08 = LordNew cItemKey(203, "key08", "keyToDoor");
	key_09 = LordNew cItemKey(204, "key09", "keyToDoor");
	key_10 = LordNew cItemKey(205, "key10", "keyToDoor");
	tpScroll_01 = LordNew cItemTeleportScroll(206, "tpScroll01", "tpScroll01");
	tpScroll_02 = LordNew cItemTeleportScroll(207, "tpScroll02", "tpScroll02");
	tpScroll_03 = LordNew cItemTeleportScroll(208, "tpScroll03", "tpScroll02");
	tpScroll_04 = LordNew cItemTeleportScroll(209, "tpScroll04", "tpScroll02");
	tpScroll_05 = LordNew cItemTeleportScroll(210, "tpScroll05", "tpScroll02");
	createBridge_01 = LordNew cItemCreateBridge(211, "createBridge01", "egg_small", "bed_egg.mesh");
	createBridge_02 = LordNew cItemCreateBridge(212, "createBridge02", "egg_small", "bed_egg.mesh");
	createBridge_03 = LordNew cItemCreateBridge(213, "createBridge03", "egg_small", "bed_egg.mesh");
	createBridge_04 = LordNew cItemCreateBridge(214, "createBridge04", "egg_small", "bed_egg.mesh");
	createBridge_05 = LordNew cItemCreateBridge(215, "createBridge05", "egg_small", "bed_egg.mesh");
	shovelRenovation = LordNew cItemToolDigger(216, "shovelRenovation", "shovel_01", "gun_2000.mesh", ToolMaterial::IRON);
	spatula = LordNew cItemToolDigger(217, "spatula", "shovel_02", "gun_2001.mesh", ToolMaterial::IRON);
	shovelToy = LordNew cItemToolDigger(218, "shovelToy", "shovel_03", "gun_2002.mesh", ToolMaterial::IRON);
	digBucket = LordNew cItemToolDigger(219, "digBucket", "shovel_04", "gun_2003.mesh", ToolMaterial::IRON);
	digShovelIron = LordNew cItemToolDigger(220, "digShovelIron", "shovel_06", "gun_2005.mesh", ToolMaterial::IRON);
	shovelRake = LordNew cItemToolDigger(221, "shovelRake", "shovel_05", "gun_2004.mesh", ToolMaterial::IRON);
	shovelMarching = LordNew cItemToolDigger(222, "shovelMarching", "shovel_08", "gun_2007.mesh", ToolMaterial::IRON);
	shovelBigScooper = LordNew cItemToolDigger(223, "shovelBigScooper", "shovel_07", "gun_2006.mesh", ToolMaterial::IRON);
	shovelJackHammer = LordNew cItemToolDigger(224, "shovelJackHammer", "shovel_10", "gun_2009.mesh", ToolMaterial::IRON);
	shovelDrill = LordNew cItemToolDigger(225, "shovelDrill", "shovel_11", "gun_2010.mesh", ToolMaterial::IRON);
	shovelEngineerGold = LordNew cItemToolDigger(226, "shovelEngineerGold", "shovel_09", "gun_2008.mesh", ToolMaterial::IRON);
	medichinePack = LordNew cItemMedicine(227, "medichinePack", "medichine_pack");
	medichinePotion = LordNew cItemMedicine(228, "medichinePotion", "potion_bottle_fill");

	record13 = LordNew cItemRecord(230, "record", "13", "record_13");
	recordCat = LordNew cItemRecord(231, "record", "cat", "record_cat");
	recordBlocks = LordNew cItemRecord(232, "record", "blocks", "record_blocks");
	recordChirp = LordNew cItemRecord(233, "record", "chirp", "record_chirp");
	recordFar = LordNew cItemRecord(234, "record", "far", "record_far");
	recordMall = LordNew cItemRecord(235, "record", "mall", "record_mall");
	recordMellohi = LordNew cItemRecord(236, "record", "mellohi", "record_mellohi");
	recordStal = LordNew cItemRecord(237, "record", "stal", "record_stal");
	recordStrad = LordNew cItemRecord(238, "record", "strad", "record_strad");
	recordWard = LordNew cItemRecord(239, "record", "ward", "record_ward");
	record11 = LordNew cItemRecord(240, "record", "11", "record_11");
	recordWait = LordNew cItemRecord(241, "record", "wait", "record_wait");

	//TODO The atlas is not, please add yourself if you need it
	goldHeart = LordNew cItemProp(242, "goldHeart","goldHeart", "heart01.mesh");
	goldShoes = LordNew cItemProp(243, "goldShoes", "goldShoes", "shoes01.mesh");
	goldArrow = LordNew cItemProp(244, "goldArrow", "goldArrow", "arrow01.mesh");
	questionMark = LordNew cItemProp(245, "questionMark", "questionMark", "questionmark01.mesh");

	propGold = LordNew cItemProp(246, "propGold", "propGold", StringUtil::BLANK);
	propExp = LordNew cItemProp(247, "propExp", "propExp", StringUtil::BLANK);

	tomato = LordNew cItemFood(246, "tomato", "tomato", 6, 0.6F, false);
	cabbage = LordNew cItemFood(247, "cabbage", "cabbage", 6, 0.6F, false);
	pea = LordNew cItemFood(248, "pea", "pea", 6, 0.6F, false);
	flareBomb = LordNew ItemClient(250, "flareBomb", "fire_small");
	rescue = LordNew ItemClient(251, "rescue", "jiuyuanpingtai_small", "bed_slime.mesh");
	goldApple = LordNew cItemFood(252, "goldApple", "goldapple_small", 0, 0.0F, false); ((ItemFood*)goldApple)->setAlwaysEdible();
	currencyMoney = LordNew ItemClient(253, "currencyMoney", "currency_money");
	doorBirch = LordNew cItemDoor(254, "doorBirch", "door_wood", BM_Material::BM_MAT_wood);
	purple_diamond = LordNew cItemProp(255, "purple_brick", "purple_brick", "g1048_diamond_handle.mesh");
	carrotIron = LordNew ItemClient(391, "carrotIron", "carrotIron");
	potatoIron = LordNew ItemClient(392, "potatoIron", "potatoIron");

	//throwing skill
	itemThrowableSkillProp_01 = LordNew cItemProp(459, "skill_01", "skill_01", "skill_01.mesh");
	itemThrowableSkillProp_02 = LordNew cItemProp(460, "skill_02", "skill_02", "skill_02.mesh");
	itemThrowableSkillProp_03 = LordNew cItemProp(461, "skill_03", "skill_03", "skill_03.mesh");
	//Reserve throwing skill Id, if necessary, please reset the atlas and so on.
	itemThrowableSkillProp_04 = LordNew cItemProp(462, "skill_04", "skill_04", "skill_04.mesh");
	itemThrowableSkillProp_05 = LordNew cItemProp(463, "skill_05", "skill_05", "skill_05.mesh");
	itemThrowableSkillProp_06 = LordNew cItemProp(464, "skill_06", "skill_06", "skill_06.mesh");
	itemThrowableSkillProp_07 = LordNew cItemProp(465, "skill_07", "skill_07", "skill_07.mesh");
	itemThrowableSkillProp_08 = LordNew cItemProp(466, "skill_08", "skill_08", "skill_08.mesh");
	itemThrowableSkillProp_09 = LordNew cItemProp(467, "skill_09", "skill_09", "skill_09.mesh");
	itemThrowableSkillProp_10 = LordNew cItemProp(468, "skill_03", "skill_03", "skill_03.mesh");

	// eat skill 
	itemEatSkillProp_01 = LordNew cItemProp(469, "skill_01", "skill_01", "skill_01.mesh");
	itemEatSkillProp_02 = LordNew cItemProp(470, "skill_02", "skill_02", "skill_02.mesh");
	itemEatSkillProp_03 = LordNew cItemProp(471, "skill_03", "skill_03", "skill_03.mesh");
	//Reserve eat skill Id, if necessary, please reset the atlas and so on.
	itemEatSkillProp_04 = LordNew cItemProp(472, "skill_01", "skill_01", "skill_01.mesh");
	itemEatSkillProp_05 = LordNew cItemProp(473, "skill_02", "skill_02", "skill_02.mesh");
	itemEatSkillProp_06 = LordNew cItemProp(474, "skill_03", "skill_03", "skill_03.mesh");
	itemEatSkillProp_07 = LordNew cItemProp(475, "skill_01", "skill_01", "skill_01.mesh");
	itemEatSkillProp_08 = LordNew cItemProp(476, "skill_02", "skill_02", "skill_02.mesh");
	itemEatSkillProp_09 = LordNew cItemProp(477, "skill_03", "skill_03", "skill_03.mesh");
	itemEatSkillProp_10 = LordNew cItemProp(478, "skill_03", "skill_03", "skill_03.mesh");

	equipmentSet1 = LordNew cItemProp(496, "equipment.set.1", "equipment_set_1", StringUtil::BLANK);
	equipmentSet2 = LordNew cItemProp(497, "equipment.set.2", "equipment_set_2", StringUtil::BLANK);
	equipmentSet3 = LordNew cItemProp(498, "equipment.set.3", "equipment_set_3", StringUtil::BLANK);
	equipmentSet4 = LordNew cItemProp(499, "equipment.set.4", "equipment_set_4", StringUtil::BLANK);
	equipmentSet5 = LordNew cItemProp(500, "equipment.set.5", "equipment_set_5", StringUtil::BLANK);

	purplePickaxe = LordNew cItemPickaxe(501, "purple.pickaxe", "purple_pickaxe", ToolMaterial::EMERALD);
	smokeGrenade = LordNew cItemGrenade(502, "grenade.smoke", "grenade_smoke");

	tremorGrenade = LordNew ItemClient(513, "grenade.tremor", "grenade_tremor");
	frozenBall = LordNew ItemClient(514, "frozen.ball", "frozen_ball");

	baseballStick = LordNew cItemProp(525, "baseball_stick", "baseball_stick", "g1036_weapon_lvl1.mesh");
	spikeStick = LordNew cItemProp(526, "spike_stick", "spike_stick", "g1036_weapon_lvl2.mesh");
	stonePillar = LordNew cItemProp(527, "stone_pillar", "stone_pillar", "g1036_weapon_lvl3.mesh");

	shortenRebirthTimeItem = LordNew cItemAppIcon(528, "shorten_rebirth_time", "items_shorten_rebirth_time");
	increaseHpItem = LordNew cItemAppIcon(529, "increase_hp", "items_increase_hp");
	diamondDefensePackItem = LordNew cItemAppIcon(530, "diamond_defense_pack", "items_diamond_defense_pack");

	grenade_skill_01 = LordNew ItemClient(530, "grenade_skill", "skill_21");
	grenade_skill_02 = LordNew ItemClient(531, "grenade_skill", "skill_21");
	colorful_grenade_skill_01 = LordNew ItemClient(532, "colorful_grenade_skill", "skill_23");
	colorful_grenade_skill_02 = LordNew ItemClient(533, "colorful_grenade_skill", "skill_23");
	change_animal_skill_01 = LordNew cItemTransparent(534, "change_animal_skill", "skill_20");
	change_animal_skill_02 = LordNew cItemTransparent(535, "change_animal_skill", "skill_20");
	speed_up_skill_01 = LordNew cItemTransparent(536, "speed_up_skill", "skill_22");
	speed_up_skill_02 = LordNew cItemTransparent(537, "speed_up_skill", "skill_22");

	pixel_gun_prop_1 = LordNew cItemProp(598, "pixel_gun_prop_1", "pixel_gun_prop_1", "pixel_gun_prop_1.mesh");
	pixel_gun_prop_2 = LordNew cItemProp(599, "pixel_gun_prop_2", "pixel_gun_prop_2", "pixel_gun_prop_2.mesh");
	pixel_gun_prop_3 = LordNew cItemProp(600, "pixel_gun_prop_3", "pixel_gun_prop_3", "pixel_gun_prop_3.mesh");
	pixel_gun_prop_4 = LordNew cItemProp(601, "pixel_gun_prop_4", "pixel_gun_prop_4", "pixel_gun_prop_4.mesh");
	pixel_gun_prop_5 = LordNew cItemProp(602, "pixel_gun_prop_5", "pixel_gun_prop_5", "pixel_gun_prop_5.mesh");
	pixel_gun_prop_6 = LordNew cItemProp(603, "pixel_gun_prop_6", "pixel_gun_prop_6", "pixel_gun_prop_6.mesh");
	pixel_gun_prop_7 = LordNew cItemProp(604, "pixel_gun_prop_7", "pixel_gun_prop_7", "pixel_gun_prop_7.mesh");
	pixel_gun_prop_8 = LordNew cItemProp(605, "pixel_gun_prop_8", "pixel_gun_prop_8", "pixel_gun_prop_8.mesh");
	pixel_gun_prop_9 = LordNew cItemProp(606, "pixel_gun_prop_9", "pixel_gun_prop_9", "pixel_gun_prop_9.mesh");
	pixel_gun_prop_10 = LordNew cItemProp(607, "pixel_gun_prop_10", "pixel_gun_prop_10", "pixel_gun_prop_10.mesh");
	pixel_gun_prop_11 = LordNew cItemProp(608, "pixel_gun_prop_11", "pixel_gun_prop_11", "pixel_gun_prop_11.mesh");
	pixel_gun_prop_12 = LordNew cItemProp(609, "pixel_gun_prop_12", "pixel_gun_prop_12", "pixel_gun_prop_12.mesh");
	pixel_gun_prop_13 = LordNew cItemProp(610, "pixel_gun_prop_13", "pixel_gun_prop_13", "pixel_gun_prop_13.mesh");
	pixel_gun_prop_14 = LordNew cItemProp(611, "pixel_gun_prop_14", "pixel_gun_prop_14", "pixel_gun_prop_14.mesh");
	pixel_gun_prop_15 = LordNew cItemProp(612, "pixel_gun_prop_15", "pixel_gun_prop_15", "pixel_gun_prop_15.mesh");
	pixel_gun_prop_16 = LordNew cItemProp(613, "pixel_gun_prop_16", "pixel_gun_prop_16", "pixel_gun_prop_16.mesh");
	pixel_gun_prop_17 = LordNew cItemProp(614, "pixel_gun_prop_17", "pixel_gun_prop_17", "pixel_gun_prop_17.mesh");
	pixel_gun_prop_18 = LordNew cItemProp(615, "pixel_gun_prop_18", "pixel_gun_prop_18", "pixel_gun_prop_18.mesh");
	pixel_gun_prop_19 = LordNew cItemProp(616, "pixel_gun_prop_19", "pixel_gun_prop_19", "pixel_gun_prop_19.mesh");
	pixel_gun_prop_20 = LordNew cItemProp(617, "pixel_gun_prop_20", "pixel_gun_prop_20", "pixel_gun_prop_20.mesh");
	pixel_gun_prop_21 = LordNew cItemProp(618, "pixel_gun_prop_21", "pixel_gun_prop_21", "pixel_gun_prop_21.mesh");
	pixel_gun_prop_22 = LordNew cItemProp(619, "pixel_gun_prop_22", "pixel_gun_prop_22", "pixel_gun_prop_22.mesh");
	pixel_gun_prop_23 = LordNew cItemProp(620, "pixel_gun_prop_23", "pixel_gun_prop_23", "pixel_gun_prop_23.mesh");
	pixel_gun_prop_24 = LordNew cItemProp(621, "pixel_gun_prop_24", "pixel_gun_prop_24", "pixel_gun_prop_24.mesh");
	pixel_gun_prop_25 = LordNew cItemProp(622, "pixel_gun_prop_25", "pixel_gun_prop_25", "pixel_gun_prop_25.mesh");
	pixel_gun_prop_26 = LordNew cItemProp(623, "pixel_gun_prop_26", "pixel_gun_prop_26", "pixel_gun_prop_26.mesh");
	pixel_gun_prop_27 = LordNew cItemProp(624, "pixel_gun_prop_27", "pixel_gun_prop_27", "pixel_gun_prop_27.mesh");
	pixel_gun_prop_28 = LordNew cItemProp(625, "pixel_gun_prop_28", "pixel_gun_prop_28", "skill_11.mesh");
	pixel_gun_prop_29 = LordNew cItemProp(626, "pixel_gun_prop_29", "pixel_gun_prop_29", "skill_11.mesh");
	pixel_gun_prop_30 = LordNew cItemProp(627, "pixel_gun_prop_30", "pixel_gun_prop_30", "skill_11.mesh");

	seedsOfNewCrops_01 = LordNew cItemSeeds(644, "seeds_wheat", "seeds_wheat1", 1256, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_02 = LordNew cItemSeeds(645, "seeds_carrot", "seeds_carrot", 1257, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_03 = LordNew cItemSeeds(646, "seeds_tomato", "seeds_tomato", 1258, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_04 = LordNew cItemSeeds(647, "seeds_cabbage", "seeds_cabbage", 1259, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_05 = LordNew cItemSeeds(648, "seeds_pea", "seeds_pea", 1260, BLOCK_ID_TILLED_FIELD);
	gemstone = LordNew cItemProp(649, "gemstone", "gemstone", "jewel02.mesh");

	ranchSeed_100001 = LordNew cRanchSeeds(701, "500001_wheat_seed", "500001_wheat_seed", 1261, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100002 = LordNew cRanchSeeds(702, "500002_corn_seed", "500002_corn_seed", 1262, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100003 = LordNew cRanchSeeds(703, "500003_carrots_seed", "500003_carrots_seed", 1263, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100004 = LordNew cRanchSeeds(704, "500004_sugar_cane_seed", "500004_sugar_cane_seed", 1264, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100005 = LordNew cRanchSeeds(705, "500005_cotton_seed", "500005_cotton_seed", 1265, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100006 = LordNew cRanchSeeds(706, "500006_strawberry_seed", "500006_strawberry_seed", 1266, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100007 = LordNew cRanchSeeds(707, "500007_tomato_seed", "500007_tomato_seed", 1267, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100008 = LordNew cRanchSeeds(708, "500008_pine_seed", "500008_pine_seed", 1268, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100009 = LordNew cRanchSeeds(709, "500009_potatoes_seed", "500009_potatoes_seed", 1269, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100010 = LordNew cRanchSeeds(710, "500010_cocoa_beans_seed", "500010_cocoa_beans_seed", 1270, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100011 = LordNew cRanchSeeds(711, "500011_rubber_wood_seed", "500011_rubber_wood_seed", 1271, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100012 = LordNew cRanchSeeds(712, "500012_silk_seed", "500012_silk_seed", 1272, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100013 = LordNew cRanchSeeds(713, "500013_chili_seed", "500013_chili_seed", 1273, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100014 = LordNew cRanchSeeds(714, "500014_rice_seed", "500014_rice_seed", 1274, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100015 = LordNew cRanchSeeds(715, "500015_the_rose_seed", "500015_the_rose_seed", 1275, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100016 = LordNew cRanchSeeds(716, "500016_jasmine_seed", "500016_jasmine_seed", 1276, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100017 = LordNew cRanchSeeds(717, "500017_coffee_beans_seed", "500017_coffee_beans_seed", 1277, BLOCK_ID_TILLED_FIELD);

	//throwing skill 1200-1211  944-955
	itemThrowableSkillProp_11 = LordNew cItemProp(944, "skill_11", "skill_11", "skill_11.mesh");
	itemThrowableSkillProp_12 = LordNew cItemProp(945, "skill_12", "skill_12", "skill_12.mesh");
	itemThrowableSkillProp_13 = LordNew cItemProp(946, "skill_13", "skill_13", "skill_13.mesh");
	itemThrowableSkillProp_14 = LordNew cItemProp(947, "skill_14", "skill_14", "skill_14.mesh");
	itemThrowableSkillProp_15 = LordNew cItemProp(948, "skill_15", "skill_15", "skill_15.mesh");
	itemThrowableSkillProp_16 = LordNew cItemProp(949, "skill_16", "skill_16", "skill_16.mesh");
	itemThrowableSkillProp_17 = LordNew cItemProp(950, "skill_17", "skill_17", "skill_17.mesh");
	itemThrowableSkillProp_18 = LordNew cItemProp(951, "skill_18", "skill_18", "skill_18.mesh");
	itemThrowableSkillProp_19 = LordNew cItemProp(952, "skill_19", "skill_19", "skill_19.mesh");
	itemThrowableSkillProp_20 = LordNew cItemProp(953, "skill_20", "skill_20", "skill_20.mesh");

	// pro 1212-1235  956-979
	itemThrowableSkillProp_04_plus = LordNew cItemProp(956, "skill_04_plus", "skill_04_plus", "skill_04_plus.mesh");
	itemThrowableSkillProp_05_plus = LordNew cItemProp(957, "skill_05_plus", "skill_05_plus", "skill_05_plus.mesh");
	itemThrowableSkillProp_06_plus = LordNew cItemProp(958, "skill_06_plus", "skill_06_plus", "skill_06_plus.mesh");
	itemThrowableSkillProp_07_plus = LordNew cItemProp(959, "skill_07_plus", "skill_07_plus", "skill_07_plus.mesh");
	itemThrowableSkillProp_08_plus = LordNew cItemProp(960, "skill_08_plus", "skill_08_plus", "skill_08_plus.mesh");
	itemThrowableSkillProp_09_plus = LordNew cItemProp(961, "skill_09_plus", "skill_09_plus", "skill_09_plus.mesh");
	itemThrowableSkillProp_11_plus = LordNew cItemProp(962, "skill_11_plus", "skill_11_plus", "skill_11_plus.mesh");
	itemThrowableSkillProp_12_plus = LordNew cItemProp(963, "skill_12_plus", "skill_12_plus", "skill_12_plus.mesh");
	itemThrowableSkillProp_13_plus = LordNew cItemProp(964, "skill_13_plus", "skill_13_plus", "skill_13_plus.mesh");
	itemThrowableSkillProp_14_plus = LordNew cItemProp(965, "skill_14_plus", "skill_14_plus", "skill_14_plus.mesh");
	itemThrowableSkillProp_15_plus = LordNew cItemProp(966, "skill_15_plus", "skill_15_plus", "skill_15_plus.mesh");
	itemThrowableSkillProp_16_plus = LordNew cItemProp(967, "skill_16_plus", "skill_16_plus", "skill_16_plus.mesh");
	itemThrowableSkillProp_17_plus = LordNew cItemProp(968, "skill_17_plus", "skill_17_plus", "skill_17_plus.mesh");
	itemThrowableSkillProp_18_plus = LordNew cItemProp(969, "skill_18_plus", "skill_18_plus", "skill_18_plus.mesh");
	itemThrowableSkillProp_19_plus = LordNew cItemProp(970, "skill_19_plus", "skill_19_plus", "skill_19_plus.mesh");
	itemThrowableSkillProp_20_plus = LordNew cItemProp(971, "skill_20_plus", "skill_20_plus", "skill_20_plus.mesh");

	//weapon  1236-1255  980-999
	bow_red = LordNew cItemBow(980, "bow_red", "bow_red");
	bow_black = LordNew cItemBow(981, "bow_black", "bow_black");
	redArrow = LordNew cItemArrow(982, "redArrow", "redArrow");
	blackArrow = LordNew cItemArrow(983, "blackArrow", "blackArrow");

	weapon_01 = LordNew cItemProp(984, "weapon_01", "weapon_01", StringUtil::BLANK);
	weapon_02 = LordNew cItemProp(985, "weapon_02", "weapon_02", "weapon_02.mesh");
	weapon_04 = LordNew cItemProp(987, "weapon_04", "weapon_04", "weapon_04.mesh");
	weapon_05 = LordNew cItemProp(988, "weapon_05", "weapon_05", "weapon_05.mesh");
	weapon_06 = LordNew cItemProp(989, "weapon_06", "weapon_06", "weapon_06.mesh");
	weapon_07 = LordNew cItemProp(990, "weapon_07", "weapon_07", "weapon_07.mesh");
	weapon_08 = LordNew cItemProp(991, "weapon_08", "weapon_08", "weapon_08.mesh");
	weapon_09 = LordNew cItemProp(992, "weapon_09", "weapon_09", "weapon_09.mesh");
	weapon_10 = LordNew cItemProp(993, "weapon_10", "weapon_10", "weapon_10.mesh");
	weapon_11 = LordNew cItemProp(994, "weapon_11", "weapon_11", "weapon_11.mesh");
	weapon_12 = LordNew cItemProp(995, "weapon_12", "weapon_12", "weapon_12.mesh");
	weapon_13 = LordNew cItemProp(996, "weapon_13", "weapon_13", "weapon_13.mesh");
	weapon_14 = LordNew cItemProp(997, "weapon_14", "weapon_14", "weapon_14.mesh");
	weapon_15 = LordNew cItemProp(998, "weapon_15", "weapon_15", "weapon_15.mesh");
	weapon_16 = LordNew cItemProp(999, "weapon_16", "weapon_16", "weapon_16.mesh");
	weapon_17 = LordNew cItemProp(1000, "weapon_17", "weapon_17", "weapon_17.mesh");
	weapon_18 = LordNew cItemProp(1001, "weapon_18", "weapon_18", "weapon_18.mesh");
	//<id 1001 ~1256 item taboo use, 1001  use game too much, do not modify>
	//<<Item id 1745~2744>>
	handGrenade = LordNew cItemHandGrenade(1745, "handGrenade", "handGrenade", "bed_grenade.mesh");
	
	// bow skin 1881~2000
	bow_01 = LordNew cItemBow(1881, "g1008_bow_01", "g1008_bow_01", "g1008_bow_01.mesh");
	bow_02 = LordNew cItemBow(1882, "g1008_bow_02", "g1008_bow_02", "g1008_bow_02.mesh");
	bow_03 = LordNew cItemBow(1883, "g1008_bow_03", "g1008_bow_03", "g1008_bow_03.mesh");

    book_1 = LordNew cItemEditableBook(2031, "writtenBook1", "book_written");
	book_2 = LordNew cItemEditableBook(2032, "writtenBook2", "book_written");
	book_3 = LordNew cItemEditableBook(2033, "writtenBook3", "book_written");
	book_4 = LordNew cItemEditableBook(2034, "writtenBook4", "book_written");
	book_5 = LordNew cItemEditableBook(2035, "writtenBook5", "book_written");
	book_6 = LordNew cItemEditableBook(2036, "writtenBook6", "book_written");
	book_7 = LordNew cItemEditableBook(2037, "writtenBook7", "book_written");
	book_8 = LordNew cItemEditableBook(2038, "writtenBook8", "book_written");

	cornIron = LordNew ItemClient(2039, "cornIron", "cornIron");
	strawberryIron = LordNew ItemClient(2040, "strawberryIron", "strawberryIron");
	tomatoIron = LordNew ItemClient(2041, "tomatoIron", "tomatoIron");
	wheatIron = LordNew ItemClient(2042, "wheatIron", "wheatIron");
	cottonIron = LordNew ItemClient(2043, "cottonIron", "cottonIron");
	silkIron = LordNew ItemClient(2044, "silkIron", "silkIron");

	decoration_01 = LordNew cItemDecoration(2345, "decoration_01", "", "");
	decoration_02 = LordNew cItemDecoration(2346, "decoration_02", "", "");
	decoration_03 = LordNew cItemDecoration(2347, "decoration_03", "", "");
	decoration_04 = LordNew cItemDecoration(2348, "decoration_04", "", "");
	decoration_05 = LordNew cItemDecoration(2349, "decoration_05", "", "");
	decoration_06 = LordNew cItemDecoration(2350, "decoration_06", "", "");
	decoration_07 = LordNew cItemDecoration(2351, "decoration_07", "", "");
	decoration_08 = LordNew cItemDecoration(2352, "decoration_08", "", "");
	decoration_09 = LordNew cItemDecoration(2353, "decoration_09", "", "");
	decoration_10 = LordNew cItemDecoration(2354, "decoration_10", "", "");
	decoration_11 = LordNew cItemDecoration(2355, "decoration_11", "", "");
	decoration_12 = LordNew cItemDecoration(2356, "decoration_12", "", "");
	decoration_13 = LordNew cItemDecoration(2357, "decoration_13", "", "");
	decoration_14 = LordNew cItemDecoration(2358, "decoration_14", "", "");
	decoration_15 = LordNew cItemDecoration(2359, "decoration_15", "", "");
	decoration_16 = LordNew cItemDecoration(2360, "decoration_16", "", "");
	decoration_17 = LordNew cItemDecoration(2361, "decoration_17", "", "");
	decoration_18 = LordNew cItemDecoration(2362, "decoration_18", "", "");
	decoration_19 = LordNew cItemDecoration(2363, "decoration_19", "", "");
	decoration_20 = LordNew cItemDecoration(2364, "decoration_20", "", "");
	decoration_22 = LordNew cItemDecoration(2365, "decoration_22", "", "");
	decoration_22 = LordNew cItemDecoration(2366, "decoration_22", "", "");
	decoration_23 = LordNew cItemDecoration(2367, "decoration_23", "", "");
	decoration_24 = LordNew cItemDecoration(2368, "decoration_24", "", "");
	decoration_25 = LordNew cItemDecoration(2369, "decoration_25", "", "");
	decoration_26 = LordNew cItemDecoration(2370, "decoration_26", "", "");
	decoration_27 = LordNew cItemDecoration(2371, "decoration_27", "", "");
	decoration_28 = LordNew cItemDecoration(2372, "decoration_28", "", "");
	decoration_29 = LordNew cItemDecoration(2373, "decoration_29", "", "");
	decoration_30 = LordNew cItemDecoration(2374, "decoration_30", "", "");
	decoration_31 = LordNew cItemDecoration(2375, "decoration_31", "", "");
	decoration_32 = LordNew cItemDecoration(2376, "decoration_32", "", "");
	decoration_33 = LordNew cItemDecoration(2377, "decoration_33", "", "");
	decoration_34 = LordNew cItemDecoration(2378, "decoration_34", "", "");
	decoration_35 = LordNew cItemDecoration(2379, "decoration_35", "", "");
	decoration_36 = LordNew cItemDecoration(2380, "decoration_36", "", "");
	decoration_37 = LordNew cItemDecoration(2381, "decoration_37", "", "");
	decoration_38 = LordNew cItemDecoration(2382, "decoration_38", "", "");
	decoration_39 = LordNew cItemDecoration(2383, "decoration_39", "", "");
	decoration_40 = LordNew cItemDecoration(2384, "decoration_40", "", "");
	decoration_41 = LordNew cItemDecoration(2385, "decoration_41", "", "");
	decoration_42 = LordNew cItemDecoration(2386, "decoration_42", "", "");
	decoration_43 = LordNew cItemDecoration(2387, "decoration_43", "", "");
	decoration_44 = LordNew cItemDecoration(2388, "decoration_44", "", "");
	decoration_45 = LordNew cItemDecoration(2389, "decoration_45", "", "");
	decoration_46 = LordNew cItemDecoration(2390, "decoration_46", "", "");
	decoration_47 = LordNew cItemDecoration(2391, "decoration_47", "", "");
	decoration_48 = LordNew cItemDecoration(2392, "decoration_48", "", "");
	decoration_49 = LordNew cItemDecoration(2393, "decoration_49", "", "");
	decoration_50 = LordNew cItemDecoration(2394, "decoration_50", "", "");
	decoration_51 = LordNew cItemDecoration(2395, "decoration_51", "", "");
	decoration_52 = LordNew cItemDecoration(2396, "decoration_52", "", "");
	decoration_53 = LordNew cItemDecoration(2397, "decoration_53", "", "");
	decoration_54 = LordNew cItemDecoration(2398, "decoration_54", "", "");
	decoration_55 = LordNew cItemDecoration(2399, "decoration_55", "", "");
	decoration_56 = LordNew cItemDecoration(2400, "decoration_56", "", "");
	decoration_57 = LordNew cItemDecoration(2401, "decoration_57", "", "");
	decoration_58 = LordNew cItemDecoration(2402, "decoration_58", "", "");
	decoration_59 = LordNew cItemDecoration(2403, "decoration_59", "", "");
	decoration_60 = LordNew cItemDecoration(2404, "decoration_60", "", "");
	decoration_61 = LordNew cItemDecoration(2405, "decoration_61", "", "");
	decoration_62 = LordNew cItemDecoration(2406, "decoration_62", "", "");
	decoration_63 = LordNew cItemDecoration(2407, "decoration_63", "", "");
	decoration_64 = LordNew cItemDecoration(2408, "decoration_64", "", "");
	decoration_65 = LordNew cItemDecoration(2409, "decoration_65", "", "");
	decoration_66 = LordNew cItemDecoration(2410, "decoration_66", "", "");
	decoration_67 = LordNew cItemDecoration(2411, "decoration_67", "", "");
	decoration_68 = LordNew cItemDecoration(2412, "decoration_68", "", "");
	decoration_69 = LordNew cItemDecoration(2413, "decoration_69", "", "");
	decoration_70 = LordNew cItemDecoration(2414, "decoration_70", "", "");
	decoration_71 = LordNew cItemDecoration(2415, "decoration_71", "", "");
	decoration_72 = LordNew cItemDecoration(2416, "decoration_72", "", "");
	decoration_73 = LordNew cItemDecoration(2417, "decoration_73", "", "");
	decoration_74 = LordNew cItemDecoration(2418, "decoration_74", "", "");
	decoration_75 = LordNew cItemDecoration(2419, "decoration_75", "", "");
	decoration_76 = LordNew cItemDecoration(2420, "decoration_76", "", "");
	decoration_77 = LordNew cItemDecoration(2421, "decoration_77", "", "");
	decoration_78 = LordNew cItemDecoration(2422, "decoration_78", "", "");
	decoration_79 = LordNew cItemDecoration(2423, "decoration_79", "", "");
	decoration_80 = LordNew cItemDecoration(2424, "decoration_80", "", "");
	decoration_81 = LordNew cItemDecoration(2425, "decoration_81", "", "");
	decoration_82 = LordNew cItemDecoration(2426, "decoration_82", "", "");
	decoration_83 = LordNew cItemDecoration(2427, "decoration_83", "", "");
	decoration_84 = LordNew cItemDecoration(2428, "decoration_84", "", "");
	decoration_85 = LordNew cItemDecoration(2429, "decoration_85", "", "");
	decoration_86 = LordNew cItemDecoration(2430, "decoration_86", "", "");
	decoration_87 = LordNew cItemDecoration(2431, "decoration_87", "", "");
	decoration_88 = LordNew cItemDecoration(2432, "decoration_88", "", "");
	decoration_89 = LordNew cItemDecoration(2433, "decoration_89", "", "");
	decoration_90 = LordNew cItemDecoration(2434, "decoration_90", "", "");
	decoration_91 = LordNew cItemDecoration(2435, "decoration_91", "", "");
	decoration_92 = LordNew cItemDecoration(2436, "decoration_92", "", "");
	decoration_93 = LordNew cItemDecoration(2437, "decoration_93", "", "");
	decoration_94 = LordNew cItemDecoration(2438, "decoration_94", "", "");
	decoration_95 = LordNew cItemDecoration(2439, "decoration_95", "", "");
	decoration_96 = LordNew cItemDecoration(2440, "decoration_96", "", "");
	decoration_97 = LordNew cItemDecoration(2441, "decoration_97", "", "");
	decoration_98 = LordNew cItemDecoration(2442, "decoration_98", "", "");
	decoration_99 = LordNew cItemDecoration(2443, "decoration_99", "", "");
	decoration_100 = LordNew cItemDecoration(2444, "decoration_100", "", "");

	initializeItemNameToIdMap();

	cBlockManager::cRegistBlockItems();
	cPotionManager::cRegisterPotionItem();

	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		if (!itemsList[i])
			continue;
		citemsList[i] = dynamic_cast<cItem*>(itemsList[i]);
		LordAssert(citemsList[i]);
	}
}

void ItemClient::cUnInitialize()
{
	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		LordSafeDelete(citemsList[i]);
		citemsList[i] = nullptr;
	}
	LordSafeDelete(itemRand);

	//cPotionManager::cUnRegisterPotionItem();
	//cBlockManager::cUnRegistBlockItems();
}

void ItemClient::registSprite(TextureAtlasRegister* pAtlas)
{
	cItem* pItem = NULL;
	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		pItem = citemsList[i];
		if (!pItem)
			continue;
		if (pItem->getRenderType() == ItemRenderType::BLOCK)
			continue;
		pItem->registerIcons(pAtlas);
	}
}


ItemRenderType ItemClient::getRenderType(ItemStackPtr pStack)
{
	if (pStack == NULL)
		return ItemRenderType::TEXTURE;

	if (pStack->getItem() != NULL)
	{
		int id = pStack->getItem()->itemID;
		if(id >= 0 && id < MAX_ITEM_COUNT && citemsList[id] != NULL)
			return citemsList[id]->getRenderType();
	}

	return ItemRenderType::TEXTURE;
}


cItemSpade::cItemSpade(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemSpade(id, name, toolMat)
, cItem(atlas)
{}

cItemPickaxe::cItemPickaxe(int id, const String& name, const String& atlas, ToolMaterial* toolMat, String meshName)
: ItemPickaxe(id, name, toolMat)
, cItem(atlas)
, m_meshName(meshName)
{}

ItemRenderType cItemPickaxe::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemPickaxe::getMeshName() const
{
	return m_meshName;
}

cItemAxe::cItemAxe(int id, const String& name, const String& atlas, ToolMaterial* toolMat, String meshName)
: ItemAxe(id, name, toolMat)
, cItem(atlas)
, m_meshName(meshName)
{}

ItemRenderType cItemAxe::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemAxe::getMeshName() const
{
	return m_meshName;
}

cItemFlintAndSteel::cItemFlintAndSteel(int id, const String& name, const String& atlas)
: ItemFlintAndSteel(id, name)
, cItem(atlas)
{}

cItemFood::cItemFood(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite, const String& mesh_name)
: ItemFood(id, name, amount, modifier, isfavorite)
, cItem(atlas)
, m_meshName(mesh_name)
{}

cItemFood::cItemFood(int id, const String& name, const String& atlas, int amount, bool isfavorite)
: ItemFood(id, name, amount, isfavorite)
, cItem(atlas)
{}

ItemRenderType cItemFood::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemFood::getMeshName() const
{
	return m_meshName;
}

cItemSoup::cItemSoup(int id, const String& name, const String& atlas, int amount)
: ItemSoup(id, name, amount)
, cItem(atlas)
{}

cItemAppleGold::cItemAppleGold(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite, const String& mesh_name)
	: ItemAppleGold(id, name, amount, modifier, isfavorite)
	, cItem(atlas)
	, m_meshName(mesh_name)
{}

cItemAppleGold::cItemAppleGold(int id, const String& name, const String& atlas, int amount, float modifier, bool isfavorite)
: ItemAppleGold(id, name, amount, modifier, isfavorite)
, cItem(atlas)
{}

ItemRenderType cItemAppleGold::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemAppleGold::getMeshName() const
{
	return m_meshName;
}

cItemSeedFood::cItemSeedFood(int id, const String& name, const String& atlas, int amount, float modifier, int crop, int soil)
: ItemSeedFood(id, name, amount, modifier, crop, soil)
, cItem(atlas)
{}

const String cItemBow::bowPullIconNameArray[MAX_ATLAS_COUNT] = { "pulling_0", "pulling_1", "pulling_2" };

cItemBow::cItemBow(int id, const String& name, const String& atlas, String meshName)
: ItemBow(id, name)
, cItem(atlas)
, m_meshName(meshName)
{
	if (m_meshName.length() > 5)
	{
		for (int i = 0; i < MAX_ATLAS_COUNT; ++i)
		{
			m_meshName_pulling[i] = m_meshName;
		}
		size_t pos = m_meshName.rfind(".mesh");
		if (pos != String::npos)
		{
			for (int i = 0; i < MAX_ATLAS_COUNT; ++i)
			{
				m_meshName_pulling[i].insert(pos, "_" + bowPullIconNameArray[i]);
			}
		}
	}
}

ItemRenderType cItemBow::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemBow::getMeshName() const
{
	return m_meshName;
}

const String & cItemBow::getMeshNameWithMeta(int meta) const
{
	if (meta > 0 && meta <= MAX_ATLAS_COUNT)
	{
		return m_meshName_pulling[meta - 1];
	}
	return m_meshName;
}

void cItemBow::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = cItem::registerIcons(atlas, getAtlasName() + "_standby");

	for (int i = 0; i < MAX_ATLAS_COUNT; ++i)
	{
		iconArray[i] = cItem::registerIcons(atlas, getAtlasName() + "_" + bowPullIconNameArray[i]);
	}
}

int cItemBow::getDurationSeg(int useDuration)
{
	int dur = 0;
	if (useDuration >= 18)
	{
		dur = 3;
	}
	else if (useDuration > 13)
	{
		dur = 2;
	}
	else if (useDuration > 0)
	{
		dur = 1;
	}
	return dur;
}

cItemGun::cItemGun(int id, const String& name, const String& atlas, const String& mesh)
	: ItemGun(id, name)
	, cItem(atlas)
	, m_meshName(mesh)
{
}

ItemRenderType cItemGun::getRenderType()
{
	return ItemRenderType::MESH;
}

const String& cItemGun::getMeshName() const
{
	return m_meshName;
}

void cItemGun::fire(ItemStackPtr pStack, World* pWorld, EntityPlayer* pPlayer)
{
	const GunSetting* setting = getGunSetting();
	auto player = Blockman::Instance()->m_pPlayer;
	if (player && !player->m_bIsInAimSight && setting && (setting->gunType < RIFLE_EFFECT_GUN || setting->gunType > MOMENTARY_PIERCING_LASER_EFFECT_GUN))
	{
		Blockman::Instance()->getWorld()->spawnParticle("gunFlame", Vector3::ZERO, Vector3::ZERO, player);
	}

	ItemGun::fire(pStack, pWorld, pPlayer);

	// adjust the pitch for gun Recoil
	float recoil = ((float)pWorld->m_Rand.nextInt((int)(pPlayer->getGunRecoil(setting) * 100))) / 100;
	Blockman::Instance()->m_pPlayer->rotationPitch -= recoil;
	if (Blockman::Instance()->m_gameSettings->getPersonView() == GameSettings::SPVT_FIRST)
	{
		Blockman::Instance()->m_pPlayer->m_isShooting = true;
	}
}

cItemBulletClip::cItemBulletClip(int id, const String& name, const String& atlas)
	: ItemBulletClip(id, name)
	, cItem(atlas)
{
}

cItemHandcuffs::cItemHandcuffs(int id, const String& name, const String& atlas)
	: ItemHandcuffs(id, name)
	, cItem(atlas)
{
}

cItemCoal::cItemCoal(int id, const String& name, const String& atlas)
: ItemCoal(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemCoal::getIconFromDamage(int damage)
{
	return damage == 1 ? m_charcoal : cItem::getIconFromDamage(damage);
}

void cItemCoal::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);
	m_charcoal = cItem::registerIcons(atlas, "charcoal");
}

cItemSword::cItemSword(int id, const String& name, const String& atlas, ToolMaterial* toolMat, String meshName)
: ItemSword(id, name, toolMat)
, cItem(atlas)
, m_meshName(meshName)
{}

ItemRenderType cItemSword::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemSword::getMeshName() const
{
	return m_meshName;
}

cItemReed::cItemReed(int id, const String& name, const String& atlas, int reedID)
: ItemReed(id, name, reedID)
, cItem(atlas)
{}

cItemHoe::cItemHoe(int id, const String& name, const String& atlas, ToolMaterial* toolMat)
: ItemHoe(id, name, toolMat)
, cItem(atlas)
{}

cItemSeeds::cItemSeeds(int id, const String& name, const String& atlas, int blocktype, int soilID)
: ItemSeeds(id, name, blocktype, soilID)
, cItem(atlas)
{}

cItemHangingEntity::cItemHangingEntity(int id, const String& name, const String& atlas, int classID)
: ItemHangingEntity(id, name, classID)
, cItem(atlas)
{}

cItemSign::cItemSign(int id, const String& name, const String& atlas)
: ItemSign(id, name)
, cItem(atlas)
{}

cItemDoor::cItemDoor(int id, const String& name, const String& atlas, const BM_Material& mat)
: ItemDoor(id, name, mat)
, cItem(atlas)
{}

cItemEnderPearl::cItemEnderPearl(int id, const String& name, const String& atlas)
: ItemEnderPearl(id, name)
, cItem(atlas)
{}

cItemGlassBottle::cItemGlassBottle(int id, const String& name, const String& atlas)
: ItemGlassBottle(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemGlassBottle::getIconFromDamage(int damage)
{
	return cItem::getIconFromDamage(0);
}

cItemEnderEye::cItemEnderEye(int id, const String& name, const String& atlas)
: ItemEnderEye(id, name)
, cItem(atlas)
{}

cItemMonsterPlacer::cItemMonsterPlacer(int id, const String& name, const String& atlas)
: ItemMonsterPlacer(id, name)
, cItem(atlas)
{}

int cItemMonsterPlacer::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	// todo.
	// EntityEggInfo var3 = (EntityEggInfo)EntityList.entityEggs.get(Integer.valueOf(pStack.getItemDamage()));
	// return var3 != null ? (par2 == 0 ? var3.primaryColor : var3.secondaryColor) : 16777215;
	return 0x00FFFFFF;
}

AtlasSprite* cItemMonsterPlacer::getIconFromDamageForRenderPass(int damage, int pass)
{
	return pass > 0 ? theIcon : cItem::getIconFromDamageForRenderPass(damage, pass);
}

void cItemMonsterPlacer::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);
	theIcon = cItem::registerIcons(atlas, getAtlasName() + "_overlay");
}

cItemArmor::cItemArmor(int id, const String& name, const String& atlas, ArmorMaterial* armorMat, 
	int armortype, String meshName, String renderName)
: ItemArmor(id, name, armorMat, armortype)
, cItem(atlas)
, m_meshName(meshName)
, m_renderName(renderName)
{}

ItemRenderType cItemArmor::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemArmor::getMeshName() const
{
	return m_meshName;
}

String cItemArmor::getRenderNamePrefix()
{
	if (m_renderName.empty())
	{
		if (material == ArmorMaterial::CLOTH)
			return "leather";
		else if (material == ArmorMaterial::CHAIN)
			return "chain";
		else if (material == ArmorMaterial::IRON)
			return "iron";
		else if (material == ArmorMaterial::DIAMOND)
			return "diamond";
		else if (material == ArmorMaterial::GOLD)
			return "gold";
		else
			return "leather";
	}
	return m_renderName == "none" ? "" : m_renderName;
}

AtlasSprite* cItemArmor::getSpriteByArmorType(int armorIdx)
{
	switch (armorIdx)
	{
	case 0:
		return ((cItemArmor*)Item::helmetDiamond)->armor_sprite;
	case 1:
		return ((cItemArmor*)Item::plateDiamond)->armor_sprite;
	case 2:
		return ((cItemArmor*)Item::legsDiamond)->armor_sprite;
	case 3:
		return ((cItemArmor*)Item::bootsDiamond)->armor_sprite;
	}
	return NULL;
}

int cItemArmor::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	if (par2 >= 0)
	{
		return 0x00FFFFFF;
	}
	else
	{
		int color = getColor(pStack);

		if (color < 0)
		{
			color = 0x00FFFFFF;
		}

		return color;
	}
}

AtlasSprite* cItemArmor::getIconFromDamageForRenderPass(int damage, int pass)
{
	return pass == 1 ? leather_sprite : cItem::getIconFromDamageForRenderPass(damage, pass);
}

void cItemArmor::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);

	if (material == ArmorMaterial::CLOTH)
	{
		leather_sprite = cItem::registerIcons(atlas, leather_armor_name[armorType]);
	}

	armor_sprite = cItem::registerIcons(atlas, empty_armor_name[armorType]);
}


const String cItemArmor::leather_armor_name[MAX_ARMOR_COUNT] = { "leather_helmet_overlay", "leather_chestplate_overlay", "leather_leggings_overlay", "leather_boots_overlay", "leather_boots_overlay", "leather_boots_overlay", "leather_boots_overlay", "leather_boots_overlay" };
const String cItemArmor::empty_armor_name[MAX_ARMOR_COUNT] = { "empty_armor_slot_helmet", "empty_armor_slot_chestplate", "empty_armor_slot_leggings", "empty_armor_slot_boots", "empty_armor_slot_boots", "empty_armor_slot_boots", "empty_armor_slot_boots", "empty_armor_slot_boots" };


cItemPotion::cItemPotion(int id, const String& name, const String& atlas, bool isSplash, const String& mesh_name)
	: ItemPotion(id, name, isSplash)
	, cItem(atlas)
	, m_meshName(mesh_name)
{}

cItemPotion::cItemPotion(int id, const String& name, const String& atlas, bool isSplash/* = false*/)
: ItemPotion(id, name, isSplash)
, cItem(atlas)
{}

ItemRenderType cItemPotion::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemPotion::getMeshName() const
{
	return m_meshName;
}

AtlasSprite* cItemPotion::getSpriteByName(const String& spritename)
{
	if (spritename == "bottle_drinkable")
		return ((cItemPotion*)Item::potion)->sprite_potion_drinkable;
	else if (spritename == "bottle_splash")
		return ((cItemPotion*)Item::potion)->sprite_potion_splash;
	else if (spritename == "overlay")
		return ((cItemPotion*)Item::potion)->sprite_potion_overlay;
	return NULL;
}

AtlasSprite* cItemPotion::getIconFromDamageForRenderPass(int damage, int pass)
{
	return pass == 0 ? sprite_potion_overlay : cItem::getIconFromDamageForRenderPass(damage, pass);
}

int cItemPotion::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	return par2 > 0 ? 0x00FFFFFF : getColorFromDamage(pStack->getItemDamage());
}

void cItemPotion::registerIcons(TextureAtlasRegister* atlas)
{
	sprite_potion_drinkable = cItem::registerIcons(atlas, getAtlasName() + "_bottle_drinkable");
	sprite_potion_splash = cItem::registerIcons(atlas, getAtlasName() + "_bottle_splash");
	sprite_potion_overlay = cItem::registerIcons(atlas, getAtlasName() + "_overlay");
}

cItemEnchantedBook::cItemEnchantedBook(int id, const String& name, const String& atlas)
: ItemEnchantedBook(id, name)
, cItem(atlas)
{}

const String cItemDye::dyeSpriteNames[MAX_DYE_NUM] = { "black", "red", "green", "brown", "blue", "purple", "cyan", "silver", "gray", "pink", "lime", "yellow", "light_blue", "magenta", "orange", "white" };

cItemDye::cItemDye(int id, const String& name, const String& atlas)
: ItemDye(id, name)
, cItem(atlas)
{
	for (int i = 0; i < MAX_DYE_NUM; ++i)
		dyeSprites[i] = nullptr;
}

AtlasSprite* cItemDye::getIconFromDamage(int damage)
{
	int idx = Math::Clamp(damage, 0, 15);
	if (dyeSprites[idx])
	{
		return dyeSprites[idx];
	}
	return cItem::getIconFromDamage(damage);
}

void cItemDye::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < MAX_DYE_NUM; ++i)
	{
		dyeSprites[i] = cItem::registerIcons(atlas, getAtlasName() + "_" + dyeSpriteNames[i]);
	}
	cItem::registerIcons(atlas);
}

cItemMapBase::cItemMapBase(int id, const String& name, const String& atlas)
: ItemMapBase(id, name)
, cItem(atlas)
{}

cItemEmptyMap::cItemEmptyMap(int id, const String& name, const String& atlas)
: ItemEmptyMap(id, name)
, cItem(atlas)
{}

cItemMap::cItemMap(int id, const String& name, const String& atlas)
: ItemMap(id, name)
, cItem(atlas)
{}

cItemBlock::cItemBlock(int id)
: cItem("empty")
, blockSprite(nullptr)
{
	int itemId = id + 256;
	LordAssert(Item::isBlockItem(itemId));
	if (Item::isBasicBlockItem(itemId))
	{
		m_blockID = itemId;
	}
	else
	{
		m_blockID = itemId - Item::EC_BLOCK_TO_ITEM_OFFSET;
	}
}

AtlasSprite* cItemBlock::getIconFromDamage(int damage)
{
	return blockSprite ? blockSprite : cBlockManager::scBlocks[m_blockID]->getSpriteFromSide(1);
}

ItemRenderType cItemBlock::getRenderType()
{
	return cBlockManager::scBlocks[m_blockID]->getItemSpriteName() != StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::BLOCK;
}

void cItemBlock::registerIcons(TextureAtlasRegister* atlas)
{
	String spriteName = cBlockManager::scBlocks[m_blockID]->getItemSpriteName();

	if (spriteName != StringUtil::BLANK)
	{
		blockSprite = cItem::registerIcons(atlas, spriteName);
	}
}

cItemShears::cItemShears(int id, const String& name, const String& atlas, const String& mesh_name)
: ItemShears(id, name)
, cItem(atlas)
{
	m_meshName = mesh_name;
}

ItemRenderType cItemShears::getRenderType()
{
	return getMeshName().empty() ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemShears::getMeshName() const
{
	return m_meshName;
}

cItemBucket::cItemBucket(int id, const String& name, const String& atlas, int fillID, const String& mesh_name)
: ItemBucket(id, name, fillID)
, cItem(atlas)
, m_meshName(mesh_name)
{}

ItemRenderType cItemBucket::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemBucket::getMeshName() const
{
	return m_meshName;
}

cItemBucketMilk::cItemBucketMilk(int id, const String& name, const String& atlas, const String& mesh_name)
: ItemBucketMilk(id, name)
, cItem(atlas)
, m_meshName(mesh_name)
{}

ItemRenderType cItemBucketMilk::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemBucketMilk::getMeshName() const
{
	return m_meshName;
}

cItemBlockman::cItemBlockman(int id, const String& name, const String& atlas, int type)
: ItemBlockman(id, name, type)
, cItem(atlas)
{}

cItemSaddle::cItemSaddle(int id, const String& name, const String& atlas)
: ItemSaddle(id, name)
, cItem(atlas)
{}

cItemRedstone::cItemRedstone(int id, const String& name, const String& atlas)
: ItemRedstone(id, name)
, cItem(atlas)
{}

const String cItemSnowball::spriteNames[SNOW_TYPE_CONUT] = { "0", "1", "2" };

cItemSnowball::cItemSnowball(int id, const String& name, const String& atlas)
: ItemSnowball(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemSnowball::getIconFromDamage(int meta)
{
	if (meta < 0 || meta >= SNOW_TYPE_CONUT)
	{
		meta = 0;
	}
	return sprites[meta];
}

void cItemSnowball::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < SNOW_TYPE_CONUT; ++i)
	{
		sprites[i] = cItem::registerIcons(atlas, getAtlasName() + "_" + spriteNames[i]);
	}
}


cItemTnt::cItemTnt(int id, const String& name)
	: ItemTnt(id, name)
	, cItemBlock(id)
{}

cItemGrenade::cItemGrenade(int id, const String& name, const String& atlas)
	: ItemGrenade(id, name)
	, cItem(atlas)
{}

void cItemGrenade::registerIcons(TextureAtlasRegister * atlas)
{
	auto grenade = GrenadeSetting::getGrenadeSetting(itemID);
	atlasName = grenade->itemIcon;
	cItem::registerIcons(atlas);
}

cItemBoat::cItemBoat(int id, const String& name, const String& atlas)
: ItemBoat(id, name)
, cItem(atlas)
{}

cItemBook::cItemBook(int id, const String& name, const String& atlas)
: ItemBook(id, name)
, cItem(atlas)
{}

cItemEgg::cItemEgg(int id, const String& name, const String& atlas)
: ItemEgg(id, name)
, cItem(atlas)
{}

cItemFishingRod::cItemFishingRod(int id, const String& name, const String& atlas)
: ItemFishingRod(id, name)
, cItem(atlas)
{}

void cItemFishingRod::registerIcons(TextureAtlasRegister* atlas)
{
	itemIcon = cItem::registerIcons(atlas, getAtlasName() + "_uncast");
	m_castIcon = cItem::registerIcons(atlas, getAtlasName() + "_cast");
}

cItemBed::cItemBed(int id, const String& name, const String& atlas)
: ItemBed(id, name)
, cItem(atlas)
{}

cItemExpBottle::cItemExpBottle(int id, const String& name, const String& atlas)
: ItemExpBottle(id, name)
, cItem(atlas)
{}

cItemFireball::cItemFireball(int id, const String& name, const String& atlas)
: ItemFireball(id, name)
, cItem(atlas)
{}

cItemWritableBook::cItemWritableBook(int id, const String& name, const String& atlas)
: ItemWritableBook(id, name)
, cItem(atlas)
{}

cItemEditableBook::cItemEditableBook(int id, const String& name, const String& atlas)
: ItemEditableBook(id, name)
, cItem(atlas)
{}

void cItemEditableBook::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	if (pStack->hasTagCompound())
	{
		NBTTagCompound* pNbtStack = pStack->getTagCompound();
		NBTTagString* pNbtAuthor = (NBTTagString*)pNbtStack->getTag("author");

		if (pNbtAuthor)
		{
			String prompt = "book.byAuthor ";
			prompt += pNbtAuthor->data;
			fills.push_back(prompt);
			// todo.
			// par3List.add(EnumChatFormatting.GRAY + String.format(StatCollector.translateToLocalFormatted("book.byAuthor", new Object[] {pNbtAuthor.data}), new Object[0]));
		}
	}
}

String cItemSkull::spriteNames[SKULL_TYPE_COUNT] = {"skeleton", "wither", "zombie", "steve", "creeper"};

cItemSkull::cItemSkull(int id, const String& name, const String& atlas)
: ItemSkull(id, name)
, cItem(atlas)
{
	for (int i = 0; i < SKULL_TYPE_COUNT; ++i)
		sprites[i] = nullptr;
}

AtlasSprite* cItemSkull::getIconFromDamage(int meta)
{
	if (meta < 0 || meta >= SKULL_TYPE_COUNT)
	{
		meta = 0;
	}

	if (sprites[meta] != NULL)
	{
		return sprites[meta];
	}

	return cItem::getIconFromDamage(meta);
}

void cItemSkull::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < SKULL_TYPE_COUNT; ++i)
	{
		sprites[i] = cItem::registerIcons(atlas, getAtlasName() + "_" + spriteNames[i]);
	}
	cItem::registerIcons(atlas);
}

cItemCarrotOnAStick::cItemCarrotOnAStick(int id, const String& name, const String& atlas)
: ItemCarrotOnAStick(id, name)
, cItem(atlas)
{}

cItemSimpleFoiled::cItemSimpleFoiled(int id, const String& name, const String& atlas)
: ItemSimpleFoiled(id, name)
, cItem(atlas)
{}

cItemFirework::cItemFirework(int id, const String& name, const String& atlas)
: ItemFirework(id, name)
, cItem(atlas)
{}

void cItemFirework::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	if (!pStack->hasTagCompound())
		return;
	
	NBTTagCompound* pNbtFireworks = pStack->getTagCompound()->getCompoundTag("Fireworks");

	if (!pNbtFireworks)
		return;
		
	if (pNbtFireworks->hasKey("Flight"))
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworks.flight") + " " + pNbtFireworks.getByte("Flight"));
		fills.push_back(StringUtil::Format("item.fireworks.flight %d", pNbtFireworks->getByte("Flight")));
	}

	NBTTagList* pNbtExplosion = pNbtFireworks->getTagList("Explosions");

	if (pNbtExplosion && pNbtExplosion->tagCount() > 0)
	{
		for (int i = 0; i < pNbtExplosion->tagCount(); ++i)
		{
			NBTTagCompound* pNbtTag = (NBTTagCompound*)pNbtExplosion->tagAt(i);
			StringArray strArr;
			cItemFireworkCharge::addExplosionInfo(pNbtTag, strArr);

			if (!strArr.empty())
			{
				for (int j = 1; j < int(strArr.size()); ++j)
				{
					String temp = strArr[j];
					strArr[j] = "  " + temp;
				}

				fills.insert(fills.end(), strArr.begin(), strArr.end());
			}
		}
	}
}

cItemFireworkCharge::cItemFireworkCharge(int id, const String& name, const String& atlas)
: ItemFireworkCharge(id, name)
, cItem(atlas)
{}

AtlasSprite* cItemFireworkCharge::getIconFromDamageForRenderPass(int damage, int pass)
{
	if (pass > 0 && theIcon)
	{
		return theIcon;
	}
	return cItem::getIconFromDamageForRenderPass(damage, pass);
}

int cItemFireworkCharge::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	if (par2 != 1)
		return cItem::getColorFromItemStack(pStack, par2);
	
	NBTBase* pNbtBase = getExplosionTag(pStack, "Colors");

	if (!pNbtBase)
		return 0x8A8A8A;
	
	NBTTagIntArray* pNbtArr = (NBTTagIntArray*)pNbtBase;

	if (pNbtArr->arraySize == 1)
		return pNbtArr->intArray[0];

	int r = 0;
	int g = 0;
	int b = 0;
	const int* pIntArr = pNbtArr->intArray;
	int len = pNbtArr->arraySize;

	for (int i = 0; i < len; ++i)
	{
		int color = pIntArr[i];
		r += (color & 16711680) >> 16;
		g += (color & 65280) >> 8;
		b += (color & 255) >> 0;
	}

	r /= len;
	g /= len;
	b /= len;
	return r << 16 | g << 8 | b;
}

void cItemFireworkCharge::registerIcons(TextureAtlasRegister* atlas)
{
	cItem::registerIcons(atlas);
	theIcon = cItem::registerIcons(atlas, getAtlasName() + "_overlay");
}

void cItemFireworkCharge::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	if (!pStack->hasTagCompound())
		return;

	NBTTagCompound* pNbtExplosion = pStack->getTagCompound()->getCompoundTag("Explosion");
	if (pNbtExplosion)
	{
		addExplosionInfo(pNbtExplosion, fills);
	}
}


NBTBase* cItemFireworkCharge::getExplosionTag(ItemStackPtr pStack, const String& key)
{
	if (pStack->hasTagCompound())
	{
		NBTTagCompound* pNbtExplosion = pStack->getTagCompound()->getCompoundTag("Explosion");

		if (pNbtExplosion)
		{
			return pNbtExplosion->getTag(key);
		}
	}

	return NULL;
}

void cItemFireworkCharge::addExplosionInfo(NBTTagCompound* pNbtCompound, StringArray& fills)
{
	int type = pNbtCompound->getByte("Type");

	if (type >= 0 && type <= 4)
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.type." + type).trim());
		fills.push_back(StringUtil::Format("item.fireworksCharge.type.%d", type));
	}
	else
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.type").trim());
		fills.push_back("item.fireworksCharge.type");
	}

	int* colors = pNbtCompound->getIntArray("Colors");
	int len = pNbtCompound->getArraySize("Colors");
	int var8;
	int var9;

	if (len > 0)
	{
		bool var4 = true;
		String var5 = "";
		int* var6 = colors;
		int var7 = len;

		for (var8 = 0; var8 < var7; ++var8)
		{
			var9 = var6[var8];

			if (!var4)
			{
				var5 = var5 + ", ";
			}

			var4 = false;
			bool var10 = false;

			for (int var11 = 0; var11 < 16; ++var11)
			{
				if (var9 == ItemDye::dyeColors[var11])
				{
					var10 = true;
					// todo.
					// var5 = var5 + StatCollector.translateToLocal("item.fireworksCharge." + ItemDye::dyeColorNames[var11]);
					var5 += "item.fireworksCharge.";
					var5 += ItemDye::dyeColorNames[var11];
					break;
				}
			}

			if (!var10)
			{
				// todo.
				// var5 = var5 + StatCollector.translateToLocal("item.fireworksCharge.customColor");
				var5 += "item.fireworksCharge.customColor";
			}
		}

		fills.push_back(var5);
	}

	int* var13 = pNbtCompound->getIntArray("FadeColors");
	len = pNbtCompound->getArraySize("FadeColors");
	bool var15;

	if (len > 0)
	{
		var15 = true;
		// todo.
		// String var14 = StatCollector.translateToLocal("item.fireworksCharge.fadeTo") + " ";
		String var14 = "item.fireworksCharge.fadeTo ";
		int* var16 = var13;
		var8 = len;

		for (var9 = 0; var9 < var8; ++var9)
		{
			int var18 = var16[var9];

			if (!var15)
			{
				var14 = var14 + ", ";
			}

			var15 = false;
			bool var19 = false;

			for (int var12 = 0; var12 < 16; ++var12)
			{
				if (var18 == ItemDye::dyeColors[var12])
				{
					var19 = true;
					// todo.
					// var14 = var14 + StatCollector.translateToLocal("item.fireworksCharge." + ItemDye::dyeColorNames[var12]);
					var14 += "item.fireworksCharge.";
					var14 += ItemDye::dyeColorNames[var12];
					break;
				}
			}

			if (!var19)
			{
				// todo.
				// var14 = var14 + StatCollector.translateToLocal("item.fireworksCharge.customColor");
				var14 += "item.fireworksCharge.customColor";
			}
		}

		fills.push_back(var14);
	}

	var15 = pNbtCompound->getBool("Trail");

	if (var15)
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.trail"));
		fills.push_back("item.fireworksCharge.trail");
	}

	bool var17 = pNbtCompound->getBool("Flicker");

	if (var17)
	{
		// todo.
		// fills.add(StatCollector.translateToLocal("item.fireworksCharge.flicker"));
		fills.push_back("item.fireworksCharge.flicker");
	}
}

cItemLeash::cItemLeash(int id, const String& name, const String& atlas)
: ItemLeash(id, name)
, cItem(atlas)
{}

cItemNameTag::cItemNameTag(int id, const String& name, const String& atlas)
: ItemNameTag(id, name)
, cItem(atlas)
{}


cItemLeaves::cItemLeaves(int id, const String& name)
	: ItemLeaves(id, name)
	, cItemBlock(id)
{
}

AtlasSprite* cItemLeaves::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[BLOCK_ID_LEAVES]->getIcon(0, damage);
}

int cItemLeaves::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	int damage = pStack->getItemDamage();	
	
	return	(damage & 1) == 1 ? 0x619961 :
			((damage & 2) == 2 ? 0x80A755 : 0x48B518);
}

cItemSlab::cItemSlab(int id, const String& name, Block* pHalfSlab, Block* pDoubleSlab, bool isFullBlock)
	:ItemSlab(id, name, pHalfSlab, pDoubleSlab, isFullBlock)
	, cItemBlock(id)
{
}

AtlasSprite* cItemSlab::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[blockID]->getIcon(2, damage);
}

cItemMultiTextureTile::cItemMultiTextureTile(int id, const String& name, const StringArray& names, cBlock* pBlock)
	: ItemMultiTextureTile(id, name, names)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemMultiTextureTile::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemAnvilBlock::cItemAnvilBlock(int id, const String& name, cBlock * pBlock)
	: ItemAnvilBlock(id, name)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemAnvilBlock::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemPiston::cItemPiston(int id)
	: ItemPiston(id)
	, cItemBlock(id)
{
}

cItemBlockWithMetadata::cItemBlockWithMetadata(int id, cBlock * pBlock)
	: ItemBlockWithMetadata(id)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemBlockWithMetadata::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemSnow::cItemSnow(int id, cBlock* pBlock)
	: ItemSnow(id)
	, cItemBlock(id)
	, m_pModelBlock(pBlock)
{
}

AtlasSprite* cItemSnow::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(2, damage);
}

cItemCloth::cItemCloth(int id, const String& name)
	: ItemCloth(id, name)
	, cItemBlock(id)
{
}

AtlasSprite* cItemCloth::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[BLOCK_ID_CLOTH]->getIcon(2, BlockColored::getBlockFromDye(damage));
}

cItemColored::cItemColored(int id)
	: cItemBlock(id)
{
	m_pModelBlock = cBlockManager::scBlocks[m_blockID];
}

AtlasSprite* cItemColored::getIconFromDamage(int damage)
{
	return m_pModelBlock->getIcon(0, damage);
}

int cItemColored::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	return m_pModelBlock->getRenderColor(pStack->getItemDamage());
}

cItemLilyPad::cItemLilyPad(int id)
	: ItemLilyPad(id)
	, cItemColored(id)
{
}

int cItemLilyPad::getColorFromItemStack(ItemStackPtr pStack, int par2)
{
	return cBlockManager::scBlocks[BLOCK_ID_WATERLILY]->getRenderColor(pStack->getItemDamage());
}

cItemBlockImp::cItemBlockImp(int id)
	: ItemBlock(id)
	, cItemBlock(id)
{
}

cItemColoredImp::cItemColoredImp(int id, bool flag, const StringArray& names)
	: ItemColored(id, flag, names)
	, cItemColored(id)
{
}

cItemColoredImp::cItemColoredImp(int id, bool flag)
	: ItemColored(id, flag)
	, cItemColored(id)
{
}

cItemColoredImp * cItemColoredImp::setBlockNames(String names[], int len)
{
	ItemColored::setBlockNames(names, len);
	return this;
}

cItemRecord::cItemRecord(int id, const String& name, const String& record, const String& atlas)
	: ItemRecord(id, name, record)
	, cItem(atlas)
{}

void cItemRecord::addInformation(ItemStackPtr pStack, EntityPlayer* pPlayer, StringArray& fills, bool advance)
{
	fills.push_back(getRecordTitle());
}

cItemCompass::cItemCompass(int id, const String& name, const String& atlas)
	: Item(id, name)
	, cItem(atlas)
{
	for (int i = 0; i < COMPASS_FRAME_COUNT; ++i)
		m_sprites[i] = nullptr;
}

void cItemCompass::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < COMPASS_FRAME_COUNT; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite("compass.json", StringUtil::Format("%s_%d", getAtlasName().c_str(), i));
	}
}

cItemClock::cItemClock(int id, const String& name, const String& atlas)
	: Item(id, name)
	, cItem(atlas)
{
	for (int i = 0; i < CLOCK_FRAME_COUNT; ++i)
		m_sprites[i] = nullptr;
}

void cItemClock::registerIcons(TextureAtlasRegister* atlas)
{
	for (int i = 0; i < CLOCK_FRAME_COUNT; ++i)
	{
		m_sprites[i] = atlas->getAtlasSprite("clock.json", StringUtil::Format("%s_%d", getAtlasName().c_str(), i));
	}
}

cItemArrow::cItemArrow(int id, const String& name, const String& atlas, bool isEffect, String effectName,bool isExplosion,String explosionEffectName,float explosionRadius,float explosionDamage, const String& mesh_name)
	: ItemArrow(id, name, isEffect, effectName, isExplosion, explosionEffectName, explosionRadius, explosionDamage)
	, cItem(atlas)
	, m_meshName(mesh_name)
{}


cItemArrow::cItemArrow(int id, const String& name, const String& atlas)
	: ItemArrow(id, name)
	, cItem(atlas)
{}

ItemRenderType cItemArrow::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemArrow::getMeshName() const
{
	return m_meshName;
}

cItemArrowPotion::cItemArrowPotion(int id, const String & name, const String & atlas)
	: ItemArrowPotion(id, name), cItem(atlas)
{
}

cItemKey::cItemKey(int id, const String & name, const String & atlas)
	: ItemKey(id, name)
	, cItem(atlas)
{
}

cItemTeleportScroll::cItemTeleportScroll(int id, const String & name, const String & atlas)
	: ItemTeleportScroll(id, name)
	, cItem(atlas)
{
}

cItemCreateBridge::cItemCreateBridge(int id, const String & name, const String & atlas, const String& mesh_name)
	: ItemCreateBridge(id, name)
	, cItem(atlas)
	, m_meshName(mesh_name)
{
}

ItemRenderType cItemCreateBridge::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemCreateBridge::getMeshName() const
{
	return m_meshName;
}

cItemToolDigger::cItemToolDigger(int id, const String & name, const String & atlas, const String & meshName, ToolMaterial * toolMat)
	: ItemToolDigger(id, name, toolMat)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemToolDigger::getRenderType()
{
	return ItemRenderType::MESH;
}

const String & cItemToolDigger::getMeshName() const
{
	return m_meshName;
}

cItemMedicine::cItemMedicine(int id, const String & name, const String & atlas)
	: ItemMedichine(id, name, 0.0f)
	, cItem(atlas)
{
}

cItemProp::cItemProp(int id, const String& name, const String& atlas, const String& mesh)
	: ItemProp(id, name)
	, cItem(atlas)
	, m_meshName(mesh)
{
}

ItemRenderType cItemProp::getRenderType()
{
	return getMeshName() == StringUtil::BLANK ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String& cItemProp::getMeshName() const
{
	return m_meshName;
}

cItemBanner::cItemBanner(int id, const String& name)
	: ItemBanner(id, name)
	, cItemBlock(id)
{}

ItemRenderType cItemBanner::getRenderType()
{
	return ItemRenderType::BLOCK;
}

cItemAppIcon::cItemAppIcon(int id, const String & name, const String & atlas)
	: ItemAppIcon(id, name)
	, cItem(atlas)
{
}

cItemTransparent::cItemTransparent(int id, const String& name, const String& atlas)
	: ItemTransparent(id, name)
	, cItem(atlas)
{
}

cItemBuild::cItemBuild(int id, const String & name, const String & atlas)
	: ItemBuild(id, name)
	, cItem(atlas)
{
}

cItemHouse::cItemHouse(int id, const String & name, const String & atlas)
	: ItemHouse(id, name)
	, cItem(atlas)
{
}

cRanchSeeds::cRanchSeeds(int id, const String & name, const String & atlas, int blocktype, int soilID)
	: RanchSeeds(id, name, blocktype, soilID)
	, cItem(atlas)
{
}

cItemToolGather::cItemToolGather(int id, const String & name, const String & atlas, const String & meshName, ToolMaterial * toolMat)
	: ItemToolGather(id, name, toolMat)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemToolGather::getRenderType()
{
	return ItemRenderType::MESH;
}

const String & cItemToolGather::getMeshName() const
{
	return m_meshName;
}

cItemStained::cItemStained(int id, const String & name)
	: ItemStained(id, name)
	, cItemBlock(id)
{
}

AtlasSprite * cItemStained::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[m_blockID]->getIcon(0, damage);
}

cItemHandGrenade::cItemHandGrenade(int id, const String & name, const String & atlas, const String& meshName)
	: ItemHandGrenade(id, name)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemHandGrenade::getRenderType()
{
	return ItemRenderType::MESH;
}

cItemTemplate::cItemTemplate(int id, const String & name, const String & atlas, const String& meshName)
	: ItemTemplate(id, name)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemTemplate::getRenderType()
{
	return getMeshName().empty() ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String & cItemTemplate::getMeshName() const
{
	return m_meshName;
}

cItemDecoration::cItemDecoration(int id, const String & name, const String & atlas, const String& meshName)
	: ItemDecoration(id, name)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

String cItemDecoration::getUnlocalizedName(ItemStackPtr pStack)
{
	Decoration* pSetting = DecorationSetting::findItemByItemId(itemID);
	String name = pSetting && pSetting->name.length() > 0 ? pSetting->name : Item::getUnlocalizedName(pStack);
	return StringUtil::Replace(name, ".name", "");
}

AtlasSprite * cItemDecoration::getIconFromDamage(int damage)
{
	if (atlasName.empty())
	{
		if (auto pSetting = DecorationSetting::findItemByItemId(itemID))
		{
			m_meshName = pSetting->meshName;
			atlasName = pSetting->icon;
			registerIcons(TextureAtlasRegister::Instance());
		}
	}
	return itemIcon;
}

ItemRenderType cItemDecoration::getRenderType()
{
	return getMeshName().empty() ? cItem::getRenderType() : ItemRenderType::MESH;
}

const String & cItemDecoration::getMeshName() const
{
	if (m_meshName.empty())
	{
		if (auto pSetting = DecorationSetting::findItemByItemId(itemID))
		{
			return pSetting->meshName;
		}
	}
	return m_meshName;
}

cItemCustom::cItemCustom(int id, const String & name, const String & atlas, const String & meshName)
	: ItemCustom(id, name)
	, cItem(atlas)
	, m_meshName(meshName)
{
}

ItemRenderType cItemCustom::getRenderType()
{
	if (m_meshName.empty())
		return cItem::getRenderType();
	else
		return ItemRenderType::MESH;
}

const String & cItemCustom::getMeshName() const
{
	return m_meshName;
}

bool cItemCustom::canLongTouchUse()
{
	return SkillItemSetting::isLongTouchItem(itemID);
}

ItemStackPtr cItemCustom::onItemRightClick(ItemStackPtr pStack, World * pWorld, EntityPlayer * pPlayer)
{
	CastSkillEvent::emit();
	return pStack;
}

cItemFlower::cItemFlower(int id, const String & name)
	: ItemFlower(id, name)
	, cItemBlock(id)
{
}

AtlasSprite * cItemFlower::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[m_blockID]->getIcon(0, damage);
}

cItemStone::cItemStone(int id, const String & name)
	: ItemStone(id, name)
	, cItemBlock(id)
{
}

AtlasSprite * cItemStone::getIconFromDamage(int damage)
{
	return cBlockManager::scBlocks[m_blockID]->getIcon(0, damage);
}

}
