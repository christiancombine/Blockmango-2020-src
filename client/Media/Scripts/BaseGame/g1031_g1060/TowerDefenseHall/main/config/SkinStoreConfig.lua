SkinStoreConfig = {}

local Config = {}

function SkinStoreConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.Id) or 0
        data.moneyType = tonumber(vConfig.moneyType) or 1
        data.price = tonumber(vConfig.price) or 9999
        data.actorName = tostring(vConfig.ActorName) or "g1055_boy.actor"
        data.thirdPersonDistance = tonumber(vConfig.thirdPersonDistance) or 3
        data.thirdPersonYOffset = tonumber(vConfig.thirdPersonYOffset) or 0
        Config[data.id] = data
    end
end

function SkinStoreConfig:getConfigById(id)
    return Config[id]
end

function SkinStoreConfig:getAllShopConfig()
    return Config
end

SkinStoreConfig:init(FileUtil.getConfigFromCsv("setting/SkinStore.csv"))