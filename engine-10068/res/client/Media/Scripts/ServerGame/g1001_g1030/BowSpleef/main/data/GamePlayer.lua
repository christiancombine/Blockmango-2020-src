--GamePlayer.lua
require "config.ScoreConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.initPos = GameConfig:getValidInitPos()
    self.teleportPos = GameConfig:getValidTeleportPos()

    self.kills = 0
    self.score = 0
    self.isLife = true
    self.isLogout = false
    self.isSingleReward = false

    GameConfig.initPos[self.initPos].use = true
    GameConfig.teleportPos[self.teleportPos].use = true

    self:teleInitPos()
    self:initScorePoint()

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.LIVE] = 0
end

function GamePlayer:initInvItem()
    self:addEnchantmentItem(261, 1, 0, 51, 1)
    self:addItem(262, 1, 0)
end

function GamePlayer:teleInitPos()
    HostApi.resetPos(self.rakssid, GameConfig.initPos[self.initPos].x,
    GameConfig.initPos[self.initPos].y + 0.5, GameConfig.initPos[self.initPos].z)
end

function GamePlayer:teleTeleportPos()
    HostApi.resetPos(self.rakssid, GameConfig.teleportPos[self.teleportPos].x,
    GameConfig.teleportPos[self.teleportPos].y + 0.5, GameConfig.teleportPos[self.teleportPos].z)
end

function GamePlayer:onLogout()
    GameConfig.teleportPos[self.teleportPos].use = false
    GameConfig.initPos[self.initPos].use = false
    if self.isLogout == false then
        self.isLogout = true
    end
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self:reward(false, self.defaultRank, GameMatch:getLifePlayer() == 1)
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = GameMatch:getPlayerRank(self)
    settlement.name = self.name
    settlement.isWin = 0
    settlement.gold = self.gold
    settlement.points = self.scorePoints
    settlement.available = self.available
    settlement.hasGet = self.hasGet
    settlement.vip = self.vip
    settlement.kills = self.kills
    settlement.adSwitch = self.adSwitch or 0
    if settlement.gold <= 0 then
        settlement.adSwitch = 0
    end
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)
end

function GamePlayer:onLifeTick(ticks)
    if self.isLife and ticks ~= 0 then
        self.scorePoints[ScoreID.LIVE] = self.scorePoints[ScoreID.LIVE] + 1
        if ticks % 10 == 0 then
            self.score = self.score + ScoreConfig.LIFE_SCORE
        end
    end
end

function GamePlayer:onKill()
    self.appIntegral = self.appIntegral + 1
end

function GamePlayer:onWin()
    if self.isLife then
        ReportManager:reportUserWin(self.userId)
        self.score = self.score + ScoreConfig.WIN_SCORE
        self.appIntegral = self.appIntegral + 10
    end
end

function GamePlayer:onGameEnd(win)
    self:reward(win, GameMatch:getPlayerRank(self), true)
end

function GamePlayer:overGame(death, win)
    self:onLogout()
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
        if GameMatch.hasEndGame == false then
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
    ReportManager:reportUserData(self.userId, self.kills, GameMatch:getPlayerRank(self), isCount)
end

return GamePlayer

--endregion
