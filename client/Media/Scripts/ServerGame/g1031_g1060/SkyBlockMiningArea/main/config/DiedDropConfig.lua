
DiedDropConfig = {}
DiedDropConfig.diedDrop = {}

function DiedDropConfig:initConfig(diedDrop)
    for _, config in pairs(diedDrop) do
        local item = {}
        item.itemId = tonumber(config.ItemId) or 0
        item.itemMeta = tonumber(config.ItemMeta) or 0
        item.dropDown = tonumber(config.DropDown) or 0
        item.dropUp = tonumber(config.DropUp) or 0
        item.probability = tonumber(config.DropProbability) or 0
        item.num = 0
        item.died_num = 0
        table.insert(self.diedDrop, item)
    end
end

function DiedDropConfig:getItemConfig(id, meta, num)
    for k, v in pairs(self.diedDrop) do
        if v.itemId == id and v.itemMeta == meta then
            local item = TableUtil.copyTable(v)
            item.num = num
            item.died_num = 0
            local Pro = HostApi.random("diedDrop_Probability", 1, 100)
            if Pro <= item.probability and (item.dropUp > v.died_num or item.dropUp == 0) then
                local sub_num = 0
                if item.dropUp > 0 then
                    local n = HostApi.random("diedDrop_num", item.dropDown, item.dropUp)
                    if n >= (item.dropUp - v.died_num) then
                        n = item.dropUp - v.died_num
                    end
                    sub_num = n
                end
                item.num = num - sub_num
                if item.num >= 0 then
                    item.died_num = sub_num
                else
                    item.died_num = num
                end
                if id == 511 and meta == 0 then
                    item.num = 0
                    item.died_num = num
                else
                    v.died_num = v.died_num + item.died_num
                end
            end
            return item
        end
    end
    return nil
end

function DiedDropConfig:getCurrencyConfig(moneyNum)
    local money = moneyNum
    local money_heap = 0
    for k, currency in pairs(self.diedDrop) do
        if currency.itemId == 10000 then
            local Pro = HostApi.random("diedDrop_Probability", 1, 100)
            if Pro <= currency.probability then
                money = money - currency.dropDown
                money_heap = money_heap + currency.dropDown
            end
        end
    end
    if money < 0 then
        money = 0
    end
    if money_heap >= moneyNum then
        money_heap = moneyNum
    end
    return money, money_heap
end

function DiedDropConfig:revertDropConfig()
    for k, item in pairs(self.diedDrop) do
        item.died_num = 0
    end
end

return DiedDropConfig