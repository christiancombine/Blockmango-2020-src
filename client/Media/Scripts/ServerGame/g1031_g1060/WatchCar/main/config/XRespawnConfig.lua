XRespawnConfig = {}
XRespawnConfig.configs = {}
function XRespawnConfig:OnInit(configs)
    for _, config in pairs(configs) do
        local configTab = {}
        configTab.PeriodTime = tonumber(config.PeriodTime)
        configTab.WaitTime = tonumber(config.WaitTime)
        table.insert(self.configs, configTab)
    end
    table.sort(self.configs,function(a,b)
        return a.PeriodTime < b.PeriodTime
    end)
end

function XRespawnConfig:getWaitTimeByPeriodTime(time)
    for index = 1, #self.configs do
        local tab1 = self.configs[index]
        local tab2 = self.configs[index + 1]
        if tab1 ~= nil then
            if tab2  ~= nil then
                if time >= tab1.PeriodTime and time < tab2.PeriodTime then
                    return tab1.WaitTime or 0
                end
            end
        end
    end
    local tab = self.configs[#self.configs]
    return tab ~= nil and tab.WaitTime or 0
end
