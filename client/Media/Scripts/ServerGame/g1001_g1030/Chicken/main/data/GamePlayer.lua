---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:20
---
require "Match"
require "config.ScoreConfig"
require "config.Area"


GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.config = {}
    self.isLife = true
    self.kills = 0
    self.score = 0
    self.lifeTime = 0
    self.standTime = 0
    self.rank = 0
    self.isSingleReward = false

    self:reset()

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:teleInitPos()
    math.randomseed(os.clock() * 1000000)
    local count = math.random(1, #GameConfig.initPosArea)
    for i, v in pairs(GameConfig.initPosArea) do
        if v.id == count then
            math.randomseed(os.clock())
            local area = Area.new(v.area)
            local x = math.random(area.minX, area.maxX)
            local y = area.maxY
            local z = math.random(area.minZ, area.maxZ)
            self:teleportPos(VectorUtil.newVector3i(x, y, z))
        end
    end
end

function GamePlayer:reset()
    self.isLife = true
    self.score = 0
    self.kills = 0
    self:teleInitPos()
    self:clearInv()
    self:resetFood()
    self:resetHP()
    self:initScorePoint()
    self:setShowName(self:buildShowName())
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.LIVE] = 0
end

function GamePlayer:resetFood()
    self:setFoodLevel(20)
end

function GamePlayer:resetHP()
    self:changeMaxHealth(100)
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.rank = GameMatch:getLifePlayer() + 1
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
        self:reward(false, self.rank, GameMatch:getLifePlayer() == 1)
    end
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + ScoreConfig.KILL_SCORE
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.appIntegral = self.appIntegral + 1
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = self.rank
    settlement.name = self.name
    settlement.isWin = 0
    settlement.points = self.scorePoints
    settlement.gold = self.gold
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), false)
end

function GamePlayer:overGame(death, win)
    if death then
        HostApi.sendGameover(self.rakssid, IMessages:msgGameOverDeath(), GameOverCode.GameOver)
    else
        if win then
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOverWin(), GameOverCode.GameOver)
        else
            HostApi.sendGameover(self.rakssid, IMessages:msgGameOver(), GameOverCode.GameOver)
        end
    end
end

function GamePlayer:onLifeTick(ticks)
    if self.isLife and ticks ~= 0 then
        self.scorePoints[ScoreID.LIVE] = self.scorePoints[ScoreID.LIVE] + 1
        self.score = self.score + ScoreConfig.LIFE_SCORE
    end
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN_SCORE
    self.appIntegral = self.appIntegral + 10
end

function GamePlayer:onGameEnd(win)
    self:reward(win, GameMatch:getPlayerRank(self), true)
end

function GamePlayer:reward(isWin, rank, isEnd)
    if RewardManager:isUserRewardFinish(self.userId) then
        return
    end

    UserExpManager:addUserExp(self.userId, isWin)

    if isWin then
        HostApi.sendPlaySound(self.rakssid, 10023)
    else
        HostApi.sendPlaySound(self.rakssid, 10024)
    end

    if isEnd then
        return
    end
    RewardManager:getUserReward(self.userId, rank, function(data)
        if GameMatch:isGameOver() == false then
            self:sendPlayerSettlement()
        end
    end)

    self.isSingleReward = true
end

function GamePlayer:onQuit()
    local isCount = 0
    if RewardManager:isUserRewardFinish(self.userId) then
        isCount = 1
    end
    ReportManager:reportUserData(self.userId, self.kills, self.rank, isCount)
end

return GamePlayer

--endregion


