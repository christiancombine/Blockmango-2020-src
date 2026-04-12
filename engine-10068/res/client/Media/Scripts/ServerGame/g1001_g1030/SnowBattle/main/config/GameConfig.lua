--GameConfig.lua
require "config.TeamConfig"
require "config.ScoreConfig"
require "config.SnowballSpeed"
require "config.Area"

GameConfig = {}

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.sppItems = {}
GameConfig.clzItems = {}

GameConfig.enableBreak = false
GameConfig.obstacle = {}

GameConfig.changeSnowIds = {}
GameConfig.noSnowIds = {}

GameConfig.winCondition = 0

function GameConfig:init(config)

    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.prepareTime = tonumber(config.prepareTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)

    self.enableBreak = config.enableBreak

    for i, v in pairs(config.changeSnowIds) do
        self.changeSnowIds[tonumber(v)] = true
    end
    for i, v in pairs(config.noSnowIds) do
        self.noSnowIds[tonumber(v)] = true
    end

    self.winCondition = config.winCondition

    TeamConfig:init(config.teams)
    ScoreConfig:init(config.scores, config.winScore)
    SnowballSpeed:init(config.snowballSpeed)
end

return GameConfig