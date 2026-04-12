---
--- Created by Jimmy.
--- DateTime: 2018/3/8 0008 11:43
---

RankListener = {}
RankListener.TYPE_WEEK = 1
RankListener.TYPE_DAY = 2

function RankListener:init()
    ZScoreFromRedisDBEvent.registerCallBack(self.onZScoreFromRedisDB)
    ZRangeFromRedisDBEvent.registerCallBack(self.onZRangeFromRedisDB)
end

function RankListener.onZScoreFromRedisDB(key, userId, score, rank)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if key == GameMatch:getWeekKey() then
        player:setWeekRank(rank, score)
    end
    if key == GameMatch:getDayKey() then
        player:setDayRank(rank, score)
    end
end

function RankListener.onZRangeFromRedisDB(key, data)
    local ranks = StringUtil.split(data, "#")
    if key == GameMatch:getWeekKey() then
        GameMatch.RankData.week = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.name = "Steve" .. rank.rank
            rank.vip = 0
            GameMatch:addWeekRank(rank)
        end
        GameMatch:rebuildRankTop10(RankListener.TYPE_WEEK)
    end
    if key == GameMatch:getDayKey() then
        GameMatch.RankData.day = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.name = "Steve" .. rank.rank
            rank.vip = 0
            GameMatch:addDayRank(rank)
        end
        GameMatch:rebuildRankTop10(RankListener.TYPE_DAY)
    end
end

return RankListener