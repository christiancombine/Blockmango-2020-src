---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
require "config.SceneConfig"
require "config.DeformConfig"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.prepareTime = 0
GameConfig.changeActorTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0
GameConfig.assignRoleTime = 0
GameConfig.resetTime = 0
GameConfig.invincibleTime = 0
GameConfig.startPlayers = 0

GameConfig.seekNum = {}

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])

    self.prepareTime = tonumber(config.prepareTime)
    self.changeActorTime = tonumber(config.changeActorTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.invincibleTime = tonumber(config.invincibleTime)
    self.resetTime = tonumber(config.resetTime)
    self.assignRoleTime = tonumber(config.assignRoleTime)
    self.startPlayers = tonumber(config.startPlayers)

    DeformConfig:init(config.deformPrice)
    RankNpcConfig:init(config.rankNpc)

    for i, v in pairs(config.seekNum) do
        self.seekNum[i] = v
    end

    for i, v in pairs(config.scenes) do
        SceneConfig:addScene(FileUtil.getConfigFromYml(v.path))
    end

end

function GameConfig:getSeekNum(number)
    local count = self.seekNum[number]
    if count then
        return count
    else
        return (number - number % 8) + 1
    end
end

return GameConfig