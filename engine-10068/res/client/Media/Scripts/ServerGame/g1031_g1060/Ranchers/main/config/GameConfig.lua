require "config.AreaConfig"
require "config.ManorConfig"
require "config.ShopConfig"
require "config.PlayerLevelConfig"
require "config.HardnessConfig"
require "config.FieldLevelConfig"
require "config.WareHouseConfig"
require "config.ItemsMappingConfig"
require "config.ItemsConfig"
require "config.SessionNpcConfig"
require "config.BuildingConfig"
require "config.HouseConfig"
require "config.AchievementConfig"
require "config.SeedLevelConfig"
require "config.TasksConfig"
require "config.TasksLevelConfig"
require "config.GiftConfig"
require "config.GiftLevelConfig"
require "config.TimePaymentConfig"
require "config.MailLanguageConfig"
require "config.AccelerateItemsConfig"
require "config.PlayerInitItemsConfig"
require "config.VehicleConfig"


GameConfig = {}

GameConfig.initPos = {}

function GameConfig:init(config)
    self.basisExpend = tonumber(config.basisExpend)
    self.increasingExpend = tonumber(config.increasingExpend)
    self.taskStartLevel = tonumber(config.taskStartLevel)
    self.exploreStartLevel = tonumber(config.exploreStartLevel)
    self.boardCastStayTime = tonumber(config.boardCastStayTime)
    self.boardCastRollTime = tonumber(config.boardCastRollTime)
    self.playerInitMoney = tonumber(config.playerInitMoney)
    self.manorFileName = config.manorFileName

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    AreaConfig:initArea(FileUtil.getConfigFromCsv("area.csv"))
    AreaConfig:initExpandConfig(FileUtil.getConfigFromCsv("areaExpand.csv"))
    ManorConfig:initManor(FileUtil.getConfigFromCsv("manor.csv"))
    PlayerLevelConfig:init(FileUtil.getConfigFromCsv("playerLevel.csv"))
    HardnessConfig:init(FileUtil.getConfigFromCsv("hardness.csv"))
    FieldLevelConfig:init(FileUtil.getConfigFromCsv("fieldLevel.csv"))
    WareHouseConfig:init(FileUtil.getConfigFromCsv("wareHouse.csv"))
    ItemsMappingConfig:init(FileUtil.getConfigFromCsv("itemsMapping.csv"))
    ItemsConfig:init(FileUtil.getConfigFromCsv("items.csv"))
    SessionNpcConfig:initNpc(FileUtil.getConfigFromCsv("sessionNpc.csv"))
    BuildingConfig:init(FileUtil.getConfigFromCsv("building.csv"))
    HouseConfig:init(FileUtil.getConfigFromCsv("house.csv"))
    AchievementConfig:initAchievement(FileUtil.getConfigFromCsv("achievement.csv"))
    SeedLevelConfig:initSeedLevel(FileUtil.getConfigFromCsv("seedLevel.csv"))
    TasksConfig:initTasks(FileUtil.getConfigFromCsv("tasks.csv"))
    TasksLevelConfig:initTasksLevel(FileUtil.getConfigFromCsv("tasksLevel.csv"))
    GiftConfig:init(FileUtil.getConfigFromCsv("gift.csv"))
    GiftLevelConfig:init(FileUtil.getConfigFromCsv("giftLevel.csv"))
    TimePaymentConfig:initPayment(FileUtil.getConfigFromCsv("timePayment.csv"))
    MailLanguageConfig:init(FileUtil.getConfigFromCsv("mailLanguage.csv"))
    AccelerateItemsConfig:init(FileUtil.getConfigFromCsv("accelerateItems.csv"))
    PlayerInitItemsConfig:init(FileUtil.getConfigFromCsv("playerInitItems.csv"))
    VehicleConfig:init(FileUtil.getConfigFromCsv("vehicle.csv"))
    PlayerInitItemsConfig:initInventory(config.playerInitInventory)
    ShopConfig:init(config.shop)
    AreaConfig:initUnlockedArea(config.initUnlockedArea)
    AreaConfig:initNpcName(config.landNpcName)
    AreaConfig:initNpcActorName(config.landNpcActorName)
    AreaConfig:initWaitForUnlockNpcName(config.waitForUnlockLandNpcName)
    AreaConfig:initWaitForUnlockNpcActorName(config.waitForUnlockLandNpcActorName)
    ManorConfig:initServiceCenter(config.serviceCenter)
    ManorConfig:initFieldPos(config.initFieldPos)
    ManorConfig:initRoadsPos(config.initRoadsPos)
    WareHouseConfig:initWareHousePos(config.wareHouseInitPos)
    BuildingConfig:initAnimalBuildingPos(config.animalBuildingInitPos)
    HouseConfig:initHousePos(config.houseInitPos)
end

return GameConfig