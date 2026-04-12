
SupplyPoolConfig = {}
SupplyPoolConfig.SupplySetting = {}

function SupplyPoolConfig:initSupplySetting(settingConfig)
    for _,configData in pairs(settingConfig) do
        local setting ={}
        setting.supplyId = tonumber(configData.supplyId) or 0
        setting.model = configData.Model or ""
        self.SupplySetting[setting.supplyId] = setting
    end
end

function SupplyPoolConfig:initItem(config)
    for _,configData in pairs(config) do
        local data = {}
        data.supplyPool = tonumber(configData.supplyPool) or 0
        data.itemId =  tonumber(configData.itemId) or 0
        data.supplyId = tonumber(configData.supplyId) or 0
        data.weights = tonumber(configData.weights) or 0
        data.number = tonumber(configData.number) or 0
        data.isSpecialMonster = tonumber(configData.isSpecialMonster) or 0
        SupplyManager:addItemToPool(data)
    end
end

return SupplyPoolConfig