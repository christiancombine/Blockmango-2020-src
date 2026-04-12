GameChampionConfig = {}

GameChampionConfig.settings = {}

function GameChampionConfig:initConfig(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.id) or 1
        data.type = tonumber(vConfig.type) or 0
        data.actor = "boy.actor"
        data.pos = vConfig.pos
        data.yaw = tonumber(vConfig.yaw) or 0
        data.scale = tonumber(vConfig.scale) or 1
        data.height = tonumber(vConfig.height) or 1
        data.textScale = tonumber(vConfig.textScale) or 1.0
        data.topInfo = tostring(vConfig.topInfo) or ""
        data.lv = tonumber(vConfig.lv) or 1
        table.insert(GameChampionConfig.settings, data)
    end
end

return GameChampionConfig