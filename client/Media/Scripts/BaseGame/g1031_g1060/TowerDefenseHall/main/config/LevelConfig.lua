LevelConfig = {}

local Config = {}

function LevelConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        data.levelLimit = tonumber(vConfig.levelLimit) or 1
        data.allowEnter = tostring(vConfig.allowEnter) or ""
        data.notEnter = tostring(vConfig.notEnter) or ""
        Config[data.Id] = data
    end
end

function LevelConfig:getConfigById(id)
    return Config[id]
end

LevelConfig:init(FileUtil.getConfigFromCsv("setting/Level.csv"))