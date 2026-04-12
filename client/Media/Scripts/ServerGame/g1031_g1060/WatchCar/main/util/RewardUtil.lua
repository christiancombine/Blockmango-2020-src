RewardUtil = {}
function RewardUtil:doReward(players, winTeamId)
    for rank, player in pairs(players) do
        RewardManager:addRewardQueue(player.userId, rank)
    end
    RewardManager:getQueueReward(function(data)
        self:sendGameSettlement(winTeamId)
    end)
end

function RewardUtil:doReport(players, winTeamId)
    for rank, player in pairs(players) do
        if type(winTeamId) == 'number' then
            if player.bindTeamId == winTeamId then
                player.appIntegral = player.appIntegral + 10
                ReportManager:reportUserWin(player.userId)
            end
        end
        ReportManager:reportUserData(player.userId, player.killPlayerCount, rank, 1)
    end
end

function RewardUtil:sendGameSettlement(winTeamId)

    local Players = self:sortPlayerRank(winTeamId)
    local players = {}

    for index = 1, #Players do
        local player = Players[index]
        table.insert(players, self:newSettlement(player, index, winTeamId))
    end

    for rank, player in pairs(Players) do
        local result = {}
        player.rank = rank
        result.players = players
        result.own = self:newSettlement(player, rank, winTeamId)
        local json_data = json.encode(result)
        HostApi.sendGameSettlement(player.rakssid, json_data, true)
    end
    for rank, player in pairs(Players) do
        if winTeamId == nil then
            UserExpManager:addUserExp(player.userId, false, 2)
        else
            UserExpManager:addUserExp(player.userId, winTeamId == player.bindTeamId, 2)
        end
    end
end

function RewardUtil:sortPlayerRank(winTeamId)
    local players = PlayerManager:copyPlayers()

    table.sort(players,
    function(a, b)
        if winTeamId ~= nil then
            if a.bindTeamId ~= b.bindTeamId then
                return a.bindTeamId == winTeamId
            end
        end
        if a.killPlayerCount ~= b.killPlayerCount then
            return a.killPlayerCount > b.killPlayerCount
        end
    end
    )
   
    return players
end

function RewardUtil:newSettlement(player, rank, winTeamId)
    local item = {}
    item.name = player.name
    item.rank = rank
    item.points = player.scorePoints
    item.gold = player.gold
    item.available = player.available
    item.hasGet = player.hasGet
    item.vip = player.vip
    item.kills = player.killPlayerCount  -- 0失败 1胜利
    item.isWin = GameResult.DRAW

    if type(winTeamId) == 'number' then
        item.isWin = winTeamId == player.bindTeamId and GameResult.WIN or GameResult.LOSE
    else
        item.isWin = GameResult.DRAW
    end
    return item
end