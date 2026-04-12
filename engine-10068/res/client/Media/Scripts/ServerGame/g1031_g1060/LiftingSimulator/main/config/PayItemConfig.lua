PayItemConfig = {}

PayItemConfig.settings = {}

function PayItemConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.type = tonumber(vConfig.n_type) or 0 --道具类型
        data.price = tonumber(vConfig.n_price) or 0 --价格
        data.guideId = tonumber(vConfig.n_guideId) or 0 --购买后引导Id
        PayItemConfig.settings[data.type] = data
    end
end

function PayItemConfig:getPayItemConfig(type)
    return PayItemConfig.settings[type]
end

return PayItemConfig
