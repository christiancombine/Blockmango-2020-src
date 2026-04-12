---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---

require "config.MerchantConfig"
require "config.ManorNpcConfig"
require "config.YardConfig"
require "config.HouseConfig"
require "config.ShopConfig"
require "config.FurnitureConfig"
require "config.BlockConfig"
require "config.VehicleConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.repairItemId = 0
GameConfig.repairItemMeta = 0
GameConfig.exchangeRate = 0

function GameConfig:init(config)
    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.repairItemId = tonumber(config.repairItemId)
    GameConfig.repairItemMeta = tonumber(config.repairItemMeta)
    GameConfig.exchangeRate = tonumber(config.exchangeRate)
    MerchantConfig:init(FileUtil.getConfigFromYml("merchant"))
    ManorNpcConfig:init(FileUtil.getConfigFromYml("manorNpc"))
    YardConfig:init(FileUtil.getConfigFromYml("yard"))
    HouseConfig:init(FileUtil.getConfigFromYml("house"))
    FurnitureConfig:init(FileUtil.getConfigFromYml("furniture"))
    ShopConfig:init(FileUtil.getConfigFromYml("shop"))
    BlockConfig:init(FileUtil.getConfigFromYml("block"))
    VehicleConfig:init(FileUtil.getConfigFromYml("vehicle"))
end

return GameConfig