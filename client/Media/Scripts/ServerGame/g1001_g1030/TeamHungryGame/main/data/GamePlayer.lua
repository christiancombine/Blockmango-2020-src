--GamePlayer.lua
require "config.GameConfig"
require "config.TeamConfig"
require "data.GameTeam"
require "config.RespawnConfig"
require "config.ScoreConfig"
require "Match"

GamePlayer = class("GamePlayer", BasePlayer)

function GamePlayer:init()

    self:initStaticAttr()

    self.team = {}
    self.initPos = GameConfig:getValidInitPos()
    self.ffPos = GameConfig:getValidffPos()

    self:initTeam()

    self.kills = 0
    self.score = 0
    self.multiKill = 0
    self.lastKillTime = 0

    self.isLife = true
    self.isLogout = false
    self.realLife = true

    GameConfig.initPos[self.initPos].use = true
    GameConfig.ffPos[self.ffPos].use = true

    self:teleInitPos()
    self:initScorePoint()

    ReportManager:reportUserEnter(self.userId)
end


function GamePlayer:initScorePoint()
    self.scorePoints[ScoreID.KILL] = 0
    self.scorePoints[ScoreID.SERIAL_KILL] = 0
end


function GamePlayer:initTeam()
    local team = GameMatch.Teams[self.dynamicAttr.team]
    if team == nil then
        local config = TeamConfig:getTeam(self.dynamicAttr.team)
        team = GameTeam.new(config)
        GameMatch.Teams[config.id] = team
    end
    team:addPlayer()
    self.team = team
end


function GamePlayer:teleInitPos()
    self.realLife = true
    HostApi.resetPos(self.rakssid, GameConfig.initPos[self.initPos].x,
    GameConfig.initPos[self.initPos].y + 0.5, GameConfig.initPos[self.initPos].z)
end

function GamePlayer:teleffPos()
    self.realLife = true
    HostApi.resetPos(self.rakssid, GameConfig.ffPos[self.ffPos].x,
    GameConfig.ffPos[self.ffPos].y + 0.5, GameConfig.ffPos[self.ffPos].z)
end

function GamePlayer:onLogout()
    GameConfig.ffPos[self.ffPos].use = false
    GameConfig.initPos[self.initPos].use = false

    if self.isLogout == false then
        self.isLogout = true
        if self.isLife == true then
            self.team:subPlayer()
        end
    end
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

function GamePlayer:getDisplayName()
    if self.team ~= nil then
        return self.team:getDisplayName() .. TextFormat.colorWrite .. self.name
    else
        return self.name
    end
end

function GamePlayer:buildShowName()
    local nameList = {}

    if self.staticAttr.role ~= -1 then
        table.insert(nameList, self:getClanName())
    end

    -- pureName line
    local disName = self.name
    if self.team ~= nil then
        disName = disName .. self.team:getDisplayName()
    end
    PlayerIdentityCache:buildNameList(self.userId, nameList, disName)
    return table.concat(nameList, "\n")
end

function GamePlayer:onWin()
    ReportManager:reportUserWin(self.userId)
    self.score = self.score + ScoreConfig.WIN_SCORE
end

function GamePlayer:onDie()
    if self.isLife then
        self.isLife = false
        self.realLife = false
        self:reward(false, self.defaultRank, GameMatch:getLifeTeams() == 1)
        self.team:subPlayer()
        HostApi.broadCastPlayerLifeStatus(self.userId, self.isLife)
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

    UserExpManager:addUserExp(self.userId, isWin, 6)

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
