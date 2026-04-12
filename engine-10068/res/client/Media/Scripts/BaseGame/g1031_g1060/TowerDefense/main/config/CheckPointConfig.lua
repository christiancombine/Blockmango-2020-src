CheckPointConfig = {}

CheckPointConfig.settings = {}

function CheckPointConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.id = tonumber(vConfig.Id) or 0
        data.hp = tonumber(vConfig.checkPointHp) or 40
        data.maxPlayerNum = tonumber(vConfig.maxPlayerNum) or 4
        data.initMoney = tonumber(vConfig.initMoney) or 40
        data.exchangeLimit = tonumber(vConfig.exchangeLimit) or 0
        data.paySkill = {}
        data.bgmId = tonumber(vConfig.bgmId or "0")
        for index, id in pairs(StringUtil.split(vConfig.paySkill, "#")) do
            data.paySkill[index] = tonumber(id)
        end
        self.settings[data.id] = data
    end
end

function CheckPointConfig:getCheckPointNum()
    return #self.settings
end

function CheckPointConfig:getAllCheckPointConfig()
    return self.settings
end

function CheckPointConfig:getCheckPointConfigById(id)
    return self.settings[id]
end

CheckPointConfig:init(FileUtil.getConfigFromCsv("setting/CheckPoint.csv"))