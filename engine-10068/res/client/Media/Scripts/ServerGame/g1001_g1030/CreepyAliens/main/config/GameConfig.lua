---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.SceneConfig"
require "config.MonsterConfig"
require "config.PlayerLevelConfig"
require "config.GameLevelConfig"
require "config.MonsterNumConfig"
require "config.BasementConfig"
require "config.EquipmentConfig"
require "config.ArmsConfig"
require "config.RespawnConfig"
require "config.ShopConfig"
require "config.SkillConfig"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.waitingTime = 0
GameConfig.prepareTime = 0
GameConfig.selectMapTime = 0
GameConfig.gameLevelTime = 0
GameConfig.gameOverTime = 0
GameConfig.refreshMonsterTime = 0

GameConfig.refreshMonsterTimes = 0
GameConfig.startPlayers = 0
GameConfig.maxMonsters = 0

GameConfig.defaultUpgradeExp = 0
GameConfig.growthX = 0
GameConfig.correctionX = 0
GameConfig.basePercentage = 0
GameConfig.killPercentage = 0
GameConfig.PlayerUpgradeHealthReply = 0
GameConfig.BasementUpgradeHealthReply = 0

GameConfig.playerAttr = {}

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])

    self.waitingTime = tonumber(config.waitingTime)
    self.prepareTime = tonumber(config.prepareTime)
    self.selectMapTime = tonumber(config.selectMapTime)
    self.gamePrepareTime = tonumber(config.gamePrepareTime)
    self.gameLevelTime = tonumber(config.gameLevelTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.refreshMonsterTime = tonumber(config.refreshMonsterTime)

    self.refreshMonsterTimes = tonumber(config.refreshMonsterTimes)
    self.startPlayers = tonumber(config.startPlayers)
    self.maxMonsters = tonumber(config.maxMonsters)

    self.defaultUpgradeExp = tonumber(config.defaultUpgradeExp)
    self.growthX = tonumber(config.growthX)
    self.correctionX = tonumber(config.correctionX)
    self.basePercentage = tonumber(config.basePercentage)
    self.killPercentage = tonumber(config.killPercentage)
    self.PlayerUpgradeHealthReply = tonumber(config.PlayerUpgradeHealthReply)
    self.BasementUpgradeHealthReply = tonumber(config.BasementUpgradeHealthReply)

    for i, scene in pairs(config.scenes) do
        SceneConfig:addScene(FileUtil.getConfigFromYml(scene.path))
    end

    self.playerAttr.hp = tonumber(config.playerAttr.hp)
    self.playerAttr.attack = tonumber(config.playerAttr.attack)
    self.playerAttr.defense = tonumber(config.playerAttr.defense)
    self.playerAttr.attackX = tonumber(config.playerAttr.attackX)
    self.playerAttr.addHp = tonumber(config.playerAttr.addHp)
    self.playerAttr.maxLv = tonumber(config.playerAttr.maxLv)

    RankNpcConfig:init(config.rankNpc)
    ShopConfig:init(config.shop)
    RespawnConfig:init(config.respawn)

    MonsterConfig:init(FileUtil.getConfigFromCsv("monster.csv"))
    PlayerLevelConfig:init(FileUtil.getConfigFromCsv("player_level.csv"))
    MonsterNumConfig:init(FileUtil.getConfigFromCsv("monster_num.csv"))
    GameLevelConfig:init(FileUtil.getConfigFromCsv("game_level.csv"))
    BasementConfig:init(FileUtil.getConfigFromCsv("basement.csv"))
    EquipmentConfig:init(FileUtil.getConfigFromCsv("equipment.csv"))
    ArmsConfig:init(FileUtil.getConfigFromCsv("arms.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("skill.csv"))

end

return GameConfig