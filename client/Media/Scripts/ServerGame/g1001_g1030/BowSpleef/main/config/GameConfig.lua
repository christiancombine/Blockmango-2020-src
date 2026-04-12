--GameConfig.lua

GameConfig = {}

GameConfig.waitingPlayerTime = 0
GameConfig.prepareTime = 0
GameConfig.gameTime = 0
GameConfig.gameOverTime = 0

GameConfig.goldGrowth = 0
GameConfig.initGold = 1000

GameConfig.initPos = {}
GameConfig.teleportPos = {}

GameConfig.sppItems = {}
GameConfig.clzItems = {}

function GameConfig:init(config)
    self.waitingPlayerTime = tonumber(config.waitingPlayerTime)
    self.prepareTime = tonumber(config.prepareTime)
    self.gameTime = tonumber(config.gameTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.goldGrowth = tonumber(config.goldGrowth)

    for i, v in pairs(config.initPos) do
        self.initPos[i] = {}
        self.initPos[i].use = false
        self.initPos[i].x = tonumber(v.x)
        self.initPos[i].y = tonumber(v.y)
        self.initPos[i].z = tonumber(v.z)
    end

    for i, v in pairs(config.teleportPos) do
        self.teleportPos[i] = {}
        self.teleportPos[i].use = false
        self.teleportPos[i].x = tonumber(v.x)
        self.teleportPos[i].y = tonumber(v.y)
        self.teleportPos[i].z = tonumber(v.z)
    end
end

function GameConfig:getValidInitPos()
    for i, v in pairs(self.initPos) do
        if(v.use == false) then
            return i
        end
    end
    return math.random(1, #self.initPos)
end

function GameConfig:getValidTeleportPos()
    for i, v in pairs(self.teleportPos) do
        if(v.use == false) then
            return i
        end
    end
    return math.random(1, #self.teleportPos)
end

return GameConfig