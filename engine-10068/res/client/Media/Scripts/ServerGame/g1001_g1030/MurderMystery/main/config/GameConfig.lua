--GameConfig.lua

require "config.SceneConfig"

GameConfig = {}

GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0
GameConfig.assignRoleTime = 0
GameConfig.resetTime = 0
GameConfig.startPlayers = 0

GameConfig.initPos = {}
GameConfig.teleportPos = {}

GameConfig.initItems = {}
GameConfig.sppItems = {}
GameConfig.clzItems = {}

GameConfig.coinMapping = {}

function GameConfig:init(config)

    self.prepareTime = tonumber(config.prepareTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.resetTime = tonumber(config.resetTime)
    self.assignRoleTime = tonumber(config.assignRoleTime)
    self.startPlayers = tonumber(config.startPlayers)

    self:initCoinMapping(config.coinMapping)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])

    for i, v in pairs(config.scenes) do
        SceneConfig:addScene(FileUtil.getConfigFromYml(v.path))
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