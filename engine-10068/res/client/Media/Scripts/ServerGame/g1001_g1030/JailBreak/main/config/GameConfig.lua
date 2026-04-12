---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---

require "config.MerchantConfig"
require "config.ShopConfig"
require "config.WeaponConfig"
require "config.ScoreConfig"
require "config.CriminalConfig"
require "config.PoliceConfig"
require "config.MerchantConfig"
require "config.ChestConfig"
require "config.BlockConfig"
require "config.ProduceConfig"
require "config.LocationConfig"
require "config.VehicleConfig"
require "config.DoorConfig"
require "config.DamageConfig"
require "config.StrongboxConfig"
require "config.RankNpcConfig"
require "config.TransferGateConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.gameTipShowTime = 0
GameConfig.rankUpdateTime = 0
GameConfig.coinMapping = {}

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.rankUpdateTime = tonumber(config.rankUpdateTime)
    GameConfig.gameTipShowTime = tonumber(config.gameTipShowTime)

    GameConfig.respawnAdMaxTimes = tonumber(config.respawnAdMaxTimes or "999")
    GameConfig.shopAdRefreshTime = tonumber(config.shopAdRefreshTime or "1800")

    ShopConfig:init(config.shop)
    WeaponConfig:init(config.weapon)
    ScoreConfig:init(config.score)
    CriminalConfig:init(FileUtil.getConfigFromYml("criminal"))
    PoliceConfig:init(FileUtil.getConfigFromYml("police"))
    ChestConfig:init(FileUtil.getConfigFromYml("chest"))
    MerchantConfig:init(FileUtil.getConfigFromYml("merchant"))
    ProduceConfig:init(FileUtil.getConfigFromYml("produce"))
    BlockConfig:init(FileUtil.getConfigFromYml("block"))
    LocationConfig:init(FileUtil.getConfigFromYml("location"))
    VehicleConfig:init(FileUtil.getConfigFromYml("vehicle"))
    DoorConfig:init(FileUtil.getConfigFromYml("door"))
    DamageConfig:init(FileUtil.getConfigFromYml("damage"))
    StrongboxConfig:init(FileUtil.getConfigFromYml("strongbox"))
    RankNpcConfig:init(FileUtil.getConfigFromYml("rankNpc"))
    TransferGateConfig:init(FileUtil.getConfigFromYml("transferGate"))
end

return GameConfig