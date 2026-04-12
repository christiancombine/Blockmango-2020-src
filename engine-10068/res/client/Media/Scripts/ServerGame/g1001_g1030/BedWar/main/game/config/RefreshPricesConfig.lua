RefreshPricesConfig = {}

RefreshPricesConfig.settings = {}
RefreshPricesConfig.maxTimes = 0

function RefreshPricesConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Times = tonumber(vConfig.n_Times) or 0 --刷新次数
        data.Type = tonumber(vConfig.n_Type) or 0 --货币类型
        data.Prices = tonumber(vConfig.n_Prices) or 0 --价格
        table.insert(self.settings, data)
        self.maxTimes = (data.Times > self.maxTimes) and data.Times or self.maxTimes
    end
end

function RefreshPricesConfig:getRefreshPrices(RefreshTimes)
    if RefreshTimes > self.maxTimes then
        return self.settings[self.maxTimes]
    end
    for _, Times in pairs(self.settings) do
        if RefreshTimes == Times.Times then
            return Times
        end
    end
end

return RefreshPricesConfig
