---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.FuncNpcConfig"
require "config.ModeSelectConfig"
require "config.ModeSelectMapConfig"
require "config.GunConfig"
require "config.GunLevelConfig"
require "config.PropConfig"
require "config.PropLevelConfig"
require "config.AttributeConfig"
require "config.ExpConfig"
require "config.ChestRewardConfig"
require "config.RewardConfig"
require "config.ChestConfig"
require "config.ArmorConfig"
require "config.SeasonConfig"
require "config.SeasonRankRewardConfig"
require "config.AppShopConfig"
require "config.PropertyConfig"
require "config.GuideArrowConfig"
require "config.NotificationConfig"
require "config.TaskConfig"
require "config.TaskActiveConfig"
require "config.TaskReportConfig"
require "config.GuideConfig"
require "config.AdvertConfig"
require "config.SignInConfig"
require "config.GiftBagConfig"
require "config.ModuleConfig"

GameConfig = {}

GameConfig.initPos = {}

function GameConfig:init(config)

    self.standByTime = config.standByTime or 300
    self.initExp = tonumber(config.initExp) or 0
    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    GameMatch.gameType = config.gameType or "g1042"
    GameMatch.refreshTime = tonumber(config.refreshTime) or 14400

    FuncNpcConfig:initConfig(FileUtil.getConfigFromCsv("FuncNpc.csv"))
    ModeSelectConfig:initConfig(FileUtil.getConfigFromCsv("ModeSelect.csv"))
    ModeSelectMapConfig:initConfig(FileUtil.getConfigFromCsv("ModeSelectMap.csv"))
    GunConfig:initConfig(FileUtil.getConfigFromCsv("setting/GunSetting.csv", 3))
    GunLevelConfig:initConfig(FileUtil.getConfigFromCsv("setting/GunLevelSetting.csv", 3))
    PropConfig:initConfig(FileUtil.getConfigFromCsv("setting/PropSetting.csv", 3))
    PropLevelConfig:initConfig(FileUtil.getConfigFromCsv("setting/PropLevelSetting.csv", 3))
    AttributeConfig:initConfig(FileUtil.getConfigFromCsv("setting/AttributeSetting.csv", 3))
    ExpConfig:initConfig(FileUtil.getConfigFromCsv("ExpConfigHall.csv"))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/RewardSetting.csv", 3))
    ChestRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/ChestRewardSetting.csv", 3))
    ChestConfig:initConfig(FileUtil.getConfigFromCsv("setting/ChestSetting.csv", 3))
    ArmorConfig:initArmor(FileUtil.getConfigFromCsv("Armor.csv"))
    SeasonConfig:initConfig(FileUtil.getConfigFromCsv("setting/SeasonSetting.csv", 3))
    SeasonRankRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/SeasonRankRewardSetting.csv", 3))
    AppShopConfig:initTabConfig(FileUtil.getConfigFromCsv("AppShopTab.csv"))
    AppShopConfig:initGoodsConfig(FileUtil.getConfigFromCsv("AppShopGoods.csv"))
    PropertyConfig:initConfig(FileUtil.getConfigFromCsv("PropertyConfig.csv"))
    GuideArrowConfig:initConfig(FileUtil.getConfigFromCsv("GuideArrow.csv"))
    NotificationConfig:initConfig(FileUtil.getConfigFromCsv("Notification.csv"))
    TaskConfig:initTaskLevelGroupConfig(FileUtil.getConfigFromCsv("setting/TaskGroupSetting.csv", 3))
    TaskConfig:initTaskListConfig(FileUtil.getConfigFromCsv("setting/TaskSetting.csv", 3))
    TaskActiveConfig:initConfig(FileUtil.getConfigFromCsv("setting/TaskActiveSetting.csv", 3))
    TaskReportConfig:initConfig(FileUtil.getConfigFromCsv("setting/TaskReportSetting.csv", 3))
    GuideConfig:initGuideConfig(FileUtil.getConfigFromCsv("setting/GuideSetting.csv", 3))
    GuideConfig:initConditionConfig(FileUtil.getConfigFromCsv("setting/ConditionSetting.csv", 3))
    GuideConfig:initEventConfig(FileUtil.getConfigFromCsv("setting/EventSetting.csv", 3))
    AdvertConfig:initConfig(FileUtil.getConfigFromCsv("AdvertSetting.csv"))
    SignInConfig:initConfig(FileUtil.getConfigFromCsv("setting/SignInSetting.csv", 3))
    GiftBagConfig:initConfig(FileUtil.getConfigFromCsv("setting/GiftSetting.csv", 3))
    ModuleConfig:initConfig(FileUtil.getConfigFromCsv("setting/ModuleSetting.csv", 3))
end

return GameConfig