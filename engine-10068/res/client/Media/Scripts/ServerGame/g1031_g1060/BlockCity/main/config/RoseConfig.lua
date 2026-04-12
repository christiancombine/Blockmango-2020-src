RoseConfig = {}
RoseConfig.rose = {}

function RoseConfig:init(config)
    for _, v in pairs(config) do
        local data = {}
        data.orderId = tonumber(v.orderId)
        data.uniqueId = tonumber(v.uniqueId)
        data.itemId = tonumber(v.itemId)
        data.praiseNum = tonumber(v.praiseNum)
        self.rose[data.orderId] = data
    end
end

function RoseConfig:getPraiseNumByItemId(itemId)
    itemId = tonumber(itemId)
    for _, v in pairs(self.rose) do
        if v.itemId == itemId then
            return v.praiseNum
        end
    end

    return 0
end

function RoseConfig:getItemIdById(uniqueId)
    uniqueId = tonumber(uniqueId)
    for _, v in pairs(self.rose) do
        if v.uniqueId == uniqueId then
            return v.itemId
        end
    end

    return nil
end

function RoseConfig:isRoseItem(itemId)
    itemId = tonumber(itemId)
    for _, v in pairs(self.rose) do
        if v.itemId == itemId then
            return true
        end
    end

    return false
end

function RoseConfig:getDefaultRoseInfo()
    return self.rose[1]
end

return RoseConfig