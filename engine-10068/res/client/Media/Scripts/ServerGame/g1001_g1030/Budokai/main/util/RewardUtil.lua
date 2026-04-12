---
--- Created by Jimmy.
--- DateTime: 2018/6/6 0006 14:58
---
RewardUtil = {}

function RewardUtil:sortPlayerRank()
    local players = PlayerManager:copyPlayers()

    table.sort(players, function(a, b)
        if a.rank ~= b.rank then
            return a.rank < b.rank
        end
        return a.userId > b.userId
    end)

    return players
end

function RewardUtil:getWinnerPlayer()
    local players = self:sortPlayerRank()
    if #players == 0 then
        return 0
    end
    local winner = players[1].rakssid
    return winner
end

function RewardUtil:doReward(players)
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
    end
    RewardManager:getQueueReward(function(data)
        self:sendGameSettlement(self:getWinnerPlayer())
    end)
end

function RewardUtil:doReport(players)
    for rank, player in pairs(players) do
        if player.isReport == false then
            ReportManager:reportUserData(player.userId, player.kills, rank, 1)
            player.kills = 0
            player.isReport = true
        end
    end
end

function RewardUtil:sendGameSettlement(winner)
    local Players = self:sortPlayerRank()

    local players = {}

    for rank, player in pairs(Players) do
        players[rank] = self:newSettlementItem(player, rank, winner)
    end

    for rank, player in pairs(Players) do
        local result = {}
        result.players = players
        result.own = self:newSettlementItem(player, rank, winner)
        HostApi.sendGameSettlement(player.rakssid, json.encode(result), false)
    end

    for rank, player in pairs(Players) do
        UserExpManager:addUserExp(player.userId, winner == player.rakssid)
    end
end

function RewardUtil:newSettlementItem(player, rank, winner)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.points = player.scorePoints
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.vip = player.vip
    item.kills = player.scorePoints[ScoreID.WIN]
    item.adSwitch = player.adSwitch or 0
    if item.gold <= 0 then --or player.isSingleReward then
        item.adSwitch = 0
    end
    if winner == 0 then
        item.isWin = 2
    else
        if winner == player.rakssid then
            item.isWin = 1
        else
            item.isWin = 0
        end
    end
    return item
end

return RewardUtil