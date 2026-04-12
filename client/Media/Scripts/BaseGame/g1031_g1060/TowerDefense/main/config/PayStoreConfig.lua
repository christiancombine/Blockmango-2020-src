PayStoreConfig = {}

local Config = {}

function PayStoreConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.Id) or 0
        data.moneyType = tonumber(vConfig.moneyType) or 1
        data.price = tonumber(vConfig.price) or 9999
        data.itemType = tonumber(vConfig.itemType) or 0
        Config[data.id] = data
    end
end

function PayStoreConfig:getConfigById(id)
    return Config[id]
end

PayStoreConfig:init(FileUtil.getConfigFromCsv("setting/PayStore.csv"))