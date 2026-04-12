--GamePlayer.lua
require "config.GameConfig"
require "config.ScoreConfig"
require "config.RespawnConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr(0)

    self.initPos = GameConfig:getValidInitPos()

    self.kills = 0
    self.score = 0
    self.isLife = true
    self.lastKillTime = 0
    self.multiKill = 0
    self.realLife = true
    self.isSingleReward = false

    GameConfig.initPos[self.initPos].use = true

    self:teleInitPos()
    self:initScorePoint()

    ReportManager:reportUserEnter(self.userId)
end

function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
end

function GamePlayer:teleInitPos()
    self.realLife = true
    HostApi.resetPos(self.rakssid, GameConfig.initPos[self.initPos].x,
    GameConfig.initPos[self.initPos].y + 0.5, GameConfig.initPos[self.initPos].z)
end

function GamePlayer:teleRespawnInitPos()
    self.realLife = true
    HostApi.resetPos(self.rakssid, GameConfig.initPos[self.initPos].x,
    GameConfig.initPos[self.initPos].g + 0.5, GameConfig.initPos[self.initPos].z)
end

function GamePlayer:removeFootBlock()
    local initX = GameConfig.initPos[self.initPos].x
    local initY = GameConfig.initPos[self.initPos].y
    local initZ = GameConfig.initPos[self.initPos].z
    for x = -1, 1 do
        for y = -2, 2 do
            for z = -1, 1 do
                local vec = VectorUtil.newVector3i(initX + x, initY + y, initZ + z)
                EngineWorld:setBlockToAir(vec)
            end
        end
    end
end

function GamePlayer:onLogout()
    GameConfig.initPos[self.initPos].use = false
end

function GamePlayer:showBuyRespawn()
    self.realLife = false
    local isRespawn = RespawnConfig:showBuyRespawn(self.rakssid, self.respawnTimes)
    if isRespawn == false then
        self:onDie()
        GameMatch:ifGameOver()
    end
end

function GamePlayer:onKill()
    self.kills = self.kills + 1
    self.score = self.score + ScoreConfig.KILL_SCORE
    self.scorePoints[ScoreID.KILL] = self.scorePoints[ScoreID.KILL] + 1
    self.appIntegral = self.appIntegral + 1

    if self.multiKill == 0 then
        self.multiKill = 1
    else
        if os.time() - self.lastKillTime <= 30 then
            self.multiKill = self.multiKill + 1
            self.scorePoints[ScoreID.SERIAL_KILL] = self.scorePoints[ScoreID.SERIAL_KILL] + 1
        else
            self.multiKill = 1
        end
    end
    self.lastKillTime = os.time()

    if self.multiKill >= 5 then
        self.score = self.score + ScoreConfig.KILL_5_SCORE
        MsgSender.sendMsg(IMessages:msgFiveKill(self:getDisplayName()))
    elseif self.multiKill >= 4 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 3 then
        self.score = self.score + ScoreConfig.KILL_4_SCORE
        MsgSender.sendMsg(IMessages:msgThirdKill(self:getDisplayName()))
    elseif self.multiKill >= 2 then
        self.score = self.score + ScoreConfig.KILL_2_SCORE
        MsgSender.sendMsg(IMessages:msgDoubleKill(self:getDisplayName()))
    end

end

function GamePlayer:onWin()
    if self.isLife then
        ReportManager:reportUserWin(self.userId)
        self.score = self.score + ScoreConfig.WIN_SCORE
        self.appIntegral = self.appIntegral + 10
    end
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.realLife = false
        self:reward(false, self.defaultRank, GameMatch:getLifePlayer() == 1)
    end
end

function GamePlayer:sendPlayerSettlement()
    local settlement = {}
    settlement.rank = GameMatch:getPlayerRank(self)
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
    HostApi.sendPlayerSettlement(self.rakssid, json.encode(settlement), true)
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
