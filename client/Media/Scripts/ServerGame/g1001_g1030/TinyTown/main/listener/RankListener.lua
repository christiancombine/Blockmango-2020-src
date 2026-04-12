
RankListener = {}
RankListener.TYPE_WEEK = 1
RankListener.TYPE_DAY = 2

function RankListener:init()
    LoadManorCharmRankEvent.registerCallBack(self.onLoadManorCharmRank)
    LoadManorPotentialRankEvent.registerCallBack(self.onLoadManorPotentialRank)
    LoadMyManorRankEvent.registerCallBack(self.onLoadMyManorRank)
end

function RankListener.onLoadManorCharmRank(isSuccess, response)
    if isSuccess then
        GameMatch.charmRankCount = 0
        ManorNpcConfig:rankNpcData("charm.rank", response, function(data)
            GameMatch.charmRank = data
            HostApi.log("==== LuaLog: LogicListener.onLoadManorCharmRank successful !")
        end)
    else
        if GameMatch.charmRankCount ~= 0 then
            GameMatch.charmRankCount = GameMatch.charmRankCount - 1
            HostApi.loadManorCharmRank(30)
        end
        HostApi.log("==== LuaLog: LogicListener.onLoadManorCharmRank failure !")
    end
end

function RankListener.onLoadManorPotentialRank(isSuccess, response)
    if isSuccess then
        GameMatch.potentialRankCount = 0
        ManorNpcConfig:rankNpcData("potential.rank", response, function(data)
            GameMatch.potentialRank = data
            HostApi.log("==== LuaLog: LogicListener.onLoadManorPotentialRank successful !")
        end)
    else
        if GameMatch.potentialRankCount ~= 0 then
            GameMatch.potentialRankCount = GameMatch.potentialRankCount - 1
            HostApi.loadManorPotentialRank(30)
        end
        HostApi.log("==== LuaLog: LogicListener.onLoadManorPotentialRank failure !")
    end
end

function RankListener.onLoadMyManorRank(isSuccess, platformId, response)
    local player = PlayerManager:getPlayerByUserId(platformId)
    if player ~= nil then
        if isSuccess then
            local rank = {}
            rank.ranks = {}
            rank.ranks.day = GameMatch.charmRank
            rank.ranks.week = GameMatch.potentialRank

            if response ~= nil and string.len(response) ~= 0 and response ~= "{}" then
                local data = json.decode(response)
                rank.own = {}
                rank.own.day = {}
                rank.own.day.rank = data.charmSort
                rank.own.day.userId = tonumber(tostring(platformId))
                rank.own.day.score = data.charmTotal
                rank.own.day.name = player.name
                rank.own.day.vip = player.vip

                rank.own.week = {}
                rank.own.week.rank = data.potentialSort
                rank.own.week.userId = tonumber(tostring(platformId))
                rank.own.week.score = data.potentialTotal
                rank.own.week.name = player.name
                rank.own.week.vip = player.vip

                ManorNpcConfig:sendRankData(player.rakssid, json.encode(rank))
            end
        else
            HostApi.log("==== LuaLog: LogicListener.onLoadMyManorRank failure !")
        end
    else
        HostApi.log("==== LuaLog: LogicListener.onLoadMyManorRank failure, player is nil !")
    end
end

return RankListener