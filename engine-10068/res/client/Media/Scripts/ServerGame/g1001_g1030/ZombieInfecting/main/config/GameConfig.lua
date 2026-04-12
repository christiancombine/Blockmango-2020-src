--GameConfig.lua
require "config.SceneConfig"
require "config.ShopConfig"
require "config.WeaponConfig"
require "config.HunterConfig"

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
    self.gameOverTime = tonumber(config.gameOverTime)
    self.resetTime = tonumber(config.resetTime)
    self.assignRoleTime = tonumber(config.assignRoleTime)
    self.startPlayers = tonumber(config.startPlayers)
    --self.maxPlayers = tonumber(config.maxPlayers)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])

    for i, v in pairs(config.scenes) do
        SceneConfig:addScene(FileUtil.getConfigFromYml(v.path))
    end

    ShopConfig:init(config.shop)
    WeaponConfig:init(config.weapon)
    HunterConfig:init(FileUtil.getConfigFromYml("hunter"))
end

return GameConfig