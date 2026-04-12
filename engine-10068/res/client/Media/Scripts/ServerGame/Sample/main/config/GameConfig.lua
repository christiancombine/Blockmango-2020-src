---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:16
---
GameConfig = {}

GameConfig.initPos = {}

GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

function GameConfig:init(config)
    GameConfig.initPos = VectorUtil.newVector3(config.initPos[1], config.initPos[2], config.initPos[3])

    self.prepareTime = tonumber(config.prepareTime or "10")
    self.gameTime = tonumber(config.gameTime or "60")
    self.gameOverTime = tonumber(config.gameOverTime or "10")

    self.startPlayers = tonumber(config.startPlayers or "1")

end

return GameConfig