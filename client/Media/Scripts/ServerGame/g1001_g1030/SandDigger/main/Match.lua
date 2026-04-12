---
--- Created by Jimmy.
--- DateTime: 2018/1/25 0025 10:10
---
require "messages.Messages"
require "listener.RankListener"
require "config.ChestConfig"
require "config.BlockConfig"
require "config.NpcConfig"

GameMatch = {}

GameMatch.gameType = "g1015"

GameMatch.RankData = {}
GameMatch.RankData.week = {}
GameMatch.RankData.day = {}
GameMatch.sendTick = 0
GameMatch.hasWeek = false
GameMatch.hasDay = false

GameMatch.curTick = 0

GameMatch.crashToStart = os.time()
GameMatch.isStartCrash = false
GameMatch.isReset = false

function GameMatch:initMatch()
    GameTimeTask:start()
end

function GameMatch:onTick(ticks)
    self.curTick = ticks
    self:showGameData()
    self:ifGameReset()
    self:saveAllPlayerRankData()
    self:ifUpdateRank()
end

function GameMatch:showGameData(hp, player)
    if player ~= nil then
        local location = math.ceil(GameConfig.initPos.y - player:getPosition().y)
        if location <= 0 then
            location = 0
        end

        if player.digStatus == true then
            if hp ~= nil then
                MsgSender.sendTopTipsToTarget(player.rakssid, 8, Messages:showDeepAndHp(location, hp))
            else
                sgSender.sendTopTipsToTarget(player.rakssid, 8, Messages:showDeep(location))
            end
        end
    else
        local players = PlayerManager:getPlayers()
        for i, v in pairs(players) do
            local location = math.ceil(GameConfig.initPos.y - v:getPosition().y)
            if location <= 0 then
                location = 0
            end
            if v.digStatus == false then
                MsgSender.sendTopTipsToTarget(v.rakssid, 8, Messages:showDeep(location))
            end
        end
    end
end

function GameMatch:ifGameReset()
    if self.isStartCrash == true and self.isReset == false then
        self.crashToStart = os.time()
        MsgSender.sendCenterTips(3, Messages:breakDown(GameConfig.crashTime))
        self.isReset = true
    end
    if os.time() - self.crashToStart >= GameConfig.crashTime and self.isReset == true then
        GameMatch:reset()
    end
end

function GameMatch:reset()
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        v:teleInitPos()
    end
    self.isStartCrash = false
    self.isReset = false
    self:resetMap()
    ChestConfig:prepareChest()
end

function GameMatch:resetMap()
    BlockConfig.blockHp = {}
    HostApi.resetMap()
end

function GameMatch:onPlayerQuit(player)
    self:savePlayerRankScore(player)
    player:reward()
    HostApi.sendResetGame(PlayerManager:getPlayerCount())
end

function GameMatch:ifUpdateRank()
    if self.curTick % 300 == 0 and PlayerManager:isPlayerExists() then
        GameMatch:getRankTop10Players()
    end
end

function GameMatch:saveAllPlayerRankData()
    if self.curTick == 0 or self.curTick % 300 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, player in pairs(players) do
        self:savePlayerRankScore(player)
    end
end

function GameMatch:savePlayerRankScore(player)
    local weekKey = self:getWeekKey()
    local dayKey = self:getDayKey()
    local uid = tostring(player.userId)
    HostApi.ZIncrBy(weekKey, uid, player.gameScore)
    HostApi.ZIncrBy(dayKey, uid, player.gameScore)
    player.gameScore = 0
end

function GameMatch:getPlayerRankData(player)
    local weekKey = self:getWeekKey()
    local dayKey = self:getDayKey()
    local uid = tostring(player.userId)
    HostApi.ZScore(weekKey, uid)
    HostApi.ZScore(dayKey, uid)
end

function GameMatch:sendRankToPlayers()
    self.sendTick = self.sendTick + 1
    if self.sendTick % 2 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for i, v in pairs(players) do
        GameMatch:getPlayerRankData(v)
    end
end

function GameMatch:getRankTop10Players()
    local weekKey = self:getWeekKey()
    local dayKey = self:getDayKey()
    HostApi.ZRange(weekKey, 0, 9)
    HostApi.ZRange(dayKey, 0, 9)
end

function GameMatch:rebuildRankTop10(type)
    local items = {}
    local key = ""
    if type == RankListener.TYPE_WEEK then
        items = self.RankData.week
        key = self:getWeekKey()
    end
    if type == RankListener.TYPE_DAY then
        items = self.RankData.day
        key = self:getDayKey()
    end
    local userIds = {}
    for i, v in pairs(items) do
        userIds[i] = v.userId
    end
    UserInfoCache:GetCacheByUserIds(userIds, function(type, items)
        for i, v in pairs(items) do
            local info = UserInfoCache:GetCache(v.userId)
            if info ~= nil then
                v.vip = info.vip
                v.name = info.name
            end
        end
        if type == RankListener.TYPE_WEEK then
            self.RankData.week = items
            self.hasWeek = true
        end
        if type == RankListener.TYPE_DAY then
            self.RankData.day = items
            self.hasDay = true
        end
        self:sendRankToPlayers()
    end, key, type, items)
end

function GameMatch:getWeekKey()
    return self.gameType .. ".week." .. DateUtil.getYearWeek()
end

function GameMatch:getDayKey()
    return self.gameType .. ".day." .. DateUtil.getYearDay()
end

function GameMatch:addWeekRank(rank)
    self.RankData.week[#self.RankData.week + 1] = rank
end

function GameMatch:addDayRank(rank)
    self.RankData.day[#self.RankData.day + 1] = rank
end

function GameMatch:sendRankDataToPlayer(player)
    local data = self:buildBaseRankData()
    NpcConfig:sendRankData(player.rakssid, data)
end

function GameMatch:buildBaseRankData()
    local data = {}
    data.ranks = self.RankData
    data.title = "gui_rank_sand_digger_title_name"
    return data
end

function GameMatch:savePlayerData(player, immediate)
    if player == nil then
        return
    end
    local data = {}
    local inventory = InventoryUtil:getPlayerInventoryInfo(player)
    data.inventory = inventory
    data.money = player.money
    data.score = player.score
    data.backpackLevel = player.backpackLevel
    DBManager:savePlayerData(player.userId, 1, data, immediate)
end

function GameMatch:getDBDataRequire(player)
    DBManager:getPlayerData(player.userId, 1)
end

function GameMatch:setZExpireat()
    HostApi.ZExpireat(self:getWeekKey(), tostring(DateUtil.getWeekLastTime()))
    HostApi.ZExpireat(self:getDayKey(), tostring(DateUtil.getDayLastTime()))
end

return GameMatch