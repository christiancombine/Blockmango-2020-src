---
--- Created by Jimmy.
--- DateTime: 2018/9/25 0025 14:45
---

KillRewardConfig = {}
KillRewardConfig.killRewards = {}

function KillRewardConfig:init(killRewards)
    self:initKillRewards(killRewards)
end

function KillRewardConfig:initKillRewards(killRewards)
    for _, killReward in pairs(killRewards) do
        local item = {}
        item.id = tonumber(killReward.id)
        local range = StringUtil.split(killReward.killRange, "#")
        item.killRange = { min = tonumber(range[1]), max = tonumber(range[2]) }
        item.kills = tonumber(killReward.kills)
        item.baseMoney = tonumber(killReward.baseMoney)
        item.killMoney = tonumber(killReward.killMoney)
        item.dieMoney = tonumber(killReward.dieMoney)
        self.killRewards[killReward.killRange .. "#" .. item.kills] = item
    end
end

function KillRewardConfig:getReward(kills)
    local killRange
    for _, reward in pairs(self.killRewards) do
        if killRange == nil then
            killRange = reward.killRange.min .. "#" .. reward.killRange.max
        end
        if GameMatch.curKills >= reward.killRange.min and GameMatch.curKills < reward.killRange.max then
            killRange = reward.killRange.min .. "#" .. reward.killRange.max
            break
        end
    end
    if killRange == nil then
        killRange = "0#18"
    end
    local reward = self.killRewards[killRange .. "#" .. kills]
    if reward == nil then
        reward = self.killRewards["0#18#0"]
    end
    return reward
end

return KillRewardConfig