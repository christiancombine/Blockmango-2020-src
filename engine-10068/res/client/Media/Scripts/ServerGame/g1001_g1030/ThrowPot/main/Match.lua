---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "util.RealRankUtil"
require "util.RewardUtil"
require "util.DbUtil"

GameMatch = {}

GameMatch.gameType = "g1026"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.Running = 3
GameMatch.Status.GameOver = 4
GameMatch.Status.WaitingReset = 5

GameMatch.hasEndGame = false

GameMatch.gamePrepareTick = 0
GameMatch.gameOverTick = 0

GameMatch.RoundStartTick = 0
GameMatch.RoundCount = 0

GameMatch.curRound = nil
GameMatch.curTick = 0

GameMatch.hasSentStartGame = false

function GameMatch:initMatch()
    RealRankUtil:init()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:ifUpdateRank()

    if self.curStatus == self.Status.WaitingPlayer then
        self:ifWaitingPlayerEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifRunningEnd()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end

    if self.curStatus == self.Status.WaitingReset then
        self:ifResetEnd()
    end

end

function GameMatch:ifWaitingPlayerEnd()
    local playerCount = PlayerManager:getPlayerCount()
    if playerCount >= GameConfig.startPlayers then
        self:doWaitingEnd()
    else
        if self.curTick % 30 == 0 then
            MsgSender.sendMsg(IMessages:msgWaitPlayer())
            MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
            MsgSender.sendMsg(IMessages:longWaitingHint())
        end
    end
end

function GameMatch:ifPrepareEnd()

    local seconds = GameConfig.prepareTime - (self.curTick - self.gamePrepareTick);

    if seconds == 45 or seconds == 30 then
        MsgSender.sendMsg(IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
    end

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds == 6 and PlayerManager:getPlayerCount() > 1 then
        HostApi.sendStartGame(PlayerManager:getPlayerCount())
        self.hasSentStartGame = true
    end

    if seconds <= 0 then
        self:doPrepareEnd()
    end
end

function GameMatch:ifRunningEnd()

    if self.curRound == nil then
        self:doRoundPrepare()
    end

    local ticks = self.curTick - self.RoundStartTick

    local seconds = 0

    if self.curRound.status == RoundConfig.Prepare then
        seconds = 5 - ticks;
    else
        seconds = self.curRound.duration - ticks;
    end

    if seconds <= 0 then
        if self.curRound.status == RoundConfig.Prepare then
            self:doRoundStarted()
        else
            self:doRoundEnd()
        end
    end

    self:onPlayerTick()
end

function GameMatch:onPlayerTick()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:ifGameOverEnd()

    local seconds = GameConfig.gameOverTime - (self.curTick - self.gameOverTick);

    MsgSender.sendBottomTips(3, IMessages:msgGameOverHint(seconds))

    if seconds <= 0 then
        self:resetGame()
    end
end

function GameMatch:ifResetEnd()
    self:doResetEnd()
end

function GameMatch:ifUpdateRank()
    local playerCount = PlayerManager:getPlayerCount()
    if self.curTick % 600 == 0 and playerCount > 0 then
        RankNpcConfig:updateRank()
    end
end

function GameMatch:doResetEnd()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:reset()
    end
    self.curStatus = self.Status.WaitingPlayer
end

function GameMatch:doWaitingEnd()
    self.curStatus = self.Status.Prepare
    self.gamePrepareTick = self.curTick
    MsgSender.sendMsg(IMessages:msgCanStartGame())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:doPrepareEnd()
    if PlayerManager:getPlayerCount() <= 1 then
        if self.hasSentStartGame then
            HostApi.sendResetGame(PlayerManager:getPlayerCount())
            self.hasSentStartGame = false
        end
        self:doResetEnd()
        return
    end
    self.curStatus = self.Status.Running
    self.gameStartTick = self.curTick
    RewardManager:startGame()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:teleGamePos()
    end

    HostApi.closeBGM(0)
end

function GameMatch:doRoundPrepare()
    self.curRound = RoundConfig:getRoundByMinPlayer(self:getLifePlayer())
    self.curRound.status = RoundConfig.Prepare
    self.RoundStartTick = self.curTick
    if self.RoundCount == 0 then
        self:doRoundStarted()
    end
end

function GameMatch:doRoundStarted()
    self.curRound.status = RoundConfig.Started
    self.RoundCount = self.RoundCount + 1
    self.RoundStartTick = self.curTick
    HostApi.syncGameTimeShowUi(0, true, self.curRound.duration)
    if self.curRound.mode == 1 then
        MsgSender.sendMsg(Messages:msgDeathmatchMode())
    end
    MsgSender.sendMsg(Messages:msgRoundStartHint(self.RoundCount))
    local num = 0
    if #self.curRound.tntNum >= 2 then
        num = math.random(self.curRound.tntNum[1], self.curRound.tntNum[2])
    else
        num = self.curRound.tntNum[1]
    end
    local players = self:randomLifePlayers(math.min(1, num))
    local result
    for _, player in pairs(players) do
        if result == nil then
            result = player:getDisplayName()
        else
            result = result .. "、" .. player:getDisplayName()
        end
        player:becomeTntPlayer()
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife and player.isHaveTnt == false then
            player:becomeNormalPlayer()
        end
        if player.isLife and self.curRound.mode == 1 then
            player:teleRoundPos(self.curRound.position)
        end
    end
    if result then
        MsgSender.sendMsg(Messages:msgTNTPlayersHint(result))
    end
end

function GameMatch:doRoundEnd()
    self.curRound = nil
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isHaveTnt then
            player:boom()
        end
    end
    self:ifGameOver()
    HostApi.syncGameTimeShowUi(0, false, 0)
end

function GameMatch:ifGameOver()
    if self:isGameRunning() == false then
        if PlayerManager:isPlayerEmpty() then
            self:doResetEnd()
        end
        return
    end
    if self:getLifePlayer() <= 1 then
        self:doGameOver()
    end
    if self:hasNormalPlayer() == false then
        self:doGameOver()
    end
end

function GameMatch:doGameOver()
    MsgSender.sendTopTips(1, " ")
    self.curStatus = self.Status.GameOver
    self.gameOverTick = self.curTick
    self.hasEndGame = true
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife then
            player.rank = 1
            player:onGameEnd()
        end
    end

    local players = RewardUtil:sortPlayerRank()
    RewardUtil:doReward(players)
    RewardUtil:doReport(players)

    HostApi.sendGameStatus(0, 2)
    HostApi.sendGameStatus(0, 1)

end

function GameMatch:resetGame()
    self.curStatus = self.Status.WaitingReset
    self.hasEndGame = false
    self.RoundCount = 0
    MsgSender.sendMsg(IMessages:msgGameResetHint())
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        RealRankUtil:getPlayerRankData(player)
    end
    HostApi.sendPlaySound(0, 10000)
    HostApi.sendPlaySound(0, 10031)
end

function GameMatch:onPlayerQuit(player)
    self:ifGameOver()
end

function GameMatch:isGameRunning()
    return self.curStatus == self.Status.Running
end

function GameMatch:randomLifePlayers(num)
    local players = {}
    local playerlist = PlayerManager:getPlayers()
    for _, player in pairs(playerlist) do
        if player.isLife then
            players[#players + 1] = player
        end
    end
    if #players < 2 then
        return players
    end
    num = math.min(num, #players - 1)
    local random = #players - num
    while random > 0 do
        table.remove(players, math.random(1, #players))
        random = random - 1
    end
    return players
end

function GameMatch:getLifePlayer()
    local lifes = 0
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife then
            lifes = lifes + 1
        end
    end
    return lifes
end

function GameMatch:hasNormalPlayer()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife and player.isHaveTnt == false then
            return true
        end
    end
    return false
end


return GameMatch