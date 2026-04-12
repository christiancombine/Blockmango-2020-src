require "config.RankConfig"

RankListener = {}
RankListener.TotalCollect = 1
RankListener.DailyCollect = 2
RankListener.TotalFight = 3

function RankListener:init()
    ZScoreFromRedisDBEvent.registerCallBack(self.onZScoreFromRedisDB)
    ZRangeFromRedisDBEvent.registerCallBack(self.onZRangeFromRedisDB)
end

function RankListener.onZScoreFromRedisDB(key, userId, score, rank)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end

    if score == 0 then
        rank = 0
    end

    if key == RankConfig:getTotalCollectKey() then
        player:setTotalCollectRank(rank, score)
    end

    if key == RankConfig:getDailyCollectKey() then
        player:setDailyCollectRank(rank, score)
    end

    if key == RankConfig:getTotalFightKey() then
        player:setTotalFightRank(rank, score)
    end

end

function RankListener.onZRangeFromRedisDB(key, data)
    local ranks = StringUtil.split(data, "#")
    if key == RankConfig:getTotalCollectKey() then
        RankConfig.totalCollectData = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.name = "Steve" .. rank.rank
            RankConfig:addTotalCollectRank(rank)
        end
        RankConfig:rebuildRank(RankListener.TotalCollect)
        return
    end

    if key == RankConfig:getDailyCollectKey() then
        RankConfig.dailyCollectData = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.name = "Steve" .. rank.rank
            RankConfig:addDailyCollectRank(rank)
        end
        RankConfig:rebuildRank(RankListener.DailyCollect)
        return
    end

    if key == RankConfig:getTotalFightKey() then
        RankConfig.totalFightData = {}
        for i, v in pairs(ranks) do
            local rank = {}
            local item = StringUtil.split(v, ":")
            rank.rank = i
            rank.userId = tonumber(item[1])
            rank.score = tonumber(item[2])
            rank.name = "Steve" .. rank.rank
            RankConfig:addTotalFightRank(rank)
        end
        RankConfig:rebuildRank(RankListener.TotalFight)
        return
    end
end

return RankListener