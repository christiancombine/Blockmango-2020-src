BuffConfig = {}

BuffConfig.settings = {}

function BuffConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.buffId = tonumber(vConfig.n_buffId) or 0 --buffId
        data.buffType = tonumber(vConfig.n_buffType) or 0 --buffType
        data.buffParam = tonumber(vConfig.n_buffParam) or 0 --buffParam
        data.buffEffect = vConfig.s_buffEffect or "" --buffEffect
        data.buffTime = tonumber(vConfig.n_buffTime) or 0 --buffTime
        table.insert(BuffConfig.settings, data)
    end
end

function BuffConfig:getBuffConfig(buffId)
    for _, v in pairs(BuffConfig.settings) do
        if v.buffId == buffId then
            return v
        end
    end
    return nil
end

return BuffConfig
