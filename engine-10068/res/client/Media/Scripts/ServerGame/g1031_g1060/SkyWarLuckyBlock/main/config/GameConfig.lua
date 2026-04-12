require "config.EventTriggerConfig"
require "config.BlockGoodsPool"
require "config.JobConfig"
require "config.JobPondConfig"
require "config.IslandConfig"
require "config.PropertyConfig"
require "config.EventConfig"
require "config.BoxConfig"
require "config.BoxItemConfig"
require "config.SkillConfig"
require "config.AreaConfig"
require "config.RespawnConfig"
require "config.BlockConfig"
require "config.ItemConfig"
require "config.AppPropConfig"
require "config.BlockHardnessConfig"
require "config.SkillEffectConfig"
require "config.MountConfig"
require "config.GameTip"
require "config.PropBlockConfig"
require "config.BowConfig"
require "config.HonourConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.standByTime = 0
GameConfig.startPlayers = 0

function GameConfig:init(config)
    GameMatch.gameType = config.gameType or "g1054"
    self.startPlayers = tonumber(config.startPlayers) or 2

    self.waitingPlayerTime = tonumber(config.waitingPlayerTime) or 30
    self.prepareStageTime = tonumber(config.prepareStageTime) or 30
    self.gameStageTime = tonumber(config.gameStageTime) or 40
    self.gameOverTime = tonumber(config.gameOverTime) or 20
    self.watchPoint = config.watchPoint
    self.gameStartCountDown = tonumber(config.gameStartCountDown) or 20
    self.pondRefreshPrice = config.pondRefreshPrice

    self.unbreakableBlock = config.unbreakableBlock or {}

    self.glideMoveAddition = tonumber(config.glideMoveAddition) or 0.05
    self.glideDropResistance = tonumber(config.glideDropResistance) or 0.25
    self.deadLineHeight = tonumber(config.deadLineHeight) or 50
    self.startGlideHeight = tonumber(config.startGlideHeight) or 70
    self.poisonDuration = tonumber(config.poisonDuration) or 30

    self.tntExplosionSize = tonumber(config.tntExplosionSize) or 1
    self.tntExplosionDamageFactor = tonumber(config.tntExplosionDamageFactor) or 0.2
    self.tntExplosionRepelDistanceFactor = tonumber(config.tntExplosionRepelDistanceFactor) or 0
    self.areaStartPos = VectorUtil.newVector3(config.areaStartPos[1], config.areaStartPos[2], config.areaStartPos[3])
    self.areaEndPos = VectorUtil.newVector3(config.areaEndPos[1], config.areaEndPos[2], config.areaEndPos[3])
    self.velocity = tonumber(config.velocity) or 10
    self.outOfAreaHurt = tonumber(config.outOfAreaHurt) or 5
    self.goldApple = config.goldApple

    self.breakBlockTime = tonumber(config.breakBlockTime) * 1000
    self.breakBlockHurt = tonumber(config.breakBlockHurt) or 2
    self.DragonHurt = tonumber(config.DragonHurt) or 2
    self.DragonHurtTime = tonumber(config.DragonHurtTime) or 2

    self.lastTntTime = tonumber(config.lastTntTime) or 10
    self.witheredVelocity = tonumber(config.witheredVelocity) or 5

    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])

    HostApi.setQuicklyBuildBlock(config.quicklyBuildBlockCost or 2, config.quicklyBuildBlockNum or 10)
    EngineWorld:getWorld():setBlockResistance(-1, 0.5)

    BlockHardnessConfig:init(FileUtil.getConfigFromCsv("BlockHardness.csv"))
    EventTriggerConfig:init(FileUtil.getConfigFromCsv("EventTriggerConfig.csv"))
    TeamConfig:init(FileUtil.getConfigFromCsv("GameTeam.csv"))
    AreaConfig:init(FileUtil.getConfigFromCsv("AreaConfig.csv"))
    IslandConfig:init(FileUtil.getConfigFromCsv("IslandConfig.csv"))
    PropertyConfig:init(FileUtil.getConfigFromCsv("PropertyConfig.csv"))
    EventConfig:init(FileUtil.getConfigFromCsv("EventConfig.csv"))
    BoxConfig:init(FileUtil.getConfigFromCsv("BoxConfig.csv"))
    BoxItemConfig:init(FileUtil.getConfigFromCsv("BoxItemConfig.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("Skill.csv"))
    RespawnConfig:init(FileUtil.getConfigFromCsv("Respawn.csv"))
    BlockConfig:init(FileUtil.getConfigFromCsv("BlockSetting.csv"))
    ItemConfig:init(FileUtil.getConfigFromCsv("setting/Items.csv", 3))
    AppPropConfig:init(FileUtil.getConfigFromCsv("AppProps.csv"))
    SkillEffectConfig:init(FileUtil.getConfigFromCsv("PropEffectConfig.csv"))
    BlockGoodsPool:init(FileUtil.getConfigFromCsv("BlockGoodsPool.csv"))
    JobConfig:init(FileUtil.getConfigFromCsv("setting/JobConfig.csv", 3))
    JobPondConfig:init(FileUtil.getConfigFromCsv("JobPondConfig.csv"))
    GameTip:init(FileUtil.getConfigFromCsv("GameTip.csv"))
    PropBlockConfig:init(FileUtil.getConfigFromCsv("PropBlock.csv"))
    BowConfig:init(FileUtil.getConfigFromCsv("BowSetting.csv"))
    HonourConfig:init(FileUtil.getConfigFromCsv("HonourConfig.csv"))
    MountConfig:init()
end

return GameConfig