require "config.ShopConfig"
require "config.CommodityConfig"
require "config.TeamConfig"
require "config.CannotDropItemConfig"
require "config.RespawnConfig"
require "config.BasicPropConfig"
require "config.AreaOfEliminateConfig"
require "config.BasementConfig"
require "config.TowerConfig"
require "config.MineResourceConfig"
require "config.BlockAttrConfig"
require "config.ToolAttrConfig"
require "config.AttackXConfig"
require "config.InventoryConfig"
require "config.OccupationNpcConfig"
require "config.OccupationConfig"
require "config.SkillEffectConfig"
require "config.SkillConfig"
require "data.GameMerchant"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gamePrepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

GameConfig.coinMapping = {}
GameConfig.merchants = {}
GameConfig.defaultAttr = {}

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.yaw = tonumber(config.initPos[4])
    GameConfig.worldTime = tonumber(config.worldTime or "6000")

    GameConfig.waitingPlayerTime = tonumber(config.waitingPlayerTime or "60")
    GameConfig.prepareTime = tonumber(config.prepareTime or "60")
    GameConfig.gamePrepareTime = tonumber(config.gamePrepareTime or "60")
    GameConfig.gameTime = tonumber(config.gameTime or "1200")
    GameConfig.gameOverTime = tonumber(config.gameOverTime or "30")
    GameConfig.startPlayers = tonumber(config.startPlayers or "1")

    self:initDefaultAttr(config.defaultAttr)
    self:initCoinMapping(config.coinMapping)
    self:initMerchants(config.merchants)

    ShopConfig:init(config.shop)
    RankNpcConfig:init(config.rankNpc)
    RespawnConfig:init(config.respawn)

    CommodityConfig:init(FileUtil.getConfigFromCsv("Commodity.csv"))
    CannotDropItemConfig:init(FileUtil.getConfigFromCsv("CannotDropItem.csv"))
    TeamConfig:init(FileUtil.getConfigFromCsv("TeamConfig.csv"))
    BasicPropConfig:init(FileUtil.getConfigFromCsv("BasicProp.csv"))
    AreaOfEliminateConfig:init(FileUtil.getConfigFromCsv("AreaOfEliminate.csv"))
    BasementConfig:init(FileUtil.getConfigFromCsv("Basement.csv"))
    TowerConfig:init(FileUtil.getConfigFromCsv("Tower.csv"))
    MineResourceConfig:init(FileUtil.getConfigFromCsv("MineResource.csv"))
    BlockAttrConfig:init(FileUtil.getConfigFromCsv("BlockAttr.csv"))
    ToolAttrConfig:init(FileUtil.getConfigFromCsv("ToolAttr.csv"))
    AttackXConfig:init(FileUtil.getConfigFromCsv("AttackX.csv"))
    InventoryConfig:init(FileUtil.getConfigFromCsv("Inventory.csv"))
    OccupationNpcConfig:init(FileUtil.getConfigFromCsv("OccupationNpc.csv"))
    OccupationConfig:init(FileUtil.getConfigFromCsv("Occupation.csv"))
    SkillEffectConfig:init(FileUtil.getConfigFromCsv("SkillEffect.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("Skill.csv"))

end

function GameConfig:initDefaultAttr(defaultAttr)
    local attr = {}
    attr.type = defaultAttr.type
    attr.health = tonumber(defaultAttr.health)
    attr.speed = tonumber(defaultAttr.speed)
    attr.attack = tonumber(defaultAttr.attack)
    attr.defense = tonumber(defaultAttr.defense)
    attr.defenseX = tonumber(defaultAttr.defenseX)
    self.defaultAttr = attr
end
function GameConfig:initMerchants(merchants)
    for i, v in pairs(merchants) do
        local merchant = {}
        merchant.name = v.name
        merchant.initPos = VectorUtil.newVector3(v.initPos[1], v.initPos[2], v.initPos[3])
        merchant.yaw = tonumber(v.initPos[4])
        merchant.groupId = v.groupId
        self.merchants[#self.merchants + 1] = GameMerchant.new(merchant)
    end
end

function GameConfig:syncMerchants(player)
    for _, merchant in pairs(self.merchants) do
        merchant:syncPlayer(player)
    end
end

function GameConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = v.coinId
        self.coinMapping[i].itemId = v.itemId
    end
end

return GameConfig