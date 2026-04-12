SupplyPool = class()

function SupplyPool:ctor(poolId)
    self.poolId = poolId
    self.weightSum = 0
    self.items = {}
end

function SupplyPool:addItem(itemConfig)
    local item = {
        weights = itemConfig.weights,
        itemId = itemConfig.itemId,
        supplyId = itemConfig.supplyId,
        number = itemConfig.number,
        isSpecialMonster = itemConfig.isSpecialMonster,
    }
    self.weightSum = self.weightSum + item.weights
    table.insert(self.items, item)
end

function SupplyPool:randomItemFromPool()
    local maxWeight = self.weightSum
    local rate = math.random(1,maxWeight)
    local sum = 0
    for _, data in pairs(self.items) do
        sum = BaseUtil:incNumber(sum, data.weights)
        if sum >= rate then
            return data
        end
    end
    return nil
end

return SupplyPool