require "data.GameRankNpc"

RankNpcConfig = {}
RankNpcConfig.rankNpc = {}

RankNpcConfig.RANK_VICTORY = 1
RankNpcConfig.RANK_KNIGHT = 2

function RankNpcConfig:init(config)
    for i, npc in pairs(config) do
        self.rankNpc[i] = GameRankNpc.new(npc)
    end
end

function RankNpcConfig:setZExpireat()
    for _, npc in pairs(self.rankNpc) do
        npc:setZExpireat()
    end
end

function RankNpcConfig:getPlayerRankData(player)
    for _, npc in pairs(self.rankNpc) do
        npc:getPlayerRankData(player)
    end
end

function RankNpcConfig:savePlayerRank(player)
    for _, npc in pairs(self.rankNpc) do
        npc:savePlayerRank(player)
    end
end

function RankNpcConfig:updateRank()
    for _, npc in pairs(self.rankNpc) do
        npc:updateRank()
    end
end

function RankNpcConfig:getRankNpc(id)
    for _, npc in pairs(self.rankNpc) do
        if npc.id == id then
            return npc
        end
    end
    return nil
end

function RankNpcConfig:getRankNpcByDayKey(key)
    for _, npc in pairs(self.rankNpc) do
        if npc:getDayKey() == key then
            return npc
        end
    end
    return nil
end

function RankNpcConfig:getRankNpcByWeekKey(key)
    for _, npc in pairs(self.rankNpc) do
        if npc:getWeekKey() == key then
            return npc
        end
    end
    return nil
end

return RankNpcConfig