require "config.ManorConfig"
require "config.HardnessConfig"
require "config.ManorLevelConfig"
require "config.PlayerLevelConfig"
require "config.SessionNpcConfig"
require "config.TagConfig"
require "config.StoreConfig"
require "config.GuideConfig"
require "config.PreInstallConfig"

GameConfig = {}
GameConfig.initPos = {}

function GameConfig:init(config)
    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.defaultName = config.defaultName or ""
    GameConfig.inviteShowTime = tonumber(config.inviteShowTime) or 3
    GameConfig.manorReleaseTime = tonumber(config.manorReleaseTime) or 120
    GameConfig.groundName = config.groundName or ""
    GameConfig.teleportPos = VectorUtil.newVector3(config.teleportPos[1], config.teleportPos[2], config.teleportPos[3])
    GameConfig.teleportYaw = tonumber(config.teleportPos[4]) or 0
    ManorConfig:init(FileUtil.getConfigFromCsv("manor.csv"))
    HardnessConfig:init(FileUtil.getConfigFromCsv("hardness.csv"))
    ManorLevelConfig:init(FileUtil.getConfigFromCsv("manorLevel.csv"))
    PlayerLevelConfig:init(FileUtil.getConfigFromCsv("playerLevel.csv"))
    SessionNpcConfig:initNpc(FileUtil.getConfigFromCsv("sessionNpc.csv"))
    TagConfig:init(FileUtil.getConfigFromCsv("tag.csv"))
    StoreConfig:init(FileUtil.getConfigFromCsv("store.csv"))
    StoreConfig:initPrice(FileUtil.getConfigFromCsv("setting/fortItems.csv", 3))
    GuideConfig:initGuide(FileUtil.getConfigFromCsv("setting/GuideSetting.csv", 3))
    GuideConfig:initCondition(FileUtil.getConfigFromCsv("setting/ConditionSetting.csv", 3))
    GuideConfig:initEvent(FileUtil.getConfigFromCsv("setting/EventSetting.csv", 3))
    GameConfig.GuideSuspendTeleportRPos = VectorUtil.newVector3(config.GuideSuspendTeleportRPos[1], config.GuideSuspendTeleportRPos[2], config.GuideSuspendTeleportRPos[3])
    GameConfig.inGuideLastId = tonumber(config.inGuideLastId) or 0
    PreInstallConfig:init(FileUtil.getConfigFromCsv("preinstall.csv"))
end

return GameConfig