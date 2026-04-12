BirdBagConfig = {}
BirdBagConfig.carryBag = {}
BirdBagConfig.capacityBag = {}

function BirdBagConfig:initCarryBag(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.price = tonumber(v.price)
        data.moneyType = tonumber(v.moneyType)
        self.carryBag[data.id] = data
    end

    table.sort(self.carryBag, function(a, b)
        return a.id < b.id
    end)

end

function BirdBagConfig:initCapacityBag(config)
    for i, v in pairs(config) do
        local data = {}
        data.id = tonumber(v.id)
        data.num = tonumber(v.num)
        data.price = tonumber(v.price)
        data.moneyType = tonumber(v.moneyType)
        self.capacityBag[data.id] = data
    end

    table.sort(self.capacityBag, function(a, b)
        return a.id < b.id
    end)
end

function BirdBagConfig:getMaxCarryBag()
    return #self.carryBag
end

function BirdBagConfig:getMaxCapacityBag()
    local maxCapacity = 0
    for _, v in pairs(self.capacityBag) do
        maxCapacity = v.num
    end

    return maxCapacity
end

function BirdBagConfig:getNextExpandCarryMoneyType(id)
    if self.carryBag[id + 1] ~= nil then
        return self.carryBag[id + 1].moneyType
    end

    return 3
end

function BirdBagConfig:getNextExpandCarryPrice(id)
    if self.carryBag[id + 1] ~= nil then
        return self.carryBag[id + 1].price
    end

    return 0
end

function BirdBagConfig:getNextExpandCapacityMoneyType(num)
    for _, v in pairs(self.capacityBag) do
        if v.num > num then
            return v.moneyType
        end
    end

    return 3
end

function BirdBagConfig:getNextExpandCapacityPrice(num)
    for _, v in pairs(self.capacityBag) do
        if v.num > num then
            return v.price
        end
    end

    return 0
end

function BirdBagConfig:getNextExpandCapacityNum(curNum)
    for _, v in pairs(self.capacityBag) do
        if v.num > curNum then
            return v.num
        end
    end

    return 0
end

return BirdBagConfig