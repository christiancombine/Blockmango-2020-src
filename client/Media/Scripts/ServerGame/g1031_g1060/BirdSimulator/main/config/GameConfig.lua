require "manager.SceneManager"
require "config.NestConfig"
require "config.PaymentConfig"
require "config.VipConfig"
require "config.BonusConfig"
require "config.BirdConfig"
require "config.MonsterConfig"
require "config.FieldConfig"
require "config.ChestConfig"
require "config.TaskConfig"
require "config.ToolConfig"
require "config.BackpackConfig"
require "config.ShopConfig"
require "config.InitItemsConfig"
require "config.StoreHouseConfig"
require "config.StoreHouseItems"
require "config.BirdBagConfig"
require "config.ActorNpcConfig"
require "config.CannonConfig"
require "config.RankConfig"
require "config.TreeConfig"
require "config.WingsConfig"
require "config.EggChestConfig"
require "config.CheckInConfig"

GameConfig = {}

GameConfig.initPos = {}

function GameConfig:init(config)
    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.hp = config.hp
    GameConfig.moveSpeed = config.moveSpeed
    GameConfig.jump = config.jump
    GameConfig.withoutHurtSecond = config.withoutHurtSecond
    GameConfig.hpRecoverCD = config.hpRecoverCD
    GameConfig.hpRecover = config.hpRecover
    GameConfig.awayFromBattleCD = config.awayFromBattleCD
    GameConfig.convertRate = config.convertRate
    GameConfig.initMaxCarryBirdNum = config.initMaxCarryBirdNum
    GameConfig.initMaxCapacityBirdNum = config.initMaxCapacityBirdNum
    GameConfig.flyHigh = config.flyHigh
    GameConfig.arrowPointToNpc = config.arrowPointToNpc
    GameConfig.drawLuckyTaskId = config.drawLuckyTaskId
    GameConfig.drawLuckyPoint = VectorUtil.newVector3(config.drawLuckyPoint[1],config.drawLuckyPoint[2],config.drawLuckyPoint[3])
    GameConfig.arrowPointToNpcPos = VectorUtil.newVector3(config.arrowPointToNpcPos[1], config.arrowPointToNpcPos[2],config.arrowPointToNpcPos[3])
    GameConfig.shopTaskId = config.shopTaskId
    GameConfig.arrowPointToShopPos = VectorUtil.newVector3(config.arrowPointToShopPos[1], config.arrowPointToShopPos[2],config.arrowPointToShopPos[3])
    GameConfig.treeGenerateCD = config.treeGenerateCD
    GameConfig.treeRemainTime = config.treeRemainTime
    GameConfig.treeRespawnCD = config.treeRespawnCD
    GameConfig.addCarryTo3TaskId = config.addCarryTo3TaskId
    GameConfig.addCarryTo4TaskId = config.addCarryTo4TaskId
    GameConfig.feedTaskId = config.feedTaskId
    GameConfig.moneyAdRefreshTime = config.moneyAdRefreshTime
    GameConfig.eggTicketAdRefreshTime = config.eggTicketAdRefreshTime
    GameConfig.chestCDAdRefreshTime = config.chestCDAdRefreshTime
    GameConfig.vipChestADRefreshTime = config.vipChestADRefreshTime
    GameConfig.feedAdRefreshTime = config.feedAdRefreshTime
    GameConfig.moneyAdReward = config.moneyAdReward
    GameConfig.eggTicketAdReward = config.eggTicketAdReward
    GameConfig.feedAdReward = config.feedAdReward
    FieldConfig:initVipArea(config.vipArea)
    FieldConfig:initGuideShopArea(config.arrowShopArea)
    NestConfig:init(FileUtil.getConfigFromCsv("nest.csv"))
    NestConfig:initIndex(FileUtil.getConfigFromCsv("nestIndex.csv"))
    PaymentConfig:initPayment(FileUtil.getConfigFromCsv("payment.csv"))
    VipConfig:init(FileUtil.getConfigFromCsv("vip.csv"))
    BonusConfig:initBonusPools(FileUtil.getConfigFromCsv("bonusPools.csv"))
    BonusConfig:initBonusTeams(FileUtil.getConfigFromCsv("bonusTeams.csv"))
    BonusConfig:initBonusItems(FileUtil.getConfigFromCsv("bonusItems.csv"))
    BonusConfig:initNpc(FileUtil.getConfigFromCsv("bonusNpc.csv"))
    BirdConfig:initBirds(FileUtil.getConfigFromCsv("bird.csv"))
    BirdConfig:initCombine(FileUtil.getConfigFromCsv("birdCombine.csv"))
    BirdConfig:initRate(FileUtil.getConfigFromCsv("birdRate.csv"))
    BirdConfig:initLevel(FileUtil.getConfigFromCsv("birdLevel.csv"))
    BirdConfig:initGifts(FileUtil.getConfigFromCsv("birdGifts.csv"))
    BirdConfig:initParts(FileUtil.getConfigFromCsv("birdParts.csv"))
    BirdConfig:initFoods(FileUtil.getConfigFromCsv("birdFoods.csv"))
    MonsterConfig:init(FileUtil.getConfigFromCsv("monster.csv"))
    FieldConfig:initField(FileUtil.getConfigFromCsv("field.csv"))
    FieldConfig:initDoor(FileUtil.getConfigFromCsv("door.csv"))
    ChestConfig:init(FileUtil.getConfigFromCsv("chest.csv"))
    TaskConfig:initMainTasks(FileUtil.getConfigFromCsv("task.csv"))
    TaskConfig:initNpc(FileUtil.getConfigFromCsv("taskNpc.csv"))
    TaskConfig:initContents(FileUtil.getConfigFromCsv("taskContent.csv"))
    TaskConfig:initTaskRequirement(FileUtil.getConfigFromCsv("taskRequirement.csv"))
    TaskConfig:initDailyTaskConfigs(FileUtil.getConfigFromCsv("taskDaily.csv"))
    BackpackConfig:init(FileUtil.getConfigFromCsv("backpack.csv"))
    ToolConfig:init(FileUtil.getConfigFromCsv("tool.csv"))
    ShopConfig:initLabels(FileUtil.getConfigFromCsv("shopLabels.csv"))
    ShopConfig:initTeams(FileUtil.getConfigFromCsv("shopTeams.csv"))
    ShopConfig:initItems(FileUtil.getConfigFromCsv("shopItems.csv"))
    InitItemsConfig:initItems(FileUtil.getConfigFromCsv("initItems.csv"))
    StoreHouseItems:Init(FileUtil.getConfigFromCsv("storeHouseItems.csv"))
    StoreHouseConfig:Init(FileUtil.getConfigFromCsv("storeHouseConfig.csv"))
    BirdBagConfig:initCapacityBag(FileUtil.getConfigFromCsv("birdCapacityBag.csv"))
    BirdBagConfig:initCarryBag(FileUtil.getConfigFromCsv("birdCarryBag.csv"))
    ActorNpcConfig:init(FileUtil.getConfigFromCsv("actorNpc.csv"))
    CannonConfig:init(FileUtil.getConfigFromCsv("cannon.csv"))
    RankConfig:init(FileUtil.getConfigFromCsv("rank.csv"))
    FieldConfig:initDoorTip(FileUtil.getConfigFromCsv("doorTip.csv"))
    TreeConfig:init(FileUtil.getConfigFromCsv("tree.csv"))
    TreeConfig:initRewards(FileUtil.getConfigFromCsv("treeReward.csv"))
    FieldConfig:initTreeWeight(FileUtil.getConfigFromCsv("fieldWeight.csv"))
    WingsConfig:init(FileUtil.getConfigFromCsv("wing.csv"))
    EggChestConfig:init(FileUtil.getConfigFromCsv("eggChest.csv"))
    CheckInConfig:init(FileUtil.getConfigFromCsv("checkIn.csv"))
end

return GameConfig