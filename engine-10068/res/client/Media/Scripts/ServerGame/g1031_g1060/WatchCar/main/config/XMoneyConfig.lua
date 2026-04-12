XMoneyConfig = {}
XMoneyConfig.configTables = {}
function XMoneyConfig:OnInit(meonyConfigs)
    for _, config in pairs(meonyConfigs) do
        local configTab = {}
        configTab.PeriodTime = tonumber(config.PeriodTime)
        configTab.Interval = tonumber(config.Interval)
        configTab.Number = tonumber(config.Number)
        table.insert(self.configTables, configTab)
    end
    table.sort(
        self.configTables,
        function(a, b)
            return a.PeriodTime < b.PeriodTime
        end
    )
    for index = 1, #self.configTables do
        local tab = self.configTables[index]
    end
end

function XMoneyConfig:getMoneyConfig(time)
    for index = 1, #self.configTables - 1 do
        local tab1 = self.configTables[index]
        local tab2 = self.configTables[index + 1]
        if tab1 then
            if tab1 and tab2 then
                if time >= tab1.PeriodTime and time < tab2.PeriodTime then
                    return tab1.Interval, tab1.Number
                end
            end
        end
    end
    local config = self.configTables[#self.configTables]
    return config.Interval, config.Number
end
