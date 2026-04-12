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
    local winner = 0
    local players = self:sortPlayerRank()
    if #players >= 2 then
        if players[1].rank < players[2].rank then
            winner = players[1].rakssid
        end
    else
        winner = players[1].rakssid
    end
    return winner
end

function RewardUtil:doReward(players)
    local winner = self:getWinnerPlayer()
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
        UserExpManager:addUserExp(player.userId, winner == player.rakssid)
    end
    RewardManager:getQueueReward(function(data, winner)
        self:sendGameSettlement(winner)
    end, winner)
end

function RewardUtil:doReport(players)
    local winner = self:getWinnerPlayer()
    for rank, player in pairs(players) do
        if player.isReport == false then
            ReportManager:reportUserData(player.userId, 0, rank, 1)
            player.isReport = true
        end
        if winner == player.rakssid then
            ReportManager:reportUserWin(player.userId)
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
    item.kills = 0
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