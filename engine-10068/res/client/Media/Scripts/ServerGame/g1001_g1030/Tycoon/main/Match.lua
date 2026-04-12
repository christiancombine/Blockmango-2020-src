--Match.lua
require "util.RewardUtil"
require "util.SkillUtil"
require "util.GameManager"
require "util.SkillManager"
require "data.GamePotionEffect"
require "data.GameExplain"

local GameRank = require "data.GameRank"

GameMatch = {}

GameMatch.Status = {}
GameMatch.Status.Init = 0
GameMatch.Status.WaitingPlayer = 1
GameMatch.Status.Prepare = 2
GameMatch.Status.SelectRole = 3
GameMatch.Status.GameTime = 4
GameMatch.Status.GameOver = 5

GameMatch.curStatus = GameMatch.Status.Init

GameMatch.gameType = "g1025"
GameMatch.RANK_WEEK_FLAG = string.format("%s.week", GameMatch.gameType)
GameMatch.RANK_DAY_FLAG = string.format("%s.day", GameMatch.gameType)

GameMatch.timer = 0

function GameMatch:init()
    WaitingPlayerTask:start()
    self:request_rank_data()
end

function GameMatch:prepareMatch()
    HostApi.sendGameStatus(0, 1)
    GamePrepareTask:start()
    MsgSender.sendMsg(Messages:msgselectRoleStartTimeHint(GameConfig.prepareTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:selectRoleMatch()
    self:teleSelectRolePos()
    GameRoleTask:start()
    GameManager:prepare()
    self:hintPlayersSelectOcc()
    GameExplain:onRemovePerperaNpc()
    MsgSender.sendMsg(IMessages:msgGameStartTimeHint(GameConfig.selectRoleTime, IMessages.UNIT_SECOND, false))
end

function GameMatch:startMatch()
    HostApi.sendGameStatus(0, 1)
    RewardManager:startGame()
    GameManager:start()
    MsgSender.sendMsg(IMessages:msgGameStart())
    self:telePlayersToGame()
    GameTimeTask:start()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:hidePrivilege()
    end
end

function GameMatch:hintPlayersSelectOcc()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        MsgSender.sendCenterTipsToTarget(player.rakssid, 5, Messages:hintPlayerSelectOcc())
    end
end

function GameMatch:teleSelectRolePos()
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:teleSelectRolePos()
    end
end

function GameMatch:telePlayersToGame()
    LuaTimer:schedule(function()
        local players = PlayerManager:getPlayers()
        for _, player in pairs(players) do
            self:telePlayerToGame(player)
        end
    end, 1000)
end

function GameMatch:telePlayerToGame(player)
    GamePotionEffect:onRemovePotionEffect(player)
    player:teleGamePos()
    self:addPublicResources(player.addResource, player.addsaveMax)
    self:sendRealRankData(player)
    player:restoreActor()
end

function GameMatch:onTick(ticks)
    GameManager:onTick(ticks)
    SkillManager:onTick(ticks)
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        player:onTick(ticks)
    end
end

function GameMatch:doGameOver()
    GamePrepareTask:pureStop()
    GameTimeTask:pureStop()

    local players = RewardUtil:sortPlayerRank()
    if #players == 0 then
        GameOverTask:start()
        return
    end
    RewardUtil:doReward(players)
    GameOverTask:start()
end

function GameMatch:cleanPlayer()
    self:doReport()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:overGame(false)
    end
end

function GameMatch:doReport()
    local players = RewardUtil:sortPlayerRank()
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
    end
    GameServer:closeServer()
end

function GameMatch:getGameRunningTime()
    return GameTimeTask.runTick or 0
end

function GameMatch:isGameRunning()
    return self.curStatus >= GameMatch.Status.GameTime
end

function GameMatch:isGameOver()
    return self.curStatus == GameMatch.Status.GameOver
end

function GameMatch:onPlayerQuit(player)
    player:OnLoginOut()
    player:onQuit()
end

function GameMatch:request_rank_data()
    GameRank:request_rank()
end

function GameMatch:sendRealRankData(player)
    local players = self:SortPlayersRank()
    local datas = {}
    datas.ranks = {}
    for index = 1, #players do
        local item = {}
        local _player = players[index]
        item.column_1 = tostring(index)
        item.column_2 = _player:getDisplayName()
        item.column_3 = _player:getPlayerMoney()
        item.column_4 = _player:getResourcesMoney();
        table.insert(datas.ranks, item)
    end
    local rank = RankManager:getPlayerRank(players, player)
    local own = {}
    own.column_1 = tostring(rank)
    own.column_2 = player:getDisplayName()
    own.column_3 = player:getPlayerMoney()
    own.column_4 = player:getResourcesMoney();
    datas.own = own
    HostApi.updateRealTimeRankInfo(player.rakssid, json.encode(datas))
end

function GameMatch:SortPlayersRank()
    local ranks = PlayerManager:copyPlayers()
    table.sort(ranks, function(p1, p2)
        if p1:getBuildProgress() ~= p2:getBuildProgress() then
            return p1:getBuildProgress() > p2:getBuildProgress()
        end
        if p1:getBuildTime() ~= p2:getBuildTime() then
            return p1:getBuildTime() < p2:getBuildTime()
        end
        return p1.userId > p2.userId
    end)
    return ranks
end

function GameMatch:addPublicResources(addOutput, addMaxStore)
    if self:isGameRunning() then
        GameManager:addPublicResources(addOutput, addMaxStore)
    end
end

function GameMatch:subPublicResources(subOutput, subMaxStore)
    if self:isGameRunning() then
        GameManager:subPublicResources(subOutput, subMaxStore)
    end
end

return GameMatch

--endregion
