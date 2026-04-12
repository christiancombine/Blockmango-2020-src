---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "hall.config.ExpConfig"
require "common.config.RewardConfig"
require "common.config.ChestRewardConfig"
require "common.config.ChestConfig"
require "hall.config.SeasonConfig"
require "hall.config.SeasonRankRewardConfig"
require "common.config.DressConfig"
require "hall.config.RuneTroughConfig"
require "hall.config.RuneConfig"
require "hall.config.GuideConfig"
require "hall.config.InitRewardConfig"
require "hall.config.FirstRechargeRewardConfig"
require "hall.config.SuperPlayerRewardConfig"
require "hall.config.TiroSignInConfig"
require "hall.config.DailySignInConfig"
require "common.config.PrivilegeConfig"

GameConfig = {}
GameConfig.initPos = {}

function GameConfig:init(config)
    self.standByTime = config.standByTime or 300
    self.initExp = tonumber(config.initExp) or 0
    self.deathLineY = tonumber(config.deathLineY) or 0
    self.unlockRuneTroughPrice = tonumber(config.unlockRuneTroughPrice or "10")
    self.worldTime = config.worldTime
    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.yaw = tonumber(config.initPos[4])

    self.runeMapping = config.RuneMapping

    self.taskGiveUpCD = tonumber(config.taskGiveUpCD or "72")

    self.challengeAdProgress = tonumber(config.challengeAdProgress or 10)

    ExpConfig:initConfig(FileUtil.getConfigFromCsv("ExpConfigHall.csv"))
    RewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/RewardSetting.csv", 3))
    ChestRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/BoxRewardSetting.csv", 3))
    ChestConfig:initConfig(FileUtil.getConfigFromCsv("setting/BoxSetting.csv", 3))
    SeasonConfig:initConfig(FileUtil.getConfigFromCsv("setting/SeasonSetting.csv", 3))
    SeasonRankRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/SeasonRankRewardSetting.csv", 3))
    DressConfig:initConfig(FileUtil.getConfigFromCsv("setting/DressSetting.csv", 3))
    RuneTroughConfig:initConfig(FileUtil.getConfigFromCsv("setting/RuneTroughSetting.csv", 3))
    RuneConfig:initConfig(FileUtil.getConfigFromCsv("setting/RuneSetting.csv", 3))
    GuideConfig:initGuideConfig(FileUtil.getConfigFromCsv("setting/GuideSetting.csv", 3))
    GuideConfig:initConditionConfig(FileUtil.getConfigFromCsv("setting/ConditionSetting.csv", 3))
    GuideConfig:initEventConfig(FileUtil.getConfigFromCsv("setting/EventSetting.csv", 3))
    InitRewardConfig:initConfig(FileUtil.getConfigFromCsv("InitReward.csv"))
    FirstRechargeRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/FirstRechargeRewardSetting.csv", 3))
    SuperPlayerRewardConfig:initConfig(FileUtil.getConfigFromCsv("setting/SuperPlayerRewardSetting.csv", 3))
    TiroSignInConfig:initConfig(FileUtil.getConfigFromCsv("setting/TiroSignInItemSetting.csv", 3))
    DailySignInConfig:initConfig(FileUtil.getConfigFromCsv("setting/DailySignInItemSetting.csv", 3))
    FreeGiftConfig:init(FileUtil.getConfigFromCsv("setting/FreeGift.csv", 2))
end

return GameConfig