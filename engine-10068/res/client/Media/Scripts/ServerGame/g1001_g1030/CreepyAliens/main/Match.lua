---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "config.GameLevelConfig"
require "config.BasementConfig"

GameMatch = {}

GameMatch.gameType = "g1019"

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.Waiting = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.SelectMap = 3
GameMatch.Status.Running = 4
GameMatch.Status.GameOver = 5
GameMatch.Status.Reset = 6

GameMatch.level = 0
GameMatch.isAddLevel = false
GameMatch.levelMonsters = {}
GameMatch.levelMonsterNum = 0

GameMatch.curTick = 0
GameMatch.curStatus = GameMatch.Status.Init

GameMatch.WaitingTick = 0
GameMatch.PrepareTick = 0
GameMatch.SelectMapTick = 0
GameMatch.GamePrepareTick = 0
GameMatch.RunningTick = 0
GameMatch.GameOverTick = 0
GameMatch.ResetTick = 0

GameMatch.curGameScene = nil
GameMatch.curGameLevel = nil

GameMatch.isFirstReady = false
GameMatch.isReady = false

function GameMatch:initMatch()
    GameTimeTask:start()
    GameMatch.curStatus = self.Status.Waiting
end

function GameMatch:onTick(ticks)
    self.curTick = ticks

    self:ifSaveRankData()
    self:ifUpdateRank()

    if self.curStatus == self.Status.Waiting then
        self:ifWaitingEnd()
    end

    if self.curStatus == self.Status.Prepare then
        self:ifPrepareEnd()
    end

    if self.curStatus == self.Status.SelectMap then
        self:ifSelectMapEnd()
    end

    if self.curStatus == self.Status.Running then
        self:ifRunningEnd()
    end

    if self.curStatus == self.Status.GameOver then
        self:ifGameOverEnd()
    end

    if self.curStatus == self.Status.Reset then
        self:ifResetEnd()
    end

end

function GameMatch:ifSaveRankData()
    if self.curTick == 0 or self.curTick % 30 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        RankNpcConfig:savePlayerRankScore(player)
    end
end

function GameMatch:ifUpdateRank()
    if self.curTick % 600 == 0 and PlayerManager:isPlayerExists() then
        RankNpcConfig:updateRank()
    end
end

function GameMatch:ifWaitingEnd()

    if self.isFirstReady then
        self.isFirstReady = false
        self.isReady = true
        self.WaitingTick = self.curTick
    end

    if self.isReady then

        local seconds = GameConfig.waitingTime - (self.curTick - self.WaitingTick);

        if PlayerManager:isPlayerEnough(GameConfig.startPlayers)  then
            self:prepareGame()
        end

        if seconds <= 0 then
            if PlayerManager:isPlayerEnough(GameConfig.startPlayers) then
                self:prepareGame()
            else
                self.WaitingTick = self.curTick
                MsgSender.sendMsg(IMessages:msgWaitPlayer())
                MsgSender.sendMsg(IMessages:msgGamePlayerNum(PlayerManager:getPlayerCount(), GameConfig.startPlayers))
                MsgSender.sendMsg(Messages:longWaitingHint())
            end
        end
    end

end

function GameMatch:ifPrepareEnd()

    local seconds = GameConfig.prepareTime - (self.curTick - self.PrepareTick);

    if seconds <= 10 and seconds > 0 then
        MsgSender.sendBottomTips(3, IMessages:msgGameStartTimeHint(seconds, IMessages.UNIT_SECOND, false))
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 or PlayerManager:isPlayerFull() then
        self:startSelectMap()
    end

end

function GameMatch:ifSelectMapEnd()
    local seconds = GameConfig.selectMapTime - (self.curTick - self.SelectMapTick);

    if seconds <= 10 and seconds > 0 then
        if seconds <= 3 then
            HostApi.sendPlaySound(0, 12);
        else
            HostApi.sendPlaySound(0, 11);
        end
    end

    if seconds <= 0 then
        self:selectMap()
    end
end

function GameMatch:ifRunningEnd()

    if self.isAddLevel then
        if self:upgradeGameLevel() == false then
            return
        end
    end

    if self.GamePrepareTick <= GameConfig.gamePrepareTime then
        local seconds = GameConfig.gamePrepareTime - self.GamePrepareTick;
        if seconds <= 10 and seconds > 0 then
            MsgSender.sendTopTips(3, Messages:gamePrepareTimeHint(seconds))
            if seconds <= 3 then
                HostApi.sendPlaySound(0, 12);
            else
                HostApi.sendPlaySound(0, 11);
            end
        end
        self.GamePrepareTick = self.GamePrepareTick + 1
        return
    end

    local ticks = self.curTick - self.RunningTick

    local seconds = GameConfig.gameLevelTime - ticks;

    if ticks % GameConfig.refreshMonsterTime == 0 then
        GameLevelConfig:prepareMonster(self.levelMonsters, self.levelMonsterNum / GameConfig.refreshMonsterTimes, self.curGameLevel.monsterLv)
    end

    self:getCurGameScene():onTick()
    self:onPlayerTick()

    if seconds <= 0 then
        self:doGameOver(false)
    end

end

function GameMatch:onPlayerTick()
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:onTick()
    end
end

function GameMatch:upgradeGameLevel()
    self.isAddLevel = false
    self.RunningTick = self.curTick
    if self.level ~= 0 then
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            if player.isInGame then
                player:onGameLevel(self.curGameLevel.gold, self.curGameLevel.exp)
            end
        end
    end
    self:getCurGameScene():clearMonster()
    self.level = self.level + 1
    if self.level > GameLevelConfig:getMaxLevel() then
        self:doGameOver(true)
        return false
    end
    self.GamePrepareTick = 0
    self.curGameLevel = GameLevelConfig:getLevel(self.level)
    self.levelMonsters, self.levelMonsterNum = GameLevelConfig:getMonstersByLevel(self.level)
    self:getCurGameScene():updateBasement()
    self:teleNoInGamePlayer()
    HostApi.sendGameStatus(0, 2)
    HostApi.sendGameStatus(0, 1)
    HostApi.syncGameTimeShowUi(0, true, GameConfig.gameLevelTime)
    HostApi.updateMonsterInfo(GameMatch.level, GameMatch.levelMonsterNum, 10 - GameMatch.level % 10)
    return true
end

function GameMatch:teleNoInGamePlayer()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if player.isInGame == false then
            player:teleScenePos()
        end
    end
end

function GameMatch:ifGameOverEnd()
    local seconds = GameConfig.gameOverTime - (self.curTick - self.GameOverTick);

    MsgSender.sendBottomTips(3, IMessages:msgGameOverHint(seconds))

    if seconds <= 0 then
        self:resetGame()
    end
end

function GameMatch:ifResetEnd()
    self.curStatus = self.Status.Waiting
    self.isFirstReady = true
end

function GameMatch:prepareGame()
    self.curStatus = self.Status.Prepare
    self.PrepareTick = self.curTick
    MsgSender.sendMsg(IMessages:msgCanStartGame())
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:startSelectMap()
    self.curStatus = self.Status.SelectMap
    self.SelectMapTick = self.curTick
end

function GameMatch:selectMap()
    self.curStatus = self.Status.Running
    self.isAddLevel = true
    self.curGameScene = SceneConfig:randomScene()
    RewardManager:startGame()
    MsgSender.sendMsg(Messages:selectMapHint(self.curGameScene.name))
    self:teleNoInGamePlayer()
end

function GameMatch:getCurGameScene()
    if self.curGameScene == nil then
        self.curGameScene = SceneConfig:randomScene()
    end
    return self.curGameScene
end

function GameMatch:doGameOver(isWin)
    self.curStatus = self.Status.GameOver
    self.GameOverTick = self.curTick
    self:getCurGameScene():reset()
    if isWin then
        MsgSender.sendMsg(Messages:gameWin())
    else
        MsgSender.sendMsg(Messages:gameLose())
    end
    HostApi.sendGameStatus(0, 2)
    HostApi.sendGameStatus(0, 1)
    HostApi.syncGameTimeShowUi(0, false, 0)
    local players = self:sortPlayerRank()
    self:doReward(isWin, players)
    self:doReport(isWin, players)
end

function GameMatch:doReward(isWin, players)
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
    end
    RewardManager:getQueueReward(function(data, isWin)
        self:sendGameSettlement(isWin)
    end, isWin)
end

function GameMatch:doReport(isWin, players)
    for rank, player in pairs(players) do
        if isWin then
            ReportManager:reportUserWin(player.userId)
        end
        ReportManager:reportUserData(player.userId, NumberUtil.getIntPart(player.kills / 20), rank, 1)
    end
end

function GameMatch:sendGameSettlement(isWin)
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, isWin)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        result.own = self:newSettlementItem(player, rank, isWin)
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), false)
    end

    for rank, player in pairs(Players) do
        UserExpManager:addUserExp(player.userId, isWin, 1)
    end
end

function GameMatch:newSettlementItem(player, rank, isWin)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.points = player.scorePoints
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.vip = player.vip
    item.kills = player.kills
    item.adSwitch = player.adSwitch or 0
    if item.gold <= 0 then
        item.adSwitch = 0
    end
    if isWin then
        item.isWin = 1
    else
        item.isWin = 0
    end
    return item
end

function GameMatch:resetGame()
    self.curStatus = self.Status.Reset
    self.level = 0
    self:resetPlayer()
    MsgSender.sendMsg(IMessages:msgGameResetHint())
end

function GameMatch:resetPlayer()
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        player:reset()
    end
    for i, player in pairs(players) do
        self:sendRealRankData(player)
    end
end

function GameMatch:ifCanUpgradeGameLevel()
    local kills = self:getCurGameScene():getKills()
    local needKills = GameLevelConfig:getKillsByLevel(self.level)
    if kills >= needKills then
        self.isAddLevel = true
    end
end

function GameMatch:onPlayerQuit(player)
    if self.curStatus == self.Status.Running then
        GameMatch:getCurGameScene():onPlayerQuit(player)
    end
    RankNpcConfig:savePlayerRankScore(player)
    player:onQuit()
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
end

function GameMatch:getDBDataRequire(player)
    DBManager:getPlayerData(player.userId, 1)
end


function GameMatch:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    local data = {}
    data.money = player.money
    data.level = player.level
    data.equipment = player.equipment
    data.arms = player.arms
    data.exp = player.exp
    data.isGoldAddition = player.isGoldAddition
    data.isExpAddition = player.isExpAddition
    data.isSpeedAddition = player.isSpeedAddition
    DBManager:savePlayerData(player.userId, 1, data, immediate)
end

function GameMatch:sortPlayerByLevel()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.level ~= b.level then
            return a.level > b.level
        end
        if a.kills ~= b.kills then
            return a.kills > b.kills
        end
        if a.score ~= b.score then
            return a.score > b.score
        end
        return a.userId > b.userId
    end)

    return players
end

function GameMatch:sendRealRankData(player)
    local players = self:sortPlayerByLevel()
    local index = 0
    local data = {}
    data.ranks = {}
    for i, player in pairs(players) do
        index = index + 1
        local item = {}
        item.column_1 = tostring(index)
        item.column_2 = player.name
        item.column_3 = tostring(player.level)
        item.column_4 = tostring(player.kills)
        item.column_5 = tostring(player.score)
        data.ranks[index] = item
        if index == 3 then
            break
        end
    end
    local rank = RankManager:getPlayerRank(players, player)
    local own = {}
    own.column_1 = tostring(rank)
    own.column_2 = player.name
    own.column_3 = tostring(player.level)
    own.column_4 = tostring(player.kills)
    own.column_5 = tostring(player.score)
    data.own = own
    HostApi.updateRealTimeRankInfo(player.rakssid, json.encode(data))
end

function GameMatch:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        if a.level ~= b.level then
            return a.level > b.level
        end
        if a.kills ~= b.kills then
            return a.kills > b.kills
        end
        return a.userId > b.userId
    end)

    return players
end

function GameMatch:getPlayerRank(player)
    local players = self:sortPlayerRank();
    return RankManager:getPlayerRank(players, player)
end

return GameMatch