---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---
require "config.Area"
require "config.ShopConfig"
require "config.MerchantConfig"

GameConfig = {}

GameConfig.initPos = {}

GameConfig.prepareTime = 0
GameConfig.customTime = 0
GameConfig.gameLevelTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

function GameConfig:init(config)

    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])
    ShopConfig:init(FileUtil.getConfigFromYml("shop"))
    MerchantConfig:init(FileUtil.getConfigFromYml("merchant"))
    RankNpcConfig:init(config.rankNpc)

    self.prepareTime = tonumber(config.prepareTime)
    self.readyToStartTime = tonumber(config.readyToStartTime)
    self.buildTime = tonumber(config.buildTime)
    self.gradeTime = tonumber(config.gradeTime)
    self.gradeIntervalTime = tonumber(config.gradeIntervalTime)
    self.guessTime = tonumber(config.guessTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.startPlayers = tonumber(config.startPlayers)
    self.targetPos = config.targetPos
    self.buildHallSize = config.buildHallSize
    self:calcBuildHallArea()
    self.questionRange = config.questionRange
    self.gradeAverage = config.gradeAverage
    self.guessRightReward = tonumber(config.guessRightReward)
    self.gradeFavouriteNum = tonumber(config.gradeFavouriteNum)
    self.waitingRoomId = tonumber(config.waitingRoomId)
    self.gradeList = config.gradeList
    self.buildOverInterval = tonumber(config.buildOverInterval)
    self.gradeOverIntervalTime = tonumber(config.gradeOverIntervalTime)
    self.guessOverIntervalTime = tonumber(config.guessOverIntervalTime)
    self.blockHardness = tonumber(config.blockHardness)
end

function GameConfig:calcBuildHallArea()
    for k, v in pairs(self.targetPos) do
        local center_x = v.pos[1] + self.buildHallSize[1]/2
        local center_y = v.pos[2] + self.buildHallSize[2]/2
        local center_z = v.pos[3]

        local matrix = {}
        matrix[1] = {}
        matrix[1][1] = center_x - (self.buildHallSize[1]/2 + 15) -- 15 blocks for tolerance
        matrix[1][2] = center_y - (self.buildHallSize[2]/2 + 15) -- 15 blocks for tolerance
        matrix[1][3] = center_z - (self.buildHallSize[3]/2 + 15) -- 15 blocks for tolerance
        matrix[2] = {}
        matrix[2][1] = center_x + (self.buildHallSize[1]/2 + 15) -- 15 blocks for tolerance
        matrix[2][2] = center_y + (self.buildHallSize[2]/2 + 15) -- 15 blocks for tolerance
        matrix[2][3] = center_z + (self.buildHallSize[3]/2 + 15) -- 15 blocks for tolerance
        self.targetPos[k].area = Area.new(matrix)
    end
end

function GameConfig:prepareBlockHardness()
    for i = 1, 256 do
        HostApi.setBlockAttr(i, self.blockHardness)
    end
end

return GameConfig