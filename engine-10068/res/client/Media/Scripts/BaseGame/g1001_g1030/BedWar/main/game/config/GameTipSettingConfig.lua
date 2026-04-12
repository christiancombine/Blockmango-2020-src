GameTipSettingConfig = {}

local Config = {}

function GameTipSettingConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tostring(vConfig.id)
        data.tipType = tostring(vConfig.tipType)
        data.tipBox = StringUtil.split(tostring(vConfig.tipBox), "#") or {}
        Config[data.id] = data
    end
end

function GameTipSettingConfig:getConfigByTipType(tipType)
    for _, data in pairs(Config) do
        if data.tipType == tostring(tipType) then
            return data
        end
    end
end

GameTipSettingConfig:init(FileUtil.getConfigFromCsv("setting/GameTipSetting.csv"))