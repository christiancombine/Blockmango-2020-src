SkillStoreConfig = {}

local Config = {}

function SkillStoreConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.id) or 0
        data.icon = tostring(vConfig.icon) or ""
        data.buyTip = tostring(vConfig.buyTip) or ""
        data.effectTip = tostring(vConfig.effectTip) or ""
        data.price = tonumber(vConfig.price) or 99
        data.off = {}
        for index, id in pairs(StringUtil.split(vConfig.off, "#")) do
            data.off[index] = tonumber(id)
        end
        data.prop = tonumber(vConfig.prop) or 1
        Config[data.id] = data
    end
end

function SkillStoreConfig:getConfigById(id)
    return Config[id]
end

function SkillStoreConfig:getAllSkillConfig()
    return Config
end

function SkillStoreConfig:getNameById(id)
    if Config[id] then
        return Config[id].buyTip
    end
    return ""
end

SkillStoreConfig:init(FileUtil.getConfigFromCsv("setting/SkillStore.csv"))