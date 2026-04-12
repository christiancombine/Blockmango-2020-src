require "config.BlockConfig"
require "config.ShopConfig"
require "config.ItemConfig"
require "config.TeamConfig"
require "config.ToolConfig"
require "config.SessionNpcConfig"
require "config.RankNpcConfig"
require "config.CommodityConfig"
require "config.MoneyConfig"
require "config.MerchantConfig"
require "config.JewelConfig"
require "config.ResourceConfig"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.waitingPlayerTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.yaw = tonumber(config.initPos[4])
    GameConfig.waitingPlayerTime = tonumber(config.waitingPlayerTime or "60")
    GameConfig.assignTeamTime = tonumber(config.assignTeamTime or "0")
    GameConfig.gameTime = tonumber(config.gameTime or "1200")
    GameConfig.gameCalculateTime = tonumber(config.gameCalculateTime or "15")
    GameConfig.gameOverTime = tonumber(config.gameOverTime or "30")
    GameConfig.startPlayers = tonumber(config.startPlayers or "4")
    GameConfig.fillBlockId = tonumber(config.fillBlockId)
    GameConfig.respawnTime = tonumber(config.respawnTime  or "1")
    GameConfig.gameTipShowTime = tonumber(config.gameTipShowTime  or "3")
    GameConfig.moneyFromKill = tonumber(config.moneyFromKill or "3")

    ShopConfig:init(config.shop)
    RankNpcConfig:init(config.rankNpc)
    MerchantConfig:initMerchants(config.merchants)
    MerchantConfig:initUpgradeMerchants(config.updateMerchants)
    MoneyConfig:initCoinMapping(config.coinMapping)
    ItemConfig:initRemoveItem(config.removeItem)
    BlockConfig:initBlock(FileUtil.getConfigFromCsv("block.csv"))
    BlockConfig:initMine(FileUtil.getConfigFromCsv("mine.csv"))
    BlockConfig:initItem(FileUtil.getConfigFromCsv("item.csv"))
    BlockConfig:initArea(FileUtil.getConfigFromCsv("mineArea.csv"))
    ItemConfig:initNoDropItem(FileUtil.getConfigFromCsv("noDropItem.csv"))
    TeamConfig:initTeams(FileUtil.getConfigFromCsv("teams.csv"))
    ToolConfig:init(FileUtil.getConfigFromCsv("tools.csv"))
    SessionNpcConfig:initNpc(FileUtil.getConfigFromCsv("sessionNpc.csv"))
    CommodityConfig:initCommoditys(FileUtil.getConfigFromCsv("commodity.csv"))
    JewelConfig:initJewel(FileUtil.getConfigFromCsv("jewel.csv"))
    MerchantConfig:initTools(FileUtil.getConfigFromCsv("toolsShop.csv"))
    ResourceConfig:init(FileUtil.getConfigFromCsv("resource.csv"))
end

return GameConfig