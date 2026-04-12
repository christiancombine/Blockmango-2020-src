
RankListener = {}
RankListener.type_clan = 1
RankListener.type_prosperity = 2
RankListener.type_achievement = 3

function RankListener:init()
    ZScoreFromRedisDBEvent.registerCallBack(self.onZScoreFromRedisDB)
    ZRangeFromRedisDBEvent.registerCallBack(self.onZRangeFromRedisDB)
end

function RankListener.onZScoreFromRedisDB(key, userId, score, rank)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        HostApi.log("=== LuaLog: [error] RankListener.onZScoreFromRedisDB : player userId[".. tostring(userId) .. "] is nil")
        return
    end

    if key == RanchersRankManager:getAchievementKey() then
        if score == 0 then
            rank = 0
        end
        player:setAchievementRankData(rank, score)
    end

    if key == RanchersRankManager:getProsperityKey() then
        if score == 0 then
            rank = 0
        end
        player:setProsperityRankData(rank, score)
    end
end

function RankListener.onZRangeFromRedisDB(key, data)
    local ranks = StringUtil.split(data, "#")
    if key == RanchersRankManager:getProsperityKey() then
        RanchersRankManager.prosperity = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.level = 0
            rank.name = "Steve" .. rank.rank
            if rank.score ~= 0 then
                RanchersRankManager:addProsperityData(rank)
            end
        end
        RanchersRankManager:rebuildRank(RankListener.type_prosperity)
    end

    if key == RanchersRankManager:getAchievementKey() then
        RanchersRankManager.achievement = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.level = 0
            rank.name = "Steve" .. rank.rank
            if rank.score ~= 0 then
                RanchersRankManager:addAchievementData(rank)
            end
        end
        RanchersRankManager:rebuildRank(RankListener.type_achievement)
    end
end

return RankListener