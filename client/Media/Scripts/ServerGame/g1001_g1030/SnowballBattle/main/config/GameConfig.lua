--GameConfig.lua
require "config.TeamConfig"
require "config.ScoreConfig"
require "config.SnowballSpeed"

GameConfig = {}

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.sppItems = {}
GameConfig.clzItems = {}

GameConfig.enableBreak = false
GameConfig.obstacle = {}
GameConfig.TNTPrice = {}

function GameConfig:init(config)
    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.prepareTime = tonumber(config.prepareTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)

    self.enableBreak = config.enableBreak
    for k, v in pairs(config.obstacle) do
        self.obstacle[k] = {}
        self.obstacle[k].id = tonumber(v.id)
        self.obstacle[k].hp = tonumber(v.hp)
    end

    self.TNTPrice.coinId = tonumber(config.TNTPrice[1])
    self.TNTPrice.blockymodsPrice = tonumber(config.TNTPrice[2])
    self.TNTPrice.blockmanPrice = tonumber(config.TNTPrice[3])

    self.pickupDuration = tonumber(config.pickupDuration)

    TeamConfig:init(config.teams)
    ScoreConfig:init(config.scores, config.winScore)
    SnowballSpeed:init(config.snowballSpeed)
end

return GameConfig