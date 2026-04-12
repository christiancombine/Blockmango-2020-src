PlayerDropItemProbabilityConfig = {}

PlayerDropItemProbabilityConfig.settings = {}

function PlayerDropItemProbabilityConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.quality = tonumber(vConfig.n_quality) or 0 --品质(1-白,2-绿,3-蓝,4-紫)
        data.probability = tonumber(vConfig.n_probability) or 0 --掉落概率
        PlayerDropItemProbabilityConfig.settings[data.quality] = data
    end
end

return PlayerDropItemProbabilityConfig
