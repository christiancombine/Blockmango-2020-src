TeamConfig = {}

local Config = {}

function TeamConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Id = tonumber(vConfig.Id) or 0
        data.countLimit = tonumber(vConfig.countLimit) or 1
        data.countDown = tonumber(vConfig.countDown) or 1
        data.teleportTime = tonumber(vConfig.teleportTime) or 2
        data.gameType = tostring(vConfig.gameType) or ""
        data.mapId = tostring(vConfig.mapId) or ""
        data.levelLimit = tonumber(vConfig.levelLimit) or 1
        data.checkPointId = tonumber(vConfig.checkPointId) or 1
        data.mapPicture = tostring(vConfig.mapPicture) or ""
        Config[data.Id] = data
    end
end

function TeamConfig:getConfigById(id)
    return Config[id]
end

TeamConfig:init(FileUtil.getConfigFromCsv("setting/Team.csv"))