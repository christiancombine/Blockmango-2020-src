---
--- Created by Jimmy.
--- DateTime: 2018/4/10 0010 11:27
---
require "data.GameRankNpc"

RankNpcConfig = {}
RankNpcConfig.rankNpc = {}

function RankNpcConfig:init(config)
    for i, npc in pairs(config) do
        self.rankNpc[i] = GameRankNpc.new(npc)
    end
end

function RankNpcConfig:setZExpireat()
    for i, npc in pairs(self.rankNpc) do
        npc:setZExpireat()
    end
end

function RankNpcConfig:getPlayerRankData(player)
    for i, npc in pairs(self.rankNpc) do
        npc:getPlayerRankData(player)
    end
end

function RankNpcConfig:savePlayerRankScore(player)
    for i, npc in pairs(self.rankNpc) do
        npc:savePlayerRankScore(player)
    end
end

function RankNpcConfig:updateRank()
    RankManager:clearRanks()
    for i, npc in pairs(self.rankNpc) do
        npc:updateRank()
    end
end

function RankNpcConfig:getRankNpc(id)
    for i, npc in pairs(self.rankNpc) do
        if npc.id == id then
            return npc
        end
    end
    return nil
end

function RankNpcConfig:getRankNpcByDayKey(key)
    local npcs = {}
    for i, npc in pairs(self.rankNpc) do
        if npc:getDayKey() == key then
            npcs[#npcs + 1] = npc
        end
    end
    return npcs
end

function RankNpcConfig:getRankNpcByWeekKey(key)
    local npcs = {}
    for i, npc in pairs(self.rankNpc) do
        if npc:getWeekKey() == key then
            npcs[#npcs + 1] = npc
        end
    end
    return npcs
end

