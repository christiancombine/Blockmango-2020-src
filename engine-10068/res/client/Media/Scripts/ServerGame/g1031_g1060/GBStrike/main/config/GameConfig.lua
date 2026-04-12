---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.GunsConfig"
require "config.TeamConfig"
require "config.GunMerchantConfig"
require "config.KillRewardConfig"
require "config.RespawnConfig"
require "config.PropGroupConfig"
require "config.ItemsConfig"
require "config.PropsConfig"
require "config.ItemSettingConfig"
require "config.InitItemsConfig"
require "config.ShopConfig"
require "config.SpecialClothingConfig"
require "config.BulletConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.initPlayerHealth = ""

GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0
GameConfig.teamMaxPlayers = 0
GameConfig.redTeamActor = ""
GameConfig.blueTeamActor = ""
GameConfig.winKill = 0
GameConfig.standByTime = 0
GameConfig.unableMoveTick = 0
GameConfig.showShopTick = 0
GameConfig.showRankTick = 0
GameConfig.itemLifeTime = 0

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    self.initPlayerHealth = config.initPlayerHealth or "20"

    self.prepareTime = tonumber(config.prepareTime or "10")
    self.gameTime = tonumber(config.gameTime or "60")
    self.gameOverTime = tonumber(config.gameOverTime or "10")

    RankNpcConfig:init(config.rankNpc)
    ShopConfig:init(config.shop)
    BulletConfig:init(config.bullet)

    self.standByTime = tonumber(config.standByTime or "100")
    self.startPlayers = tonumber(config.startPlayers or "1")
    self.teamMaxPlayers = tonumber(config.teamMaxPlayers or "1")
    self.redTeamActor = config.redTeamActor or ""
    self.blueTeamActor = config.blueTeamActor or ""
    self.winKill = tonumber(config.winKill or "10")
    self.unableMoveTick = tonumber(config.unableMoveTick or "1")
    self.showShopTick = tonumber(config.showShopTick or "1")
    self.showRankTick = tonumber(config.showRankTick or "1")
    self.itemLifeTime = tonumber(config.itemLifeTime or "1")

    GunsConfig:init(FileUtil.getConfigFromCsv("Guns.csv"))
    TeamConfig:init(FileUtil.getConfigFromCsv("Team.csv"))
    GunMerchantConfig:init(FileUtil.getConfigFromCsv("GunMerchant.csv"))
    KillRewardConfig:init(FileUtil.getConfigFromCsv("KillReward.csv"))
    RespawnConfig:init(FileUtil.getConfigFromCsv("Respawn.csv"))
    ItemsConfig:init(FileUtil.getConfigFromCsv("Items.csv"))
    PropsConfig:init(FileUtil.getConfigFromCsv("Props.csv"))
    PropGroupConfig:init(FileUtil.getConfigFromCsv("PropGroup.csv"))
    ItemSettingConfig:init(FileUtil.getConfigFromCsv("ItemSetting.csv"))
    InitItemsConfig:init(FileUtil.getConfigFromCsv("InitItems.csv"))
    SpecialClothingConfig:init(FileUtil.getConfigFromCsv("SpecialClothing.csv"))
end

return GameConfig