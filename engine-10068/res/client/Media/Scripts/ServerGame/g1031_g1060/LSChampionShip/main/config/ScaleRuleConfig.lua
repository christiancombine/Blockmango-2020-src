ScaleRuleConfig = {}

ScaleRuleConfig.settings = {}

function ScaleRuleConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.muscleMassRange = tonumber(vConfig.n_muscleMassRange) or 0 --肌肉值范围
        data.scale = tonumber(vConfig.n_scale) or 0 --缩放值
        table.insert(ScaleRuleConfig.settings, data)
    end

    table.sort(ScaleRuleConfig.settings, function(data1, data2)
        return data1.muscleMassRange > data2.muscleMassRange
    end)

    --print(inspect.inspect(ScaleRuleConfig.settings))
end

function ScaleRuleConfig:getScale(muscleMass)
    for i = 1, #ScaleRuleConfig.settings do
        if muscleMass >= ScaleRuleConfig.settings[i].muscleMassRange then
            return ScaleRuleConfig.settings[i].scale
        end
    end
    return 1.0
end

return ScaleRuleConfig
