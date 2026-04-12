---
--- Created by Jimmy.
--- DateTime: 2018/7/18 0018 10:53
---

RankRewardConfig = {}
RankRewardConfig.rewards = {}

function RankRewardConfig:init(rewards)
    self:initRankRewards(rewards)
end

function RankRewardConfig:initRankRewards(rewards)
    for _, reward in pairs(rewards) do
        local item = {}
        item.rank = tonumber(reward.rank)
        item.money = tonumber(reward.money)
        self.rewards[item.rank] = item
    end
end

function RankRewardConfig:getRewardByRank(rank)
    local reward = self.rewards[rank]
    if reward == nil then
        reward = self.rewards[16]
    end
    return reward or { rank = 16, money = 15 }
end

return RankRewardConfig