---
--- Created by Jimmy.
--- DateTime: 2017/11/21 0021 10:52
---

MoneyConfig = {}
MoneyConfig.coinMapping = {}

function MoneyConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = v.coinId
        self.coinMapping[i].itemId = v.itemId
    end
end

function MoneyConfig:getItemIdByCoinId(coinId)
    for _, coin in pairs(self.coinMapping) do
        if coin.coinId == coinId then
            return coin.itemId
        end
    end
    return 0
end

function MoneyConfig:hasMoneyNum(player, coinId)
    local itemId = self:getItemIdByCoinId(coinId)
    return player:getInventory():getItemNum(itemId)
end

function MoneyConfig:costMoney(player, coinId, coinNum)
    local itemId = self:getItemIdByCoinId(coinId)
    player:removeItem(itemId, coinNum)
end

return MoneyConfig