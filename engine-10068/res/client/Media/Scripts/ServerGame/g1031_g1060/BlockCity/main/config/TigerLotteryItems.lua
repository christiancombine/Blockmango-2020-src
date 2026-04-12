TigerLotteryItems = {}
TigerLotteryItems.tigerLotteryItems = {}

function TigerLotteryItems:init(config)
    self:initTigerLotteryItems(config)
end

function TigerLotteryItems:initTigerLotteryItems(Datas)
    for id, data in pairs(Datas) do
        local item = {}
        item.id = data.number
        item.itemId = data.itemId
        item.itemIdMate = data.itemId2
        item.pool = data.pool
        item.pro = data.weights
        item.num = data.sum
        item.type = data.type
        self.tigerLotteryItems[tonumber(id)] = item
    end
    --LotteryUtil:initLotteryConfig()
end

function TigerLotteryItems:GetTigerLotteryItems()
    return self.tigerLotteryItems
end

function TigerLotteryItems:GetOneLineTigerLotteryItemInfoById(id)
    if self.tigerLotteryItems[tonumber(id)] == nil then
        return nil
    end
    return self.tigerLotteryItems[tonumber(id)]
end

return TigerLotteryItems