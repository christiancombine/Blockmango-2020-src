---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.Define"
require "config.AppShopConfig"
require "config.IslandLevelConfig"
require "config.IslandAreaConfig"
require "config.ProductConfig"
require "config.ProductLimitConfig"
require "config.ProductResourceConfig"
require "config.ProductUnlockConfig"
require "config.Area"
require "config.AppShopConfig"
require "config.PrivilegeConfig"
require "config.SellItemConfig"
require "config.TaskConfig"
require "config.RewardConfig"
require "config.GunConfig"
require "config.ToolDurableConfig"
require "config.GemStoneConfig"
require "config.NotificationConfig"
require "config.CustomItemConfig"
require "config.SkillConfig"
require "config.ChristmasConfig"
require "config.GiftRewardConfig"

GameConfig = {}
GameConfig.RootPath = ""
GameConfig.ProductArea = {}
GameConfig.ProductNpc = {}

function GameConfig:init(config)

    self.initHP = tonumber(config.initHP or 20)
    self.hungerValue = tonumber(config.hungerSubHP[1] or 1)
    self.hungerInterval = tonumber(config.hungerSubHP[2] or 2)
    self.maxFoodAddHp = config.maxFoodAddHp
    self.deathLineY = tonumber(config.deathLineY or 2) 
    self.manorStart = config.manorStart
    self.randomRadius = tonumber(config.randomRadius or 5)
    self.maxTime = tonumber(config.maxTime or 36000)
    self.maxNum = tonumber(config.maxNum or 150)
    self.SignPostName = config.SignPostName
    self.refreshDareTask = config.refreshDareTask[1]
    self.buyDaretask = config.buyDaretask[1]
    self.moonName = tostring(config.moonName) or "moon_phases.png"
    self.sunName = tostring(config.sunName) or "sun.png"
    self.miningAreaCD = tonumber(config.miningAreaCD or 5)
    self.monetaryUnit = tonumber(config.monetaryUnit or 200)
    self.maxMillNum = tonumber(config.maxMillNum or 1000)
    self.onceBuyMoneyCost = tonumber(config.onceBuyMoneyCost or 60)
    self.maxBuyMoneyCost = tonumber(config.maxBuyMoneyCost or 600)
    self.onceMoneyNum = tonumber(config.onceMoneyNum or 100000)
    self.christmasFinishTime = config.christmasFinishTime or 1578499200

    ProductResourceConfig:init(FileUtil.getConfigFromCsv("ProductResourceConfig.csv"))
    ProductConfig:init(FileUtil.getConfigFromCsv("ProductConfig.csv"))
    ProductLimitConfig:init(FileUtil.getConfigFromCsv("ProductLimitConfig.csv"))
    ProductUnlockConfig:init(FileUtil.getConfigFromCsv("ProductUnlockConfig.csv"))
    IslandAreaConfig:init(FileUtil.getConfigFromCsv("IslandAreaConfig.csv"))
    IslandLevelConfig:init(FileUtil.getConfigFromCsv("IslandLevelConfig.csv"))
    AppShopConfig:initConfig(FileUtil.getConfigFromCsv("setting/AppShopSetting.csv", 3))
    AppShopConfig:initSortConfig(FileUtil.getConfigFromCsv("setting/AppShopSortSetting.csv", 3))
    PrivilegeConfig:initPrivilege(FileUtil.getConfigFromCsv("PrivilegeConfig.csv"))
    PrivilegeConfig:initItem(FileUtil.getConfigFromCsv("PrivilegeItemConfig.csv"))
    SellItemConfig:init(FileUtil.getConfigFromCsv("setting/SellItem.csv", 3))
    TaskConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockTaskSetting.csv", 3))
    TaskConfig:initPossibleConfig(FileUtil.getConfigFromCsv("TaskPossible.csv", 3))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/SkyBlockTaskRewardSetting.csv", 3))
    GunConfig:init(FileUtil.getConfigFromCsv("GunConfig.csv"))
    ToolDurableConfig:initConfig(FileUtil.getConfigFromCsv("ToolDurable.csv"))
    NotificationConfig:initConfig(FileUtil.getConfigFromCsv("Notification.csv"))
    CustomItemConfig:initConfig(FileUtil.getConfigFromCsv("setting/customItem.csv", 3))
    SkillConfig:init(FileUtil.getConfigFromCsv("SkillConfig.csv"))
    ChristmasConfig:initConfig(FileUtil.getConfigFromCsv("setting/ChristmasSetting.csv"))
    GiftRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/GiftRewardSetting.csv"))
end

function GameConfig:getNpcIdByEntityId(manorIndex, npcEntityId)
    for index, npcs in pairs(self.ProductNpc) do
        if tonumber(index) == manorIndex then
            for _, npc in pairs(npcs) do
                if npc.entityId == npcEntityId then
                    HostApi.log("npcEntityId and npc.id is : "..npcEntityId.." : "..npc.id)
                    return npc.id            
                end
            end
        end
    end
    return 0
end

function GameConfig:canPickupItemInProductArea(manorIndex, pos)
    for k, v in pairs(self.ProductArea) do
        if tonumber(k) == manorIndex then
            return v:inArea(pos)
        end
    end
    return false
end

function GameConfig:getInitPosByManorIndex(index)
    for _, manor in pairs(self.manorStart) do
        if tonumber(manor.index) == tonumber(index) then
            return VectorUtil.newVector3i(manor.init_pos[1], manor.init_pos[2], manor.init_pos[3])
        end
    end
    return nil
end

function GameConfig:getOriginPosByManorIndex(index)
    for _, manor in pairs(self.manorStart) do
        if tonumber(manor.index) == tonumber(index) then
            return VectorUtil.newVector3i(manor.origin_pos[1], manor.origin_pos[2], manor.origin_pos[3])
        end
    end
    return nil
end

return GameConfig