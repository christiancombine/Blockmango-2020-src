require "Match"

GameRankData = class()
function GameRankData:ctor(config)
    self:init(config)
end

function GameRankData:init(config)
    self.id = tonumber(config.id)
    self.key = config.key
    self.data = {}
    self.getTick = 0
    self.hasGet = false
end


function GameRankData:getPlayerRankData(player)
    local key = self:getKey()
    local uid = tostring(player.userId)
    HostApi.ZScore(key, uid)
end

function GameRankData:getPlayerSelfRankData()
    self.getTick = self.getTick + 1
    if self.getTick % 2 ~= 0 then
        return
    end
    local players = PlayerManager:getPlayers()
    for _, v in pairs(players) do
        self:getPlayerRankData(v)
    end
end

function GameRankData:savePlayerRankScore(player)
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_PLAYER) or not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA)  then
        return
    end

    HostApi.log("savePlayerRankScore " .. tostring(player.userId))

    if self:getKey() == RankConfig:getWeekKey() then
        local addExp = player.exp - player.oldExp
        if addExp ~= 0 then
            local key = self:getKey()
            local uid = tostring(player.userId)
            HostApi.log("savePlayerRankScore week userId " .. tostring(player.userId) .. " "
                    .. tostring(self:getKey()) .. " " .. player.oldExp .. " " .. player.exp .. " " .. addExp)
            HostApi.ZIncrBy(key, uid, addExp)
            player.oldExp = player.exp
        end
        return
    end

    if self:getKey() == RankConfig:getAllKey() then
        if player.getAllKeyScore then
            local addExp = player.exp - player.allKeyScore
            if addExp ~= 0 then
                HostApi.log("savePlayerRankScore all userId " .. tostring(player.userId) .. " "
                        .. tostring(self:getKey()) .. " " .. player.allKeyScore .. " " .. player.exp .. " " .. addExp)
                local key = self:getKey()
                local uid = tostring(player.userId)
                HostApi.ZIncrBy(key, uid, addExp)
            end
            player.getAllKeyScore = false
            player.allKeyScore = player.exp
        end
        return
    end
end

function GameRankData:setZExpireat()
    if self:getKey() == RankConfig:getWeekKey() then
        HostApi.ZExpireat(self:getKey(), tostring(DateUtil.getWeekLastTime()))
    end
end

function GameRankData:updateRank()
    local key = self:getKey()
    HostApi.ZRange(key, 0, 30)
end

function GameRankData:rebuildRank()
    local items = self.data or {}
    local key = self:getKey() or ""
    local userIds = {}
    for i, v in pairs(items) do
        userIds[i] = v.userId
    end

    UserInfoCache:GetCacheByUserIds(userIds, function(items)

        for i, v in pairs(items) do
            local info = UserInfoCache:GetCache(v.userId)
            if info ~= nil then
                v.vip = info.vip
                v.name = info.name
                v.picUrl = info.picUrl or ""
            end
        end

        self.data = items
        self.hasGet = true

        self:getPlayerSelfRankData()
    end, key, items)
end

function GameRankData:getKey()
    return GameMatch.gameType .. "." .. self.key .. ".rank"
end

function GameRankData:addRank(rank)
    self.data[#self.data + 1] = rank
end

function GameRankData:sendRankData(player, selfdata)
    if self.hasGet then
        local data = {}
        data.ranks = self.data
        data.own = selfdata
        data.type = self.id
        GamePacketSender:sendSkyBlockRankData(player.rakssid, json.encode(data))
    end
end