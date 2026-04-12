require "config.XTools"
XNpcShopConfig = {}
XNpcShopConfig.configTables = {}
function XNpcShopConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local configTab = {}
        configTab.MerchantId = tonumber(config.MerchantId)
        configTab.MerchantName = config.MerchantName
        configTab.MerchantPos = XTools:CastToVector(config.MerchantPosX, config.MerchantPosY, config.MerchantPosZ)
        configTab.BelongTeamId = tonumber(config.BelongTeamId)
        configTab.MerchantYaw = tonumber(config.MerchantYaw)
        table.insert(self.configTables, configTab)
    end
end

function XNpcShopConfig:getConfigByTeamId(teamId)
    local configs = {}
    for index = 1, #self.configTables do
        local merchant = self.configTables[index]
        if merchant.BelongTeamId == teamId then
            table.insert(configs, merchant)
        end
    end
    return configs
end

function XNpcShopConfig:getConfigs()
    return self.configTables
end