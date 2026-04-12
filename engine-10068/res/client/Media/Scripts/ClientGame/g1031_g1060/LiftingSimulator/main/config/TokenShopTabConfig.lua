local TokenTabs = MemoryPool.newTable()

TokenShopTabConfig = {}

function TokenShopTabConfig:loadConfig()
    local temp = FileUtil.getConfigFromCsv("setting/TokenShopTab.csv", 3)

    for _, Value in pairs(temp) do
        TokenTabs[tonumber(Value.n_Id)] = {
            Id = tonumber(Value.n_Id),
            Type = tonumber(Value.n_Type),
            Icon = Value.s_Icon,
            Name = Value.s_Name
        }
    end
end

function TokenShopTabConfig:getTabs()
    return TokenTabs
end


function TokenShopTabConfig:getTabsById(Id)

    return TokenTabs[Id]
end