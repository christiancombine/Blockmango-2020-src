KillBroadCastConfig = {}

local Config = {}

function KillBroadCastConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tostring(vConfig.id)
        data.price = tonumber(vConfig.price)
        data.oneKill = tostring(vConfig.oneKill)
        data.twoKill = tostring(vConfig.twoKill)
        data.threeKill = tostring(vConfig.threeKill)
        data.fourKill = tostring(vConfig.fourKill)
        data.FiveKill = tostring(vConfig.FiveKill)
        data.bg = tostring(vConfig.bg)
        Config[data.id] = data
    end
end

function KillBroadCastConfig:getConfig(id)
    return Config[tostring(id)]
end

KillBroadCastConfig:init(FileUtil.getConfigFromCsv("setting/KillBroadCast.csv"))