---
--- Created by Jimmy.
--- DateTime: 2018/3/8 0008 11:43
---
require "config.RankConfig"

RankListener = {}

function RankListener:init()
    ZScoreFromRedisDBEvent.registerCallBack(self.onZScoreFromRedisDB)
    ZRangeFromRedisDBEvent.registerCallBack(self.onZRangeFromRedisDB)
end

function RankListener.onZScoreFromRedisDB(key, userId, score, rank)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    local npc = RankConfig:getRankNpcByKey(key)
    if npc then
        HostApi.log(" npc.id  ="..npc.id.." rank =  "..rank.." score =  "..score)
        player:setRank(npc.id, rank, score)

        if npc:getKey() == RankConfig:getAllKey() then
            player.allKeyScore = score
            player.getAllKeyScore = true
        end

        return
    end
end

function RankListener.onZRangeFromRedisDB(key, data)
    local ranks = StringUtil.split(data, "#")
    for i, npc in pairs(RankConfig.rankData) do
        if tostring(key) == tostring(npc:getKey()) then
            npc.data = {}
            for i, v in ipairs(ranks) do
                local rank = {}
                local item = StringUtil.split(v, ":")
                rank.rank = i
                rank.userId = tonumber(item[1])
                rank.score = tonumber(item[2])
                rank.name = "Steve" .. rank.rank
                npc:addRank(rank)
            end
            npc:rebuildRank()
        end
    end
end

return RankListener