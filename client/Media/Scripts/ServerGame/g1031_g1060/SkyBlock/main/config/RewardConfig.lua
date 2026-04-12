RewardConfig = {}
RewardConfig.Rewards = {}
function RewardConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            Id = tonumber(config.Id) or 0,
            Type = tonumber(config.Type) or 0,
            RewardId = tonumber(config.RewardId) or 0,
            Num = tonumber(config.Num) or 0,
            Meta = tonumber(config.Meta) or 0,
            Icon = config.Icon == "#" and "" or config.Icon,
            NumText = config.NumText or "",
        }
        self.Rewards[item.Id] = item
    end
end

function RewardConfig:getRewardById(id)
    return self.Rewards[id]
end

function RewardConfig:getRewardByIds(ids)
    local rewards = {}
    ids = StringUtil.split(ids, "#")
    for _, id in pairs(ids) do
        local reward = RewardConfig:getRewardById(tonumber(id))
        if reward ~= nil then
            table.insert(rewards, reward)
        end
    end
    return rewards
end

function RewardConfig:onPlayerGetReward(player, reward)
    local hasGet = true
    if reward.Type == Define.TaskRewardType.item then
        self:onPlayerRewarditem(player, reward.RewardId, reward.Meta, reward.Num)
    end

    if reward.Type == Define.TaskRewardType.money then
        self:onPlayerRewardMoney(player, reward.RewardId, reward.Num)
    end

    HostApi.notifyGetGoodsByItemId(player.rakssid, reward.RewardId, reward.Meta, reward.Num, reward.Icon)
end

function RewardConfig:onPlayerRewarditem(player, id, meta, count)
    HostApi.log("onPlayerRewarditem")
    player:addItem(id, count, meta or 0)
    player.taskController:packingProgressData(player, Define.TaskType.collect, GameMatch.gameType, id, count, meta)
end

function RewardConfig:onPlayerRewardMoney(player, id, count)
    if not player:CanSavePlayerShareData() then
        return
    end
    HostApi.log("onPlayerRewardMoney")
    -- player.money = player.money + count
    local money = player.money + count
	player:setCurrency(money)
    GameAnalytics:design(player.userId, 0, { "g1048addMoney", tostring(player.userId), count})
end

return RewardConfig
