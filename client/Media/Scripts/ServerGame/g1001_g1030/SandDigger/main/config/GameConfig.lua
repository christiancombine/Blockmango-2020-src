---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.BlockConfig"
require "config.ToolConfig"
require "config.ChestConfig"
require "config.MerchantConfig"
require "config.BackpackConfig"
require "config.ShopConfig"
require "config.NpcConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.coinMapping = {}
GameConfig.teleportCD = 0
GameConfig.crashTime = 0
GameConfig.initEquip = {}

function GameConfig:init(config)

    self.teleportCD = config.teleportCD
    self.crashTime = config.crashTime

    for i, v in pairs(config.initEquip) do
        self.initEquip[i] = {}
        self.initEquip[i].id = v.id
        self.initEquip[i].num = v.num
    end

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    BlockConfig:init(FileUtil.getConfigFromYml("block"))
    ToolConfig:init(FileUtil.getConfigFromYml("tool"))
    ChestConfig:init(FileUtil.getConfigFromYml("chest"))
    BackpackConfig:init(FileUtil.getConfigFromYml("backpack"))
    MerchantConfig:init(FileUtil.getConfigFromYml("merchant"))
    ShopConfig:init(FileUtil.getConfigFromYml("shop"))
    NpcConfig:init(FileUtil.getConfigFromYml("npc"))
end

return GameConfig