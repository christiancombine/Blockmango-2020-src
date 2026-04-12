#include "Item.h"
#include "Items.h"
#include "ItemStack.h"
#include "Potion.h"
#include "PotionManager.h"

#include "Block/BM_Material.h"
#include "Block/Block.h"
#include "Block/BlockManager.h"
#include "Entity/EntityPlayer.h"
#include "World/RayTracyResult.h"
#include "World/World.h"
#include "Setting/CustomArmorSetting.h"

namespace BLOCKMAN
{

Random* Item::itemRand = NULL;
UUID Item::ATTACK_DAMAGE_MODIFIER = UUID::fromString("CB3F55D3-645C-4F38-A497-9C13A33DB5CF");
Item* Item::itemsList[MAX_ITEM_COUNT];
map<String, int>::type Item::itemNameToIdMap;
Item* Item::shovelIron			= NULL;
Item* Item::pickaxeIron			= NULL;
Item* Item::axeIron				= NULL; 
Item* Item::flintAndSteel		= NULL;
Item* Item::appleRed			= NULL;
ItemBow* Item::bow				= NULL;
Item* Item::arrow				= NULL;
Item* Item::coal				= NULL; 
Item* Item::diamond				= NULL;
Item* Item::ingotIron			= NULL;
Item* Item::ingotGold			= NULL;
Item* Item::swordIron			= NULL;
Item* Item::swordWood			= NULL;
Item* Item::shovelWood			= NULL;
Item* Item::pickaxeWood			= NULL;
Item* Item::axeWood				= NULL;
Item* Item::swordStone			= NULL;
Item* Item::shovelStone			= NULL;
Item* Item::pickaxeStone		= NULL;
Item* Item::axeStone			= NULL;
Item* Item::swordDiamond		= NULL;
Item* Item::shovelDiamond		= NULL;
Item* Item::pickaxeDiamond		= NULL;
Item* Item::axeDiamond			= NULL;
Item* Item::stick				= NULL;
Item* Item::bowlEmpty			= NULL;
Item* Item::bowlSoup			= NULL;
Item* Item::swordGold			= NULL;
Item* Item::shovelGold			= NULL;
Item* Item::pickaxeGold			= NULL;
Item* Item::axeGold				= NULL;
Item* Item::silk				= NULL;
Item* Item::feather				= NULL;
Item* Item::gunpowder			= NULL;
Item* Item::hoeWood				= NULL;
Item* Item::hoeStone			= NULL;
Item* Item::hoeIron				= NULL;
Item* Item::hoeDiamond			= NULL;
Item* Item::hoeGold				= NULL;
Item* Item::seeds				= NULL;
Item* Item::wheat				= NULL;
Item* Item::bread				= NULL;
ItemArmor* Item::helmetLeather	= NULL;
ItemArmor* Item::plateLeather	= NULL;
ItemArmor* Item::legsLeather	= NULL;
ItemArmor* Item::bootsLeather	= NULL;
ItemArmor* Item::helmetChain	= NULL;
ItemArmor* Item::plateChain		= NULL;
ItemArmor* Item::legsChain		= NULL;
ItemArmor* Item::bootsChain		= NULL;
ItemArmor* Item::helmetIron		= NULL;
ItemArmor* Item::plateIron		= NULL;
ItemArmor* Item::legsIron		= NULL;
ItemArmor* Item::bootsIron		= NULL;
ItemArmor* Item::helmetDiamond	= NULL;
ItemArmor* Item::plateDiamond	= NULL;
ItemArmor* Item::legsDiamond	= NULL;
ItemArmor* Item::bootsDiamond	= NULL;
ItemArmor* Item::helmetGold		= NULL;
ItemArmor* Item::plateGold		= NULL;
ItemArmor* Item::legsGold		= NULL;
ItemArmor* Item::bootsGold		= NULL;
Item* Item::flint				= NULL;
Item* Item::porkRaw				= NULL;
Item* Item::porkCooked			= NULL;
Item* Item::painting			= NULL;
Item* Item::appleGold			= NULL;
Item* Item::sign				= NULL;
Item* Item::doorWood			= NULL;
Item* Item::bucketEmpty			= NULL;
Item* Item::bucketWater			= NULL;
Item* Item::bucketLava			= NULL;
Item* Item::blockmanEmpty		= NULL;
Item* Item::saddle				= NULL;
Item* Item::doorIron			= NULL;
Item* Item::redstone			= NULL;
Item* Item::snowball			= NULL;
Item* Item::boat				= NULL; 
Item* Item::leather				= NULL;
Item* Item::bucketMilk			= NULL;
Item* Item::brick				= NULL;
Item* Item::clay				= NULL;
Item* Item::reed				= NULL;
Item* Item::paper				= NULL;
Item* Item::book				= NULL;
Item* Item::slimeBall			= NULL;
Item* Item::blockmanCrate		= NULL;
Item* Item::blockmanPowered		= NULL;
Item* Item::egg					= NULL;
Item* Item::compass				= NULL; 
ItemFishingRod* Item::fishingRod = NULL;
Item* Item::pocketSundial		= NULL;
Item* Item::glowstone			= NULL;
Item* Item::fishRaw				= NULL;
Item* Item::fishCooked			= NULL;
Item* Item::dyePowder			= NULL;
Item* Item::bone				= NULL;
Item* Item::sugar				= NULL;
Item* Item::cake				= NULL;
Item* Item::bed					= NULL;
Item* Item::redstoneRepeater	= NULL;
Item* Item::cookie				= NULL;
ItemMap* Item::imap				= NULL;
ItemShears* Item::shears		= NULL;
Item* Item::melon				= NULL;
Item* Item::pumpkinSeeds		= NULL;
Item* Item::melonSeeds			= NULL;
Item* Item::beefRaw				= NULL;
Item* Item::beefCooked			= NULL;
Item* Item::chickenRaw			= NULL;
Item* Item::chickenCooked		= NULL;
Item* Item::rottenFlesh			= NULL;
Item* Item::enderPearl			= NULL;
Item* Item::blazeRod			= NULL;
Item* Item::ghastTear			= NULL;
Item* Item::goldNugget			= NULL;
Item* Item::netherStalkSeeds	= NULL;
ItemPotion* Item::potion		= NULL;
Item* Item::glassBottle			= NULL;
Item* Item::spiderEye			= NULL;
Item* Item::fermentedSpiderEye	= NULL;
Item* Item::blazePowder			= NULL; 
Item* Item::magmaCream			= NULL; 
Item* Item::brewingStand		= NULL;
Item* Item::cauldron			= NULL;
Item* Item::eyeOfEnder			= NULL;
Item* Item::speckledMelon		= NULL;
Item* Item::monsterPlacer		= NULL;
Item* Item::expBottle			= NULL;
Item* Item::fireballCharge		= NULL;
Item* Item::writableBook		= NULL;
Item* Item::writtenBook			= NULL;
Item* Item::emerald				= NULL;
Item* Item::itemFrame			= NULL;
Item* Item::flowerPot			= NULL;
Item* Item::carrot				= NULL;
Item* Item::potato				= NULL;
Item* Item::bakedPotato			= NULL;
Item* Item::poisonousPotato		= NULL;
ItemEmptyMap* Item::emptyMap	= NULL;
Item* Item::goldenCarrot		= NULL;
Item* Item::skull				= NULL;
Item* Item::carrotOnAStick		= NULL;
Item* Item::netherStar			= NULL;
Item* Item::pumpkinPie			= NULL;
Item* Item::firework			= NULL;
Item* Item::fireworkCharge		= NULL;
ItemEnchantedBook* Item::enchantedBook = NULL;
Item* Item::comparator			= NULL;
Item* Item::netherrackBrick		= NULL;
Item* Item::netherQuartz		= NULL;
Item* Item::blockmanTnt			= NULL;
Item* Item::blockmanHopper		= NULL;
Item* Item::prismarine_shard	= NULL;
Item* Item::prismarine_crystals = NULL;
Item* Item::rabbitCooked		= NULL;
Item* Item::iron_horse_armor	= NULL;
Item* Item::golden_horse_armor	= NULL;
Item* Item::diamond_horse_armor	= NULL;
Item* Item::lead				= NULL;
Item* Item::name_tag			= NULL;
ItemArrowPotion* Item::arrowPoison			= NULL;
ItemArrowPotion* Item::arrowLongPoison		= NULL;
ItemArrowPotion* Item::arrowStrongPoison	= NULL;
ItemGrenade* Item::grenade		= NULL;
Item* Item::record13			= NULL;
Item* Item::recordCat			= NULL;
Item* Item::recordBlocks		= NULL;
Item* Item::recordChirp			= NULL;
Item* Item::recordFar			= NULL;
Item* Item::recordMall			= NULL;
Item* Item::recordMellohi		= NULL;
Item* Item::recordStal			= NULL;
Item* Item::recordStrad			= NULL;
Item* Item::recordWard			= NULL;
Item* Item::record11			= NULL;
Item* Item::recordWait			= NULL;
ItemHandcuffs*  Item::handcuffs		= NULL;
ItemKey*		Item::key_01 = NULL;
ItemKey*		Item::key_02 = NULL;
ItemKey*		Item::key_03 = NULL;
ItemKey*		Item::key_04 = NULL;
ItemKey*		Item::key_05 = NULL;
ItemKey*		Item::key_06 = NULL;
ItemKey*		Item::key_07 = NULL;
ItemKey*		Item::key_08 = NULL;
ItemKey*		Item::key_09 = NULL;
ItemKey*		Item::key_10 = NULL;
ItemTeleportScroll* Item::tpScroll_01 = NULL;
ItemTeleportScroll* Item::tpScroll_02 = NULL;
ItemTeleportScroll* Item::tpScroll_03 = NULL;
ItemTeleportScroll* Item::tpScroll_04 = NULL;
ItemTeleportScroll* Item::tpScroll_05 = NULL;
ItemCreateBridge*   Item::createBridge_01 = NULL;
ItemCreateBridge*   Item::createBridge_02 = NULL;
ItemCreateBridge*   Item::createBridge_03 = NULL;
ItemCreateBridge*   Item::createBridge_04 = NULL;
ItemCreateBridge*   Item::createBridge_05 = NULL;
ItemToolDigger*	    Item::shovelRenovation = NULL;
ItemToolDigger*	    Item::spatula = NULL;
ItemToolDigger*	    Item::shovelToy = NULL;
ItemToolDigger*	    Item::digBucket = NULL;
ItemToolDigger*	    Item::digShovelIron = NULL;
ItemToolDigger*	    Item::shovelRake = NULL;
ItemToolDigger*	    Item::shovelMarching = NULL;
ItemToolDigger*	    Item::shovelBigScooper = NULL;
ItemToolDigger*	    Item::shovelJackHammer = NULL;
ItemToolDigger*	    Item::shovelDrill = NULL;
ItemToolDigger*	    Item::shovelEngineerGold = NULL;

ItemProp*    Item::goldHeart = NULL;
ItemProp*    Item::goldShoes = NULL;
ItemProp*    Item::goldArrow = NULL;
ItemProp*    Item::questionMark = NULL;
ItemProp*    Item::purple_diamond = NULL;

ItemMedichine*	    Item::medichinePack = NULL;
ItemMedichine*	    Item::medichinePotion = NULL;
Item*							Item::seedsOfNewCrops_01 = NULL;
Item*							Item::seedsOfNewCrops_02 = NULL;
Item*							Item::seedsOfNewCrops_03 = NULL;
Item*							Item::seedsOfNewCrops_04 = NULL;
Item*							Item::seedsOfNewCrops_05 = NULL;
Item* Item::tomato = NULL;
Item* Item::cabbage = NULL;
Item* Item::pea = NULL;
Item* Item::flareBomb = NULL;
Item* Item::rescue = NULL;
Item* Item::goldApple = NULL;
Item* Item::currencyMoney = NULL;
Item* Item::doorBirch = NULL;
Item* Item::carrotIron = NULL;
Item* Item::cornIron = NULL;
Item* Item::cottonIron = NULL;
Item* Item::potatoIron = NULL;
Item* Item::silkIron = NULL;
Item* Item::strawberryIron = NULL;
Item* Item::tomatoIron = NULL;
Item* Item::wheatIron = NULL;

ItemProp*    Item::propGold = NULL;
ItemProp*    Item::propExp = NULL;

ItemProp* Item::itemThrowableSkillProp_01 = NULL;
ItemProp* Item::itemThrowableSkillProp_02 = NULL;
ItemProp* Item::itemThrowableSkillProp_03 = NULL;
ItemProp* Item::itemThrowableSkillProp_04 = NULL;
ItemProp* Item::itemThrowableSkillProp_05 = NULL;
ItemProp* Item::itemThrowableSkillProp_06 = NULL;
ItemProp* Item::itemThrowableSkillProp_07 = NULL;
ItemProp* Item::itemThrowableSkillProp_08 = NULL;
ItemProp* Item::itemThrowableSkillProp_09 = NULL;
ItemProp* Item::itemThrowableSkillProp_10 = NULL;

ItemProp* Item::itemEatSkillProp_01 = NULL;
ItemProp* Item::itemEatSkillProp_02 = NULL;
ItemProp* Item::itemEatSkillProp_03 = NULL;
ItemProp* Item::itemEatSkillProp_04 = NULL;
ItemProp* Item::itemEatSkillProp_05 = NULL;
ItemProp* Item::itemEatSkillProp_06 = NULL;
ItemProp* Item::itemEatSkillProp_07 = NULL;
ItemProp* Item::itemEatSkillProp_08 = NULL;
ItemProp* Item::itemEatSkillProp_09 = NULL;
ItemProp* Item::itemEatSkillProp_10 = NULL;

ItemProp* Item::gemstone = NULL;

ItemProp* Item::equipmentSet1 = NULL;
ItemProp* Item::equipmentSet2 = NULL;
ItemProp* Item::equipmentSet3 = NULL;
ItemProp* Item::equipmentSet4 = NULL;
ItemProp* Item::equipmentSet5 = NULL;

Item* Item::purplePickaxe = NULL;
ItemGrenade* Item::smokeGrenade = NULL;

Item* Item::tremorGrenade = NULL;
Item* Item::frozenBall = NULL;

ItemProp* Item::baseballStick = NULL;
ItemProp* Item::spikeStick = NULL;
ItemProp* Item::stonePillar = NULL;

ItemAppIcon* Item::shortenRebirthTimeItem = NULL;
ItemAppIcon* Item::increaseHpItem = NULL;
ItemAppIcon* Item::diamondDefensePackItem = NULL;
Item* Item::grenade_skill_01 = NULL;
Item* Item::grenade_skill_02 = NULL;
Item* Item::colorful_grenade_skill_01 = NULL;
Item* Item::colorful_grenade_skill_02 = NULL;

RanchSeeds* Item::ranchSeed_100001 = NULL;
RanchSeeds* Item::ranchSeed_100002 = NULL;
RanchSeeds* Item::ranchSeed_100003 = NULL;
RanchSeeds* Item::ranchSeed_100004 = NULL;
RanchSeeds* Item::ranchSeed_100005 = NULL;
RanchSeeds* Item::ranchSeed_100006 = NULL;
RanchSeeds* Item::ranchSeed_100007 = NULL;
RanchSeeds* Item::ranchSeed_100008 = NULL;
RanchSeeds* Item::ranchSeed_100009 = NULL;
RanchSeeds* Item::ranchSeed_100010 = NULL;
RanchSeeds* Item::ranchSeed_100011 = NULL;
RanchSeeds* Item::ranchSeed_100012 = NULL;
RanchSeeds* Item::ranchSeed_100013 = NULL;
RanchSeeds* Item::ranchSeed_100014 = NULL;
RanchSeeds* Item::ranchSeed_100015 = NULL;
RanchSeeds* Item::ranchSeed_100016 = NULL;
RanchSeeds* Item::ranchSeed_100017 = NULL;

ItemProp* Item::itemThrowableSkillProp_11 = NULL;
ItemProp* Item::itemThrowableSkillProp_12 = NULL;
ItemProp* Item::itemThrowableSkillProp_13 = NULL;
ItemProp* Item::itemThrowableSkillProp_14 = NULL;
ItemProp* Item::itemThrowableSkillProp_15 = NULL;
ItemProp* Item::itemThrowableSkillProp_16 = NULL;
ItemProp* Item::itemThrowableSkillProp_17 = NULL;
ItemProp* Item::itemThrowableSkillProp_18 = NULL;
ItemProp* Item::itemThrowableSkillProp_19 = NULL;
ItemProp* Item::itemThrowableSkillProp_20 = NULL;

ItemProp* Item::itemThrowableSkillProp_04_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_05_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_06_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_07_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_08_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_09_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_11_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_12_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_13_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_14_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_15_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_16_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_17_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_18_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_19_plus = NULL;
ItemProp* Item::itemThrowableSkillProp_20_plus = NULL;

ItemBow* Item::bow_red = NULL;
ItemBow* Item::bow_black = NULL;
Item* Item::redArrow = NULL;
Item* Item::blackArrow = NULL;

ItemProp* Item::weapon_01 = NULL;
ItemProp* Item::weapon_02 = NULL;
ItemProp* Item::weapon_04 = NULL;
ItemProp* Item::weapon_05 = NULL;
ItemProp* Item::weapon_06 = NULL;
ItemProp* Item::weapon_07 = NULL;
ItemProp* Item::weapon_08 = NULL;
ItemProp* Item::weapon_09 = NULL;
ItemProp* Item::weapon_10 = NULL;
ItemProp* Item::weapon_11 = NULL;
ItemProp* Item::weapon_12 = NULL;
ItemProp* Item::weapon_13 = NULL;
ItemProp* Item::weapon_14 = NULL;
ItemProp* Item::weapon_15 = NULL;
ItemProp* Item::weapon_16 = NULL;
ItemProp* Item::weapon_17 = NULL;
ItemProp* Item::weapon_18 = NULL;

ItemHandGrenade* Item::handGrenade = NULL;

ItemTransparent* Item::change_animal_skill_01 = NULL;
ItemTransparent* Item::change_animal_skill_02 = NULL;
ItemTransparent* Item::speed_up_skill_01 = NULL;
ItemTransparent* Item::speed_up_skill_02 = NULL;

ItemProp* Item::pixel_gun_prop_1 = NULL;
ItemProp* Item::pixel_gun_prop_2 = NULL;
ItemProp* Item::pixel_gun_prop_3 = NULL;
ItemProp* Item::pixel_gun_prop_4 = NULL;
ItemProp* Item::pixel_gun_prop_5 = NULL;
ItemProp* Item::pixel_gun_prop_6 = NULL;
ItemProp* Item::pixel_gun_prop_7 = NULL;
ItemProp* Item::pixel_gun_prop_8 = NULL;
ItemProp* Item::pixel_gun_prop_9 = NULL;
ItemProp* Item::pixel_gun_prop_10 = NULL;
ItemProp* Item::pixel_gun_prop_11 = NULL;
ItemProp* Item::pixel_gun_prop_12 = NULL;
ItemProp* Item::pixel_gun_prop_13 = NULL;
ItemProp* Item::pixel_gun_prop_14 = NULL;
ItemProp* Item::pixel_gun_prop_15 = NULL;
ItemProp* Item::pixel_gun_prop_16 = NULL;
ItemProp* Item::pixel_gun_prop_17 = NULL;
ItemProp* Item::pixel_gun_prop_18 = NULL;
ItemProp* Item::pixel_gun_prop_19 = NULL;
ItemProp* Item::pixel_gun_prop_20 = NULL;
ItemProp* Item::pixel_gun_prop_21 = NULL;
ItemProp* Item::pixel_gun_prop_22 = NULL;
ItemProp* Item::pixel_gun_prop_23 = NULL;
ItemProp* Item::pixel_gun_prop_24 = NULL;
ItemProp* Item::pixel_gun_prop_25 = NULL;
ItemProp* Item::pixel_gun_prop_26 = NULL;
ItemProp* Item::pixel_gun_prop_27 = NULL;
ItemProp* Item::pixel_gun_prop_28 = NULL;
ItemProp* Item::pixel_gun_prop_29 = NULL;
ItemProp* Item::pixel_gun_prop_30 = NULL;

Item* Item::bow_01 = NULL;
Item* Item::bow_02 = NULL;
Item* Item::bow_03 = NULL;

Item* Item::book_1 = NULL;
Item* Item::book_2 = NULL;
Item* Item::book_3 = NULL;
Item* Item::book_4 = NULL;
Item* Item::book_5 = NULL;
Item* Item::book_6 = NULL;
Item* Item::book_7 = NULL;
Item* Item::book_8 = NULL;

ItemDecoration* Item::decoration_01 = NULL;
ItemDecoration* Item::decoration_02 = NULL;
ItemDecoration* Item::decoration_03 = NULL;
ItemDecoration* Item::decoration_04 = NULL;
ItemDecoration* Item::decoration_05 = NULL;
ItemDecoration* Item::decoration_06 = NULL;
ItemDecoration* Item::decoration_07 = NULL;
ItemDecoration* Item::decoration_08 = NULL;
ItemDecoration* Item::decoration_09 = NULL;
ItemDecoration* Item::decoration_10 = NULL;
ItemDecoration* Item::decoration_11 = NULL;
ItemDecoration* Item::decoration_12 = NULL;
ItemDecoration* Item::decoration_13 = NULL;
ItemDecoration* Item::decoration_14 = NULL;
ItemDecoration* Item::decoration_15 = NULL;
ItemDecoration* Item::decoration_16 = NULL;
ItemDecoration* Item::decoration_17 = NULL;
ItemDecoration* Item::decoration_18 = NULL;
ItemDecoration* Item::decoration_19 = NULL;
ItemDecoration* Item::decoration_20 = NULL;
ItemDecoration* Item::decoration_21 = NULL;
ItemDecoration* Item::decoration_22 = NULL;
ItemDecoration* Item::decoration_23 = NULL;
ItemDecoration* Item::decoration_24 = NULL;
ItemDecoration* Item::decoration_25 = NULL;
ItemDecoration* Item::decoration_26 = NULL;
ItemDecoration* Item::decoration_27 = NULL;
ItemDecoration* Item::decoration_28 = NULL;
ItemDecoration* Item::decoration_29 = NULL;
ItemDecoration* Item::decoration_30 = NULL;
ItemDecoration* Item::decoration_31 = NULL;
ItemDecoration* Item::decoration_32 = NULL;
ItemDecoration* Item::decoration_33 = NULL;
ItemDecoration* Item::decoration_34 = NULL;
ItemDecoration* Item::decoration_35 = NULL;
ItemDecoration* Item::decoration_36 = NULL;
ItemDecoration* Item::decoration_37 = NULL;
ItemDecoration* Item::decoration_38 = NULL;
ItemDecoration* Item::decoration_39 = NULL;
ItemDecoration* Item::decoration_40 = NULL;
ItemDecoration* Item::decoration_41 = NULL;
ItemDecoration* Item::decoration_42 = NULL;
ItemDecoration* Item::decoration_43 = NULL;
ItemDecoration* Item::decoration_44 = NULL;
ItemDecoration* Item::decoration_45 = NULL;
ItemDecoration* Item::decoration_46 = NULL;
ItemDecoration* Item::decoration_47 = NULL;
ItemDecoration* Item::decoration_48 = NULL;
ItemDecoration* Item::decoration_49 = NULL;
ItemDecoration* Item::decoration_50 = NULL;
ItemDecoration* Item::decoration_51 = NULL;
ItemDecoration* Item::decoration_52 = NULL;
ItemDecoration* Item::decoration_53 = NULL;
ItemDecoration* Item::decoration_54 = NULL;
ItemDecoration* Item::decoration_55 = NULL;
ItemDecoration* Item::decoration_56 = NULL;
ItemDecoration* Item::decoration_57 = NULL;
ItemDecoration* Item::decoration_58 = NULL;
ItemDecoration* Item::decoration_59 = NULL;
ItemDecoration* Item::decoration_60 = NULL;
ItemDecoration* Item::decoration_61 = NULL;
ItemDecoration* Item::decoration_62 = NULL;
ItemDecoration* Item::decoration_63 = NULL;
ItemDecoration* Item::decoration_64 = NULL;
ItemDecoration* Item::decoration_65 = NULL;
ItemDecoration* Item::decoration_66 = NULL;
ItemDecoration* Item::decoration_67 = NULL;
ItemDecoration* Item::decoration_68 = NULL;
ItemDecoration* Item::decoration_69 = NULL;
ItemDecoration* Item::decoration_70 = NULL;
ItemDecoration* Item::decoration_71 = NULL;
ItemDecoration* Item::decoration_72 = NULL;
ItemDecoration* Item::decoration_73 = NULL;
ItemDecoration* Item::decoration_74 = NULL;
ItemDecoration* Item::decoration_75 = NULL;
ItemDecoration* Item::decoration_76 = NULL;
ItemDecoration* Item::decoration_77 = NULL;
ItemDecoration* Item::decoration_78 = NULL;
ItemDecoration* Item::decoration_79 = NULL;
ItemDecoration* Item::decoration_80 = NULL;
ItemDecoration* Item::decoration_81 = NULL;
ItemDecoration* Item::decoration_82 = NULL;
ItemDecoration* Item::decoration_83 = NULL;
ItemDecoration* Item::decoration_84 = NULL;
ItemDecoration* Item::decoration_85 = NULL;
ItemDecoration* Item::decoration_86 = NULL;
ItemDecoration* Item::decoration_87 = NULL;
ItemDecoration* Item::decoration_88 = NULL;
ItemDecoration* Item::decoration_89 = NULL;
ItemDecoration* Item::decoration_90 = NULL;
ItemDecoration* Item::decoration_91 = NULL;
ItemDecoration* Item::decoration_92 = NULL;
ItemDecoration* Item::decoration_93 = NULL;
ItemDecoration* Item::decoration_94 = NULL;
ItemDecoration* Item::decoration_95 = NULL;
ItemDecoration* Item::decoration_96 = NULL;
ItemDecoration* Item::decoration_97 = NULL;
ItemDecoration* Item::decoration_98 = NULL;
ItemDecoration* Item::decoration_99 = NULL;
ItemDecoration* Item::decoration_100 = NULL;

void Item::initialize()
{
	itemRand = LordNew Random();
	memset(itemsList, 0, sizeof(Item*) * MAX_ITEM_COUNT);
	shovelIron		= LordNew ItemSpade(0,"shovelIron", ToolMaterial::IRON);
	pickaxeIron		= LordNew ItemPickaxe(1, "pickaxeIron", ToolMaterial::IRON);
	axeIron			= LordNew ItemAxe(2, "hatchetIron", ToolMaterial::IRON);
	flintAndSteel	= LordNew ItemFlintAndSteel(3, "flintAndSteel");
	appleRed		= LordNew ItemFood(4, "apple", 4, 0.3F, false);
	bow				= LordNew ItemBow(5, "bow");
	arrow			= LordNew ItemArrow(6, "arrow");
	coal			= LordNew ItemCoal(7, "coal");
	diamond			= LordNew Item(8, "diamond");
	ingotIron		= LordNew Item(9, "ingotIron");
	ingotGold		= LordNew Item(10, "ingotGold"); 
	swordIron		= LordNew ItemSword(11, "swordIron", ToolMaterial::IRON);
	swordWood		= LordNew ItemSword(12, "swordWood", ToolMaterial::WOOD);
	shovelWood		= LordNew ItemSpade(13, "shovelWood", ToolMaterial::WOOD);
	pickaxeWood		= LordNew ItemPickaxe(14, "pickaxeWood", ToolMaterial::WOOD);
	axeWood			= LordNew ItemAxe(15, "hatchetWood", ToolMaterial::WOOD);
	swordStone		= LordNew ItemSword(16, "swordStone", ToolMaterial::STONE);
	shovelStone		= LordNew ItemSpade(17, "shovelStone", ToolMaterial::STONE);
	pickaxeStone	= LordNew ItemPickaxe(18, "pickaxeStone", ToolMaterial::STONE);
	axeStone		= LordNew ItemAxe(19, "hatchetStone", ToolMaterial::STONE);
	swordDiamond	= LordNew ItemSword(20, "swordDiamond", ToolMaterial::EMERALD);
	shovelDiamond	= LordNew ItemSpade(21, "shovelDiamond", ToolMaterial::EMERALD); 
	pickaxeDiamond	= LordNew ItemPickaxe(22, "pickaxeDiamond", ToolMaterial::EMERALD);
	axeDiamond		= LordNew ItemAxe(23, "hatchetDiamond", ToolMaterial::EMERALD);
	stick			= LordNew Item(24, "stick");
	bowlEmpty		= LordNew Item(25, "bowl");
	bowlSoup		= LordNew ItemSoup(26, "mushroomStew", 6);
	swordGold		= LordNew ItemSword(27, "swordGold", ToolMaterial::GOLD); 
	shovelGold		= LordNew ItemSpade(28, "shovelGold", ToolMaterial::GOLD);
	pickaxeGold		= LordNew ItemPickaxe(29, "pickaxeGold", ToolMaterial::GOLD);
	axeGold			= LordNew ItemAxe(30, "hatchetGold", ToolMaterial::GOLD); 
	silk			= LordNew ItemReed(31, "string", BLOCK_ID_TRIP_WIRE); 
	feather			= LordNew Item(32, "feather");
	gunpowder		= LordNew Item(33, "sulphur"); gunpowder->setPotionEffect(PotionHelper::gunpowderEffect); 
	hoeWood			= LordNew ItemHoe(34, "hoeWood", ToolMaterial::WOOD); 
	hoeStone		= LordNew ItemHoe(35, "hoeStone", ToolMaterial::STONE); 
	hoeIron			= LordNew ItemHoe(36, "hoeIron", ToolMaterial::IRON);
	hoeDiamond		= LordNew ItemHoe(37, "hoeDiamond", ToolMaterial::EMERALD);
	hoeGold			= LordNew ItemHoe(38, "hoeGold", ToolMaterial::GOLD);
	seeds			= LordNew ItemSeeds(39, "seeds", BLOCK_ID_CROPS, BLOCK_ID_TILLED_FIELD);
	wheat			= LordNew Item(40, "wheat"); 
	bread			= LordNew ItemFood(41, "bread", 5, 0.6F, false); 
	helmetLeather	= LordNew ItemArmor(42, "helmetCloth", ArmorMaterial::CLOTH, 0);
	plateLeather	= LordNew ItemArmor(43, "chestplateCloth", ArmorMaterial::CLOTH, 1);
	legsLeather		= LordNew ItemArmor(44, "leggingsCloth", ArmorMaterial::CLOTH, 2);
	bootsLeather	= LordNew ItemArmor(45, "bootsCloth", ArmorMaterial::CLOTH, 3);
	helmetChain		= LordNew ItemArmor(46, "helmetChain", ArmorMaterial::CHAIN, 0);
	plateChain		= LordNew ItemArmor(47, "chestplateChain", ArmorMaterial::CHAIN, 1);
	legsChain		= LordNew ItemArmor(48, "leggingsChain", ArmorMaterial::CHAIN, 2);
	bootsChain		= LordNew ItemArmor(49, "bootsChain", ArmorMaterial::CHAIN, 3);
	helmetIron		= LordNew ItemArmor(50, "helmetIron", ArmorMaterial::IRON, 0);
	plateIron		= LordNew ItemArmor(51, "chestplateIron", ArmorMaterial::IRON, 1);
	legsIron		= LordNew ItemArmor(52, "leggingsIron", ArmorMaterial::IRON, 2);
	bootsIron		= LordNew ItemArmor(53, "bootsIron", ArmorMaterial::IRON, 3);
	helmetDiamond	= LordNew ItemArmor(54, "helmetDiamond", ArmorMaterial::DIAMOND, 0);
	plateDiamond	= LordNew ItemArmor(55, "chestplateDiamond", ArmorMaterial::DIAMOND, 1);
	legsDiamond		= LordNew ItemArmor(56, "leggingsDiamond", ArmorMaterial::DIAMOND, 2); 
	bootsDiamond	= LordNew ItemArmor(57, "bootsDiamond", ArmorMaterial::DIAMOND, 3);
	helmetGold		= LordNew ItemArmor(58, "helmetGold", ArmorMaterial::GOLD, 0);
	plateGold		= LordNew ItemArmor(59, "chestplateGold", ArmorMaterial::GOLD, 1);
	legsGold		= LordNew ItemArmor(60, "leggingsGold", ArmorMaterial::GOLD, 2);
	bootsGold		= LordNew ItemArmor(61, "bootsGold", ArmorMaterial::GOLD, 3);
	flint			= LordNew Item(62, "flint");
	porkRaw			= LordNew ItemFood(63, "porkchopRaw", 3, 0.3F, true);
	porkCooked		= LordNew ItemFood(64, "porkchopCooked", 8, 0.8F, true);
	painting		= LordNew ItemHangingEntity(65, "painting", 1);
	appleGold		= LordNew ItemAppleGold(66, "appleGold", 4, 1.2F, false); ((ItemFood*)appleGold)->setAlwaysEdible(); ((ItemFood*)appleGold)->setPotionEffect(Potion::regeneration->getId(), 5, 1, 1.0F);
	sign			= LordNew ItemSign(67, "sign");
	doorWood		= LordNew ItemDoor(68, "doorWood", BM_Material::BM_MAT_wood);
	bucketEmpty		= LordNew ItemBucket(69, "bucket", 0); bucketEmpty->setMaxStackSize(16); 
	bucketWater		= LordNew ItemBucket(70, "bucketWater", BLOCK_ID_WATERMOVING); bucketWater->setContainerItem(bucketEmpty);
	bucketLava		= LordNew ItemBucket(71, "bucketLava", BLOCK_ID_LAVAMOVING); bucketWater->setContainerItem(bucketEmpty);
	blockmanEmpty	= LordNew ItemBlockman(72, "blockman", 0);
	saddle			= LordNew ItemSaddle(73, "saddle"); saddle->setMaxStackSize(1);
	doorIron		= LordNew ItemDoor(74, "doorIron", BM_Material::BM_MAT_iron);
	redstone		= LordNew ItemRedstone(75, "redstone"); redstone->setPotionEffect(PotionHelper::redstoneEffect);
	snowball		= LordNew ItemSnowball(76, "snowball"); snowball->setMaxStackSize(64);
	boat			= LordNew ItemBoat(77, "boat"); 
	leather			= LordNew Item(78, "leather"); 
	bucketMilk		= LordNew ItemBucketMilk(79, "milk"); bucketMilk->setContainerItem(bucketEmpty);
	brick			= LordNew Item(80, "brick"); 
	clay			= LordNew Item(81, "clay");
	reed			= LordNew ItemReed(82, "reeds", BLOCK_ID_REED);
	paper			= LordNew Item(83, "paper");
	book			= LordNew ItemBook(84, "book");
	slimeBall		= LordNew Item(85, "slimeball"); 
	blockmanCrate	= LordNew ItemBlockman(86, "blockmanChest", 1);
	blockmanPowered = LordNew ItemBlockman(87, "blockmanFurnace", 2);
	egg				= LordNew ItemEgg(88, "egg");
	compass			= LordNew Item(89, "compass");
	fishingRod		= LordNew ItemFishingRod(90, "fishingRod");
	pocketSundial	= LordNew Item(91, "clock");
	glowstone		= LordNew Item(92, "yellowDust"); glowstone->setPotionEffect(PotionHelper::glowstoneEffect);
	fishRaw			= LordNew ItemFood(93, "fishRaw", 2, 0.3F, false);
	fishCooked		= LordNew ItemFood(94, "fishCooked", 5, 0.6F, false);
	dyePowder		= LordNew ItemDye(95, "dyePowder");
	bone			= LordNew Item(96, "bone");
	sugar			= LordNew Item(97, "sugar"); sugar->setPotionEffect(PotionHelper::sugarEffect); 
	cake			= LordNew ItemReed(98, "cake", BLOCK_ID_CAKE); cake->setMaxStackSize(1);
	bed				= LordNew ItemBed(99, "bed"); bed->setMaxStackSize(1);
	redstoneRepeater= LordNew ItemReed(100, "diode", BLOCK_ID_REDSTONE_REPEATER_IDLE);
	cookie			= LordNew ItemFood(101, "cookie", 2, 0.1F, false);
	//imap			= (ItemMap)(LordNew ItemMap(102)).setUnlocalizedName("map").setAtlasName("map_filled");
	shears			= LordNew ItemShears(103, "shears");
	melon			= LordNew ItemFood(104, "melon", 2, 0.3F, false);
	pumpkinSeeds	= LordNew ItemSeeds(105, "seeds_pumpkin", BLOCK_ID_PUMPKIN_STEM, BLOCK_ID_TILLED_FIELD);
	melonSeeds		= LordNew ItemSeeds(106, "seeds_melon", BLOCK_ID_MELON_STEM, BLOCK_ID_TILLED_FIELD);
	beefRaw			= LordNew ItemFood(107, "beefRaw",  3, 0.3F, true);
	beefCooked		= LordNew ItemFood(108, "beefCooked", 8, 0.8F, true);
	chickenRaw		= LordNew ItemFood(109, "chickenRaw", 2, 0.3F, true); ((ItemFood*)chickenRaw)->setPotionEffect(Potion::hunger->getId(), 30, 0, 0.3F);
	chickenCooked	= LordNew ItemFood(110, "chickenCooked", 6, 0.6F, true);
	rottenFlesh		= LordNew ItemFood(111, "rottenFlesh", 4, 0.1F, true); ((ItemFood*)rottenFlesh)->setPotionEffect(Potion::hunger->getId(), 30, 0, 0.8F);
	enderPearl		= LordNew Item(112, "enderPearl");
	blazeRod		= LordNew Item(113, "blazeRod"); 
	ghastTear		= LordNew Item(114, "ghastTear"); ghastTear->setPotionEffect(PotionHelper::ghastTearEffect);
	goldNugget		= LordNew Item(115, "goldNugget"); 
	netherStalkSeeds= LordNew ItemSeeds(116, "netherStalkSeeds", BLOCK_ID_NETHER_STALK, BLOCK_ID_SLOW_SAND); netherStalkSeeds->setPotionEffect("+4");
	potion			= LordNew ItemPotion(117, "potion");
	glassBottle		= LordNew ItemGlassBottle(118, "glassBottle");
	spiderEye		= LordNew ItemFood(119, "spiderEye", 2, 0.8F, false); ((ItemFood*)spiderEye)->setPotionEffect(Potion::poison->getId(), 5, 0, 1.0F); spiderEye->setPotionEffect(PotionHelper::spiderEyeEffect);
	fermentedSpiderEye = LordNew Item(120, "fermentedSpiderEye"); fermentedSpiderEye->setPotionEffect(PotionHelper::fermentedSpiderEyeEffect);
	blazePowder		= LordNew Item(121, "blazePowder"); blazePowder->setPotionEffect(PotionHelper::blazePowderEffect); 
	magmaCream		= LordNew Item(122, "magmaCream"); magmaCream->setPotionEffect(PotionHelper::magmaCreamEffect); 
	brewingStand	= LordNew ItemReed(123, "brewingStand", BLOCK_ID_BREWING_STAND);
	cauldron		= LordNew ItemReed(124, "cauldron", BLOCK_ID_CAULDRON);
	eyeOfEnder		= LordNew ItemEnderEye(125, "eyeOfEnder");
	speckledMelon	= LordNew Item(126, "speckledMelon"); speckledMelon->setPotionEffect(PotionHelper::speckledMelonEffect); 
	monsterPlacer	= LordNew ItemMonsterPlacer(127, "monsterPlacer");
	expBottle		= LordNew ItemExpBottle(128, "expBottle");
	fireballCharge  = LordNew ItemFireball(129, "fireball");
	writableBook	= LordNew ItemWritableBook(130, "writingBook");
	writtenBook		= LordNew ItemEditableBook(131, "writtenBook");
	emerald			= LordNew Item(132, "emerald");
	itemFrame		= LordNew ItemHangingEntity(133, "frame", 2);
	flowerPot		= LordNew ItemReed(134, "flowerPot", BLOCK_ID_FLOWER_POT);
	carrot			= LordNew ItemSeedFood(135, "carrots", 4, 0.6F, BLOCK_ID_CARROT, BLOCK_ID_TILLED_FIELD);
	potato			= LordNew ItemSeedFood(136, "potato", 1, 0.3F, BLOCK_ID_POTATO, BLOCK_ID_TILLED_FIELD);
	bakedPotato		= LordNew ItemFood(137, "potatoBaked", 6, 0.6F, false);
	poisonousPotato	= LordNew ItemFood(138, "potatoPoisonous", 2, 0.3F, false); ((ItemFood*)poisonousPotato)->setPotionEffect(Potion::poison->getId(), 5, 0, 0.6F);
	//emptyMap		= LordNew ItemEmptyMap(139)).setUnlocalizedName("emptyMap").setAtlasName("map_empty");
	goldenCarrot	= LordNew ItemFood(140, "carrotGolden", 6, 1.2F, false); goldenCarrot->setPotionEffect(PotionHelper::goldenCarrotEffect);
	skull			= LordNew ItemSkull(141, "skull");
	carrotOnAStick  = LordNew ItemCarrotOnAStick(142, "carrotOnAStick");
	netherStar		= LordNew ItemSimpleFoiled(143, "netherStar");
	pumpkinPie		= LordNew ItemFood(144, "pumpkinPie", 8, 0.3F, false);
	firework		= LordNew ItemFirework(145, "fireworks");
	fireworkCharge  = LordNew ItemFireworkCharge(146, "fireworksCharge");
	enchantedBook	= LordNew ItemEnchantedBook(147, "enchantedBook"); enchantedBook->setMaxStackSize(1);
	comparator		= LordNew ItemReed(148, "comparator", BLOCK_ID_REDSTONE_COMPARATOR_IDLE);
	netherrackBrick	= LordNew Item(149, "netherbrick"); 
	netherQuartz	= LordNew Item(150, "netherquartz"); 
	blockmanTnt		= LordNew ItemBlockman(151, "blockmanTnt", 3);
	blockmanHopper	= LordNew ItemBlockman(152, "blockmanHopper", 5);
	prismarine_shard = LordNew Item(153, "prismarine_shard");
	prismarine_crystals = LordNew Item(154, "prismarine_crystals");
	rabbitCooked	= LordNew ItemFood(156, "rabbitCooked", 5, 1.2F, true);
	iron_horse_armor = LordNew Item(161, "horsearmormetal"); iron_horse_armor->setMaxStackSize(1); 
	golden_horse_armor = LordNew Item(162, "horsearmorgold"); golden_horse_armor->setMaxStackSize(1); 
	diamond_horse_armor = LordNew Item(163, "horsearmordiamond"); diamond_horse_armor->setMaxStackSize(1);
	lead			= LordNew ItemLeash(164, "leash");
	name_tag		= LordNew ItemNameTag(165, "nameTag");
	arrowPoison		= LordNew ItemArrowPotion(166, "arrow_poison"); arrowPoison->setPotionParams(PotionManager::effectPoison->getPotionID(), int(Math::Floor(PotionManager::effectPoison->getDuration() / 8.f)), PotionManager::effectPoison->getAmplifier());
	arrowLongPoison = LordNew ItemArrowPotion(167, "arrow_long_poison"); arrowLongPoison->setPotionParams(PotionManager::effectLongPoison->getPotionID(), int(Math::Floor(PotionManager::effectLongPoison->getDuration() / 8.f)), PotionManager::effectLongPoison->getAmplifier());
	arrowStrongPoison = LordNew ItemArrowPotion(168, "arrow_strong_poison"); arrowStrongPoison->setPotionParams(PotionManager::effectStrongPoison->getPotionID(), int(Math::Floor(PotionManager::effectStrongPoison->getDuration() / 8.f)), PotionManager::effectStrongPoison->getAmplifier());
	grenade			= LordNew ItemGrenade(169, "grenade");
	handcuffs		= LordNew ItemHandcuffs(195, "handcuffs");
	key_01			= LordNew ItemKey(196, "key01");
	key_02			= LordNew ItemKey(197, "key02");
	key_03			= LordNew ItemKey(198, "key03");
	key_04			= LordNew ItemKey(199, "key04");
	key_05			= LordNew ItemKey(200, "key05");
	key_06			= LordNew ItemKey(201, "key06");
	key_07			= LordNew ItemKey(202, "key07");
	key_08			= LordNew ItemKey(203, "key08");
	key_09			= LordNew ItemKey(204, "key09");
	key_10			= LordNew ItemKey(205, "key10");
	tpScroll_01     = LordNew ItemTeleportScroll(206, "tpScroll01");
	tpScroll_02		= LordNew ItemTeleportScroll(207, "tpScroll02");
	tpScroll_03		= LordNew ItemTeleportScroll(208, "tpScroll03");
	tpScroll_04		= LordNew ItemTeleportScroll(209, "tpScroll04");
	tpScroll_05		= LordNew ItemTeleportScroll(210, "tpScroll05");
	createBridge_01	= LordNew ItemCreateBridge(211, "createBridge01");
	createBridge_02 = LordNew ItemCreateBridge(212, "createBridge02");
	createBridge_03 = LordNew ItemCreateBridge(213, "createBridge03");
	createBridge_04 = LordNew ItemCreateBridge(214, "createBridge04");
	createBridge_05 = LordNew ItemCreateBridge(215, "createBridge05");
	shovelRenovation = LordNew ItemToolDigger(216, "shovelRenovation", ToolMaterial::IRON);
	spatula = LordNew ItemToolDigger(217, "spatula", ToolMaterial::IRON);
	shovelToy = LordNew ItemToolDigger(218, "shovelToy", ToolMaterial::IRON);
	digBucket = LordNew ItemToolDigger(219, "digBucket", ToolMaterial::IRON);
	digShovelIron = LordNew ItemToolDigger(220, "digShovelIron", ToolMaterial::IRON);
	shovelRake = LordNew ItemToolDigger(221, "shovelRake", ToolMaterial::IRON);
	shovelMarching = LordNew ItemToolDigger(222, "shovelMarching", ToolMaterial::IRON);
	shovelBigScooper = LordNew ItemToolDigger(223, "shovelBigScooper", ToolMaterial::IRON);
	shovelJackHammer = LordNew ItemToolDigger(224, "shovelJackHammer", ToolMaterial::IRON);
	shovelDrill = LordNew ItemToolDigger(225, "shovelDrill", ToolMaterial::IRON);
	shovelEngineerGold = LordNew ItemToolDigger(226, "shovelEngineerGold", ToolMaterial::IRON);
	medichinePack = LordNew ItemMedichine(227, "medichinePack", 40.0f);
	medichinePotion = LordNew ItemMedichine(228, "medichinePotion", 10.0f);

	record13		= LordNew ItemRecord(230, "record", "13");
	recordCat		= LordNew ItemRecord(231, "record", "cat");
	recordBlocks	= LordNew ItemRecord(232, "record", "blocks");
	recordChirp		= LordNew ItemRecord(233, "record", "chirp");
	recordFar		= LordNew ItemRecord(234, "record", "far");
	recordMall		= LordNew ItemRecord(235, "record", "mall");
	recordMellohi	= LordNew ItemRecord(236, "record", "mellohi");
	recordStal		= LordNew ItemRecord(237, "record", "stal");
	recordStrad		= LordNew ItemRecord(238, "record", "strad");
	recordWard		= LordNew ItemRecord(239, "record", "ward");
	record11		= LordNew ItemRecord(240, "record", "11");
	recordWait		= LordNew ItemRecord(241, "record", "wait");

	goldHeart = LordNew ItemProp(242, "goldHeart");
	goldShoes = LordNew ItemProp(243, "goldShoes");
	goldArrow = LordNew ItemProp(244, "goldArrow");
	questionMark = LordNew ItemProp(245, "questionMark");
	propGold = LordNew ItemProp(246, "propGold");
	propExp = LordNew ItemProp(247, "propExp");

	tomato = LordNew ItemFood(246, "tomato", 6, 0.6F, false);
	cabbage = LordNew ItemFood(247, "potatoBaked", 6, 0.6F, false);
	pea = LordNew ItemFood(248, "pea", 6, 0.6F, false);
	flareBomb = LordNew Item(250, "flareBomb");
	rescue = LordNew Item(251, "rescue");
	goldApple = LordNew ItemFood(252, "goldApple", 0, 0.0F, false); ((ItemFood*)goldApple)->setAlwaysEdible();
	currencyMoney = LordNew Item(253, "currencyMoney");
	doorBirch = LordNew ItemDoor(254, "doorBirch", BM_Material::BM_MAT_wood);
	purple_diamond = LordNew ItemProp(255, "purple_diamond");
	carrotIron = LordNew Item(391, "carrotIron");
	potatoIron = LordNew Item(392, "potatoIron");

	//throwing skill
	itemThrowableSkillProp_01 = LordNew ItemProp(459, "skill_01");
	itemThrowableSkillProp_02 = LordNew ItemProp(460, "skill_02");
	itemThrowableSkillProp_03 = LordNew ItemProp(461, "skill_03");
	//Reserve throwing skill Id, if necessary, please reset the atlas and so on.
	itemThrowableSkillProp_04 = LordNew ItemProp(462, "skill_01");
	itemThrowableSkillProp_05 = LordNew ItemProp(463, "skill_02");
	itemThrowableSkillProp_06 = LordNew ItemProp(464, "skill_03");
	itemThrowableSkillProp_07 = LordNew ItemProp(465, "skill_01");
	itemThrowableSkillProp_08 = LordNew ItemProp(466, "skill_02");
	itemThrowableSkillProp_09 = LordNew ItemProp(467, "skill_03");
	itemThrowableSkillProp_10 = LordNew ItemProp(468, "skill_03");

	// eat skill 
	itemEatSkillProp_01 = LordNew ItemProp(469, "skill_01");
	itemEatSkillProp_02 = LordNew ItemProp(470, "skill_02");
	itemEatSkillProp_03 = LordNew ItemProp(471, "skill_03");
	//Reserve eat skill Id, if necessary, please reset the atlas and so on.
	itemEatSkillProp_04 = LordNew ItemProp(472, "skill_01");
	itemEatSkillProp_05 = LordNew ItemProp(473, "skill_02");
	itemEatSkillProp_06 = LordNew ItemProp(474, "skill_03");
	itemEatSkillProp_07 = LordNew ItemProp(475, "skill_01");
	itemEatSkillProp_08 = LordNew ItemProp(476, "skill_02");
	itemEatSkillProp_09 = LordNew ItemProp(477, "skill_03");
	itemEatSkillProp_10 = LordNew ItemProp(478, "skill_03");

	equipmentSet1 = LordNew ItemProp(496, "equipment.set.1");
	equipmentSet2 = LordNew ItemProp(497, "equipment.set.2");
	equipmentSet3 = LordNew ItemProp(498, "equipment.set.3");
	equipmentSet4 = LordNew ItemProp(499, "equipment.set.4");
	equipmentSet5 = LordNew ItemProp(500, "equipment.set.5");

	purplePickaxe = LordNew ItemPickaxe(501, "purple.pickaxe", ToolMaterial::EMERALD);
	smokeGrenade = LordNew ItemGrenade(502, "grenade.smoke");

	tremorGrenade = LordNew Item(513, "grenade.tremor");
	frozenBall = LordNew Item(514, "frozen.ball");

	baseballStick = LordNew ItemProp(525, "baseball_stick");
	spikeStick = LordNew ItemProp(526, "spike_stick");
	stonePillar = LordNew ItemProp(527, "stone_pillar");

	shortenRebirthTimeItem = LordNew ItemAppIcon(528, "shorten_rebirth_time");
	increaseHpItem = LordNew ItemAppIcon(529, "increase_hp");
	diamondDefensePackItem = LordNew ItemAppIcon(530, "diamond_defense_pack");

	grenade_skill_01 = LordNew Item(530, "grenade_skill");
	grenade_skill_02 = LordNew Item(531, "grenade_skill");
	colorful_grenade_skill_01 = LordNew Item(532, "colorful_grenade_skill");
	colorful_grenade_skill_02 = LordNew Item(533, "colorful_grenade_skill");
	change_animal_skill_01 = LordNew ItemTransparent(534, "change_animal_skill");
	change_animal_skill_02 = LordNew ItemTransparent(535, "change_animal_skill");
	speed_up_skill_01 = LordNew ItemTransparent(536, "speed_up_skill");
	speed_up_skill_02 = LordNew ItemTransparent(537, "speed_up_skill");

	pixel_gun_prop_1 = LordNew ItemProp(598, "pixel_gun_prop_1");
	pixel_gun_prop_2 = LordNew ItemProp(599, "pixel_gun_prop_2");
	pixel_gun_prop_3 = LordNew ItemProp(600, "pixel_gun_prop_3");
	pixel_gun_prop_4 = LordNew ItemProp(601, "pixel_gun_prop_4");
	pixel_gun_prop_5 = LordNew ItemProp(602, "pixel_gun_prop_5");
	pixel_gun_prop_6 = LordNew ItemProp(603, "pixel_gun_prop_6");
	pixel_gun_prop_7 = LordNew ItemProp(604, "pixel_gun_prop_7");
	pixel_gun_prop_8 = LordNew ItemProp(605, "pixel_gun_prop_8");
	pixel_gun_prop_9 = LordNew ItemProp(606, "pixel_gun_prop_9");
	pixel_gun_prop_10 = LordNew ItemProp(607, "pixel_gun_prop_10");
	pixel_gun_prop_11 = LordNew ItemProp(608, "pixel_gun_prop_11");
	pixel_gun_prop_12 = LordNew ItemProp(609, "pixel_gun_prop_12");
	pixel_gun_prop_13 = LordNew ItemProp(610, "pixel_gun_prop_13");
	pixel_gun_prop_14 = LordNew ItemProp(611, "pixel_gun_prop_14");
	pixel_gun_prop_15 = LordNew ItemProp(612, "pixel_gun_prop_15");
	pixel_gun_prop_16 = LordNew ItemProp(613, "pixel_gun_prop_16");
	pixel_gun_prop_17 = LordNew ItemProp(614, "pixel_gun_prop_17");
	pixel_gun_prop_18 = LordNew ItemProp(615, "pixel_gun_prop_18");
	pixel_gun_prop_19 = LordNew ItemProp(616, "pixel_gun_prop_19");
	pixel_gun_prop_20 = LordNew ItemProp(617, "pixel_gun_prop_20");
	pixel_gun_prop_21 = LordNew ItemProp(618, "pixel_gun_prop_21");
	pixel_gun_prop_22 = LordNew ItemProp(619, "pixel_gun_prop_22");
	pixel_gun_prop_23 = LordNew ItemProp(620, "pixel_gun_prop_23");
	pixel_gun_prop_24 = LordNew ItemProp(621, "pixel_gun_prop_24");
	pixel_gun_prop_25 = LordNew ItemProp(622, "pixel_gun_prop_25");
	pixel_gun_prop_26 = LordNew ItemProp(623, "pixel_gun_prop_26");
	pixel_gun_prop_27 = LordNew ItemProp(624, "pixel_gun_prop_27");
	pixel_gun_prop_28 = LordNew ItemProp(625, "pixel_gun_prop_28");
	pixel_gun_prop_29 = LordNew ItemProp(626, "pixel_gun_prop_29");
	pixel_gun_prop_30 = LordNew ItemProp(627, "pixel_gun_prop_30");

	seedsOfNewCrops_01 = LordNew ItemSeeds(644, "seeds_wheat", 1256, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_02 = LordNew ItemSeeds(645, "seeds_carrot", 1257, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_03 = LordNew ItemSeeds(646, "seeds_tomato", 1258, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_04 = LordNew ItemSeeds(647, "seeds_cabbage", 1259, BLOCK_ID_TILLED_FIELD);
	seedsOfNewCrops_05 = LordNew ItemSeeds(648, "seeds_pea", 1260, BLOCK_ID_TILLED_FIELD);
	gemstone = LordNew ItemProp(649, "gemstone");

	ranchSeed_100001 = LordNew RanchSeeds(701, "500001_wheat_seed", 1261, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100002 = LordNew RanchSeeds(702, "500002_corn_seed", 1262, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100003 = LordNew RanchSeeds(703, "500003_carrots_seed", 1263, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100004 = LordNew RanchSeeds(704, "500004_sugar_cane_seed", 1264, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100005 = LordNew RanchSeeds(705, "500005_cotton_seed", 1265, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100006 = LordNew RanchSeeds(706, "500006_strawberry_seed", 1266, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100007 = LordNew RanchSeeds(707, "500007_tomato_seed", 1267, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100008 = LordNew RanchSeeds(708, "500008_pine_seed", 1268, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100009 = LordNew RanchSeeds(709, "500009_potatoes_seed", 1269, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100010 = LordNew RanchSeeds(710, "500010_cocoa_beans_seed", 1270, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100011 = LordNew RanchSeeds(711, "500011_rubber_wood_seed", 1271, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100012 = LordNew RanchSeeds(712, "500012_silk_seed", 1272, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100013 = LordNew RanchSeeds(713, "500013_chili_seed", 1273, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100014 = LordNew RanchSeeds(714, "500014_rice_seed", 1274, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100015 = LordNew RanchSeeds(715, "500015_the_rose_seed", 1275, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100016 = LordNew RanchSeeds(716, "500016_jasmine_seed", 1276, BLOCK_ID_TILLED_FIELD);
	ranchSeed_100017 = LordNew RanchSeeds(717, "500017_coffee_beans_seed", 1277, BLOCK_ID_TILLED_FIELD);

	itemThrowableSkillProp_11 = LordNew ItemProp(944, "skill_11");
	itemThrowableSkillProp_12 = LordNew ItemProp(945, "skill_12");
	itemThrowableSkillProp_13 = LordNew ItemProp(946, "skill_13");
	itemThrowableSkillProp_14 = LordNew ItemProp(947, "skill_14");
	itemThrowableSkillProp_15 = LordNew ItemProp(948, "skill_15");
	itemThrowableSkillProp_16 = LordNew ItemProp(949, "skill_16");
	itemThrowableSkillProp_17 = LordNew ItemProp(950, "skill_17");
	itemThrowableSkillProp_18 = LordNew ItemProp(951, "skill_18");
	itemThrowableSkillProp_19 = LordNew ItemProp(952, "skill_19");
	itemThrowableSkillProp_20 = LordNew ItemProp(953, "skill_20");

	itemThrowableSkillProp_04_plus = LordNew ItemProp(956, "skill_01");
	itemThrowableSkillProp_05_plus = LordNew ItemProp(957, "skill_02");
	itemThrowableSkillProp_06_plus = LordNew ItemProp(958, "skill_03");
	itemThrowableSkillProp_07_plus = LordNew ItemProp(959, "skill_01");
	itemThrowableSkillProp_08_plus = LordNew ItemProp(960, "skill_02");
	itemThrowableSkillProp_09_plus = LordNew ItemProp(961, "skill_03");
	itemThrowableSkillProp_11_plus = LordNew ItemProp(962, "skill_01");
	itemThrowableSkillProp_12_plus = LordNew ItemProp(963, "skill_01");
	itemThrowableSkillProp_13_plus = LordNew ItemProp(964, "skill_01");
	itemThrowableSkillProp_14_plus = LordNew ItemProp(965, "skill_01");
	itemThrowableSkillProp_15_plus = LordNew ItemProp(966, "skill_01");
	itemThrowableSkillProp_16_plus = LordNew ItemProp(967, "skill_01");
	itemThrowableSkillProp_17_plus = LordNew ItemProp(968, "skill_01");
	itemThrowableSkillProp_18_plus = LordNew ItemProp(969, "skill_01");
	itemThrowableSkillProp_19_plus = LordNew ItemProp(970, "skill_01");
	itemThrowableSkillProp_20_plus = LordNew ItemProp(971, "skill_01");

	bow_red = LordNew ItemBow(980, "bow");
	bow_black = LordNew ItemBow(981, "bow");
	redArrow = LordNew ItemArrow(982, "arrow");
	blackArrow = LordNew ItemArrow(983, "arrow");

	weapon_01 = LordNew ItemProp(984, "weapon_01");
	weapon_02 = LordNew ItemProp(985, "weapon_02");
	weapon_04 = LordNew ItemProp(987, "weapon_04");
	weapon_05 = LordNew ItemProp(988, "weapon_05");
	weapon_06 = LordNew ItemProp(989, "weapon_06");
	weapon_07 = LordNew ItemProp(990, "weapon_07");
	weapon_08 = LordNew ItemProp(991, "weapon_08");
	weapon_09 = LordNew ItemProp(992, "weapon_09");
	weapon_10 = LordNew ItemProp(993, "weapon_10");
	weapon_11 = LordNew ItemProp(994, "weapon_11");
	weapon_12 = LordNew ItemProp(995, "weapon_12");
	weapon_13 = LordNew ItemProp(996, "weapon_13");
	weapon_14 = LordNew ItemProp(997, "weapon_14");
	weapon_15 = LordNew ItemProp(998, "weapon_15");
	weapon_16 = LordNew ItemProp(999, "weapon_16");
	weapon_17 = LordNew ItemProp(1000, "weapon_17");
	weapon_18 = LordNew ItemProp(1001, "weapon_18");

	handGrenade = LordNew ItemHandGrenade(1745, "handGrenade");

	bow_01 = LordNew ItemBow(1881, "bow");
	bow_02 = LordNew ItemBow(1882, "bow");
	bow_03 = LordNew ItemBow(1883, "bow");

	book_1 = LordNew ItemWritableBook(2031, "writingBook1");
	book_2 = LordNew ItemWritableBook(2032, "writingBook2");
	book_3 = LordNew ItemWritableBook(2033, "writingBook3");
	book_4 = LordNew ItemWritableBook(2034, "writingBook4");
	book_5 = LordNew ItemWritableBook(2035, "writingBook5");
	book_6 = LordNew ItemWritableBook(2036, "writingBook6");
	book_7 = LordNew ItemWritableBook(2037, "writingBook7");
	book_8 = LordNew ItemWritableBook(2038, "writingBook8");

	cornIron = LordNew Item(2039, "cornIron");
	strawberryIron = LordNew Item(2040, "strawberryIron");
	tomatoIron = LordNew Item(2041, "tomatoIron");
	wheatIron = LordNew Item(2042, "wheatIron");
	cottonIron = LordNew Item(2043, "cottonIron");
	silkIron = LordNew Item(2044, "silkIron");

	decoration_01 = LordNew ItemDecoration(2345, "decoration_01");
	decoration_02 = LordNew ItemDecoration(2346, "decoration_02");
	decoration_03 = LordNew ItemDecoration(2347, "decoration_03");
	decoration_04 = LordNew ItemDecoration(2348, "decoration_04");
	decoration_05 = LordNew ItemDecoration(2349, "decoration_05");
	decoration_06 = LordNew ItemDecoration(2350, "decoration_06");
	decoration_07 = LordNew ItemDecoration(2351, "decoration_07");
	decoration_08 = LordNew ItemDecoration(2352, "decoration_08");
	decoration_09 = LordNew ItemDecoration(2353, "decoration_09");
	decoration_10 = LordNew ItemDecoration(2354, "decoration_10");
	decoration_11 = LordNew ItemDecoration(2355, "decoration_11");
	decoration_12 = LordNew ItemDecoration(2356, "decoration_12");
	decoration_13 = LordNew ItemDecoration(2357, "decoration_13");
	decoration_14 = LordNew ItemDecoration(2358, "decoration_14");
	decoration_15 = LordNew ItemDecoration(2359, "decoration_15");
	decoration_16 = LordNew ItemDecoration(2360, "decoration_16");
	decoration_17 = LordNew ItemDecoration(2361, "decoration_17");
	decoration_18 = LordNew ItemDecoration(2362, "decoration_18");
	decoration_19 = LordNew ItemDecoration(2363, "decoration_19");
	decoration_20 = LordNew ItemDecoration(2364, "decoration_20");
	decoration_21 = LordNew ItemDecoration(2365, "decoration_21");
	decoration_22 = LordNew ItemDecoration(2366, "decoration_22");
	decoration_23 = LordNew ItemDecoration(2367, "decoration_23");
	decoration_24 = LordNew ItemDecoration(2368, "decoration_24");
	decoration_25 = LordNew ItemDecoration(2369, "decoration_25");
	decoration_26 = LordNew ItemDecoration(2370, "decoration_26");
	decoration_27 = LordNew ItemDecoration(2371, "decoration_27");
	decoration_28 = LordNew ItemDecoration(2372, "decoration_28");
	decoration_29 = LordNew ItemDecoration(2373, "decoration_29");
	decoration_30 = LordNew ItemDecoration(2374, "decoration_30");
	decoration_31 = LordNew ItemDecoration(2375, "decoration_31");
	decoration_32 = LordNew ItemDecoration(2376, "decoration_32");
	decoration_33 = LordNew ItemDecoration(2377, "decoration_33");
	decoration_34 = LordNew ItemDecoration(2378, "decoration_34");
	decoration_35 = LordNew ItemDecoration(2379, "decoration_35");
	decoration_36 = LordNew ItemDecoration(2380, "decoration_36");
	decoration_37 = LordNew ItemDecoration(2381, "decoration_37");
	decoration_38 = LordNew ItemDecoration(2382, "decoration_38");
	decoration_39 = LordNew ItemDecoration(2383, "decoration_39");
	decoration_40 = LordNew ItemDecoration(2384, "decoration_40");
	decoration_41 = LordNew ItemDecoration(2385, "decoration_41");
	decoration_42 = LordNew ItemDecoration(2386, "decoration_42");
	decoration_43 = LordNew ItemDecoration(2387, "decoration_43");
	decoration_44 = LordNew ItemDecoration(2388, "decoration_44");
	decoration_45 = LordNew ItemDecoration(2389, "decoration_45");
	decoration_46 = LordNew ItemDecoration(2390, "decoration_46");
	decoration_47 = LordNew ItemDecoration(2391, "decoration_47");
	decoration_48 = LordNew ItemDecoration(2392, "decoration_48");
	decoration_49 = LordNew ItemDecoration(2393, "decoration_49");
	decoration_50 = LordNew ItemDecoration(2394, "decoration_50");
	decoration_51 = LordNew ItemDecoration(2395, "decoration_51");
	decoration_52 = LordNew ItemDecoration(2396, "decoration_52");
	decoration_53 = LordNew ItemDecoration(2397, "decoration_53");
	decoration_54 = LordNew ItemDecoration(2398, "decoration_54");
	decoration_55 = LordNew ItemDecoration(2399, "decoration_55");
	decoration_56 = LordNew ItemDecoration(2400, "decoration_56");
	decoration_57 = LordNew ItemDecoration(2401, "decoration_57");
	decoration_58 = LordNew ItemDecoration(2402, "decoration_58");
	decoration_59 = LordNew ItemDecoration(2403, "decoration_59");
	decoration_60 = LordNew ItemDecoration(2404, "decoration_60");
	decoration_61 = LordNew ItemDecoration(2405, "decoration_61");
	decoration_62 = LordNew ItemDecoration(2406, "decoration_62");
	decoration_63 = LordNew ItemDecoration(2407, "decoration_63");
	decoration_64 = LordNew ItemDecoration(2408, "decoration_64");
	decoration_65 = LordNew ItemDecoration(2409, "decoration_65");
	decoration_66 = LordNew ItemDecoration(2410, "decoration_66");
	decoration_67 = LordNew ItemDecoration(2411, "decoration_67");
	decoration_68 = LordNew ItemDecoration(2412, "decoration_68");
	decoration_69 = LordNew ItemDecoration(2413, "decoration_69");
	decoration_70 = LordNew ItemDecoration(2414, "decoration_70");
	decoration_71 = LordNew ItemDecoration(2415, "decoration_71");
	decoration_72 = LordNew ItemDecoration(2416, "decoration_72");
	decoration_73 = LordNew ItemDecoration(2417, "decoration_73");
	decoration_74 = LordNew ItemDecoration(2418, "decoration_74");
	decoration_75 = LordNew ItemDecoration(2419, "decoration_75");
	decoration_76 = LordNew ItemDecoration(2420, "decoration_76");
	decoration_77 = LordNew ItemDecoration(2421, "decoration_77");
	decoration_78 = LordNew ItemDecoration(2422, "decoration_78");
	decoration_79 = LordNew ItemDecoration(2423, "decoration_79");
	decoration_80 = LordNew ItemDecoration(2424, "decoration_80");
	decoration_81 = LordNew ItemDecoration(2425, "decoration_81");
	decoration_82 = LordNew ItemDecoration(2426, "decoration_82");
	decoration_83 = LordNew ItemDecoration(2427, "decoration_83");
	decoration_84 = LordNew ItemDecoration(2428, "decoration_84");
	decoration_85 = LordNew ItemDecoration(2429, "decoration_85");
	decoration_86 = LordNew ItemDecoration(2430, "decoration_86");
	decoration_87 = LordNew ItemDecoration(2431, "decoration_87");
	decoration_88 = LordNew ItemDecoration(2432, "decoration_88");
	decoration_89 = LordNew ItemDecoration(2433, "decoration_89");
	decoration_90 = LordNew ItemDecoration(2434, "decoration_90");
	decoration_91 = LordNew ItemDecoration(2435, "decoration_91");
	decoration_92 = LordNew ItemDecoration(2436, "decoration_92");
	decoration_93 = LordNew ItemDecoration(2437, "decoration_93");
	decoration_94 = LordNew ItemDecoration(2438, "decoration_94");
	decoration_95 = LordNew ItemDecoration(2439, "decoration_95");
	decoration_96 = LordNew ItemDecoration(2440, "decoration_96");
	decoration_97 = LordNew ItemDecoration(2441, "decoration_97");
	decoration_98 = LordNew ItemDecoration(2442, "decoration_98");
	decoration_99 = LordNew ItemDecoration(2443, "decoration_99");
	decoration_100 = LordNew ItemDecoration(2444, "decoration_100");

	initializeItemNameToIdMap();

	BlockManager::registBlockItems();
	PotionManager::registerPotionItem();
}

void Item::initializeItemNameToIdMap()
{
	itemNameToIdMap["blockman:air"] = 0;
	itemNameToIdMap["blockman:stone"] = 1;
	itemNameToIdMap["blockman:grass"] = 2;
	itemNameToIdMap["blockman:dirt"] = 3;
	itemNameToIdMap["blockman:cobblestone"] = 4;
	itemNameToIdMap["blockman:planks"] = 5;
	itemNameToIdMap["blockman:sapling"] = 6;
	itemNameToIdMap["blockman:bedrock"] = 7;
	itemNameToIdMap["blockman:flowing_water"] = 8;
	itemNameToIdMap["blockman:water"] = 9;
	itemNameToIdMap["blockman:flowing_lava"] = 10;
	itemNameToIdMap["blockman:lava"] = 11;
	itemNameToIdMap["blockman:sand"] = 12;
	itemNameToIdMap["blockman:gravel"] = 13;
	itemNameToIdMap["blockman:gold_ore"] = 14;
	itemNameToIdMap["blockman:iron_ore"] = 15;
	itemNameToIdMap["blockman:coal_ore"] = 16;
	itemNameToIdMap["blockman:log"] = 17;
	itemNameToIdMap["blockman:leaves"] = 18;
	itemNameToIdMap["blockman:sponge"] = 19;
	itemNameToIdMap["blockman:glass"] = 20;
	itemNameToIdMap["blockman:lapis_ore"] = 21;
	itemNameToIdMap["blockman:lapis_block"] = 22;
	itemNameToIdMap["blockman:dispenser"] = 23;
	itemNameToIdMap["blockman:sandstone"] = 24;
	itemNameToIdMap["blockman:noteblock"] = 25;
	itemNameToIdMap["blockman:bed"] = 26;
	itemNameToIdMap["blockman:golden_rail"] = 27;
	itemNameToIdMap["blockman:detector_rail"] = 28;
	itemNameToIdMap["blockman:sticky_piston"] = 29;
	itemNameToIdMap["blockman:web"] = 30;
	itemNameToIdMap["blockman:tallgrass"] = 31;
	itemNameToIdMap["blockman:deadbush"] = 32;
	itemNameToIdMap["blockman:piston"] = 33;
	itemNameToIdMap["blockman:piston_head"] = 34;
	itemNameToIdMap["blockman:wool"] = 35;
	itemNameToIdMap["blockman:piston_extension"] = 36;
	itemNameToIdMap["blockman:yellow_flower"] = 37;
	itemNameToIdMap["blockman:red_flower"] = 38;
	itemNameToIdMap["blockman:brown_mushroom"] = 39;
	itemNameToIdMap["blockman:red_mushroom"] = 40;
	itemNameToIdMap["blockman:gold_block"] = 41;
	itemNameToIdMap["blockman:iron_block"] = 42;
	itemNameToIdMap["blockman:double_stone_slab"] = 43;
	itemNameToIdMap["blockman:stone_slab"] = 44;
	itemNameToIdMap["blockman:brick_block"] = 45;
	itemNameToIdMap["blockman:tnt"] = 46;
	itemNameToIdMap["blockman:bookshelf"] = 47;
	itemNameToIdMap["blockman:mossy_cobblestone"] = 48;
	itemNameToIdMap["blockman:obsidian"] = 49;
	itemNameToIdMap["blockman:torch"] = 50;
	itemNameToIdMap["blockman:fire"] = 51;
	itemNameToIdMap["blockman:mob_spawner"] = 52;
	itemNameToIdMap["blockman:oak_stairs"] = 53;
	itemNameToIdMap["blockman:chest"] = 54;
	itemNameToIdMap["blockman:redstone_wire"] = 55;
	itemNameToIdMap["blockman:diamond_ore"] = 56;
	itemNameToIdMap["blockman:diamond_block"] = 57;
	itemNameToIdMap["blockman:crafting_table"] = 58;
	itemNameToIdMap["blockman:wheat"] = 59;
	itemNameToIdMap["blockman:farmland"] = 60;
	itemNameToIdMap["blockman:furnace"] = 61;
	itemNameToIdMap["blockman:lit_furnace"] = 62;
	itemNameToIdMap["blockman:standing_sign"] = 63;
	itemNameToIdMap["blockman:wooden_door"] = 64;
	itemNameToIdMap["blockman:ladder"] = 65;
	itemNameToIdMap["blockman:rail"] = 66;
	itemNameToIdMap["blockman:stone_stairs"] = 67;
	itemNameToIdMap["blockman:wall_sign"] = 68;
	itemNameToIdMap["blockman:lever"] = 69;
	itemNameToIdMap["blockman:stone_pressure_plate"] = 70;
	itemNameToIdMap["blockman:iron_door"] = 71;
	itemNameToIdMap["blockman:wooden_pressure_plate"] = 72;
	itemNameToIdMap["blockman:redstone_ore"] = 73;
	itemNameToIdMap["blockman:lit_redstone_ore"] = 74;
	itemNameToIdMap["blockman:unlit_redstone_torch"] = 75;
	itemNameToIdMap["blockman:redstone_torch"] = 76;
	itemNameToIdMap["blockman:stone_button"] = 77;
	itemNameToIdMap["blockman:snow_layer"] = 78;
	itemNameToIdMap["blockman:ice"] = 79;
	itemNameToIdMap["blockman:snow"] = 80;
	itemNameToIdMap["blockman:cactus"] = 81;
	itemNameToIdMap["blockman:clay"] = 82;
	itemNameToIdMap["blockman:reeds"] = 83;
	itemNameToIdMap["blockman:jukebox"] = 84;
	itemNameToIdMap["blockman:fence"] = 85;
	itemNameToIdMap["blockman:pumpkin"] = 86;
	itemNameToIdMap["blockman:netherrack"] = 87;
	itemNameToIdMap["blockman:soul_sand"] = 88;
	itemNameToIdMap["blockman:glowstone"] = 89;
	itemNameToIdMap["blockman:portal"] = 90;
	itemNameToIdMap["blockman:lit_pumpkin"] = 91;
	itemNameToIdMap["blockman:cake"] = 92;
	itemNameToIdMap["blockman:unpowered_repeater"] = 93;
	itemNameToIdMap["blockman:powered_repeater"] = 94;
	itemNameToIdMap["blockman:stained_glass"] = 95;
	itemNameToIdMap["blockman:trapdoor"] = 96;
	itemNameToIdMap["blockman:monster_egg"] = 97;
	itemNameToIdMap["blockman:stonebrick"] = 98;
	itemNameToIdMap["blockman:brown_mushroom_block"] = 99;
	itemNameToIdMap["blockman:red_mushroom_block"] = 100;
	itemNameToIdMap["blockman:iron_bars"] = 101;
	itemNameToIdMap["blockman:glass_pane"] = 102;
	itemNameToIdMap["blockman:melon_block"] = 103;
	itemNameToIdMap["blockman:pumpkin_stem"] = 104;
	itemNameToIdMap["blockman:melon_stem"] = 105;
	itemNameToIdMap["blockman:vine"] = 106;
	itemNameToIdMap["blockman:fence_gate"] = 107;
	itemNameToIdMap["blockman:brick_stairs"] = 108;
	itemNameToIdMap["blockman:stone_brick_stairs"] = 109;
	itemNameToIdMap["blockman:mycelium"] = 110;
	itemNameToIdMap["blockman:waterlily"] = 111;
	itemNameToIdMap["blockman:nether_brick"] = 112;
	itemNameToIdMap["blockman:nether_brick_fence"] = 113;
	itemNameToIdMap["blockman:nether_brick_stairs"] = 114;
	itemNameToIdMap["blockman:nether_wart"] = 115;
	itemNameToIdMap["blockman:enchanting_table"] = 116;
	itemNameToIdMap["blockman:brewing_stand"] = 117;
	itemNameToIdMap["blockman:cauldron"] = 118;
	itemNameToIdMap["blockman:end_portal"] = 119;
	itemNameToIdMap["blockman:end_portal_frame"] = 120;
	itemNameToIdMap["blockman:end_stone"] = 121;
	itemNameToIdMap["blockman:dragon_egg"] = 122;
	itemNameToIdMap["blockman:redstone_lamp"] = 123;
	itemNameToIdMap["blockman:lit_redstone_lamp"] = 124;
	itemNameToIdMap["blockman:double_wooden_slab"] = 125;
	itemNameToIdMap["blockman:wooden_slab"] = 126;
	itemNameToIdMap["blockman:cocoa"] = 127;
	itemNameToIdMap["blockman:sandstone_stairs"] = 128;
	itemNameToIdMap["blockman:emerald_ore"] = 129;
	itemNameToIdMap["blockman:ender_chest"] = 130;
	itemNameToIdMap["blockman:tripwire_hook"] = 131;
	itemNameToIdMap["blockman:tripwire"] = 132;
	itemNameToIdMap["blockman:emerald_block"] = 133;
	itemNameToIdMap["blockman:spruce_stairs"] = 134;
	itemNameToIdMap["blockman:birch_stairs"] = 135;
	itemNameToIdMap["blockman:jungle_stairs"] = 136;
	itemNameToIdMap["blockman:command_block"] = 137;
	itemNameToIdMap["blockman:beacon"] = 138;
	itemNameToIdMap["blockman:cobblestone_wall"] = 139;
	itemNameToIdMap["blockman:flower_pot"] = 140;
	itemNameToIdMap["blockman:carrots"] = 141;
	itemNameToIdMap["blockman:potatoes"] = 142;
	itemNameToIdMap["blockman:wooden_button"] = 143;
	itemNameToIdMap["blockman:skull"] = 144;
	itemNameToIdMap["blockman:anvil"] = 145;
	itemNameToIdMap["blockman:trapped_chest"] = 146;
	itemNameToIdMap["blockman:light_weighted_pressure_plate"] = 147;
	itemNameToIdMap["blockman:heavy_weighted_pressure_plate"] = 148;
	itemNameToIdMap["blockman:unpowered_comparator"] = 149;
	itemNameToIdMap["blockman:powered_comparator"] = 150;
	itemNameToIdMap["blockman:daylight_detector"] = 151;
	itemNameToIdMap["blockman:redstone_block"] = 152;
	itemNameToIdMap["blockman:quartz_ore"] = 153;
	itemNameToIdMap["blockman:hopper"] = 154;
	itemNameToIdMap["blockman:quartz_block"] = 155;
	itemNameToIdMap["blockman:quartz_stairs"] = 156;
	itemNameToIdMap["blockman:activator_rail"] = 157;
	itemNameToIdMap["blockman:dropper"] = 158;
	itemNameToIdMap["blockman:stained_hardened_clay"] = 159;
	itemNameToIdMap["blockman:stained_glass_pane"] = 160;
	itemNameToIdMap["blockman:leaves2"] = 161;
	itemNameToIdMap["blockman:log2"] = 162;
	itemNameToIdMap["blockman:acacia_stairs"] = 163;
	itemNameToIdMap["blockman:dark_oak_stairs"] = 164;
	itemNameToIdMap["blockman:slime"] = 165;
	itemNameToIdMap["blockman:barrier"] = 166;
	itemNameToIdMap["blockman:iron_trapdoor"] = 167;
	itemNameToIdMap["blockman:sea_lantern"] = 168;
	itemNameToIdMap["blockman:hay_block"] = 170;
	itemNameToIdMap["blockman:carpet"] = 171;
	itemNameToIdMap["blockman:hardened_clay"] = 172;
	itemNameToIdMap["blockman:coal_block"] = 173;
	itemNameToIdMap["blockman:packed_ice"] = 174;
	itemNameToIdMap["blockman:double_plant"] = 175;
	itemNameToIdMap["blockman:standing_banner"] = 176;
	itemNameToIdMap["blockman:wall_banner"] = 177;
	itemNameToIdMap["blockman:daylight_detector_inverted"] = 178;
	itemNameToIdMap["blockman:red_sandstone"] = 179;
	itemNameToIdMap["blockman:red_sandstone_stairs"] = 180;
	itemNameToIdMap["blockman:double_stone_slab2"] = 181;
	itemNameToIdMap["blockman:stone_slab2"] = 182;
	itemNameToIdMap["blockman:spruce_fence_gate"] = 183;
	itemNameToIdMap["blockman:birch_fence_gate"] = 184;
	itemNameToIdMap["blockman:jungle_fence_gate"] = 185;
	itemNameToIdMap["blockman:dark_oak_fence_gate"] = 186;
	itemNameToIdMap["blockman:acacia_fence_gate"] = 187;
	itemNameToIdMap["blockman:spruce_fence"] = 188;
	itemNameToIdMap["blockman:birch_fence"] = 189;
	itemNameToIdMap["blockman:jungle_fence"] = 190;
	itemNameToIdMap["blockman:dark_oak_fence"] = 191;
	itemNameToIdMap["blockman:acacia_fence"] = 192;
	itemNameToIdMap["blockman:spruce_door"] = 193;
	itemNameToIdMap["blockman:birch_door"] = 194;
	itemNameToIdMap["blockman:jungle_door"] = 195;
	itemNameToIdMap["blockman:acacia_door"] = 196;
	itemNameToIdMap["blockman:dark_oak_door"] = 197;
	itemNameToIdMap["blockman:end_rod"] = 198;
	itemNameToIdMap["blockman:chorus_plant"] = 199;
	itemNameToIdMap["blockman:chorus_flower"] = 200;
	itemNameToIdMap["blockman:purpur_block"] = 201;
	itemNameToIdMap["blockman:purpur_pillar"] = 202;
	itemNameToIdMap["blockman:purpur_stairs"] = 203;
	itemNameToIdMap["blockman:purpur_double_slab"] = 204;
	itemNameToIdMap["blockman:purpur_slab"] = 205;
	itemNameToIdMap["blockman:end_bricks"] = 206;
	itemNameToIdMap["blockman:beetroots"] = 207;
	itemNameToIdMap["blockman:grass_path"] = 208;
	itemNameToIdMap["blockman:end_gateway"] = 209;
	itemNameToIdMap["blockman:repeating_command_block"] = 210;
	itemNameToIdMap["blockman:chain_command_block"] = 211;
	itemNameToIdMap["blockman:frosted_ice"] = 212;
	itemNameToIdMap["blockman:magma"] = 213;
	itemNameToIdMap["blockman:nether_wart_block"] = 214;
	itemNameToIdMap["blockman:red_nether_brick"] = 215;
	itemNameToIdMap["blockman:bone_block"] = 216;
	itemNameToIdMap["blockman:structure_void"] = 217;
	itemNameToIdMap["blockman:observer"] = 218;
	itemNameToIdMap["blockman:white_shulker_box"] = 219;
	itemNameToIdMap["blockman:orange_shulker_box"] = 220;
	itemNameToIdMap["blockman:magenta_shulker_box"] = 221;
	itemNameToIdMap["blockman:light_blue_shulker_box"] = 222;
	itemNameToIdMap["blockman:yellow_shulker_box"] = 223;
	itemNameToIdMap["blockman:lime_shulker_box"] = 224;
	itemNameToIdMap["blockman:pink_shulker_box"] = 225;
	itemNameToIdMap["blockman:gray_shulker_box"] = 226;
	itemNameToIdMap["blockman:silver_shulker_box"] = 227;
	itemNameToIdMap["blockman:cyan_shulker_box"] = 228;
	itemNameToIdMap["blockman:purple_shulker_box"] = 229;
	itemNameToIdMap["blockman:blue_shulker_box"] = 230;
	itemNameToIdMap["blockman:brown_shulker_box"] = 231;
	itemNameToIdMap["blockman:green_shulker_box"] = 232;
	itemNameToIdMap["blockman:red_shulker_box"] = 233;
	itemNameToIdMap["blockman:black_shulker_box"] = 234;
	itemNameToIdMap["blockman:white_glazed_terracotta"] = 235;
	itemNameToIdMap["blockman:orange_glazed_terracotta"] = 236;
	itemNameToIdMap["blockman:magenta_glazed_terracotta"] = 237;
	itemNameToIdMap["blockman:light_blue_glazed_terracotta"] = 238;
	itemNameToIdMap["blockman:yellow_glazed_terracotta"] = 239;
	itemNameToIdMap["blockman:lime_glazed_terracotta"] = 240;
	itemNameToIdMap["blockman:pink_glazed_terracotta"] = 241;
	itemNameToIdMap["blockman:gray_glazed_terracotta"] = 242;
	itemNameToIdMap["blockman:silver_glazed_terracotta"] = 243;
	itemNameToIdMap["blockman:cyan_glazed_terracotta"] = 244;
	itemNameToIdMap["blockman:purple_glazed_terracotta"] = 245;
	itemNameToIdMap["blockman:blue_glazed_terracotta"] = 246;
	itemNameToIdMap["blockman:brown_glazed_terracotta"] = 247;
	itemNameToIdMap["blockman:green_glazed_terracotta"] = 248;
	itemNameToIdMap["blockman:red_glazed_terracotta"] = 249;
	itemNameToIdMap["blockman:black_glazed_terracotta"] = 250;
	itemNameToIdMap["blockman:concrete"] = 251;
	itemNameToIdMap["blockman:concrete_powder"] = 252;

	itemNameToIdMap["blockman:iron_shovel"] = 256;
	itemNameToIdMap["blockman:iron_pickaxe"] = 257;
	itemNameToIdMap["blockman:iron_axe"] = 258;
	itemNameToIdMap["blockman:flint_and_steel"] = 259;
	itemNameToIdMap["blockman:apple"] = 260;
	itemNameToIdMap["blockman:bow"] = 261;
	itemNameToIdMap["blockman:arrow"] = 262;
	itemNameToIdMap["blockman:coal"] = 263;
	itemNameToIdMap["blockman:diamond"] = 264;
	itemNameToIdMap["blockman:iron_ingot"] = 265;
	itemNameToIdMap["blockman:gold_ingot"] = 266;
	itemNameToIdMap["blockman:iron_sword"] = 267;
	itemNameToIdMap["blockman:wooden_sword"] = 268;
	itemNameToIdMap["blockman:wooden_shovel"] = 269;
	itemNameToIdMap["blockman:wooden_pickaxe"] = 270;
	itemNameToIdMap["blockman:wooden_axe"] = 271;
	itemNameToIdMap["blockman:stone_sword"] = 272;
	itemNameToIdMap["blockman:stone_shovel"] = 273;
	itemNameToIdMap["blockman:stone_pickaxe"] = 274;
	itemNameToIdMap["blockman:stone_axe"] = 275;
	itemNameToIdMap["blockman:diamond_sword"] = 276;
	itemNameToIdMap["blockman:diamond_shovel"] = 277;
	itemNameToIdMap["blockman:diamond_pickaxe"] = 278;
	itemNameToIdMap["blockman:diamond_axe"] = 279;
	itemNameToIdMap["blockman:stick"] = 280;
	itemNameToIdMap["blockman:bowl"] = 281;
	itemNameToIdMap["blockman:mushroom_stew"] = 282;
	itemNameToIdMap["blockman:golden_sword"] = 283;
	itemNameToIdMap["blockman:golden_shovel"] = 284;
	itemNameToIdMap["blockman:golden_pickaxe"] = 285;
	itemNameToIdMap["blockman:golden_axe"] = 286;
	itemNameToIdMap["blockman:string"] = 287;
	itemNameToIdMap["blockman:feather"] = 288;
	itemNameToIdMap["blockman:gunpowder"] = 289;
	itemNameToIdMap["blockman:wooden_hoe"] = 290;
	itemNameToIdMap["blockman:stone_hoe"] = 291;
	itemNameToIdMap["blockman:iron_hoe"] = 292;
	itemNameToIdMap["blockman:diamond_hoe"] = 293;
	itemNameToIdMap["blockman:golden_hoe"] = 294;
	itemNameToIdMap["blockman:wheat_seeds"] = 295;
	itemNameToIdMap["blockman:wheat"] = 296;
	itemNameToIdMap["blockman:bread"] = 297;
	itemNameToIdMap["blockman:leather_helmet"] = 298;
	itemNameToIdMap["blockman:leather_chestplate"] = 299;
	itemNameToIdMap["blockman:leather_leggings"] = 300;
	itemNameToIdMap["blockman:leather_boots"] = 301;
	itemNameToIdMap["blockman:chainmail_helmet"] = 302;
	itemNameToIdMap["blockman:chainmail_chestplate"] = 303;
	itemNameToIdMap["blockman:chainmail_leggings"] = 304;
	itemNameToIdMap["blockman:chainmail_boots"] = 305;
	itemNameToIdMap["blockman:iron_helmet"] = 306;
	itemNameToIdMap["blockman:iron_chestplate"] = 307;
	itemNameToIdMap["blockman:iron_leggings"] = 308;
	itemNameToIdMap["blockman:iron_boots"] = 309;
	itemNameToIdMap["blockman:diamond_helmet"] = 310;
	itemNameToIdMap["blockman:diamond_chestplate"] = 311;
	itemNameToIdMap["blockman:diamond_leggings"] = 312;
	itemNameToIdMap["blockman:diamond_boots"] = 313;
	itemNameToIdMap["blockman:golden_helmet"] = 314;
	itemNameToIdMap["blockman:golden_chestplate"] = 315;
	itemNameToIdMap["blockman:golden_leggings"] = 316;
	itemNameToIdMap["blockman:golden_boots"] = 317;
	itemNameToIdMap["blockman:flint"] = 318;
	itemNameToIdMap["blockman:porkchop"] = 319;
	itemNameToIdMap["blockman:cooked_porkchop"] = 320;
	itemNameToIdMap["blockman:painting"] = 321;
	itemNameToIdMap["blockman:golden_apple"] = 322;
	itemNameToIdMap["blockman:sign"] = 323;
	itemNameToIdMap["blockman:wooden_door"] = 324;
	itemNameToIdMap["blockman:bucket"] = 325;
	itemNameToIdMap["blockman:water_bucket"] = 326;
	itemNameToIdMap["blockman:lava_bucket"] = 327;
	itemNameToIdMap["blockman:blockman"] = 328;
	itemNameToIdMap["blockman:saddle"] = 329;
	itemNameToIdMap["blockman:iron_door"] = 330;
	itemNameToIdMap["blockman:redstone"] = 331;
	itemNameToIdMap["blockman:snowball"] = 332;
	itemNameToIdMap["blockman:boat"] = 333;
	itemNameToIdMap["blockman:leather"] = 334;
	itemNameToIdMap["blockman:milk_bucket"] = 335;
	itemNameToIdMap["blockman:brick"] = 336;
	itemNameToIdMap["blockman:clay_ball"] = 337;
	itemNameToIdMap["blockman:reeds"] = 338;
	itemNameToIdMap["blockman:paper"] = 339;
	itemNameToIdMap["blockman:book"] = 340;
	itemNameToIdMap["blockman:slime_ball"] = 341;
	itemNameToIdMap["blockman:blockmanChest"] = 342;
	itemNameToIdMap["blockman:blockmanFurnace"] = 343;
	itemNameToIdMap["blockman:egg"] = 344;
	itemNameToIdMap["blockman:compass"] = 345;
	itemNameToIdMap["blockman:fishing_rod"] = 346;
	itemNameToIdMap["blockman:clock"] = 347;
	itemNameToIdMap["blockman:glowstone_dust"] = 348;
	itemNameToIdMap["blockman:fish"] = 349;
	itemNameToIdMap["blockman:cooked_fish"] = 350;
	itemNameToIdMap["blockman:dye"] = 351;
	itemNameToIdMap["blockman:bone"] = 352;
	itemNameToIdMap["blockman:sugar"] = 353;
	itemNameToIdMap["blockman:cake"] = 354;
	itemNameToIdMap["blockman:bed"] = 355;
	itemNameToIdMap["blockman:repeater"] = 356;
	itemNameToIdMap["blockman:cookie"] = 357;
	itemNameToIdMap["blockman:filled_map"] = 358;
	itemNameToIdMap["blockman:shears"] = 359;
	itemNameToIdMap["blockman:melon"] = 360;
	itemNameToIdMap["blockman:pumpkin_seeds"] = 361;
	itemNameToIdMap["blockman:melon_seeds"] = 362;
	itemNameToIdMap["blockman:beef"] = 363;
	itemNameToIdMap["blockman:cooked_beef"] = 364;
	itemNameToIdMap["blockman:chicken"] = 365;
	itemNameToIdMap["blockman:cooked_chicken"] = 366;
	itemNameToIdMap["blockman:rotten_flesh"] = 367;
	itemNameToIdMap["blockman:ender_pearl"] = 368;
	itemNameToIdMap["blockman:blaze_rod"] = 369;
	itemNameToIdMap["blockman:ghast_tear"] = 370;
	itemNameToIdMap["blockman:gold_nugget"] = 371;
	itemNameToIdMap["blockman:nether_wart"] = 372;
	itemNameToIdMap["blockman:potion"] = 373;
	itemNameToIdMap["blockman:glass_bottle"] = 374;
	itemNameToIdMap["blockman:spider_eye"] = 375;
	itemNameToIdMap["blockman:fermented_spider_eye"] = 376;
	itemNameToIdMap["blockman:blaze_powder"] = 377;
	itemNameToIdMap["blockman:magma_cream"] = 378;
	itemNameToIdMap["blockman:brewing_stand"] = 379;
	itemNameToIdMap["blockman:cauldron"] = 380;
	itemNameToIdMap["blockman:ender_eye"] = 381;
	itemNameToIdMap["blockman:speckled_melon"] = 382;
	itemNameToIdMap["blockman:spawn_egg"] = 383;
	itemNameToIdMap["blockman:experience_bottle"] = 384;
	itemNameToIdMap["blockman:fire_charge"] = 385;
	itemNameToIdMap["blockman:writable_book"] = 386;
	itemNameToIdMap["blockman:written_book"] = 387;
	itemNameToIdMap["blockman:emerald"] = 388;
	itemNameToIdMap["blockman:item_frame"] = 389;
	itemNameToIdMap["blockman:flower_pot"] = 390;
	itemNameToIdMap["blockman:carrot"] = 391;
	itemNameToIdMap["blockman:potato"] = 392;
	itemNameToIdMap["blockman:baked_potato"] = 393;
	itemNameToIdMap["blockman:poisonous_potato"] = 394;
	itemNameToIdMap["blockman:map"] = 395;
	itemNameToIdMap["blockman:golden_carrot"] = 396;
	itemNameToIdMap["blockman:skull"] = 397;
	itemNameToIdMap["blockman:carrot_on_a_stick"] = 398;
	itemNameToIdMap["blockman:nether_star"] = 399;
	itemNameToIdMap["blockman:pumpkin_pie"] = 400;
	itemNameToIdMap["blockman:fireworks"] = 401;
	itemNameToIdMap["blockman:firework_charge"] = 402;
	itemNameToIdMap["blockman:enchanted_book"] = 403;
	itemNameToIdMap["blockman:comparator"] = 404;
	itemNameToIdMap["blockman:netherbrick"] = 405;
	itemNameToIdMap["blockman:quartz"] = 406;
	itemNameToIdMap["blockman:blockmanTnt"] = 407;
	itemNameToIdMap["blockman:hopper_blockman"] = 408;
	itemNameToIdMap["blockman:prismarine_shard"] = 409;
	itemNameToIdMap["blockman:prismarine_crystals"] = 410;
	itemNameToIdMap["blockman:rabbit"] = 411;
	itemNameToIdMap["blockman:cooked_rabbit"] = 412;
	itemNameToIdMap["blockman:rabbit_stew"] = 413;
	itemNameToIdMap["blockman:rabbit_foot"] = 414;
	itemNameToIdMap["blockman:rabbit_hide"] = 415;
	itemNameToIdMap["blockman:armor_stand"] = 416;
	itemNameToIdMap["blockman:iron_horse_armor"] = 417;
	itemNameToIdMap["blockman:golden_horse_armor"] = 418;
	itemNameToIdMap["blockman:diamond_horse_armor"] = 419;
	itemNameToIdMap["blockman:lead"] = 420;
	itemNameToIdMap["blockman:name_tag"] = 421;
	itemNameToIdMap["blockman:command_block_minecart"] = 422;
	itemNameToIdMap["blockman:mutton"] = 423;
	itemNameToIdMap["blockman:cooked_mutton"] = 424;
	itemNameToIdMap["blockman:banner"] = 425;
	itemNameToIdMap["blockman:end_crystal"] = 426;
	itemNameToIdMap["blockman:spruce_door"] = 427;
	itemNameToIdMap["blockman:birch_door"] = 510;
	itemNameToIdMap["blockman:jungle_door"] = 429;
	itemNameToIdMap["blockman:acacia_door"] = 430;
	itemNameToIdMap["blockman:dark_oak_door"] = 431;
	itemNameToIdMap["blockman:chorus_fruit"] = 432;
	itemNameToIdMap["blockman:chorus_fruit_popped"] = 433;
	itemNameToIdMap["blockman:beetroot"] = 434;
	itemNameToIdMap["blockman:beetroot_seeds"] = 435;
	itemNameToIdMap["blockman:beetroot_soup"] = 436;
	itemNameToIdMap["blockman:dragon_breath"] = 437;
	itemNameToIdMap["blockman:splash_potion"] = 438;
	itemNameToIdMap["blockman:spectral_arrow"] = 439;
	itemNameToIdMap["blockman:tipped_arrow"] = 440;
	itemNameToIdMap["blockman:lingering_potion"] = 441;
	itemNameToIdMap["blockman:shield"] = 442;
	itemNameToIdMap["blockman:elytra"] = 443;
	itemNameToIdMap["blockman:spruce_boat"] = 444;
	itemNameToIdMap["blockman:birch_boat"] = 445;
	itemNameToIdMap["blockman:jungle_boat"] = 446;
	itemNameToIdMap["blockman:acacia_boat"] = 447;
	itemNameToIdMap["blockman:dark_oak_boat"] = 448;
	itemNameToIdMap["blockman:totem_of_undying"] = 449;
	itemNameToIdMap["blockman:shulker_shell"] = 450;
	itemNameToIdMap["blockman:iron_nugget"] = 452;

	itemNameToIdMap["blockman:allium"] = 1351 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:blue_orchid"] = 1352 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:houstonia"] = 1353 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:oxeye_daisy"] = 1354 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:paeonia"] = 1355 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:tulip_pink"] = 1356 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:tulip_red"] = 1357 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:tulip_white"] = 1358 + EC_BLOCK_TO_ITEM_OFFSET;

	itemNameToIdMap["blockman:prismarine"] = 1419 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:prismarine_bricks"] = 1420 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:prismarine_dark"] = 1421 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:starry_sky"] = 1422 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:stairs_prismarine"] = 1423 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:stairs_prismarine_bricks"] = 1424 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:stairs_prismarine_dark"] = 1425 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:stairs_red_nether_brick"] = 1426 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:prismarineDoubleSlab"] = 1427 + EC_BLOCK_TO_ITEM_OFFSET;
	itemNameToIdMap["blockman:prismarineSingleSlab"] = 1428 + EC_BLOCK_TO_ITEM_OFFSET;

	itemNameToIdMap["blockman:boneMeal"] = 1500;
}

void Item::unInitialize()
{
	for (int i = 0; i < MAX_ITEM_COUNT; ++i)
	{
		LordSafeDelete(itemsList[i]);
	}
	LordSafeDelete(itemRand);
}

Item::Item(int id, const String& name)
	: maxStackSize(64)
	, maxDamage(0)
	, hasSubtypes(false)
	, containerItem(NULL)
	, unlocalizedName(name)
{
	itemID = 256 + id;

	if (itemsList[256 + id] != NULL)
	{
		LordLogError("Item CONFLICT %d", id);
	}

	itemsList[256 + id] = this;


	if (CustomArmorSetting::Instance()->isCustomArmorByItemId(itemID))
	{
		m_modifierAttr = LordNew AttributeModifier(ATTACK_DAMAGE_MODIFIER, "Item modifier", 0, 0);
	}
}

bool Item::onItemUse(ItemStackPtr pStack, EntityPlayer* pPlayer, World* pWorld, const BlockPos& pos, int face, const Vector3& hit)
{
	return false;
}

String Item::getLocalizedName(ItemStackPtr pStack)
{
	return getUnlocalizedName(pStack);
	// return result == StringUtil::BLANK ? "" : StatCollector::translateToLocal(var2);
}

String Item::getUnlocalizedName(ItemStackPtr pStack)
{
	if (isCustomItem)
		return StringUtil::Replace(unlocalizedName, ".name", "");
	return "item." + unlocalizedName;
}

String Item::getStatName()
{
	// return StatCollector.translateToLocal(getUnlocalizedName() + ".name");
	return getUnlocalizedName();
}

String Item::getItemStackDisplayName(ItemStackPtr pStack)
{
	// return StatCollector.translateToLocal(getUnlocalizedName(pStack) + ".name");
	return getUnlocalizedName();
}

String Item::getItemDisplayName(ItemStackPtr pStack)
{
	// return ("" + StatCollector.translateToLocal(getLocalizedName(pStack) + ".name")).trim();
	return getLocalizedName(pStack);
}

bool Item::hasEffect(ItemStackPtr pStack)
{
	return pStack->isItemEnchanted();
}

RARITY_TYPE Item::getRarity(ItemStackPtr par1ItemStack)
{
	return par1ItemStack->isItemEnchanted() ? RARITY_TYPE_RARE : RARITY_TYPE_COMMON;
}

vector<Vector3i>::type Item::getPlacePositions(World * pWorld, BlockPos& pos, int face)
{
	vector<Vector3i>::type positions = vector<Vector3i>::type();
	int blockID = pWorld->getBlockId(pos);
	if (blockID == BLOCK_ID_SNOW)
	{
		face = BM_FACE_UP;
	}
	else if (blockID != BLOCK_ID_VINE && blockID != BLOCK_ID_TALL_GRASS && blockID != BLOCK_ID_DEADBUSH)
	{
		switch (face)
		{
		case BM_FACE_DOWN:	--pos.y; break;
		case BM_FACE_UP:	++pos.y; break;
		case BM_FACE_NORTH: --pos.z; break;
		case BM_FACE_SOUTH: ++pos.z; break;
		case BM_FACE_WEST:	--pos.x; break;
		case BM_FACE_EAST:	++pos.x; break;
		default: break;
		}
	}
	positions.push_back(pos);
	return positions;
}

String Item::getClassName()
{
	String className = typeid(*this).name();
	auto pos = className.find("Item");
	if (pos < 0 || pos >= className.length())
		return className;
#if LORD_PLATFORM == LORD_PLATFORM_WINDOWS
	className = className.substr(pos, className.length() - pos);
#else
	className = className.substr(pos, className.length() - pos - 1);
#endif
	return className;
}

RayTraceResult Item::getMovingObjectPositionFromPlayer(World* pWorld, EntityPlayer* pPlayer, bool useLiquids)
{
	float ticktime = 1.0F;
	float dpitch = pPlayer->prevRotationPitch + (pPlayer->rotationPitch - pPlayer->prevRotationPitch) * ticktime;
	float dyaw = pPlayer->prevRotationYaw + (pPlayer->rotationYaw - pPlayer->prevRotationYaw) * ticktime;
	float posx = pPlayer->prevPos.x + (pPlayer->position.x - pPlayer->prevPos.x) * ticktime;
	float posy = pPlayer->prevPos.y + (pPlayer->position.y - pPlayer->prevPos.y) * ticktime + 1.62f - pPlayer->yOffset;
	float posz = pPlayer->prevPos.z + (pPlayer->position.z - pPlayer->prevPos.z) * ticktime;
	Vector3 start(posx, posy, posz);
	
	Vector3 end = start + pPlayer->getLook(ticktime) * 5.0f;
	return pWorld->rayTraceBlocks(start, end, useLiquids, !useLiquids);
}

Item * Item::getItemByName(const String & name)
{
	auto iter = itemNameToIdMap.find(name);
	if (iter == itemNameToIdMap.end())
	{
		return nullptr;
	}
	return itemsList[iter->second];
}

Item* Item::getItemById(int id)
{
	if (id < 0 || id >= MAX_ITEM_COUNT)
	{
		LordLogError("getItemById out of range!!! id=%d", id);
		return NULL;
	}
	return itemsList[id];
}
}
