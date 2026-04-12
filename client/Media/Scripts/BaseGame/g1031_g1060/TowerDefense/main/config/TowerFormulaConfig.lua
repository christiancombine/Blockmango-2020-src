TowerFormulaConfig = {}

local Config = {}
local ConfigByTowerId = {}

function TowerFormulaConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        data.ClassAId = tonumber(vConfig.ClassAId) or 0
        data.ClassBId = tonumber(vConfig.ClassBId) or 0
        data.ClassCId = tonumber(vConfig.ClassCId) or 0
        data.TowerId = tonumber(vConfig.TowerId) or ""
        Config[data.Id] = data
        ConfigByTowerId[data.TowerId] = data
    end
end

function TowerFormulaConfig:getConfigById(id)
    return Config[id]
end

function TowerFormulaConfig:getConfigByTowerId(towerId)
    return ConfigByTowerId[towerId]
end

TowerFormulaConfig:init(FileUtil.getConfigFromCsv("setting/TowerFormula.csv"))