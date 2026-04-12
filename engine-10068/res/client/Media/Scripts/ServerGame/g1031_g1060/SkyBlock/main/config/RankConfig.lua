require "data.GameRankData"
require "data.GamePlayer"

RankConfig = {}
RankConfig.rankData = {}
function RankConfig:init(config)
    for i, v in pairs(config) do
        self.rankData[i] = GameRankData.new(v)
    end
end

function RankConfig:getPlayerRankData(player)
    for i, npc in pairs(self.rankData) do
        npc:getPlayerRankData(player)
    end
end

function RankConfig:savePlayerRankScore(player)
    if not DbUtil.CanSavePlayerData(player, DbUtil.TAG_PLAYER) or not DbUtil.CanSavePlayerData(player, DbUtil.TAG_SHAREDATA)  then
        return
    end

    for i, npc in pairs(self.rankData) do
        npc:savePlayerRankScore(player)
    end
end

function RankConfig:setZExpireat()
    for i, npc in pairs(self.rankData) do
        npc:setZExpireat()
    end
end

function RankConfig:updateRank()
    for i, npc in pairs(self.rankData) do
        npc:updateRank()
    end
end

function RankConfig:getRankNpc(id)
    for i, npc in pairs(self.rankData) do
        if npc.id == id then
            return npc
        end
    end
    return nil
end

function RankConfig:getRankNpcByKey(key)
    for i, npc in pairs(self.rankData) do
        if npc:getKey() == key then
            return npc
        end
    end
    return nil
end

function RankConfig:getAllKey()
    return GameMatch.gameType .. ".forever.rank"
end

function RankConfig:getWeekKey()
    return GameMatch.gameType .. ".week.rank"
end

