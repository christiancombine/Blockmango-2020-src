CustomItemConfig = {}
CustomItemConfig.List = {}

function CustomItemConfig:initConfig(configs)
    for _, config in pairs(configs) do
        local item = {
            ItemId = tonumber(config.ItemId),
        }
        self.List[item.ItemId] = item
        self:setItemMaxStackSize(item.ItemId, 1)
    end

end

function CustomItemConfig:isCustomItem(itemId)
    for _, item in pairs(self.List) do
        if item.ItemId == itemId then
            return true
        end
    end
    return false
end

function CustomItemConfig:setItemMaxStackSize(itemId, itemMaxStackSize)
    local itemStack = Item.getItemById(itemId)
    if itemStack ~= nil then
        itemStack:setMaxStackSize(itemMaxStackSize)
    end
end

return CustomItemConfig