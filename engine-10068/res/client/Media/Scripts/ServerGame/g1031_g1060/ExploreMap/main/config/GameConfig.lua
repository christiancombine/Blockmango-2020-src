---
--- Created by Yaoqiang.
--- DateTime: 2018/6/22 0025 17:50
---

GameConfig = {}

GameConfig.initPos = {}

GameConfig.prepareTime = 0
GameConfig.customTime = 0
GameConfig.gameLevelTime = 0
GameConfig.gameOverTime = 0

GameConfig.startPlayers = 0

function GameConfig:init(config)
    GameConfig.initPos = VectorUtil.newVector3i(config.initPos[1], config.initPos[2], config.initPos[3])

    self.prepareTime = tonumber(config.prepareTime)
    self.readyToStartTime = tonumber(config.readyToStartTime)
    self.buildTime = tonumber(config.buildTime)
    self.gradeTime = tonumber(config.gradeTime)
    self.gradeIntervalTime = tonumber(config.gradeIntervalTime)
    self.guessTime = tonumber(config.guessTime)
    self.gameOverTime = tonumber(config.gameOverTime)
    self.startPlayers = tonumber(config.startPlayers)
    -- self.targetPos = config.targetPos
    -- self.questionRange = config.questionRange
    -- self.gradeAverage = config.gradeAverage
    -- self.guessRightReward = tonumber(config.guessRightReward)
    -- self.gradeFavouriteNum = tonumber(config.gradeFavouriteNum)
    -- self.waitingRoomId = tonumber(config.waitingRoomId)
    -- self.gradeList = config.gradeList
    -- self.buildOverInterval = tonumber(config.buildOverInterval)
    -- self.gradeOverIntervalTime = tonumber(config.gradeOverIntervalTime)
    -- self.guessOverIntervalTime = tonumber(config.guessOverIntervalTime)
    -- self.blockHardness = tonumber(config.blockHardness)

    self.roomCount = tonumber(config.roomCount)
    self.spaceSize = config.spaceSize
    self.spaceStart = config.spaceStart
    self.roomInfo = config.roomInfo
    self.typeRadio = config.typeRadio
    self.fillBlockId = config.fillBlockId
    self.needFillBlock = config.needFillBlock
    self.roomBlockInfo = config.roomBlockInfo
end

function GameConfig:prepareBlockHardness()
    for i = 1, 256 do
        HostApi.setBlockAttr(i, self.blockHardness)
    end
end

function GameConfig:getRoomBlockConfig(file)
    if self.roomBlockInfo then
        for k, v in pairs(self.roomBlockInfo) do
            if tostring(v.file) == tostring(file) then
                return v
            end
        end
    end
    return nil
end

return GameConfig