---
--- Created by Jimmy.
--- DateTime: 2018/3/8 0008 11:43
---
require "config.RankNpcConfig"

RankListener = {}

function RankListener:init()
    ZScoreFromRedisDBEvent.registerCallBack(self.onZScoreFromRedisDB)
end

function RankListener.onZScoreFromRedisDB(key, userId, score, rank)
    local player = PlayerManager:getPlayerByUserId(userId)
    if player == nil then
        return
    end
    if key == GameMatch:getPoliceKey() then
        player:setPoliceRank(rank, score)
    end

    if key ==  GameMatch:getCriminalKey() then
        player:setCriminalRank(rank, score)
    end
end

return RankListener