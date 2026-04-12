UnitConfig = {}

UnitConfig.settings = {}

function UnitConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.n_id) or 0 --id
        data.digitNum = tonumber(vConfig.n_digitNum) or 0 --位数
        data.abbreviation = vConfig.s_abbreviation or "" --缩写单位
        table.insert(UnitConfig.settings, data)
    end

    table.sort(self.settings, function(a, b)
        return a.digitNum > b.digitNum
    end)

    --print("UnitConfig:init " .. inspect.inspect(self.settings))

    --LogUtil.log(string.format("Utility.toKMBString %s", Utility.toKMBString("300000000000000")))
end

function UnitConfig:getAbbreviation(num)
    for _, setting in pairs(self.settings) do
        if setting.digitNum == num then
            return setting
        end
    end
    return nil
end

return UnitConfig
