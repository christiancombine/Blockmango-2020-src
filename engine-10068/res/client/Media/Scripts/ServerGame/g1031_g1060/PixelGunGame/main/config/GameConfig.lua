---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---

require "config.RespawnConfig"
require "config.SiteConfig"
require "config.TeamConfig"
require "config.GunConfig"
require "config.ExtraAwardConfig"
require "config.ChestConfig"
require "config.PropConfig"
require "config.FirstAwardCheck"
require "config.SkillConfig"
require "config.AwardConfig"
require "config.ExpConfig"
require "config.SupplyConfig"
require "config.PropertyConfig"
require "config.HonorExponentConfig"
require "config.ComboKillConfig"
require "config.BlockConfig"
require "config.AirplaneConfig"
require "config.SafeAreaConfig"
require "config.AreaConfig"
require "config.WeaponSupplyConfig"
require "config.PlayerInitialConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.standByTime = 0
GameConfig.startPlayers = 0
GameConfig.hallGameType = "g1042"

function GameConfig:init(config)
    GameMatch.gameType = config.gameType or "g1043"
    self.hallGameType = config.hallGameType or "g1042"
    self.maxSupplyNum = tonumber(config.maxSupplyNum) or 5
    self.waitRespawnTime = tonumber(config.waitRespawnTime) or 5
    self.invincibleTime = tonumber(config.invincibleTime) or 10
    self.standByTime = tonumber(config.standByTime) or 30
    self.startPlayers = tonumber(config.startPlayers) or 6
    self.prepareTime = tonumber(config.prepareTime) or 20
    self.gameTime = tonumber(config.gameTime) or 200
    self.gameOverTime = tonumber(config.gameOverTime) or 20
    self.extraTime = tonumber(config.extraTime) or 10
    self.showRankTick = tonumber(config.showRankTick) or 10
    self.maxPlayers = tonumber(config.maxPlayers) or 12
    self.autoRespawnTime = tonumber(config.autoRespawnTime) or 10
    self.deathLineY = tonumber(config.deathLineY) or -256
    self.quitSubHonor = tonumber(config.quitSubHonor) or -10
    self.finalTime = tonumber(config.finalTime) or (self.gameTime / 2)
    self.stayTime = tonumber(config.stayTime) or 1000
    self.animateTime = tonumber(config.animateTime) or 500
    self.comboKillTime = tonumber(config.comboKillTime) or 30
    self.maxWinAndKillCount = tonumber(config.maxWinAndKillCount) or 20
    self.maxGameAdTimes = tonumber(config.maxGameAdTimes) or 0

    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1].x, config.initPos[1].y, config.initPos[1].z)
    GunConfig:init(FileUtil.getConfigFromCsv("GunConfig.csv"))
    ExtraAwardConfig:init(FileUtil.getConfigFromCsv("ExtraAwardConfig.csv"))
    AwardConfig:init(FileUtil.getConfigFromCsv("AwardConfig.csv"))
    HonorExponentConfig:init(FileUtil.getConfigFromCsv("HonorExponentConfig.csv"))
    FirstAwardCheck:init(FileUtil.getConfigFromCsv("FirstAwardCheck.csv"))
    PropConfig:init(FileUtil.getConfigFromCsv("PropConfig.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("Skill.csv"))
    ExpConfig:initConfig(FileUtil.getConfigFromCsv("ExpConfig.csv"))
    SupplyConfig:initSupplySetting(FileUtil.getConfigFromCsv("SupplySetting.csv"))
    SupplyConfig:initSupply(FileUtil.getConfigFromCsv("SupplyConfig.csv"))
    PropertyConfig:initConfig(FileUtil.getConfigFromCsv("PropertyConfig.csv"))
    ComboKillConfig:initConfig(FileUtil.getConfigFromCsv("ComboKill.csv"))
    BlockConfig:initBlockSafeAreas(FileUtil.getConfigFromCsv("BlockSafeArea.csv"))
    BlockConfig:initMapBlocks(FileUtil.getConfigFromCsv("MapBlock.csv"))

    if GameMatch.gameType == "g1043" or GameMatch.gameType == "g3043" then
        self.teamMaxPlayers = config.teamMaxPlayers or 4
        self:initTeamConfig()
    elseif GameMatch.gameType == "g1044" or GameMatch.gameType == "g3044"  then
        self.topNumber = tonumber(config.topNumber) or 1
        self:initPersonalConfig()
    elseif GameMatch.gameType == "g1045" or GameMatch.gameType == "g3045"  then
        self.waitRevengeTime = tonumber(config.waitRevengeTime)
        self.selectTime = tonumber(config.selectTime) or 60
        self:initPvpConfig()
    elseif GameMatch.gameType == "g1053" then
        self.glideMoveAddition = tonumber(config.glideMoveAddition) or 0.5
        self.glideDropResistance = tonumber(config.glideDropResistance) or 0.4
        self.healthValue = tonumber(config.healthValue) or 100
        self.defenseValue = tonumber(config.defenseValue) or 100
        self.glideHeight = tonumber(config.glideHeight) or 10
        self.forceGlideHeight = tonumber(config.forceGlideHeight) or 50
        self.changeWeaponTime = tonumber(config.changeWeaponTime) or 5
        self.weaponWarningRange = tonumber(config.weaponWarningRange) or 10
        self.shootWarningRange = tonumber(config.shootWarningRange) or 10
        self.footWarningRange = tonumber(config.footWarningRange) or 10
        self.canReviveTime = tonumber(config.canReviveTime) or 200
        self.mapId = config.mapId or "m1053_1"
        self.randomWeaponTime = config.creatSupplyTime
        self:initChickenConfig()
        PlayerInitialConfig:init(config.initialConfig)
    end
end

function GameConfig:initTeamConfig()
    RespawnConfig:init(FileUtil.getConfigFromCsv("Respawn.csv"))
    TeamConfig:init(FileUtil.getConfigFromCsv("TeamConfig.csv"))
end

function GameConfig:initPersonalConfig()
    RespawnConfig:init(FileUtil.getConfigFromCsv("PersonalRespawn.csv"))
end

function GameConfig:initPvpConfig()
    ChestConfig:init(FileUtil.getConfigFromCsv("ChestConfig.csv"))
    SiteConfig:init(FileUtil.getConfigFromCsv("Site.csv"))
end

function GameConfig:initChickenConfig()
    AirplaneConfig:init(FileUtil.getConfigFromCsv("AirplaneConfig.csv"))
    AreaConfig:init(FileUtil.getConfigFromCsv("AreaConfig.csv"))
    SafeAreaConfig:init(FileUtil.getConfigFromCsv("SafeAreaConfig.csv"))
    WeaponSupplyConfig:initQuality(FileUtil.getConfigFromCsv("WeaponQualityConfig.csv"))
    WeaponSupplyConfig:initArea(FileUtil.getConfigFromCsv("WeaponAreaConfig.csv"))
    WeaponSupplyConfig:initPosition(FileUtil.getConfigFromCsv("MapAreaConfigResult.csv", 1))

    RespawnConfig:init(FileUtil.getConfigFromCsv("ChickenRespawn.csv"))
end

return GameConfig