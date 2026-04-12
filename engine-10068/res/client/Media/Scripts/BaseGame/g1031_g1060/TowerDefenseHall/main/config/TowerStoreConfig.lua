TowerStoreConfig = {}

local Config = {}
local SeqList = {}

function TowerStoreConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.Id) or 0
        data.moneyType = tonumber(vConfig.moneyType) or 1
        data.price = tonumber(vConfig.price) or 9999
        data.towerId = tonumber(vConfig.towerId) or 1
        data.rewardId = tonumber(vConfig.rewardId) or 1
        data.seq = tonumber(vConfig.seq) or 1
        Config[data.id] = data
        SeqList[tonumber(data.seq)] = data
    end
end

function TowerStoreConfig:getConfigById(id)
    return Config[id]
end

function TowerStoreConfig:getAllShopConfig()
    return Config
end

function TowerStoreConfig:getShopConfigSeq()
    return SeqList
end

TowerStoreConfig:init(FileUtil.getConfigFromCsv("setting/TowerStore.csv"))