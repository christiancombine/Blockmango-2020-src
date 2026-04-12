RewardLevelConfig = {}
RewardLevelConfig.settings = {}

function RewardLevelConfig:init(config)
    for i, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.n_Id) or 0 --序号
        data.Number = tonumber(vConfig.n_Number) or 0 --核心物品数量
        data.Weight = tonumber(vConfig.n_Weight) or 0 --权重
        RewardLevelConfig.settings[tonumber(i)] = data
    end
end

function RewardLevelConfig:getRewardLevel()
    local num = MathUtil:randomIntNoRepeat(1, TableUtil.getTableSize(RewardLevelConfig.settings), 1)
    local result = RewardLevelConfig.settings[tonumber(num[1])]
    if result ~= nil then
        return result
    else
        return nil
    end
end

return RewardLevelConfig
