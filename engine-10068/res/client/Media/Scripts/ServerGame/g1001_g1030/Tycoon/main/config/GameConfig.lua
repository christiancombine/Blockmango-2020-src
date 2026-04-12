--GameConfig.lua
require "config.PrivateResourceConfig"
require "config.UpgradeEquipBoxConfig"
require "config.PublicResourceConfig"
require "config.GameExplainConfig"
require "config.ExtraPortalConfig"
require "config.SkillEffectConfig"
require "config.ResourceBuilding"
require "config.OccupationConfig"
require "config.InventoryConfig"
require "config.CommodityConfig"
require "config.EquipBoxConfig"
require "config.BuildingConfig"
require "config.MerchantConfig"
require "config.RespawnConfig"
require "config.PortalConfig"
require "config.DomainConfig"
require "config.BasicConfig"
require "config.SkillConfig"
require "config.GateConfig"
require "config.RankConfig"
require "config.ShopConfig"
require "Match"

GameConfig = {}

GameConfig.waitingTime = 0
GameConfig.prepareTime = 0
GameConfig.gameOverTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0
GameConfig.maxPlayers = 0

GameConfig.initPos = {}
GameConfig.gamePos = {}

GameConfig.coinMapping = {}
GameConfig.baseOccIds = {}
function GameConfig:init(config)

    self.waitingTime = tonumber(config.waitingTime or 30)
    self.prepareTime = tonumber(config.prepareTime or 10)
    self.selectRoleTime = tonumber(config.selectRoleTime or 20)
    self.gameTime = tonumber(config.gameTime or 1200)
    self.gameOverTime = tonumber(config.gameOverTime or 20)
    self.money = tonumber(config.money or 11)
    self.startPlayers = tonumber(config.startPlayers or 1)
    self.dropMoneyPercent = tonumber(config.dropMoneyPercentbyDie or 0.5)
    self.rewardMoneyPercent = tonumber(config.rewardMoneyPercent or 0.3)
    self.maxPlayers = PlayerManager:getMaxPlayer()
    self.occSelectMaxNum = tonumber(config.occSelectMaxNum or 2)
    self.CollisionDistance = tonumber(config.CollisionDistance or 2)
    self.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    self.gamePos = VectorUtil.newVector3i(config.gamePos[1], config.gamePos[2], config.gamePos[3])
    self.selectRolePos = VectorUtil.newVector3i(config.selectRolePos[1], config.selectRolePos[2], config.selectRolePos[3])

    self.videoAdMoney = tonumber(config.videoAdMoney or "2500")
    self.videoAdMoneyCd = tonumber(config.videoAdMoneyCd or "120")
    self.videoAdMoneyTimes = tonumber(config.videoAdMoneyTimes or "999")
    self.videoAdBuilding = tonumber(config.videoAdBuilding or "4000")
    self.videoAdBuildingCd = tonumber(config.videoAdBuildingCd or "120")
    self.videoAdBuildingTimes = tonumber(config.videoAdBuildingTimes or "999")
    self.videoAdTntTimes = tonumber(config.videoAdTntTimes or "999")
    self.videoAdLadderTimes = tonumber(config.videoAdLadderTimes or "999")

    for _, id in pairs(config.baseOccIds) do
        table.insert(self.baseOccIds, id)
    end
    self:initCoinMapping(config.coinMapping)
    RankConfig:init(config.ranks)
    RespawnConfig:init(config.respawn)
    ShopConfig:init(config.shop)
    MerchantConfig:initMerchants(config.merchants)

    BasicConfig:init(FileUtil.getConfigFromCsv("BasicPoint.csv"))
    InventoryConfig:init(FileUtil.getConfigFromCsv("Inventory.csv"))
    OccupationConfig:init(FileUtil.getConfigFromCsv("Occupation.csv"))
    GateConfig:init(FileUtil.getConfigFromCsv("Gate.csv"))
    UpgradeEquipBoxConfig:init(FileUtil.getConfigFromCsv("UpgradeEquipBox.csv"))
    EquipBoxConfig:init(FileUtil.getConfigFromCsv("EquipBox.csv"))
    BuildingConfig:init(FileUtil.getConfigFromCsv("Building.csv"))
    ResourceBuilding:init(FileUtil.getConfigFromCsv("ResourceBuilding.csv"))
    PrivateResourceConfig:init(FileUtil.getConfigFromCsv("PrivateResource.csv"))
    PortalConfig:init(FileUtil.getConfigFromCsv("PortalBuilding.csv"))
    ExtraPortalConfig:init(FileUtil.getConfigFromCsv("ExtraPortal.csv"))
    DomainConfig:init(FileUtil.getConfigFromCsv("Domain.csv"))
    PublicResourceConfig:init(FileUtil.getConfigFromCsv("PublicResource.csv"))
    SkillEffectConfig:init(FileUtil.getConfigFromCsv("SkillEffect.csv"))
    SkillConfig:init(FileUtil.getConfigFromCsv("Skill.csv"))
    GameExplainConfig:init(FileUtil.getConfigFromCsv("GameExplain.csv"))
    CommodityConfig:initCommoditys(FileUtil.getConfigFromCsv("Commodity.csv"))
    PersonResourceConfig:init(FileUtil.getConfigFromCsv("PersonResource.csv"))
    OccupationConfig:initUnlockOccupationInfo(FileUtil.getConfigFromCsv("OccupationUnlock.csv"))
end

function GameConfig:initCoinMapping(config)
    for i, v in pairs(config) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = tonumber(v.coinId)
        self.coinMapping[i].itemId = tonumber(v.itemId)
    end
    WalletUtils:addCoinMappings(self.coinMapping)
end

function GameConfig:isCoinbyItemId(itemId)
    for _, item in pairs(self.coinMapping) do
        if tonumber(item.itemId) == tonumber(itemId) then
            return true
        end
    end
    return false
end

return GameConfig