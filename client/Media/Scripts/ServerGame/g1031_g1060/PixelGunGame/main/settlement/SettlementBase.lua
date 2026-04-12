SettlementBase = class()
GameResult = {
    WIN = 1, -- 赢
    LOSE = 0, -- 输
    DRAW = 2 -- 平局
}

function SettlementBase:sortPlayers(players)
    table.sort(players, function(p1, p2)
        if p1.kills ~= p2.kills then
            return p1.kills > p2.kills
        end
        return p1.userId < p2.userId
    end)
end

function SettlementBase:doReward(players, winner, trueRank)
    for rank, player in pairs(players) do
        if trueRank then
            RewardManager:addRewardQueue(player.userId, trueRank)
            break
        end
        RewardManager:addRewardQueue(player.userId, rank)
    end
    self:doAddExp(players)
    RewardManager:getQueueReward(function(data)
        if trueRank then
            self:doGameReward(players, trueRank)
        else
            self:doGameReward(players, winner)
        end
    end)
end

function SettlementBase:doReport(players)
    for rank, player in pairs(players) do
        if type(player.outCome) == "number" then
            if player.outCome == GameResult.WIN then
                player.appIntegral = player.appIntegral + 10
                ReportManager:reportUserWin(player.userId)
            end
        end
        ReportManager:reportUserData(player.userId, player.kills, rank, 1)
    end
end

function SettlementBase:doAddExp(players)
    for rank, player in pairs(players) do
        UserExpManager:addUserExp(player.userId, player.outCome == GameResult.WIN, 2)
    end
end

function SettlementBase:doGameReward(players, winner)

end

function SettlementBase:getPlayerRewardData(player, rank, isWin)
    local data = {}
    local reward
    if isWin then
        reward = AwardConfig:getWinByRank(rank)
    else
        reward = AwardConfig:getLoseByRank(rank)
    end
    local c_reward = player:getRewardInfo(reward, isWin)
    table.insert(data, {
        img = "set:pixelgungameresult.json image:exp",
        num = c_reward.exp
    })
    table.insert(data, {
        img = "set:pixelgungamebig.json image:jinbi",
        num = c_reward.money
    })
    table.insert(data, {
        img = "set:pixelgungamebig.json image:yaoshi",
        num = c_reward.yaoshi
    })
    table.insert(data, {
        img = "set:pixelgungameresult.json image:jiangbei",
        num = c_reward.integral
    })
    return data
end

function SettlementBase:sendGameSettlement(players)

end

