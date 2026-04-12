LotteryRewardConfig = {}

LotteryRewardConfig.settings = {}
LotteryRewardConfig.commonSettings = {}
LotteryRewardConfig.coreSetting = {}
LotteryRewardConfig.coreWeightSum = 0
LotteryRewardConfig.commonWeightSum = 0
function LotteryRewardConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0 --奖品序号
        data.RewardId = tonumber(vConfig.RewardId) or 0 --奖品Id
        data.RedressRewardId = tonumber(vConfig.RedressRewardId) or 0 --补偿奖励Id
        data.RewardType = tonumber(vConfig.RewardType) or 0 --奖品类型
        data.IsCore = tonumber(vConfig.IsCore) or 1 --是否为核心物品
        data.Time = 3600 --1小时
        data.Count = tonumber(vConfig.Count) or 0 --物品数量
        data.Weight = tonumber(vConfig.Weight) or 0 --抽奖权重
        data.IsEnchant = tonumber(vConfig.Isenchant) or 0 --是否附魔
        data.name = tostring(vConfig.Name) or ""
        data.image = tostring(vConfig.Image) or ""

        if data.IsCore == 1 then
            table.insert(self.commonSettings, data)
            self.commonWeightSum = self.commonWeightSum + data.Weight
        elseif data.IsCore == 2 then
            table.insert(self.coreSetting, data)
            self.coreWeightSum = self.coreWeightSum + data.Weight
        end
        self.settings[data.Id] = data
    end
end

function LotteryRewardConfig:getRandomCommonItem(num)
    if #self.commonSettings == 0 or num == 0 then
        return nil
    end
    local itemNum = num
    local result = {}
    while (itemNum > 0) do
        local rate = math.random(1, self.commonWeightSum)
        local sum = 0
        for _, data in pairs(self.commonSettings) do
            sum = BaseUtil:incNumber(sum, data.Weight)
            if sum >= rate then
                local notRepeat = true
                for _, itemId in pairs(result) do
                    if itemId == data.Id then
                        notRepeat = false
                        break
                    end
                end
                if notRepeat then
                    table.insert(result, data.Id)
                    itemNum = itemNum - 1
                    break
                end
            end
        end
    end
    return result
end

function LotteryRewardConfig:getRandomCoreItem(num)
    if #self.coreSetting == 0 or num == 0 then
        return
    end
    local result = {}
    local itemNum = num
    while (itemNum > 0) do
        local rate = math.random(1, self.coreWeightSum)
        local sum = 0
        for _, data in pairs(self.coreSetting) do
            sum = BaseUtil:incNumber(sum, data.Weight)
            if sum >= rate then
                local notRepeat = true
                for _, itemId in pairs(result) do
                    if itemId == data.Id then
                        notRepeat = false
                        break
                    end
                end
                if notRepeat then
                    table.insert(result, data.Id)
                    itemNum = itemNum - 1
                    break
                end
            end
        end
    end
    return result
end

function LotteryRewardConfig:getRewardById(id)
    if self.settings[tonumber(id)] == nil then
        return
    end
    return self.settings[tonumber(id)]
end

function LotteryRewardConfig:getRewardByIds(ids)
    local rewards = {}
    ids = StringUtil.split(ids, "#")
    for _, id in pairs(ids) do
        local reward = self:getRewardById(tonumber(id))
        if reward ~= nil then
            table.insert(rewards, reward)
        end
    end
    return rewards
end

function LotteryRewardConfig:getIndexByRewardId(rewardId)
    for _, reward in pairs(self.settings) do
        if reward.RewardId == tonumber(rewardId) then
            return reward
        end
        return nil
    end
end

return LotteryRewardConfig
