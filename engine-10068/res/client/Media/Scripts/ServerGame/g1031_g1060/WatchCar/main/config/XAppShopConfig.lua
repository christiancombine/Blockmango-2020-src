XAppShopConfig = {}
XAppShopConfig.configTables = {}
function XAppShopConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local configTab = {}
        configTab.ShopType = tonumber(config.ShopType)
        configTab.ShopIcon = config.ShopIcon
        configTab.ShopName = config.ShopName
        table.insert(self.configTables, configTab)
    end
end

function XAppShopConfig:getConfigs()
    return self.configTables
end
