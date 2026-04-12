---
--- Created by Jimmy.
--- DateTime: 2018/7/20 0020 15:59
---

RealRankUtil = {}
RealRankUtil.users = {}

function RealRankUtil:init()
    HostApi.ZExpireat(self:getRankKey(), tostring(DateUtil.getWeekLastTime() + DateUtil.getWeekSeconds()))
end

function RealRankUtil:getPlayerRankData(player)
    local key = self:getRankKey()
    local uid = tostring(player.userId)
    HostApi.ZScore(key, uid)
end

function RealRankUtil:savePlayerRankScore(player)
    if player.score > 0 then
        local key = self:getRankKey()
        local uid = tostring(player.userId)
        HostApi.ZIncrBy(key, uid, player.score)
    end
end

function RealRankUtil:getRankKey()
    return GameMatch.gameType .. ".score.record." .. DateUtil.getYearWeek()
end

function RealRankUtil:getRangeRank(userId, rank)
    self.users[tostring(userId)] = { 0, rank }
    local key = self:getRankKey()
    if rank > 1 then
        HostApi.ZRange(key, rank - 2, rank)
    else
        HostApi.ZRange(key, 0, 2)
    end
end

function RealRankUtil:rebuildRank(ranks)
    local items = {}
    for i, v in pairs(ranks) do
        local rank = {}
        local item = StringUtil.split(v, ":")
        rank.rank = i
        rank.userId = tonumber(item[1])
        rank.score = tonumber(item[2])
        rank.name = "Steve" .. rank.rank
        rank.vip = 0
        items[#items + 1] = rank
    end
    local rank = { 0, -1 }
    local userId = 0
    if #items == 0 then
        return
    end
    if #items == 1 then
        rank = { 0, 0 }
        userId = tostring(items[1].userId)
    end
    if #items == 2 or #items == 3 then
        userId = tostring(items[2].userId)
        if self.users[userId] ~= nil then
            rank = { 0, self.users[userId][2] - 2 }
        end
        if rank[2] == -1 and #items == 3 then
            userId = tostring(items[3].userId)
            if self.users[userId] ~= nil then
                rank = { 0, self.users[userId][2] - 3 }
            end
        end
        if rank[2] == -1 then
            rank = { 0, 0 }
        end
    end
    if userId == 0 then
        return
    end
    for i, v in pairs(items) do
        v.rank = v.rank + rank[2]
    end
    local userIds = {}
    for i, v in pairs(items) do
        userIds[i] = v.userId
    end
    UserInfoCache:GetCacheByUserIds(userIds, function(items, userId)
        for i, v in pairs(items) do
            local info = UserInfoCache:GetCache(v.userId)
            if info ~= nil then
                v.vip = info.vip
                v.name = info.name
            end
        end
        local data = {}
        data.ranks = {}
        local owns = {}
        for i, rank in pairs(items) do
            local item = {}
            item.column_1 = tostring(rank.rank)
            item.column_2 = rank.name
            item.column_3 = tostring(rank.score)
            data.ranks[#data.ranks + 1] = item
            if tostring(rank.userId) == tostring(userId)
            and self.users[tostring(rank.userId)] ~= nil
            and self.users[tostring(userId)][1] == 0 then
                owns[#owns + 1] = rank
            end
            if self.users[tostring(rank.userId)] ~= nil
            and self.users[tostring(rank.userId)][1] == 0
            and self.users[tostring(rank.userId)][2] == 1 then
                owns[#owns + 1] = rank
            end
        end
        for _, ownRank in pairs(owns) do
            local own = {}
            own.column_1 = tostring(ownRank.rank)
            own.column_2 = ownRank.name
            own.column_3 = tostring(ownRank.score)
            data.own = own
            local player = PlayerManager:getPlayerByUserId(ownRank.userId)
            if player ~= nil then
                HostApi.updateRealTimeRankInfo(player.rakssid, json.encode(data))
                self.users[tostring(ownRank.userId)][1] = os.time()
            end
        end
    end, self:getRankKey(), items, userId)
end

return RealRankUtil