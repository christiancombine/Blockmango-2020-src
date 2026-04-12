--GameConfig.lua

GameConfig = {}
GameConfig.WorldInfo = {
    DayTime = 30,
    NightTime = 30,
    ChangeTime = 3
}
GameConfig.randomPos = {}
GameConfig.randomPos_totalWeight = 0

function GameConfig:init(config)
    self:parseGameConfig(config)

    MonsterRuleConfig:init(FileUtil.getConfigFromCsv("setting/MonsterRule.csv"))
    MonsterRegionConfig:init(FileUtil.getConfigFromCsv("setting/MonsterRegion.csv"))
    MonsterRegionConfig:InitRefreshMonsterPos(FileUtil.getConfigFromCsv("setting/MapAreaMonsterConfigResult.csv"))
    MonsterGroupConfig:init(FileUtil.getConfigFromCsv("setting/MonsterGroup.csv"))
    MonsterConfig:init(FileUtil.getConfigFromCsv("setting/Monster.csv", 3))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/RewardSetting.csv", 3))
    GunConfig:init(FileUtil.getConfigFromCsv("setting/GunConfig.csv"))
    PropertyConfig:initConfig(FileUtil.getConfigFromCsv("setting/PropertyConfig.csv"))
    BuffConfig:init(FileUtil.getConfigFromCsv("setting/BuffConfig.csv"))
    ItemsConfig:init(FileUtil.getConfigFromCsv("setting/ItemsConfig.csv"))
    SupplyPoolConfig:initSupplySetting(FileUtil.getConfigFromCsv("setting/SupplySetting.csv"))
    SupplyPoolConfig:initItem(FileUtil.getConfigFromCsv("setting/SupplyPool.csv"))
    SupplyAreaConfig:initArea(FileUtil.getConfigFromCsv("setting/SupplyArea.csv"))
    SupplyAreaConfig:initPosition(FileUtil.getConfigFromCsv("setting/MapAreaConfigResult.csv"))
    FormulationConfig:init(FileUtil.getConfigFromCsv("setting/FormulationSetting.csv", 3))
    MaterialConfig:init(FileUtil.getConfigFromCsv("setting/MaterialSetting.csv", 3))
    RobotConfig:init(FileUtil.getConfigFromCsv("setting/Robot.csv"))
    TitleConfig:init(FileUtil.getConfigFromCsv("setting/TitleSetting.csv", 3))
    ArmorConfig:init(FileUtil.getConfigFromCsv("setting/ArmorConfig.csv", 3))
    TitleConfig:initType(FileUtil.getConfigFromCsv("setting/TitleTypeSetting.csv", 3))
    MusicBoxConfig:init(FileUtil.getConfigFromCsv("setting/MusicBox.csv"))
    StoreGoodsConfig:initConfig(FileUtil.getConfigFromCsv("setting/StoreGoods.csv", 3))
    SupplyBoxConfig:initConfig(FileUtil.getConfigFromCsv("setting/SupplyChest.csv", 3))
    GoodsRandomPoolConfig:initConfig(FileUtil.getConfigFromCsv("setting/GoodsRandomPool.csv"))
    MerchantConfig:initConfig(FileUtil.getConfigFromCsv("setting/Merchant.csv"))
    TravellerConfig:initConfig(FileUtil.getConfigFromCsv("setting/Traveller.csv"))
    GradeConfig:init(FileUtil.getConfigFromCsv("setting/Grade.csv"))
    PlayerItemInitConfig:init(FileUtil.getConfigFromCsv("setting/PlayerItemInit.csv"))
    PlayerDropItemProbabilityConfig:init(FileUtil.getConfigFromCsv("setting/PlayerDropItemProbability.csv"))
end

function GameConfig:parseGameConfig(config)
    if config.WorldInfo ~= nil then
        self.WorldInfo = {
            DayTime = tonumber(config.WorldInfo.DayTime or "30"),
            NightTime = tonumber(config.WorldInfo.NightTime or "30"),
            ChangeTime = tonumber(config.WorldInfo.ChangeTime or "3")
        }
    end

    self.PlayerChestExistTime = tonumber(config.PlayerChestExistTime)

    if config.PlayerAttribute ~= nil then
        self.PlayerAttribute = {
            MaxHealth = tonumber(config.PlayerAttribute.MaxHealth or "100"),
            MaxFoodLevel = tonumber(config.PlayerAttribute.MaxFoodLevel or "50"),
            onHurtSubFoodLevel = tonumber(config.PlayerAttribute.OnHurtSubFoodLevel or "10"),
            MaxWaterLevel = tonumber(config.PlayerAttribute.MaxWaterLevel or "100"),
            waterLevelSubValue = tonumber(config.PlayerAttribute.WaterLevelSubValue or "1"),
            waterLevelSubTime = tonumber(config.PlayerAttribute.WaterLevelSubTime or "10"),
            MaxDefense = tonumber(config.PlayerAttribute.MaxDefense or "0"),
            AddSpeed = tonumber(config.PlayerAttribute.AddSpeed or "0"),
            CloseDamage = tonumber(config.PlayerAttribute.CloseDamage or "1"),
            RemoteDamage = tonumber(config.PlayerAttribute.RemoteDamage or "1"),
            SatietyHeal = tonumber(config.PlayerAttribute.SatietyHeal or "1"),
            FallDamageResistance = tonumber(config.PlayerAttribute.FallDamageResistance or "1"),
        }
    end

    if config.PlayerStateBuffId ~= nil then
        self.PlayerStateBuffId = {
            Hunger = tonumber(config.PlayerStateBuffId.Hunger or "0"),
            Satiety = tonumber(config.PlayerStateBuffId.Satiety or "0"),
            Thirst = tonumber(config.PlayerStateBuffId.Thirst or "0"),
            DrinkFull = tonumber(config.PlayerStateBuffId.DrinkFull or "0"),
            Fracture = tonumber(config.PlayerStateBuffId.Fracture or "0")
        }
    end

    PlayerManager:setMaxPlayer(config.maxPlayers or 8)

    for i, v in pairs(config.randomPos) do
        self.randomPos[i] = {}
        self.randomPos[i].weight = tonumber(v.w)
        self.randomPos[i].x = tonumber(v.x)
        self.randomPos[i].y = tonumber(v.y)
        self.randomPos[i].z = tonumber(v.z)
        self.randomPos_totalWeight = self.randomPos_totalWeight + self.randomPos[i].weight
    end

    self.MaxEnderInventorySize = tonumber(config.MaxEnderInventorySize or 7)
    HostApi.setMaxEnderInventorySize(self.MaxEnderInventorySize)

    self.ItemLifeCycle = tonumber(config.ItemLifeCycle or "60")
    self.LifetimeScore = tonumber(config.LifetimeScore or "1")
    GameMatch.MaxRunTime = tonumber(config.MaxRunTime or "60")
    HangUpManager.checkTime = tonumber(config.CheckHangUpTime or "120")
    self.InFireDamage = tonumber(config.InFireDamage or "1")
    self.FireDamageCd = tonumber(config.FireDamageCd or "1")
    self.FireDurationTime = tonumber(config.FireDurationTime or "4")
end

function GameConfig:getRandomPos()
    local rand = math.random(self.randomPos_totalWeight)
    local totalWeight = 0
    for i = 1, #self.randomPos do
        totalWeight = totalWeight + self.randomPos[i].weight
        if totalWeight >= rand then
            --HostApi.log(string.format("GameConfig:getRandomPos %d %f %f %f", i, self.randomPos[i].x, self.randomPos[i].y, self.randomPos[i].z))
            return VectorUtil.newVector3(self.randomPos[i].x, self.randomPos[i].y, self.randomPos[i].z)
        end
    end
    return VectorUtil.newVector3(self.randomPos[1].x, self.randomPos[1].y, self.randomPos[1].z)
end

return GameConfig