---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.SceneConfig"
require "config.MoneyConfig"
require "config.CommodityConfig"
require "config.InventoryConfig"
require "config.OccupationConfig"
require "config.RespawnConfig"
require "config.PotionEffectConfig"
require "config.ArmsConfig"
require "config.FashionStoreConfig"
require "config.ShopConfig"
require "config.SkillEffectConfig"
require "config.SkillConfig"

GameConfig = {}

GameConfig.initPos = {}
GameConfig.selectRolePos = {}

GameConfig.waitingTime = 0
GameConfig.prepareTime = 0
GameConfig.selectRoleTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.selectRolePos = VectorUtil.newVector3i(config.selectRolePos[1], config.selectRolePos[2], config.selectRolePos[3])

    self.waitingTime = tonumber(config.waitingTime or "30")
    self.prepareTime = tonumber(config.prepareTime or "30")
    self.selectRoleTime = tonumber(config.selectRoleTime or "30")
    self.gameTime = tonumber(config.gameTime or "1200")
    self.gameOverTime = tonumber(config.gameOverTime or "30")

    self.startPlayers = tonumber(config.startPlayers or "1")

    for i, scene in pairs(config.scenes) do
        SceneConfig:addScene(FileUtil.getConfigFromYml(scene.path))
    end

    MoneyConfig:initCoinMapping(config.coinMapping)
    RespawnConfig:init(config.respawn)
    ShopConfig:init(config.shop)
    MoneyConfig:initMoneys(FileUtil.getConfigFromYml("money"))
    CommodityConfig:init(FileUtil.getConfigFromCsv("Commodity.csv"))
    InventoryConfig:init(FileUtil.getConfigFromCsv("Inventory.csv"))
    OccupationConfig:init(FileUtil.getConfigFromCsv("Occupation.csv"))
    PotionEffectConfig:init(FileUtil.getConfigFromCsv("PotionEffect.csv"))
    ArmsConfig:init(FileUtil.getConfigFromCsv("Arms.csv"))
    FashionStoreConfig:init(FileUtil.getConfigFromCsv("FashionStore.csv"))
    SkillEffectConfig:init(FileUtil.getConfigFromCsv("SkillEffect.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("Skill.csv"))

end

return GameConfig