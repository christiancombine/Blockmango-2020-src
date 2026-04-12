---
--- Created by Jimmy.
--- DateTime: 2018/6/6 0006 14:58
---
RewardUtil = {}

function RewardUtil:sortPlayerRank()
    local players = {}
    local c_players = PlayerManager:getPlayers()
    for _, player in pairs(c_players) do
        if player.isInGame then
            table.insert(players, player)
        end
    end

    table.sort(players, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        return a.userId > b.userId
    end)

    return players
end

function RewardUtil:getPlayerRank(player)
    local players = self:sortPlayerRank();
    return RankManager:getPlayerRank(players, player)
end

function RewardUtil:getWinnerTeam()
    local winner
    local teams = GameMatch.Teams
    local random = math.random(1, 2)
    table.sort(teams, function(a, b)
        if a.score ~= b.score then
            return a.score > b.score
        end
        if a.flags ~= b.flags then
            return a.flags > b.flags
        end
        if a.curPlayers > b.curPlayers then
            return a.curPlayers > b.curPlayers
        end
        if random == 1 then
            return a.id < b.id
        else
            return a.id > b.id
        end
    end)
    local hasWinner = false
    if teams[1] and teams[2] then
        hasWinner = teams[1].score ~= teams[2].score
    end
    if teams[1] and hasWinner then
        winner = teams[1].id
    else
        winner = 0
    end
    local players = PlayerManager:getPlayers()
    for _, player in pairs(players) do
        if winner == player:getTeamId() then
            player:onWin()
        end
    end
    return winner
end

function RewardUtil:doReward(players)
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
    end
    RewardManager:getQueueReward(function(data)
        self:sendGameSettlement(self:getWinnerTeam())
    end)
end

function RewardUtil:doReport(players)
    for rank, player in pairs(players) do
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
        player.kills = 0
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
        UserExpManager:addUserExp(player.userId, winner == player:getTeamId(), 2)
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
    item.kills = player.scorePoints[ScoreID.KILL]
    item.adSwitch = player.adSwitch or 0
    if item.gold <= 0 then
        item.adSwitch = 0
    end
    if winner == 0 then
        item.isWin = 2
    else
        if winner == player:getTeamId() then
            item.isWin = 1
        else
            item.isWin = 0
        end
    end
    return item
end

return RewardUtil