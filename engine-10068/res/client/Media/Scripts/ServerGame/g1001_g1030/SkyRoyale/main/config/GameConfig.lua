require "config.TeamConfig"
require "config.BlockConfig"
require "config.ChestConfig"
require "config.EnchantmentShop"
require "config.ShopConfig"
require "config.ItemConfig"
require "config.NewIslandConfig"
require "config.BridgeConfig"

GameConfig = {}


GameConfig.waitingPlayerTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

function GameConfig:init(config)
    
    GameConfig.waitingPlayerTime = tonumber(config.waitingPlayerTime or "60")
    GameConfig.assignTeamTime = tonumber(config.assignTeamTime or "0")
    GameConfig.gameTime = tonumber(config.gameTime or "1200")
    GameConfig.gameOverTime = tonumber(config.gameOverTime or "30")
    GameConfig.startPlayers = tonumber(config.startPlayers or "1")
    GameConfig.respawnTime = tonumber(config.respawnTime or "3")
    GameConfig.gameTipShowTime = tonumber(config.gameTipShowTime or "3")
    GameConfig.hpRecoverCD = tonumber(config.hpRecoverCD)
    GameConfig.hpRecover = tonumber(config.hpRecover)
    GameConfig.entityItemLifeTime = tonumber(config.entityItemLifeTime or "6000")
    GameConfig.isKeepInventory = tonumber(config.isKeepInventory or "0")
    GameConfig.playerHp = tonumber(config.playerHp or "20")
    GameConfig.teamNum = tonumber(config.teamNum or "4")
    GameConfig.canRespawnBeforeTime = tonumber(config.canRespawnBeforeTime or "500")
    --广告配置次数
    GameConfig.respawnAdMaxTimes = tonumber(config.respawnAdMaxTimes or "1")

    ShopConfig:init(config.shop)
    BridgeConfig:init(config.bridge)

    SnowBallConfig:initSnowBall(config.snowBall)
    SnowBallConfig:initDamage(tonumber(config.snowBallDamage or "1"))
    NewIslandConfig:initIslandWall(config.islandWall)

    ChestConfig:initChestArea(FileUtil.getConfigFromCsv("chestArea.csv"))
    ChestConfig:initChestTeam(FileUtil.getConfigFromCsv("chestTeam.csv"))
    ChestConfig:initChest(FileUtil.getConfigFromCsv("chest.csv"))
    TeamConfig:initTeams(FileUtil.getConfigFromCsv("teams.csv"))
    RespawnConfig:initRespawnGoods(FileUtil.getConfigFromCsv("respawn.csv"))
    BlockConfig:initBlock(FileUtil.getConfigFromCsv("block.csv"))
    BlockConfig:initBlockHardness(FileUtil.getConfigFromCsv("blockHardness.csv"))
    EnchantmentShop:initEnchantment(FileUtil.getConfigFromCsv("enchantmentShop.csv"))
    ItemConfig:initItem(FileUtil.getConfigFromCsv("playerItem.csv"))

end

return GameConfig