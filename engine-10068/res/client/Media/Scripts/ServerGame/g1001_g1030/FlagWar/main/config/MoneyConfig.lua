---
--- Created by Jimmy.
--- DateTime: 2017/11/21 0021 10:52
---

MoneyConfig = {}
MoneyConfig.coinMapping = {}
MoneyConfig.moneys = {}

function MoneyConfig:initCoinMapping(coinMapping)
    for i, v in pairs(coinMapping) do
        self.coinMapping[i] = {}
        self.coinMapping[i].coinId = v.coinId
        self.coinMapping[i].itemId = v.itemId
    end
    WalletUtils:addCoinMappings(self.coinMapping)
end

function MoneyConfig:initMoneys(config)
    for m_i, m_item in pairs(config.moneys) do
        local money = {}
        money.lv = tonumber(m_item.lv)
        money.time = tonumber(m_item.time)
        money.output = {}
        for o_i, o_item in pairs(m_item.output) do
            local occupationId = tonumber(o_item.occupationId)
            local output = {}
            output.occupationId = occupationId
            output.interval = tonumber(o_item.interval)
            output.item = {}
            output.item.id = tonumber(o_item.item[1])
            output.item.meta = tonumber(o_item.item[2])
            output.item.num = tonumber(o_item.item[3])
            money.output[occupationId] = output
        end
        self.moneys[money.lv] = money
    end
end

function MoneyConfig:tryGiveMoneyToPlayer(tick, player)
    if tick == 0 or player == nil or player.occupationId == 0 then
        return
    end
    local money
    for lv, m_money in pairs(self.moneys) do
        if money == nil then
            money = m_money
        end
        if m_money.time <= tick and m_money.lv > money.lv then
            money = m_money
        end
    end
    if money == nil then
        return
    end
    local output = money.output[player.occupationId]
    if output == nil then
        return
    end
    if output.interval == 0 or tick % output.interval ~= 0 then
        return
    end
    if output.item.num == 0 then
        return
    end
    player:addItem(output.item.id, output.item.num, output.item.meta)
end

return MoneyConfig