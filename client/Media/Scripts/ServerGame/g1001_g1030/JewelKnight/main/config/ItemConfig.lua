ItemConfig = {}
ItemConfig.noDrop = {}
ItemConfig.removeItem = {}

function ItemConfig:initNoDropItem(noDrop)
    for i, v in pairs(noDrop) do
        local item = {}
        item.id = tonumber(v.id)
        item.meta = tonumber(v.meta)
        self.noDrop[i] = item
    end
end

function ItemConfig:initRemoveItem(removeItem)
    for i, v in pairs(removeItem) do
        local item = {}
        item.id = tonumber(v.id)
        self.removeItem[i] = item
    end
end

return ItemConfig