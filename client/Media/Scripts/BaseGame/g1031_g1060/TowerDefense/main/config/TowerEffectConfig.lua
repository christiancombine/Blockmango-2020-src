TowerEffectConfig = {}

local Config = {}

function TowerEffectConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.id) or 0
        data.buildEffect = tostring(vConfig.buildEffect) or "fragment_block.effect"
        data.formulaEffect = tostring(vConfig.formulaEffect) or "g1056_explode.effect"
        data.destroyEffect = tostring(vConfig.destroyEffect) or "g1056_disappear.effect"
        Config[data.id] = data
    end
end

function TowerEffectConfig:getConfigById(id)
    return Config[id]
end

TowerEffectConfig:init(FileUtil.getConfigFromCsv("setting/TowerEffect.csv"))