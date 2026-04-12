
MoneyConfig = {}
MoneyConfig.coinMapping = {}

function MoneyConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = tonumber(v.coinId)
        self.coinMapping[i].itemId = tonumber(v.itemId)
    end
    WalletUtils:addCoinMappings(self.coinMapping)
end

return MoneyConfig