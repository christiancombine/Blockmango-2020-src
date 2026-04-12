XGameRank = {}

function XGameRank:sendRealRankData(player)
    local players = self:sortPlayerByKillNum()
    local index = 0
    local data = {}
    data.ranks = {}
    for i, v in pairs(players) do
        index = index + 1
        local item = {}
        item.column_1 = tostring(index)
        item.column_2 = v.name
        item.column_3 = tostring(v.currentMoney)
        item.column_4 = tostring(v:getKills())
        item.column_5 = v.bindTeam ~= nil and v.bindTeam.teamName or ''
        data.ranks[index] = item
    end
    local rank = RankManager:getPlayerRank(players, player)
    local own = {}
    own.column_1 = tostring(rank)
    own.column_2 = player.name
    own.column_3 = tostring(player.currentMoney)
    own.column_4 = tostring(player:getKills())
    if player.bindTeam == nil then
        return
    end
    own.column_5 = tostring(player.bindTeam.teamName) or ''
    data.own = own
    local jsonData = json.encode(data)
    HostApi.updateRealTimeRankInfo(player.rakssid, jsonData)
end

function XGameRank:sortPlayerByKillNum()
    local players = PlayerManager:copyPlayers()

    table.sort(players,     
        function(a, b)         
            if a.currentMoney ~= b.currentMoney then
                return a.currentMoney > b.currentMoney
            end
            if a.killPlayerCount ~= b.killPlayerCount then
                return a.killPlayerCount > b.killPlayerCount
            end
            return a.userId > b.userId
        end
    )

    return players
end
