---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.GunsConfig"
require "config.ExperienceGunsConfig"
require "config.ModeNpcConfig"
require "config.GunsForgeNpcConfig"
require "config.ForgeProbabilityConfig"
require "config.TipNpcConfig"
require "config.ForgeGroupConfig"
require "config.ShopConfig"
require "config.SprayPaintConfig"
require "config.SpecialClothingConfig"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.standByTime = 0

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.standByTime = tonumber(config.standByTime or "100")

    RankNpcConfig:init(config.rankNpc)

    GunsConfig:init(FileUtil.getConfigFromCsv("Guns.csv"))
    ExperienceGunsConfig:init(FileUtil.getConfigFromCsv("ExperienceGuns.csv"))
    ModeNpcConfig:init(FileUtil.getConfigFromCsv("ModeNpc.csv"))
    GunsForgeNpcConfig:init(FileUtil.getConfigFromCsv("GunsForgeNpc.csv"))
    ForgeProbabilityConfig:init(FileUtil.getConfigFromCsv("ForgeProbability.csv"))
    TipNpcConfig:init(FileUtil.getConfigFromCsv("TipNpc.csv"))
    ForgeGroupConfig:init(FileUtil.getConfigFromCsv("ForgeGroup.csv"))
    SprayPaintConfig:init(FileUtil.getConfigFromCsv("SprayPaint.csv"))
    SpecialClothingConfig:init(FileUtil.getConfigFromCsv("SpecialClothing.csv"))

    ShopConfig:init(config.shop)
end

return GameConfig