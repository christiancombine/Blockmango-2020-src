LotteryPricesConfig = {}

LotteryPricesConfig.settings = {}
LotteryPricesConfig.maxTimes = 0

function LotteryPricesConfig:init(config)
    for _, vConfig in pairs(config) do
        local data = {}
        data.Times = tonumber(vConfig.n_Times) or 0 --抽奖次数
        data.Type = tonumber(vConfig.n_Type) or 0 --货币类型（5.钥匙）
        data.Prices = tonumber(vConfig.n_Prices) or 0 --价格
        self.settings[data.Times] = data
        self.maxTimes = (data.Times > self.maxTimes) and data.Times or self.maxTimes
    end
end

function LotteryPricesConfig:getLotteryPrices(lotteryTimes)
    if lotteryTimes >= self.maxTimes then
        return self.settings[self.maxTimes]
    end
    for _,Times in pairs(self.settings) do
        if lotteryTimes == Times.Times then
            return self.settings[lotteryTimes]
        end
    end
end

return LotteryPricesConfig
