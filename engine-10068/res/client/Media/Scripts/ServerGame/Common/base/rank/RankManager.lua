---
--- Created by Jimmy.
--- DateTime: 2018/7/2 0002 16:39
---
RankManager = {}
RankManager.PlayerRanks = {}
RankManager.HistoryRecords = {}

function RankManager:sortPlayer(params, reverse, players)
    reverse = reverse or {}
    players = players or PlayerManager:copyPlayers()
    table.sort(players, function(p1, p2)
        for _, param in pairs(params) do
            local score1, score2 = p1[param], p2[param]
            if type(score1) == "number" and type(score2) == "number" then
                if score1 ~= score2 then
                    if not reverse[param] then
                        return score1 > score2
                    else
                        return score1 < score2
                    end
                end
            end
        end
        return p1.userId < p2.userId
    end)
    return players
end

function RankManager:getPlayerRank(players, player)
    local rank = 0
    local find = false
    for _, c_player in pairs(players) do
        rank = rank + 1
        if c_player.userId == player.userId then
            find = true
            break
        end
    end
    if not find then
        rank = rank + 1
    end
    return rank
end

function RankManager:removePlayerRank(userId)
    userId = tostring(userId)
    self.PlayerRanks[userId] = nil
    for key, record in pairs(self.HistoryRecords) do
        if StringUtil.split(key, "#")[1] == userId then
            self.HistoryRecords[key] = nil
        end
    end
end

function RankManager:buildPlayerRank(userId, key)
    local player_rank = self.PlayerRanks[tostring(userId)]
    if player_rank == nil then
        player_rank = {}
        player_rank[key] = {}
        player_rank[key].userId = userId
        player_rank[key].day = {}
        player_rank[key].week = {}
        player_rank[key].hasDay = false
        player_rank[key].hasWeek = false
        self.PlayerRanks[tostring(userId)] = player_rank
    else
        if player_rank[key] == nil then
            player_rank[key] = {}
            player_rank[key].userId = userId
            player_rank[key].day = {}
            player_rank[key].week = {}
            player_rank[key].hasDay = false
            player_rank[key].hasWeek = false
        end
    end
    return player_rank
end

function RankManager:addPlayerDayRank(player, key, rank, score, func, ...)
    local rank_data = self:buildPlayerRank(player.userId, key)
    local data = {}
    data.rank = rank
    data.userId = tonumber(tostring(player.userId))
    data.score = tonumber(score)
    data.name = player.name
    data.vip = player.vip
    rank_data[key].day = data
    if rank_data[key].hasDay and rank_data[key].hasWeek then
        rank_data[key].hasWeek = false
    end
    rank_data[key].hasDay = true
    if rank_data[key].hasDay and rank_data[key].hasWeek then
        func(...)
    end
end

function RankManager:addPlayerWeekRank(player, key, rank, score, func, ...)
    local rank_data = self:buildPlayerRank(player.userId, key)
    local data = {}
    data.rank = rank
    data.userId = tonumber(tostring(player.userId))
    data.score = tonumber(score)
    data.name = player.name
    data.vip = player.vip
    rank_data[key].week = data
    if rank_data[key].hasDay and rank_data[key].hasWeek then
        rank_data[key].hasDay = false
    end
    rank_data[key].hasWeek = true
    if rank_data[key].hasDay and rank_data[key].hasWeek then
        func(...)
    end
end

function RankManager:sendPlayerRank(rakssid, key, data, entityId)
    local player = PlayerManager:getPlayerByRakssid(rakssid)
    if not player then
        return
    end
    local player_rank = self:buildPlayerRank(player.userId, key)
    local rank_data = player_rank[key]
    if rank_data.hasDay and rank_data.hasWeek then
        local own = {}
        own.day = rank_data.day
        own.week = rank_data.week
        data.own = own
        local result = json.encode(data)
        local hash = tostring(player.userId) .. "#" .. tostring(entityId)
        if self.HistoryRecords[hash] == result then
            return
        end
        self.HistoryRecords[hash] = result
        HostApi.sendRankData(rakssid, entityId, result)
    end
end

function RankManager:clearRanks()
    self.PlayerRanks = {}
    self.HistoryRecords = {}
end

return RankManager