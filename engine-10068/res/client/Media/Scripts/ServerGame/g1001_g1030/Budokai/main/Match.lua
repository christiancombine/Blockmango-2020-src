---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "config.SiteConfig"
require "config.RankRewardConfig"
require "util.RewardUtil"
require "util.DbUtil"

GameMatch = {}

GameMatch.gameType = "g1028"

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

GameMatch.curRound = nil
GameMatch.curTick = 0

function GameMatch:initMatch()
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

function GameMatch:ifUpdateRank()
    if self.curTick % 600 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

function GameMatch:ifWaitingPlayerEnd()
    if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
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
        seconds = self.curRound.prepare - ticks;
    else
        seconds = self.curRound.duration - ticks;
        SiteConfig:onTick(ticks)
    end

    if seconds <= 10 and seconds > 0 then
        HostApi.sendPlaySound(0, 11);
    end

    if seconds <= 0 then
        if self.curRound.status == RoundConfig.Prepare then
            self:doRoundStarted()
        else
            self:doRoundEnd()
        end
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
    local playerCount = PlayerManager:getPlayerCount()
    if playerCount == 1 then
        self:doResetEnd()
        HostApi.syncGameTimeShowUi(0, false, 0)
        return
    end
    self.curStatus = self.Status.Running
    self.gameStartTick = self.curTick
    RewardManager:startGame()
    HostApi.sendStartGame(playerCount)
end

function GameMatch:doRoundPrepare()
    self.curRound = RoundConfig:getRoundByMinPlayer(self:getLifePlayer())
    self.RoundStartTick = self.curTick
    self.curRound.status = RoundConfig.Prepare
    HostApi.syncGameTimeShowUi(0, true, self.curRound.prepare)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife then
            player:teleGamePos()
            MsgSender.sendMsgToTarget(player.rakssid, Messages:msgGamePrepareTip(self.curRound.roundTip))
            MsgSender.sendTopTipsToTarget(player.rakssid, 60, Messages:msgGameBuyTip())
            for _, coin in pairs(self.curRound.coins) do
                player:addItem(coin.id, coin.num, 0)
            end
        else
            player:teleWatchPos(self.curRound.watchPos)
            MsgSender.sendTopTipsToTarget(player.rakssid, 300, Messages:msgWatchModeTip())
        end
    end
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(self.curRound.prepare, IMessages.UNIT_SECOND, false))
end

function GameMatch:doRoundStarted()
    SiteConfig:onGameReset()
    self.RoundStartTick = self.curTick
    self.curRound.status = RoundConfig.Started
    HostApi.syncGameTimeShowUi(0, true, self.curRound.duration)
    local players = self:getLifePlayers()
    for _, player in pairs(players) do
        MsgSender.sendMsgToTarget(player.rakssid, Messages:msgGameStartedTip(self.curRound.roundTip))
    end
    local times = 0
    local sites = SiteConfig:getSitesByType(self.curRound.siteType)
    local site
    while #players > 0 do
        if times % 2 == 0 then
            site = sites[times / 2 + 1]
        end
        if site == nil then
            break
        end
        times = times + 1
        local random = math.random(1, #players)
        site:addPlayer(players[random])
        table.remove(players, random)
    end
    SiteConfig:onGameStart()
end

function GameMatch:doRoundEnd()
    self.curRound = nil
    SiteConfig:onGameEnd()
    self:ifGameOver()
end

function GameMatch:ifGameOver()
    if self.curStatus ~= self.Status.Running then
        if PlayerManager:getPlayerCount() <= 1 then
            self:doResetEnd()
            HostApi.syncGameTimeShowUi(0, false, 0)
        end
        return
    end
    if self.curRound ~= nil and self.curRound.status == RoundConfig.Started then
        if SiteConfig:ifRoundEnd() then
            self.curRound = nil
            SiteConfig:onGameEnd()
        end
    end
    if self:getLifePlayer() <= 1 then
        self:doGameOver()
    end
end

function GameMatch:doGameOver()
    if self.curStatus == self.Status.GameOver then
        return
    end
    self.curStatus = self.Status.GameOver
    self.gameOverTick = self.curTick
    self.hasEndGame = true
    self.curRound = nil
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isLife then
            player.rank = 1
            player:onGameEnd()
            ReportManager:reportUserWin(player.userId)
        end
        local reward = RankRewardConfig:getRewardByRank(player.rank)
        player:addCurrency(reward.money)
        MsgSender.sendMsgToTarget(player.rakssid, Messages:msgGetMoney(reward.money))
    end
    players = RewardUtil:sortPlayerRank()
    RewardUtil:doReward(players)
    RewardUtil:doReport(players)
    HostApi.syncGameTimeShowUi(0, false, 0)
end

function GameMatch:resetGame()
    self.curStatus = self.Status.WaitingReset
    self.hasEndGame = false
    MsgSender.sendMsg(IMessages:msgGameResetHint())
    MsgSender.sendTopTips(1, " ")
    SiteConfig:onGameReset()
    HostApi.sendGameStatus(0, 2)
    HostApi.sendGameStatus(0, 1)
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
end

function GameMatch:onPlayerQuit(player)
    if self:isGameRunning() then
        SiteConfig:onPlayerQuit(player)
    else
        player.isLife = false
        self:ifGameOver()
    end
    player:onQuit()
end

function GameMatch:isGameStarted()
    return self.curStatus == self.Status.Running
end

function GameMatch:isGameRunning()
    if self.curStatus == self.Status.Running then
        if self.curRound ~= nil and self.curRound.status == RoundConfig.Started then
            return true
        end
    end
    return false
end

function GameMatch:getLifePlayers()
    local players = {}
    local playerlist = PlayerManager:getPlayers()
    for _, player in pairs(playerlist) do
        if player.isLife then
           table.insert(players, player)
        end
    end
    return players
end

function GameMatch:getLifePlayer()
    local lifes = 0
    local playerlist = PlayerManager:getPlayers()
    for _, player in pairs(playerlist) do
        if player.isLife then
            lifes = lifes + 1
        end
    end
    return lifes
end

return GameMatch