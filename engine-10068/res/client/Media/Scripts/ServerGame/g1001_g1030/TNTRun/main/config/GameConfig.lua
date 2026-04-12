--GameConfig.lua

GameConfig = {}

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.initPos = {}
GameConfig.floorY = {}

GameConfig.sppItems = {}
GameConfig.clzItems = {}

function GameConfig:init(config)

    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.prepareTime = tonumber(config.prepareTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    GameConfig.floorY.F1 = tonumber(config.floorY.F1)
    GameConfig.floorY.F2 = tonumber(config.floorY.F2)
    GameConfig.floorY.F3 = tonumber(config.floorY.F3)
    GameConfig.floorY.F4 = tonumber(config.floorY.F4)

end

function GameConfig:getFloorNum(y)
    if y >= self.floorY.F4 then
        return 4
    elseif y >= self.floorY.F3 then
        return 3
    elseif y >= self.floorY.F2 then
        return 2
    elseif y >= self.floorY.F1 then
        return 1
    else
        return 0
    end
end

return GameConfig