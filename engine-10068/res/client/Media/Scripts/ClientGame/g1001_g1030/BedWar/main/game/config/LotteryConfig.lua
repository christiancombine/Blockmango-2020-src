local Rewards = MemoryPool.newTable()

LotteryConfig = {}

function LotteryConfig.loadConfig()
    local rewards = FileUtil.getConfigFromCsv("setting/LotteryRewardSetting.csv", 3)
    for _, reward in pairs(rewards) do
        Rewards[reward.Id] = {
            Id = tonumber(reward.Id),
            RewardId = tonumber(reward.RewardId),
            Image = tostring(reward.Image),
            SortWeight = tonumber(reward.SortWeight),
            IsEnchant = tonumber(reward.Isenchant),
            Name = tostring(reward.Name),
            Image = tostring(reward.Image),
            Desc = tostring(reward.Desc),
            Desc1 = tostring(reward.Desc1 or ""),
            IsCore = tostring(reward.IsCore),
            CountText = reward.CountText,
            IsEnchant = reward.Isenchant,
        }
    end
end

function LotteryConfig.findRewardById(Id)
    return Rewards[tostring(Id)]
end

function LotteryConfig.findRewardByRewardId(rewardId)
    for _, reward in pairs(Rewards) do
        if reward.RewardId == rewardId then
            return reward
        end
    end
    return nil
end

function LotteryConfig.rewards()
    return Rewards
end