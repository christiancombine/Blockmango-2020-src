---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---

require "config.BlockSettingConfig"
require "config.ToolDurableConfig"
require "config.Area"
require "config.IslandLevelConfig"
require "config.IslandAreaConfig"
require "config.AppShopConfig"
require "config.Define"
require "config.ProductConfig"
require "config.MonsterConfig"
require "config.MonsterGenerateConfig"
require "config.PrivilegeConfig"
require "config.TaskConfig"
require "config.RewardConfig"
require "config.SellItemConfig"
require "config.DiedDropConfig"
require "config.GunConfig"
require "config.ChestConfig"
require "config.FlipFrontConfig"
require "config.FlipBackConfig"
require "config.GemStoneConfig"
require "config.CustomItemConfig"
require "config.SkillConfig"
require "config.ChristmasConfig"
require "config.GiftRewardConfig"

GameConfig = {}
GameConfig.RootPath = ""
GameConfig.initPos = {}
GameConfig.canBreakArea = {}
GameConfig.pvpArea = {}
GameConfig.ArrowArea = {}
function GameConfig:init(config)
    self.initHP = tonumber(config.initHP or 20)
    self.hungerValue = tonumber(config.hungerSubHP[1] or 1)
    self.hungerInterval = tonumber(config.hungerSubHP[2] or 2)
    self.maxFoodAddHp = config.maxFoodAddHp
    self.refreshDareTask = config.refreshDareTask[1]
    self.buyDaretask = config.buyDaretask[1]
    self.initPos = VectorUtil.newVector3(config.initPos[1] or 8.49 , config.initPos[2] or 26, config.initPos[3] or -2.42)
    GameConfig.yaw = tonumber(config.initPos[4] or 0)
    self:initArea(config.canBreakArea, self.canBreakArea)
    self:initArea(config.pvpArea, self.pvpArea)
    self.lavaDamageCoe = tonumber(config.lavaDamageCoe or 0.15)
    self.lavaFireTime = tonumber(config.lavaFireTime or 6)
    self.onFireDamage = tonumber(config.onFireDamage or 1)
    self.monetaryUnit = tonumber(config.monetaryUnit or 200)
    self.drop_radius  = tonumber(config.drop_radius or 1)
    self.map_resetCD = tonumber(config.map_resetCD or 1200)
    self.map_resetTip = tonumber(config.map_resetTip or 5)
    self.map_tipCD = self.map_resetTip
    self.map_initTip = GameConfig.map_resetCD - GameConfig.map_resetTip
    self.map_isReset = false
    self.chestActor = tostring(config.chestActor) or ""
    self.chestOpenEffect = tostring(config.chestOpenEffect) or ""
    self.chestPosActor = tostring(config.chestPosActor) or ""
    self.chestCDHeight = tonumber(config.chestCDHeight)
    self.clickRange = tonumber(config.clickRange) or 5
    self.ArrowPos = VectorUtil.newVector3(config.ArrowPos[1] or 91 , config.ArrowPos[2] or 51, config.ArrowPos[3] or 130)
    self.cancelArrowRadius = tonumber(config.cancelArrowRadius) or 1
    self:initAreaByRadius(GameConfig.ArrowArea, self.ArrowPos, self.cancelArrowRadius)
    self.backHomeCD = tonumber(config.backHomeCD) or 5
    self.hintText = config.hintText
    self.onceBuyMoneyCost = tonumber(config.onceBuyMoneyCost or 60)
    self.maxBuyMoneyCost = tonumber(config.maxBuyMoneyCost or 600)
    self.onceMoneyNum = tonumber(config.onceMoneyNum or 100000)
    self.christmasFinishTime = config.christmasFinishTime or 1578499200

    ProductConfig:init(FileUtil.getConfigFromCsv("ProductConfig.csv"))
    BlockSettingConfig:initConfig(FileUtil.getConfigFromCsv("setting/BlockSetting.csv", 3))
    ToolDurableConfig:initConfig(FileUtil.getConfigFromCsv("ToolDurable.csv"))
    IslandAreaConfig:init(FileUtil.getConfigFromCsv("IslandAreaConfig.csv"))
    IslandLevelConfig:init(FileUtil.getConfigFromCsv("IslandLevelConfig.csv"))
    AppShopConfig:initConfig(FileUtil.getConfigFromCsv("setting/AppShopSetting.csv", 3))
    AppShopConfig:initSortConfig(FileUtil.getConfigFromCsv("setting/AppShopSortSetting.csv", 3))
    MonsterConfig:init(FileUtil.getConfigFromCsv("monster.csv"))
    MonsterGenerateConfig:init(FileUtil.getConfigFromCsv("monsterGenerate.csv"))
    PrivilegeConfig:initPrivilege(FileUtil.getConfigFromCsv("PrivilegeConfig.csv"))
    PrivilegeConfig:initItem(FileUtil.getConfigFromCsv("PrivilegeItemConfig.csv"))
    TaskConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockTaskSetting.csv", 3))
    TaskConfig:initPossibleConfig(FileUtil.getConfigFromCsv("TaskPossible.csv", 3))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockTaskRewardSetting.csv", 3))
    SellItemConfig:init(FileUtil.getConfigFromCsv("setting/SellItem.csv", 3))
    DiedDropConfig:initConfig(FileUtil.getConfigFromCsv("setting/DiedDropSetting.csv", 3))
    GunConfig:init(FileUtil.getConfigFromCsv("GunConfig.csv"))
    ChestConfig:initConfig(config.chestPos, config.chestCD)
    FlipFrontConfig:initConfig(config.flipFront)
    FlipBackConfig:initConfig(config.flipBack)
    CustomItemConfig:initConfig(FileUtil.getConfigFromCsv("setting/customItem.csv", 3))
    SkillConfig:init(FileUtil.getConfigFromCsv("SkillConfig.csv"))
    ChristmasConfig:initConfig(FileUtil.getConfigFromCsv("setting/ChristmasSetting.csv"))
    GiftRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/GiftRewardSetting.csv"))
end

function GameConfig:initGamePos(destPos, srcPos, isBlockPos)
    isBlockPos = isBlockPos or false
    for i, config in pairs(srcPos) do
        for _, Pos in pairs(config) do
            if isBlockPos then
                destPos[i] = VectorUtil.toBlockVector3(tonumber(Pos[1]), tonumber(Pos[2]), tonumber(Pos[3]))
            else
                destPos[i] = VectorUtil.newVector3(tonumber(Pos[1]), tonumber(Pos[2]), tonumber(Pos[3]))
            end
        end
    end
end

function GameConfig:initArea(configArea, selfArea)
    for _, area in pairs(configArea) do
        local matrix = {}
        for _,v in pairs(area) do
            table.insert(matrix, v)
        end
        table.insert(selfArea, Area.new(matrix))
    end
end

function GameConfig:isInArea(pos, selfArea, isZX)
    if next(selfArea) ~= nil then
        for k,v in pairs(selfArea) do
            if isZX then
                if v:inAreaXZ(pos) then
                    return true
                end
            elseif v:inArea(pos) then
                return true
            end
        end
    end
    return false
end

function GameConfig:getRandomPosAZ(pos, radius)
    radius = radius or 1
    if pos.x and pos.y and pos.z then
        if radius <= 0 then
            return pos
        end
        local x = HostApi.random("getRandomPosAZ", (pos.x - radius) * 10000, (pos.x + radius) * 10000)
        local z = HostApi.random("getRandomPosAZ", (pos.z - radius) * 10000, (pos.z + radius) * 10000)
        return VectorUtil.newVector3(x / 10000 , pos.y, z / 10000)
    end
    return VectorUtil.ZERO
end

function GameConfig:initAreaByRadius(area, pos, radius)
    local tab = {{},{}}
    tab[1][1] = pos.x - radius
    tab[1][2] = pos.y
    tab[1][3] = pos.z - radius
    tab[2][1] = pos.x + radius
    tab[2][2] = pos.y + radius
    tab[2][3] = pos.z + radius
    table.insert(area, Area.new(tab))
end

return GameConfig