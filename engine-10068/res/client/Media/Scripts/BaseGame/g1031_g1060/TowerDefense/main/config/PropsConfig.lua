PropsConfig = {}

local Config = {}

function PropsConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        data.targetType = tonumber(vConfig.targetType) or 0
        data.targetRange = tonumber(vConfig.targetRange) or 0
        data.buffId = tonumber(vConfig.buffId) or 0
        data.stopDuration = tonumber(vConfig.stopDuration) or -1
        data.addHp = tonumber(vConfig.addHp) or 0
        data.boxEffect = tostring(vConfig.boxEffect) or ""
        data.cd = tonumber(vConfig.cd) * 1000 or 1000
        data.screenEffect = tostring(vConfig.screenEffect) or ""
        data.effectDuration = tonumber(vConfig.effectDuration) or 1
        data.screenEffectIcon = tostring(vConfig.screenEffectIcon) or ""
        Config[data.Id] = data
    end
end

function PropsConfig:getConfigById(id)
    return Config[id]
end

PropsConfig:init(FileUtil.getConfigFromCsv("setting/Props.csv"))