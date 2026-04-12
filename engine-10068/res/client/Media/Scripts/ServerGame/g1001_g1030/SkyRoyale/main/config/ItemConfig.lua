ItemConfig = {}
ItemConfig.playerInitItem = {}

function ItemConfig:initItem(config)
    for i, v in pairs(config) do
        local item = {}
        item.id = tonumber(v.id)
        item.itemId = tonumber(v.itemId)
        item.meta = tonumber(v.meta)
        item.num = tonumber(v.num)
        self.playerInitItem[item.id] = item
    end
end

return ItemConfig