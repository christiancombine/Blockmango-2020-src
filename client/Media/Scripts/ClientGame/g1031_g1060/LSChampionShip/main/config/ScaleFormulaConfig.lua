ScaleFormulaConfig = {}

ScaleFormulaConfig.settings = MemoryPool.newTable()

function ScaleFormulaConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.muscleMassRange = tonumber(vConfig.n_muscleMassRange) or 0 --肌肉值范围
        data.coeA = tonumber(vConfig.n_coeA) or 0 --系数1
        data.coeB = tonumber(vConfig.n_coeB) or 0 --系数2
        data.coeC = tonumber(vConfig.n_coeC) or 0 --系数3
        table.insert(ScaleFormulaConfig.settings, data)
    end

    table.sort(ScaleFormulaConfig.settings, function(data1, data2)
        return data1.muscleMassRange < data2.muscleMassRange
    end)

    --print(inspect.inspect(ScaleFormulaConfig.settings))
end

function ScaleFormulaConfig:getScale(muscle)
    local lastRange = 0
    for i = 1, #ScaleFormulaConfig.settings do
        local config = ScaleFormulaConfig.settings[i]
        if muscle <= config.muscleMassRange then
            return config.coeA + (muscle - lastRange) / config.coeB * config.coeC
        end
        lastRange = config.muscleMassRange
    end
    return GameConfig.maxScale
end

return ScaleFormulaConfig
